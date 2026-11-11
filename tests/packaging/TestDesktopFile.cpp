/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include <memory>

#include <KLocalizedString>
#include <QFile>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QMimeDatabase>
#include <QTemporaryDir>
#include <QTest>

#include "ImageLoader.hpp"
#include "ImageLoaderFactory.hpp"

namespace
{

// Formats whose plugin depends on codecs not every system has. HEIF needs an HEVC decoder,
// which some distributions leave out for patent reasons (the Flatpak runtime has one).
const QStringList optionalMimeTypes{QStringLiteral("image/heif")};

QStringList mimeTypesInDesktopFile()
{
    QFile desktopFile(QStringLiteral(DESKTOP_FILE));
    if (!desktopFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qFatal("Cannot open %s", DESKTOP_FILE);
    }

    const QString mimeTypeKey = QStringLiteral("MimeType=");
    while (!desktopFile.atEnd())
    {
        const QString line = QString::fromUtf8(desktopFile.readLine()).trimmed();
        if (line.startsWith(mimeTypeKey))
        {
            return line.mid(mimeTypeKey.size()).split(QLatin1Char(';'), Qt::SkipEmptyParts);
        }
    }

    return {};
}

bool isReadable(const QString& mimeType)
{
    return QImageReader::supportedMimeTypes().contains(mimeType.toUtf8());
}

}  // namespace

namespace pictorio::image_loader
{

// Checks that the app can open every image type it registers for in the .desktop file.
class TestDesktopFile : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void listsImageTypes();
    void opensEveryListedImageType_data();
    void opensEveryListedImageType();

private:
    std::unique_ptr<ImageLoader> m_imageLoader;
    QTemporaryDir m_temporaryDir;
};

void TestDesktopFile::initTestCase()
{
    KLocalizedString::setApplicationDomain("pictorio");
    QVERIFY(m_temporaryDir.isValid());
    m_imageLoader = createImageLoader();
}

void TestDesktopFile::listsImageTypes()
{
    QVERIFY(mimeTypesInDesktopFile().contains(QStringLiteral("image/jpeg")));
}

void TestDesktopFile::opensEveryListedImageType_data()
{
    QTest::addColumn<QString>("mimeType");

    for (const QString& mimeType : mimeTypesInDesktopFile())
    {
        QTest::newRow(qPrintable(mimeType)) << mimeType;
    }
}

void TestDesktopFile::opensEveryListedImageType()
{
    QFETCH(QString, mimeType);
    if (!isReadable(mimeType) && optionalMimeTypes.contains(mimeType))
    {
        QSKIP("The image plugin for this optional type is not available on this system");
    }

    QVERIFY2(isReadable(mimeType), "No Qt image plugin reads this type");

    // Round trip through ImageLoader, for types Qt can also write.
    const QList<QByteArray> writerFormats = QImageWriter::imageFormatsForMimeType(mimeType.toUtf8());
    if (writerFormats.isEmpty())
    {
        QSKIP("Qt cannot write this type, so no test image can be generated");
    }

    const QString suffix = QMimeDatabase().mimeTypeForName(mimeType).preferredSuffix();
    QImageWriter writer(m_temporaryDir.filePath(QStringLiteral("image.") + suffix), writerFormats.first());
    QImage image(QSize(8, 6), QImage::Format_RGB32);
    image.fill(Qt::darkCyan);
    QVERIFY2(writer.write(image), qPrintable(writer.errorString()));

    const auto result = m_imageLoader->loadInfo(writer.fileName());

    if (!result.has_value())
    {
        QFAIL(qPrintable(describeError(result.error())));
    }

    QCOMPARE(result->displaySize, QSize(8, 6));
    QCOMPARE(result->mimeType, mimeType);
}

}  // namespace pictorio::image_loader

QTEST_GUILESS_MAIN(pictorio::image_loader::TestDesktopFile)

#include "TestDesktopFile.moc"
