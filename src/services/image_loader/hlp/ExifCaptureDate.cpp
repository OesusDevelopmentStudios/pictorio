/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "hlp/ExifCaptureDate.hpp"

#include <array>
#include <exception>

#include <QFile>

#include <exiv2/exiv2.hpp>

namespace
{

// Tried in this order. Exif.Image.DateTime is left out: it is when the file was last edited.
constexpr std::array captureDateKeys{"Exif.Photo.DateTimeOriginal", "Exif.Photo.DateTimeDigitized"};

// EXIF date format, e.g. "2024:07:14 18:30:05"
const QString exifDateFormat = QStringLiteral("yyyy:MM:dd HH:mm:ss");

QDateTime parseExifDate(const std::string& value)
{
    return QDateTime::fromString(QString::fromStdString(value).trimmed(), exifDateFormat);
}

QDateTime findCaptureDate(const Exiv2::ExifData& exifData)
{
    for (const char* key : captureDateKeys)
    {
        const auto entry = exifData.findKey(Exiv2::ExifKey(key));
        if (entry == exifData.end())
        {
            continue;
        }

        const QDateTime captureDate = parseExifDate(entry->toString());
        if (captureDate.isValid())
        {
            return captureDate;
        }
    }

    return {};
}

// exiv2 prints warnings about unusual metadata to stderr; a viewer has no use for them.
void muteExiv2WarningsOnce()
{
    static const bool muted = [] {
        Exiv2::LogMsg::setLevel(Exiv2::LogMsg::mute);
        return true;
    }();

    Q_UNUSED(muted)
}

}  // namespace

namespace pictorio::image_loader
{

QDateTime readExifCaptureDate(const QString& filePath)
{
    muteExiv2WarningsOnce();

    try
    {
        const auto image = Exiv2::ImageFactory::open(QFile::encodeName(filePath).toStdString());
        image->readMetadata();
        return findCaptureDate(image->exifData());
    }
    catch (const std::exception&)
    {
        return {};  // not a format exiv2 knows, or damaged metadata
    }
}

}  // namespace pictorio::image_loader
