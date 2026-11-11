/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include <atomic>
#include <memory>

#include <QSignalSpy>
#include <QTest>

#include "NavigatorFactory.hpp"
#include "Slideshow.hpp"
#include "SlideshowFactory.hpp"

namespace
{

using pictorio::folder_scanner::FolderScanner;
using pictorio::folder_scanner::Sorting;
using pictorio::folder_scanner::SortOrder;

const QStringList folderImages{
    QStringLiteral("/photos/a.jpg"),
    QStringLiteral("/photos/b.jpg"),
    QStringLiteral("/photos/c.jpg"),
};

// Always lists the same folder and remembers the order it was asked for. Called from a background thread.
class FakeFolderScanner : public FolderScanner
{
public:
    QStringList listImages(const QString& /*folderPath*/, const Sorting& sorting) const override
    {
        lastSortOrder = sorting.order;
        return folderImages;
    }

    mutable std::atomic<SortOrder> lastSortOrder = SortOrder::Name;
};

// The shortest interval; tests that wait for the timer take about this long.
constexpr int oneSecondInMilliseconds = 1000;

}  // namespace

namespace pictorio::slideshow
{

class TestSlideshow : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();

    void startsStopped();
    void startUsesChosenOrderAndWrapsAround_data();
    void startUsesChosenOrderAndWrapsAround();
    void stopReturnsToNameOrder();
    void limitsInterval();
    void pauseAndResume();
    void advancesAfterInterval();
    void doesNotAdvanceWhilePaused();

private:
    void openImageAndWaitForListing(const QString& filePath);
    void waitForListing();

    std::unique_ptr<navigator::Navigator> m_navigator;
    FakeFolderScanner* m_folderScanner = nullptr;  // owned by m_navigator
    std::unique_ptr<Slideshow> m_slideshow;
};

void TestSlideshow::init()
{
    m_slideshow.reset();  // before the navigator it uses
    auto folderScanner = std::make_unique<FakeFolderScanner>();
    m_folderScanner = folderScanner.get();
    m_navigator = navigator::createNavigator(std::move(folderScanner));
    m_slideshow = createSlideshow(m_navigator.get());
}

void TestSlideshow::startsStopped()
{
    QVERIFY(!m_slideshow->isRunning());
    QVERIFY(!m_slideshow->isPaused());
    QCOMPARE(m_slideshow->intervalSeconds(), Slideshow::defaultIntervalSeconds);
}

void TestSlideshow::startUsesChosenOrderAndWrapsAround_data()
{
    QTest::addColumn<Slideshow::Order>("order");
    QTest::addColumn<SortOrder>("expectedSortOrder");

    QTest::newRow("by name") << Slideshow::Order::ByName << SortOrder::Name;
    QTest::newRow("by date") << Slideshow::Order::ByDate << SortOrder::Date;
    QTest::newRow("random") << Slideshow::Order::Random << SortOrder::Random;
}

void TestSlideshow::startUsesChosenOrderAndWrapsAround()
{
    QFETCH(Slideshow::Order, order);
    QFETCH(SortOrder, expectedSortOrder);
    openImageAndWaitForListing(folderImages.last());

    m_slideshow->start(order);
    if (order != Slideshow::Order::ByName)
    {
        waitForListing();
    }

    QVERIFY(m_slideshow->isRunning());
    QCOMPARE(m_folderScanner->lastSortOrder.load(), expectedSortOrder);
    QVERIFY(m_navigator->canGoToNext());  // at the last image, because of the wrap-around
}

void TestSlideshow::stopReturnsToNameOrder()
{
    openImageAndWaitForListing(folderImages.last());
    m_slideshow->start(Slideshow::Order::ByDate);
    waitForListing();

    m_slideshow->stop();
    waitForListing();

    QVERIFY(!m_slideshow->isRunning());
    QCOMPARE(m_folderScanner->lastSortOrder.load(), SortOrder::Name);
    QVERIFY(!m_navigator->canGoToNext());  // no wrap-around any more
}

void TestSlideshow::limitsInterval()
{
    m_slideshow->setIntervalSeconds(0);
    QCOMPARE(m_slideshow->intervalSeconds(), Slideshow::minimumIntervalSeconds);

    m_slideshow->setIntervalSeconds(100000);
    QCOMPARE(m_slideshow->intervalSeconds(), Slideshow::maximumIntervalSeconds);
}

void TestSlideshow::pauseAndResume()
{
    QSignalSpy stateSpy(m_slideshow.get(), &Slideshow::stateChanged);
    m_slideshow->start(Slideshow::Order::ByName);

    m_slideshow->pause();
    QVERIFY(m_slideshow->isRunning());
    QVERIFY(m_slideshow->isPaused());
    m_slideshow->resume();

    QVERIFY(!m_slideshow->isPaused());
    QCOMPARE(stateSpy.count(), 3);
}

void TestSlideshow::advancesAfterInterval()
{
    openImageAndWaitForListing(folderImages.first());
    m_slideshow->setIntervalSeconds(1);

    m_slideshow->start(Slideshow::Order::ByName);

    QTRY_COMPARE_WITH_TIMEOUT(m_navigator->currentIndex(), 1, 3 * oneSecondInMilliseconds);
}

void TestSlideshow::doesNotAdvanceWhilePaused()
{
    openImageAndWaitForListing(folderImages.first());
    m_slideshow->setIntervalSeconds(1);
    m_slideshow->start(Slideshow::Order::ByName);

    m_slideshow->pause();
    QTest::qWait(oneSecondInMilliseconds + oneSecondInMilliseconds / 2);

    QCOMPARE(m_navigator->currentIndex(), 0);
}

void TestSlideshow::openImageAndWaitForListing(const QString& filePath)
{
    QSignalSpy positionSpy(m_navigator.get(), &navigator::Navigator::positionChanged);
    m_navigator->openImage(filePath);
    QVERIFY(positionSpy.wait());
}

void TestSlideshow::waitForListing()
{
    QSignalSpy positionSpy(m_navigator.get(), &navigator::Navigator::positionChanged);
    QVERIFY(positionSpy.wait());
}

}  // namespace pictorio::slideshow

QTEST_GUILESS_MAIN(pictorio::slideshow::TestSlideshow)

#include "TestSlideshow.moc"
