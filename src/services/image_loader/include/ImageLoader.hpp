/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <expected>

#include <QByteArray>
#include <QImageIOHandler>
#include <QSize>
#include <QString>

namespace pictorio::image_loader
{

// What the viewer needs to know about an image file before it displays it.
struct ImageInfo
{
    // Absolute, cleaned-up path of the file
    QString filePath;

    // Size as the user sees it, i.e. after applying the EXIF orientation:
    // a portrait photo stored sideways reports a portrait size here.
    QSize displaySize;

    // Rotation and/or mirroring stored in the file's EXIF data. The viewer must apply it.
    QImageIOHandler::Transformations exifTransformation = QImageIOHandler::TransformationNone;

    // Detected from the file content, not from the file name. Example: "image/jpeg"
    QString mimeType;
};

// Why an image could not be loaded. Use describeError() for a message that can be shown to the user.
enum class ImageLoadError
{
    FileNotFound,
    NotAFile,  // e.g. a folder
    PermissionDenied,
    UnsupportedFormat,  // not an image, or an image format this system cannot decode
    InvalidImageData,   // the format is known but the file is damaged
};

// Reads the information the viewer needs from an image file. Does not decode the pixels.
class ImageLoader
{
public:
    virtual ~ImageLoader();

    virtual std::expected<ImageInfo, ImageLoadError> loadInfo(const QString& filePath) const = 0;
};

// Translated, user-facing explanation of a load error.
QString describeError(ImageLoadError error);

}  // namespace pictorio::image_loader
