/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include <memory>

#include <QDir>
#include <QFile>
#include <QHash>
#include <QTemporaryDir>
#include <QTest>

#include "FolderScanner.hpp"
#include "FolderScannerFactory.hpp"

namespace
{

using pictorio::image_loader::ImageInfo;
using pictorio::image_loader::ImageLoader;
using pictorio::image_loader::ImageLoadError;

// Capture dates by file name, set up by the test. Read-only during a listing, so thread-safe.
class FakeImageLoader : public ImageLoader
{
public:
    std::expected<ImageInfo, ImageLoadError> loadInfo(const QString& /*filePath*/) const override
    {
        return std::unexpected(ImageLoadError::UnsupportedFormat);
    }

    QDateTime readCaptureDate(const QString& filePath) const override
    {
        return captureDates.value(QFileInfo(filePath).fileName());
    }

    QHash<QString, QDateTime> captureDates;
};

QDateTime dayInYear(int year)
{
    return QDateTime(QDate(year, 6, 1), QTime(12, 0));
}

}  // namespace

namespace pictorio::folder_scanner
{

// Images are recognized by extension, so the test files can be empty.
class TestFolderScanner : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();

    void listsOnlyImages();
    void recognizesUppercaseExtensions();
    void sortsNumbersNaturally();
    void ignoresCaseWhenSorting();
    void returnsAbsolutePaths();
    void returnsNothingForMissingFolder();

    void sortsByCaptureDate();
    void usesModificationTimeWithoutCaptureDate();
    void sortsEqualDatesByName();
    void shufflesTheSameWayForTheSameSeed();
    void shufflesDifferentlyForAnotherSeed();

private:
    void createFiles(const QStringList& relativePaths);
    void setModificationTime(const QString& fileName, const QDateTime& modificationTime);
    QStringList listedFileNames(const Sorting& sorting = {}) const;
    QStringList createNumberedFiles(int count);

    std::unique_ptr<QTemporaryDir> m_folder;
    std::unique_ptr<FolderScanner> m_folderScanner;
    FakeImageLoader* m_imageLoader = nullptr;  // owned by m_folderScanner
};

void TestFolderScanner::init()
{
    m_folder = std::make_unique<QTemporaryDir>();
    QVERIFY(m_folder->isValid());
    auto imageLoader = std::make_unique<FakeImageLoader>();
    m_imageLoader = imageLoader.get();
    m_folderScanner = createFolderScanner(std::move(imageLoader));
}

void TestFolderScanner::listsOnlyImages()
{
    createFiles({
        QStringLiteral("photo.jpg"),
        QStringLiteral("drawing.png"),
        QStringLiteral("notes.txt"),
        QStringLiteral("manual.pdf"),  // Qt can read PDF pages, but they are not images
        QStringLiteral(".hidden.png"),
        QStringLiteral("subfolder/nested.png"),
    });

    QCOMPARE(listedFileNames(), (QStringList{QStringLiteral("drawing.png"), QStringLiteral("photo.jpg")}));
}

void TestFolderScanner::recognizesUppercaseExtensions()
{
    createFiles({QStringLiteral("CAMERA.JPG")});

    QCOMPARE(listedFileNames(), QStringList{QStringLiteral("CAMERA.JPG")});
}

void TestFolderScanner::sortsNumbersNaturally()
{
    createFiles({QStringLiteral("img10.png"), QStringLiteral("img2.png"), QStringLiteral("img1.png")});

    QCOMPARE(listedFileNames(),
             (QStringList{QStringLiteral("img1.png"), QStringLiteral("img2.png"), QStringLiteral("img10.png")}));
}

void TestFolderScanner::ignoresCaseWhenSorting()
{
    createFiles({QStringLiteral("b.png"), QStringLiteral("C.png"), QStringLiteral("A.png")});

    QCOMPARE(listedFileNames(),
             (QStringList{QStringLiteral("A.png"), QStringLiteral("b.png"), QStringLiteral("C.png")}));
}

void TestFolderScanner::returnsAbsolutePaths()
{
    createFiles({QStringLiteral("photo.jpg")});

    const QStringList imagePaths = m_folderScanner->listImages(m_folder->path(), {});

    QCOMPARE(imagePaths, QStringList{QDir(m_folder->path()).absoluteFilePath(QStringLiteral("photo.jpg"))});
}

void TestFolderScanner::returnsNothingForMissingFolder()
{
    QVERIFY(m_folderScanner->listImages(m_folder->filePath(QStringLiteral("missing")), {}).isEmpty());
}

void TestFolderScanner::sortsByCaptureDate()
{
    createFiles({QStringLiteral("a.jpg"), QStringLiteral("b.jpg"), QStringLiteral("c.jpg")});
    m_imageLoader->captureDates = {
        {QStringLiteral("a.jpg"), dayInYear(2024)},
        {QStringLiteral("b.jpg"), dayInYear(2022)},
        {QStringLiteral("c.jpg"), dayInYear(2023)},
    };

    QCOMPARE(listedFileNames({SortOrder::Date}),
             (QStringList{QStringLiteral("b.jpg"), QStringLiteral("c.jpg"), QStringLiteral("a.jpg")}));
}

void TestFolderScanner::usesModificationTimeWithoutCaptureDate()
{
    createFiles({QStringLiteral("photo.jpg"), QStringLiteral("old-drawing.png"), QStringLiteral("new-drawing.png")});
    m_imageLoader->captureDates = {{QStringLiteral("photo.jpg"), dayInYear(2023)}};
    setModificationTime(QStringLiteral("photo.jpg"), dayInYear(2030));  // ignored: it has a capture date
    setModificationTime(QStringLiteral("old-drawing.png"), dayInYear(2021));
    setModificationTime(QStringLiteral("new-drawing.png"), dayInYear(2025));

    QCOMPARE(listedFileNames({SortOrder::Date}),
             (QStringList{
                 QStringLiteral("old-drawing.png"), QStringLiteral("photo.jpg"), QStringLiteral("new-drawing.png")}));
}

void TestFolderScanner::sortsEqualDatesByName()
{
    createFiles({QStringLiteral("img10.jpg"), QStringLiteral("img2.jpg"), QStringLiteral("img1.jpg")});
    m_imageLoader->captureDates = {
        {QStringLiteral("img10.jpg"), dayInYear(2024)},
        {QStringLiteral("img2.jpg"), dayInYear(2024)},
        {QStringLiteral("img1.jpg"), dayInYear(2024)},
    };

    QCOMPARE(listedFileNames({SortOrder::Date}),
             (QStringList{QStringLiteral("img1.jpg"), QStringLiteral("img2.jpg"), QStringLiteral("img10.jpg")}));
}

void TestFolderScanner::shufflesTheSameWayForTheSameSeed()
{
    const QStringList nameOrder = createNumberedFiles(20);

    const QStringList shuffled = listedFileNames({SortOrder::Random, 42});

    QCOMPARE(listedFileNames({SortOrder::Random, 42}), shuffled);
    QVERIFY(shuffled != nameOrder);
    QStringList sortedBack = shuffled;
    sortedBack.sort();
    QStringList sortedNames = nameOrder;
    sortedNames.sort();
    QCOMPARE(sortedBack, sortedNames);  // the same files, only in another order
}

void TestFolderScanner::shufflesDifferentlyForAnotherSeed()
{
    createNumberedFiles(20);

    QVERIFY(listedFileNames({SortOrder::Random, 1}) != listedFileNames({SortOrder::Random, 2}));
}

void TestFolderScanner::createFiles(const QStringList& relativePaths)
{
    for (const QString& relativePath : relativePaths)
    {
        const QString filePath = m_folder->filePath(relativePath);
        QDir().mkpath(QFileInfo(filePath).path());
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly))
        {
            qFatal("Could not create test file %s", qPrintable(filePath));
        }
    }
}

void TestFolderScanner::setModificationTime(const QString& fileName, const QDateTime& modificationTime)
{
    QFile file(m_folder->filePath(fileName));
    if (!file.open(QIODevice::ReadWrite) || !file.setFileTime(modificationTime, QFileDevice::FileModificationTime))
    {
        qFatal("Could not set the modification time of %s", qPrintable(file.fileName()));
    }
}

// Returns the file names in name order.
QStringList TestFolderScanner::createNumberedFiles(int count)
{
    QStringList fileNames;
    for (int number = 1; number <= count; ++number)
    {
        fileNames.append(QStringLiteral("img%1.jpg").arg(number));
    }

    createFiles(fileNames);
    return fileNames;
}

QStringList TestFolderScanner::listedFileNames(const Sorting& sorting) const
{
    QStringList fileNames;
    for (const QString& imagePath : m_folderScanner->listImages(m_folder->path(), sorting))
    {
        fileNames.append(QFileInfo(imagePath).fileName());
    }

    return fileNames;
}

}  // namespace pictorio::folder_scanner

QTEST_GUILESS_MAIN(pictorio::folder_scanner::TestFolderScanner)

#include "TestFolderScanner.moc"
