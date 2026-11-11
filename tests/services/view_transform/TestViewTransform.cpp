/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include <limits>
#include <memory>

#include <QSignalSpy>
#include <QTest>

#include "ViewTransform.hpp"
#include "ViewTransformFactory.hpp"

namespace
{

// A 4000 x 3000 photo in an 800 x 600 viewport fits at 20 %.
const QSize largeImage(4000, 3000);
const QSize smallImage(400, 300);
const QSizeF viewport(800, 600);
constexpr qreal largeImageFitScale = 0.2;

}  // namespace

namespace pictorio::view_transform
{

class TestViewTransform : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();

    void doesNothingWithoutImage();

    void fitsLargeImageIntoViewport();
    void doesNotEnlargeSmallImage();
    void followsViewportWhileFitting();

    void zoomInLeavesFit();
    void zoomOutStopsAtFit();
    void zoomBySteps();
    void limitsZoomToMaximum();
    void ignoresInvalidZoomFactors_data();
    void ignoresInvalidZoomFactors();
    void showsActualSize();
    void actualSizeOfSmallImageIsFit();
    void turningFitOffShowsActualSize();
    void switchesToFitWhenViewportShowsWholeImage();

    void rotatesClockwise();
    void rotatesCounterclockwise();
    void rotationSwapsSidesOfDisplayedSize();

    void newImageResetsZoomAndRotation();
    void notifiesChanges();

private:
    std::unique_ptr<ViewTransform> m_viewTransform;
};

void TestViewTransform::init()
{
    m_viewTransform = createViewTransform();
    m_viewTransform->setViewportSize(viewport);
}

void TestViewTransform::doesNothingWithoutImage()
{
    m_viewTransform->zoomIn();
    m_viewTransform->showActualSize();

    QVERIFY(m_viewTransform->fitToWindow());
    QCOMPARE(m_viewTransform->scale(), 1.0);
    QVERIFY(!m_viewTransform->canZoomIn());
    QVERIFY(!m_viewTransform->canZoomOut());
}

void TestViewTransform::fitsLargeImageIntoViewport()
{
    m_viewTransform->showImage(largeImage);

    QVERIFY(m_viewTransform->fitToWindow());
    QCOMPARE(m_viewTransform->scale(), largeImageFitScale);
    QCOMPARE(m_viewTransform->displayedSize(), viewport);
    QVERIFY(m_viewTransform->canZoomIn());
    QVERIFY(!m_viewTransform->canZoomOut());
}

void TestViewTransform::doesNotEnlargeSmallImage()
{
    m_viewTransform->showImage(smallImage);

    QCOMPARE(m_viewTransform->scale(), 1.0);
    QCOMPARE(m_viewTransform->displayedSize(), QSizeF(smallImage));
}

void TestViewTransform::followsViewportWhileFitting()
{
    m_viewTransform->showImage(largeImage);

    m_viewTransform->setViewportSize(QSizeF(2000, 600));  // now the height limits the scale

    QCOMPARE(m_viewTransform->scale(), largeImageFitScale);
    m_viewTransform->setViewportSize(QSizeF(2000, 3000));
    QCOMPARE(m_viewTransform->scale(), 0.5);
}

void TestViewTransform::zoomInLeavesFit()
{
    m_viewTransform->showImage(largeImage);

    m_viewTransform->zoomIn();

    QVERIFY(!m_viewTransform->fitToWindow());
    QCOMPARE(m_viewTransform->scale(), largeImageFitScale * ViewTransform::zoomStep);
    QVERIFY(m_viewTransform->canZoomOut());
}

void TestViewTransform::zoomOutStopsAtFit()
{
    m_viewTransform->showImage(largeImage);
    m_viewTransform->zoomIn();

    m_viewTransform->zoomOut();
    m_viewTransform->zoomOut();

    QVERIFY(m_viewTransform->fitToWindow());
    QCOMPARE(m_viewTransform->scale(), largeImageFitScale);
}

void TestViewTransform::zoomBySteps()
{
    m_viewTransform->showImage(largeImage);

    m_viewTransform->zoomBySteps(2);

    QCOMPARE(m_viewTransform->scale(), largeImageFitScale * ViewTransform::zoomStep * ViewTransform::zoomStep);
}

void TestViewTransform::limitsZoomToMaximum()
{
    m_viewTransform->showImage(largeImage);

    m_viewTransform->zoomBy(1000);

    QCOMPARE(m_viewTransform->scale(), ViewTransform::maximumScale);
    QVERIFY(!m_viewTransform->canZoomIn());
}

void TestViewTransform::ignoresInvalidZoomFactors_data()
{
    QTest::addColumn<qreal>("factor");

    QTest::newRow("zero") << 0.0;
    QTest::newRow("negative") << -2.0;
    QTest::newRow("infinite") << std::numeric_limits<qreal>::infinity();
    QTest::newRow("not a number") << std::numeric_limits<qreal>::quiet_NaN();
}

void TestViewTransform::ignoresInvalidZoomFactors()
{
    QFETCH(qreal, factor);
    m_viewTransform->showImage(largeImage);

    m_viewTransform->zoomBy(factor);

    QVERIFY(m_viewTransform->fitToWindow());
    QCOMPARE(m_viewTransform->scale(), largeImageFitScale);
}

void TestViewTransform::showsActualSize()
{
    m_viewTransform->showImage(largeImage);

    m_viewTransform->showActualSize();

    QVERIFY(!m_viewTransform->fitToWindow());
    QCOMPARE(m_viewTransform->scale(), 1.0);
    QCOMPARE(m_viewTransform->displayedSize(), QSizeF(largeImage));
}

void TestViewTransform::actualSizeOfSmallImageIsFit()
{
    m_viewTransform->showImage(smallImage);

    m_viewTransform->showActualSize();

    QVERIFY(m_viewTransform->fitToWindow());
    QCOMPARE(m_viewTransform->scale(), 1.0);
}

void TestViewTransform::turningFitOffShowsActualSize()
{
    m_viewTransform->showImage(largeImage);

    m_viewTransform->setFitToWindow(false);

    QVERIFY(!m_viewTransform->fitToWindow());
    QCOMPARE(m_viewTransform->scale(), 1.0);
    m_viewTransform->setFitToWindow(true);
    QCOMPARE(m_viewTransform->scale(), largeImageFitScale);
}

void TestViewTransform::switchesToFitWhenViewportShowsWholeImage()
{
    m_viewTransform->showImage(largeImage);
    m_viewTransform->zoomIn();  // 25 %

    m_viewTransform->setViewportSize(QSizeF(2000, 1500));  // fits at 50 %

    QVERIFY(m_viewTransform->fitToWindow());
    QCOMPARE(m_viewTransform->scale(), 0.5);
}

void TestViewTransform::rotatesClockwise()
{
    m_viewTransform->showImage(largeImage);

    QList<int> angles;
    for (int turn = 0; turn < 4; ++turn)
    {
        m_viewTransform->rotateClockwise();
        angles.append(m_viewTransform->rotationAngle());
    }

    QCOMPARE(angles, (QList<int>{90, 180, 270, 0}));
}

void TestViewTransform::rotatesCounterclockwise()
{
    m_viewTransform->showImage(largeImage);

    QList<int> angles;
    for (int turn = 0; turn < 4; ++turn)
    {
        m_viewTransform->rotateCounterclockwise();
        angles.append(m_viewTransform->rotationAngle());
    }

    QCOMPARE(angles, (QList<int>{270, 180, 90, 0}));
}

void TestViewTransform::rotationSwapsSidesOfDisplayedSize()
{
    m_viewTransform->showImage(largeImage);

    m_viewTransform->rotateClockwise();

    // Portrait 3000 x 4000 in 800 x 600: the height limits the scale to 15 %.
    QCOMPARE(m_viewTransform->scale(), 0.15);
    QCOMPARE(m_viewTransform->displayedSize(), QSizeF(450, 600));
}

void TestViewTransform::newImageResetsZoomAndRotation()
{
    m_viewTransform->showImage(largeImage);
    m_viewTransform->rotateClockwise();
    m_viewTransform->zoomIn();

    m_viewTransform->showImage(largeImage);

    QVERIFY(m_viewTransform->fitToWindow());
    QCOMPARE(m_viewTransform->rotationAngle(), 0);
    QCOMPARE(m_viewTransform->scale(), largeImageFitScale);
}

void TestViewTransform::notifiesChanges()
{
    m_viewTransform->showImage(largeImage);
    QSignalSpy changeSpy(m_viewTransform.get(), &ViewTransform::transformChanged);

    m_viewTransform->zoomIn();
    m_viewTransform->rotateClockwise();
    m_viewTransform->setViewportSize(viewport);  // unchanged: no notification

    QCOMPARE(changeSpy.count(), 2);
}

}  // namespace pictorio::view_transform

QTEST_GUILESS_MAIN(pictorio::view_transform::TestViewTransform)

#include "TestViewTransform.moc"
