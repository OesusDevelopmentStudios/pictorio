/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include <memory>

#include <KLocalizedString>
#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QImageWriter>
#include <QTemporaryDir>
#include <QTest>

#include <exiv2/exiv2.hpp>

#include "ImageLoader.hpp"
#include "ImageLoaderFactory.hpp"

namespace
{

// Builds a minimal EXIF block (a JPEG "APP1" segment) that contains only the orientation tag.
// Qt's JPEG writer cannot store an orientation, so the tests add it themselves.
// Orientation values (EXIF tag 0x0112): 1 = normal, 3 = rotate 180°, 6 = rotate 90° clockwise,
// 8 = rotate 270° clockwise.
QByteArray exifSegmentWithOrientation(quint8 orientation)
{
    QByteArray segment;
    segment += QByteArray::fromHex("FFE1");              // APP1 marker
    segment += QByteArray::fromHex("0022");              // segment length: 34 bytes, marker not counted
    segment += QByteArray::fromHex("457869660000");      // "Exif" followed by two zero bytes
    segment += QByteArray::fromHex("4D4D002A00000008");  // TIFF header: big-endian, 42, directory at offset 8
    segment += QByteArray::fromHex("0001");              // the directory has one entry:
    segment += QByteArray::fromHex("0112000300000001");  //   tag 0112 (Orientation), type 0003 (SHORT), 1 value
    segment += QByteArray::fromHex("00");                //   value, high byte
    segment += static_cast<char>(orientation);           //   value, low byte
    segment += QByteArray::fromHex("0000");              //   padding to 4 bytes
    segment += QByteArray::fromHex("00000000");          // no further directory
    return segment;
}

// A 1 x 1 GIF with the given number of frames. Built by hand because Qt cannot write GIF files.
QByteArray gifWithFrames(int frameCount)
{
    QByteArray gif;
    gif += QByteArray::fromHex("474946383961");  // "GIF89a"
    gif += QByteArray::fromHex("01000100");      // width 1, height 1
    gif += QByteArray::fromHex("800000");        // global color table with 2 colors follows
    gif += QByteArray::fromHex("000000FFFFFF");  // the colors: black, white
    for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex)
    {
        gif += QByteArray::fromHex("21F904000A0000");      // graphic control extension: show for 0.1 s
        gif += QByteArray::fromHex("00");                  //   end of extension
        gif += QByteArray::fromHex("2C0000000001000100");  // image descriptor: 1 x 1 at position 0, 0
        gif += QByteArray::fromHex("00");                  //   no local color table
        gif += QByteArray::fromHex("0202440100");          // LZW data: one black pixel
    }

    gif += QByteArray::fromHex("3B");  // end of file
    return gif;
}

}  // namespace

namespace pictorio::image_loader
{

// All test images are generated here, in a temporary folder, so the tests need no image files in git.
class TestImageLoader : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void readsSizeAndTypeOfPng();
    void returnsAbsolutePath();
    void detectsTypeFromContentWhenExtensionIsWrong();
    void detectsTypeFromContentWhenExtensionIsMissing();
    void appliesExifOrientation_data();
    void appliesExifOrientation();
    void detectsAnimation_data();
    void detectsAnimation();
    void readsCaptureDate_data();
    void readsCaptureDate();

    void reportsMissingFile();
    void reportsFolderAsNotAFile();
    void reportsUnreadableFile();
    void reportsNonImageAsUnsupported();
    void reportsDamagedImageAsInvalid();

    void describesEveryError();

private:
    QString pathInTemporaryFolder(const QString& fileName) const;
    QString writeTestImage(const QString& fileName, const char* format, QSize storedSize);
    QString writeJpegWithExifOrientation(const QString& fileName, QSize storedSize, quint8 exifOrientation);
    QString writeFile(const QString& fileName, const QByteArray& content);
    void writeExifTags(const QString& imagePath, const QList<std::pair<QString, QString>>& tags);

    QTemporaryDir m_temporaryFolder;
    std::unique_ptr<ImageLoader> m_imageLoader;
};

void TestImageLoader::initTestCase()
{
    KLocalizedString::setApplicationDomain("pictorio");

    QVERIFY(m_temporaryFolder.isValid());
    m_imageLoader = createImageLoader();
}

void TestImageLoader::readsSizeAndTypeOfPng()
{
    const QString imagePath = writeTestImage(QStringLiteral("landscape.png"), "PNG", QSize(40, 20));

    const auto result = m_imageLoader->loadInfo(imagePath);

    QVERIFY(result.has_value());
    QCOMPARE(result->displaySize, QSize(40, 20));
    QCOMPARE(result->mimeType, QStringLiteral("image/png"));
    QCOMPARE(result->exifTransformation, QImageIOHandler::TransformationNone);
    QVERIFY(!result->animated);
    QVERIFY(!result->captureDate.isValid());
}

void TestImageLoader::returnsAbsolutePath()
{
    const QString absolutePath = writeTestImage(QStringLiteral("relative.png"), "PNG", QSize(10, 10));
    const QString relativePath = QDir::current().relativeFilePath(absolutePath);

    const auto result = m_imageLoader->loadInfo(relativePath);

    QVERIFY(result.has_value());
    QCOMPARE(result->filePath, absolutePath);
}

void TestImageLoader::detectsTypeFromContentWhenExtensionIsWrong()
{
    const QString imagePath = writeTestImage(QStringLiteral("really_a_png.jpg"), "PNG", QSize(30, 10));

    const auto result = m_imageLoader->loadInfo(imagePath);

    QVERIFY(result.has_value());
    QCOMPARE(result->mimeType, QStringLiteral("image/png"));
    QCOMPARE(result->displaySize, QSize(30, 10));
}

void TestImageLoader::detectsTypeFromContentWhenExtensionIsMissing()
{
    const QString imagePath = writeTestImage(QStringLiteral("no_extension"), "JPEG", QSize(16, 8));

    const auto result = m_imageLoader->loadInfo(imagePath);

    QVERIFY(result.has_value());
    QCOMPARE(result->mimeType, QStringLiteral("image/jpeg"));
    QCOMPARE(result->displaySize, QSize(16, 8));
}

void TestImageLoader::appliesExifOrientation_data()
{
    QTest::addColumn<int>("exifOrientation");
    QTest::addColumn<int>("expectedTransformation");
    QTest::addColumn<QSize>("expectedDisplaySize");

    // Every test image is stored as 40 x 20 (landscape).
    QTest::newRow("normal") << 1 << int(QImageIOHandler::TransformationNone) << QSize(40, 20);
    QTest::newRow("rotate 180") << 3 << int(QImageIOHandler::TransformationRotate180) << QSize(40, 20);
    QTest::newRow("rotate 90 (camera held upright)")
        << 6 << int(QImageIOHandler::TransformationRotate90) << QSize(20, 40);
    QTest::newRow("rotate 270") << 8 << int(QImageIOHandler::TransformationRotate270) << QSize(20, 40);
}

void TestImageLoader::appliesExifOrientation()
{
    QFETCH(int, exifOrientation);
    QFETCH(int, expectedTransformation);
    QFETCH(QSize, expectedDisplaySize);

    const QString imagePath = writeJpegWithExifOrientation(
        QStringLiteral("orientation_%1.jpg").arg(exifOrientation), QSize(40, 20), quint8(exifOrientation));

    const auto result = m_imageLoader->loadInfo(imagePath);

    QVERIFY(result.has_value());
    QCOMPARE(int(result->exifTransformation), expectedTransformation);
    QCOMPARE(result->displaySize, expectedDisplaySize);
}

void TestImageLoader::detectsAnimation_data()
{
    QTest::addColumn<int>("frameCount");
    QTest::addColumn<bool>("expectedAnimated");

    QTest::newRow("single frame") << 1 << false;
    QTest::newRow("two frames") << 2 << true;
}

void TestImageLoader::detectsAnimation()
{
    QFETCH(int, frameCount);
    QFETCH(bool, expectedAnimated);
    const QString imagePath = writeFile(QStringLiteral("frames.gif"), gifWithFrames(frameCount));

    const auto result = m_imageLoader->loadInfo(imagePath);

    QVERIFY(result.has_value());
    QCOMPARE(result->mimeType, QStringLiteral("image/gif"));
    QCOMPARE(result->displaySize, QSize(1, 1));
    QCOMPARE(result->animated, expectedAnimated);
}

void TestImageLoader::readsCaptureDate_data()
{
    // EXIF values; an empty string leaves the tag out
    QTest::addColumn<QString>("originalDate");
    QTest::addColumn<QString>("digitizedDate");
    QTest::addColumn<QString>("editDate");
    QTest::addColumn<QDateTime>("expectedCaptureDate");

    const QDateTime summerEvening(QDate(2024, 7, 14), QTime(18, 30, 5));
    const QDateTime winterMorning(QDate(2023, 1, 2), QTime(8, 0, 0));

    QTest::newRow("original") << QStringLiteral("2024:07:14 18:30:05") << QString() << QString() << summerEvening;
    QTest::newRow("digitized only") << QString() << QStringLiteral("2024:07:14 18:30:05") << QString() << summerEvening;
    QTest::newRow("original wins") << QStringLiteral("2024:07:14 18:30:05") << QStringLiteral("2023:01:02 08:00:00")
                                   << QString() << summerEvening;
    QTest::newRow("unset camera clock") << QStringLiteral("0000:00:00 00:00:00") << QString() << QString()
                                        << QDateTime();
    QTest::newRow("only an edit date") << QString() << QString() << QStringLiteral("2023:01:02 08:00:00")
                                       << QDateTime();
    QTest::newRow("unset original, digitized set")
        << QStringLiteral("0000:00:00 00:00:00") << QStringLiteral("2023:01:02 08:00:00") << QString() << winterMorning;
}

void TestImageLoader::readsCaptureDate()
{
    QFETCH(QString, originalDate);
    QFETCH(QString, digitizedDate);
    QFETCH(QString, editDate);
    QFETCH(QDateTime, expectedCaptureDate);
    const QString imagePath = writeTestImage(QStringLiteral("dated.jpg"), "JPEG", QSize(10, 10));
    writeExifTags(imagePath,
                  {
                      {QStringLiteral("Exif.Photo.DateTimeOriginal"), originalDate},
                      {QStringLiteral("Exif.Photo.DateTimeDigitized"), digitizedDate},
                      {QStringLiteral("Exif.Image.DateTime"), editDate},
                  });

    const auto result = m_imageLoader->loadInfo(imagePath);

    QVERIFY(result.has_value());
    QCOMPARE(result->captureDate, expectedCaptureDate);
}

void TestImageLoader::reportsMissingFile()
{
    const auto result = m_imageLoader->loadInfo(pathInTemporaryFolder(QStringLiteral("does_not_exist.png")));

    QVERIFY(!result.has_value());
    QCOMPARE(result.error(), ImageLoadError::FileNotFound);
}

void TestImageLoader::reportsFolderAsNotAFile()
{
    const auto result = m_imageLoader->loadInfo(m_temporaryFolder.path());

    QVERIFY(!result.has_value());
    QCOMPARE(result.error(), ImageLoadError::NotAFile);
}

void TestImageLoader::reportsUnreadableFile()
{
    const QString imagePath = writeTestImage(QStringLiteral("unreadable.png"), "PNG", QSize(10, 10));
    QFile::setPermissions(imagePath, QFileDevice::WriteOwner);
    if (QFileInfo(imagePath).isReadable())
    {
        QSKIP("File permissions are not enforced here (e.g. running as root).");
    }

    const auto result = m_imageLoader->loadInfo(imagePath);

    QVERIFY(!result.has_value());
    QCOMPARE(result.error(), ImageLoadError::PermissionDenied);
}

void TestImageLoader::reportsNonImageAsUnsupported()
{
    const QString textPath = writeFile(QStringLiteral("notes.png"), QByteArrayLiteral("Just some text."));

    const auto result = m_imageLoader->loadInfo(textPath);

    QVERIFY(!result.has_value());
    QCOMPARE(result.error(), ImageLoadError::UnsupportedFormat);
}

void TestImageLoader::reportsDamagedImageAsInvalid()
{
    // A valid PNG signature followed by garbage: the format is recognised, but the data cannot be read.
    const QByteArray pngSignature = QByteArray::fromHex("89504E470D0A1A0A");
    const QString damagedPath = writeFile(QStringLiteral("damaged.png"), pngSignature + QByteArray(64, 'x'));

    const auto result = m_imageLoader->loadInfo(damagedPath);

    QVERIFY(!result.has_value());
    QCOMPARE(result.error(), ImageLoadError::InvalidImageData);
}

void TestImageLoader::describesEveryError()
{
    const ImageLoadError allErrors[] = {
        ImageLoadError::FileNotFound,
        ImageLoadError::NotAFile,
        ImageLoadError::PermissionDenied,
        ImageLoadError::UnsupportedFormat,
        ImageLoadError::InvalidImageData,
    };

    for (const ImageLoadError error : allErrors)
    {
        QVERIFY2(!describeError(error).isEmpty(), "every error needs a message the user can read");
    }
}

QString TestImageLoader::pathInTemporaryFolder(const QString& fileName) const
{
    return m_temporaryFolder.filePath(fileName);
}

QString TestImageLoader::writeTestImage(const QString& fileName, const char* format, QSize storedSize)
{
    QImage image(storedSize, QImage::Format_RGB32);
    image.fill(Qt::darkCyan);

    const QString imagePath = pathInTemporaryFolder(fileName);
    QImageWriter writer(imagePath, format);
    if (!writer.write(image))
    {
        qFatal("Could not write test image %s: %s", qPrintable(imagePath), qPrintable(writer.errorString()));
    }

    return imagePath;
}

QString TestImageLoader::writeJpegWithExifOrientation(const QString& fileName, QSize storedSize, quint8 exifOrientation)
{
    QImage image(storedSize, QImage::Format_RGB32);
    image.fill(Qt::darkCyan);

    QByteArray jpegData;
    QBuffer jpegBuffer(&jpegData);
    jpegBuffer.open(QIODevice::WriteOnly);
    if (!image.save(&jpegBuffer, "JPEG"))
    {
        qFatal("Could not encode test JPEG");
    }

    // A JPEG file starts with the 2-byte "start of image" marker; the EXIF segment goes right after it.
    const int startOfImageMarkerLength = 2;
    jpegData.insert(startOfImageMarkerLength, exifSegmentWithOrientation(exifOrientation));

    return writeFile(fileName, jpegData);
}

// Uses exiv2 to add the tags; tags with an empty value are left out.
void TestImageLoader::writeExifTags(const QString& imagePath, const QList<std::pair<QString, QString>>& tags)
{
    const auto image = Exiv2::ImageFactory::open(QFile::encodeName(imagePath).toStdString());
    image->readMetadata();
    Exiv2::ExifData& exifData = image->exifData();
    for (const auto& [key, value] : tags)
    {
        if (!value.isEmpty())
        {
            exifData[key.toStdString()] = value.toStdString();
        }
    }

    image->writeMetadata();
}

QString TestImageLoader::writeFile(const QString& fileName, const QByteArray& content)
{
    const QString filePath = pathInTemporaryFolder(fileName);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly) || file.write(content) != content.size())
    {
        qFatal("Could not write test file %s", qPrintable(filePath));
    }

    return filePath;
}

}  // namespace pictorio::image_loader

QTEST_GUILESS_MAIN(pictorio::image_loader::TestImageLoader)

#include "TestImageLoader.moc"
