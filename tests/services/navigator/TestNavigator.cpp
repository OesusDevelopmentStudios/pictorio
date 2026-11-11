/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include <atomic>
#include <memory>

#include <QHash>
#include <QSemaphore>
#include <QSignalSpy>
#include <QTest>

#include "Navigator.hpp"
#include "NavigatorFactory.hpp"

namespace
{

using pictorio::folder_scanner::FolderScanner;

constexpr int openGate = 1000;

// Returns the listing the test set for a folder. Each listing first passes the gate, so a test can
// hold listings back and decide when they finish. Called from a background thread.
class FakeFolderScanner : public FolderScanner
{
public:
    QStringList listImages(const QString& folderPath) const override
    {
        gate.acquire();
        ++listingCount;
        return listings.value(folderPath);
    }

    mutable QSemaphore gate{openGate};
    QHash<QString, QStringList> listings;  // set up before any listing starts
    mutable std::atomic<int> listingCount = 0;
};

const QString holidayFolder = QStringLiteral("/photos/holiday");
const QStringList holidayImages{
    QStringLiteral("/photos/holiday/beach.jpg"),
    QStringLiteral("/photos/holiday/hotel.jpg"),
    QStringLiteral("/photos/holiday/sunset.jpg"),
};

const QString petsFolder = QStringLiteral("/photos/pets");
const QStringList petImages{QStringLiteral("/photos/pets/cat.jpg"), QStringLiteral("/photos/pets/dog.jpg")};

}  // namespace

namespace pictorio::navigator
{

class TestNavigator : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();
    void cleanup();

    void startsWithoutPosition();
    void requestsOpenedImageAtOnce();
    void findsPositionWhenFolderIsListed();
    void goesToNextAndPrevious();
    void stopsAtEnds();
    void disablesNavigationForImageNotInListing();
    void doesNotListSameFolderAgain();
    void ignoresListingOfPreviousFolder();

private:
    void openAndWaitForListing(const QString& filePath);

    std::unique_ptr<Navigator> m_navigator;
    FakeFolderScanner* m_folderScanner = nullptr;  // owned by m_navigator
};

void TestNavigator::init()
{
    auto folderScanner = std::make_unique<FakeFolderScanner>();
    folderScanner->listings.insert(holidayFolder, holidayImages);
    folderScanner->listings.insert(petsFolder, petImages);
    m_folderScanner = folderScanner.get();
    m_navigator = createNavigator(std::move(folderScanner));
}

void TestNavigator::cleanup()
{
    m_folderScanner->gate.release(openGate);  // never leave a background listing blocked
}

void TestNavigator::startsWithoutPosition()
{
    QCOMPARE(m_navigator->currentIndex(), -1);
    QCOMPARE(m_navigator->imageCount(), 0);
    QVERIFY(!m_navigator->canGoToPrevious());
    QVERIFY(!m_navigator->canGoToNext());
}

void TestNavigator::requestsOpenedImageAtOnce()
{
    m_folderScanner->gate.acquire(openGate);  // the listing does not finish
    QSignalSpy requestSpy(m_navigator.get(), &Navigator::imageRequested);

    m_navigator->openImage(QStringLiteral("/photos/holiday/./hotel.jpg"));

    QCOMPARE(requestSpy.count(), 1);
    QCOMPARE(requestSpy.first().first().toString(), QStringLiteral("/photos/holiday/hotel.jpg"));
    QCOMPARE(m_navigator->currentIndex(), -1);
    QVERIFY(!m_navigator->canGoToNext());
}

void TestNavigator::findsPositionWhenFolderIsListed()
{
    openAndWaitForListing(QStringLiteral("/photos/holiday/hotel.jpg"));

    QCOMPARE(m_navigator->currentIndex(), 1);
    QCOMPARE(m_navigator->imageCount(), 3);
    QVERIFY(m_navigator->canGoToPrevious());
    QVERIFY(m_navigator->canGoToNext());
}

void TestNavigator::goesToNextAndPrevious()
{
    openAndWaitForListing(QStringLiteral("/photos/holiday/hotel.jpg"));
    QSignalSpy requestSpy(m_navigator.get(), &Navigator::imageRequested);

    m_navigator->goToNext();
    m_navigator->goToPrevious();
    m_navigator->goToPrevious();

    QCOMPARE(requestSpy.count(), 3);
    QCOMPARE(requestSpy.at(0).first().toString(), holidayImages.at(2));
    QCOMPARE(requestSpy.at(1).first().toString(), holidayImages.at(1));
    QCOMPARE(requestSpy.at(2).first().toString(), holidayImages.at(0));
    QCOMPARE(m_navigator->currentIndex(), 0);
}

void TestNavigator::stopsAtEnds()
{
    openAndWaitForListing(holidayImages.first());
    QSignalSpy requestSpy(m_navigator.get(), &Navigator::imageRequested);

    m_navigator->goToPrevious();
    QVERIFY(!m_navigator->canGoToPrevious());
    m_navigator->goToNext();
    m_navigator->goToNext();
    m_navigator->goToNext();

    QCOMPARE(requestSpy.count(), 2);  // previous at the start and next at the end did nothing
    QCOMPARE(m_navigator->currentIndex(), 2);
    QVERIFY(!m_navigator->canGoToNext());
}

void TestNavigator::disablesNavigationForImageNotInListing()
{
    openAndWaitForListing(QStringLiteral("/photos/holiday/scan"));  // no extension, so not listed

    QCOMPARE(m_navigator->currentIndex(), -1);
    QCOMPARE(m_navigator->imageCount(), 3);
    QVERIFY(!m_navigator->canGoToPrevious());
    QVERIFY(!m_navigator->canGoToNext());
}

void TestNavigator::doesNotListSameFolderAgain()
{
    openAndWaitForListing(holidayImages.first());

    m_navigator->openImage(holidayImages.last());

    QCOMPARE(m_navigator->currentIndex(), 2);
    QCOMPARE(m_folderScanner->listingCount.load(), 1);
}

void TestNavigator::ignoresListingOfPreviousFolder()
{
    m_folderScanner->gate.acquire(openGate);
    m_navigator->openImage(holidayImages.first());
    m_navigator->openImage(petImages.last());
    QSignalSpy positionSpy(m_navigator.get(), &Navigator::positionChanged);

    m_folderScanner->gate.release(2);  // both listings finish, in either order

    QTRY_COMPARE(m_folderScanner->listingCount.load(), 2);
    QTRY_COMPARE(m_navigator->imageCount(), 2);
    QCOMPARE(m_navigator->currentIndex(), 1);
    QCOMPARE(positionSpy.count(), 1);
}

void TestNavigator::openAndWaitForListing(const QString& filePath)
{
    QSignalSpy positionSpy(m_navigator.get(), &Navigator::positionChanged);
    m_navigator->openImage(filePath);
    QVERIFY(positionSpy.wait());
}

}  // namespace pictorio::navigator

QTEST_GUILESS_MAIN(pictorio::navigator::TestNavigator)

#include "TestNavigator.moc"
