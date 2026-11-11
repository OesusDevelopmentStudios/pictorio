/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "ImageLoader.hpp"

#include <KLocalizedString>

namespace pictorio::image_loader
{

// Defined here rather than in the header, so the class's virtual table is created in one place only.
ImageLoader::~ImageLoader() = default;

QString describeError(ImageLoadError error)
{
    switch (error)
    {
    case ImageLoadError::FileNotFound:
        return i18nc("@info", "The file does not exist.");
    case ImageLoadError::NotAFile:
        return i18nc("@info", "This is not a file.");
    case ImageLoadError::PermissionDenied:
        return i18nc("@info", "You do not have permission to read this file.");
    case ImageLoadError::UnsupportedFormat:
        return i18nc("@info", "This file is not an image in a supported format.");
    case ImageLoadError::InvalidImageData:
        return i18nc("@info", "The image file is damaged.");
    }

    return {};
}

}  // namespace pictorio::image_loader
