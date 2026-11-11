/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include <memory>

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTest>

#include "FolderScanner.hpp"
#include "FolderScannerFactory.hpp"

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

private:
    void createFiles(const QStringList& relativePaths);
    QStringList listedFileNames() const;

    std::unique_ptr<QTemporaryDir> m_folder;
    std::unique_ptr<FolderScanner> m_folderScanner;
};

void TestFolderScanner::init()
{
    m_folder = std::make_unique<QTemporaryDir>();
    QVERIFY(m_folder->isValid());
    m_folderScanner = createFolderScanner();
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

    const QStringList imagePaths = m_folderScanner->listImages(m_folder->path());

    QCOMPARE(imagePaths, QStringList{QDir(m_folder->path()).absoluteFilePath(QStringLiteral("photo.jpg"))});
}

void TestFolderScanner::returnsNothingForMissingFolder()
{
    QVERIFY(m_folderScanner->listImages(m_folder->filePath(QStringLiteral("missing"))).isEmpty());
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

QStringList TestFolderScanner::listedFileNames() const
{
    QStringList fileNames;
    for (const QString& imagePath : m_folderScanner->listImages(m_folder->path()))
    {
        fileNames.append(QFileInfo(imagePath).fileName());
    }

    return fileNames;
}

}  // namespace pictorio::folder_scanner

QTEST_GUILESS_MAIN(pictorio::folder_scanner::TestFolderScanner)

#include "TestFolderScanner.moc"
