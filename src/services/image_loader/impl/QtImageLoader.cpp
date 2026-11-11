/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "impl/QtImageLoader.hpp"

#include <optional>

#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include <QMimeDatabase>

#include "hlp/ExifCaptureDate.hpp"

namespace
{

using pictorio::image_loader::ImageLoadError;

// Returns why the file cannot be read, or nothing if it can.
std::optional<ImageLoadError> findFileAccessProblem(const QFileInfo& fileInfo)
{
    if (!fileInfo.exists())
    {
        return ImageLoadError::FileNotFound;
    }

    if (!fileInfo.isFile())
    {
        return ImageLoadError::NotAFile;
    }

    if (!fileInfo.isReadable())
    {
        return ImageLoadError::PermissionDenied;
    }

    return std::nullopt;
}

QSize readStoredSize(QImageReader& reader)
{
    const QSize sizeFromHeader = reader.size();
    if (!sizeFromHeader.isEmpty())
    {
        return sizeFromHeader;
    }

    const QImage decodedImage = reader.read();
    if (decodedImage.isNull())
    {
        return {};
    }

    return decodedImage.size();
}

// Rotating by 90° or 270° swaps width and height. Both rotations include the Rotate90 flag.
QSize applyTransformationToSize(const QSize& storedSize, QImageIOHandler::Transformations transformation)
{
    const bool swapsWidthAndHeight = transformation.testFlag(QImageIOHandler::TransformationRotate90);
    return swapsWidthAndHeight ? storedSize.transposed() : storedSize;
}

}  // namespace

namespace pictorio::image_loader
{

std::expected<ImageInfo, ImageLoadError> QtImageLoader::loadInfo(const QString& filePath) const
{
    const QFileInfo fileInfo(filePath);
    if (const auto accessProblem = findFileAccessProblem(fileInfo))
    {
        return std::unexpected(*accessProblem);
    }

    ImageInfo imageInfo;
    imageInfo.filePath = fileInfo.absoluteFilePath();

    QImageReader reader(imageInfo.filePath);
    reader.setDecideFormatFromContent(true);  // file names can be missing or have the wrong extension
    reader.setAutoTransform(false);           // we want the size as stored; the orientation is applied below

    if (!reader.canRead())
    {
        return std::unexpected(ImageLoadError::UnsupportedFormat);
    }

    const QSize storedSize = readStoredSize(reader);
    if (storedSize.isEmpty())
    {
        return std::unexpected(ImageLoadError::InvalidImageData);
    }

    imageInfo.exifTransformation = reader.transformation();
    imageInfo.displaySize = applyTransformationToSize(storedSize, imageInfo.exifTransformation);
    imageInfo.mimeType = QMimeDatabase().mimeTypeForFile(imageInfo.filePath, QMimeDatabase::MatchContent).name();
    imageInfo.animated = reader.supportsAnimation() && reader.imageCount() > 1;
    imageInfo.captureDate = readCaptureDate(imageInfo.filePath);

    return imageInfo;
}

QDateTime QtImageLoader::readCaptureDate(const QString& filePath) const
{
    return readExifCaptureDate(filePath);
}

}  // namespace pictorio::image_loader
