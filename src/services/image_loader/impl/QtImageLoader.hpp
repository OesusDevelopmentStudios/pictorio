/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include "ImageLoader.hpp"

namespace pictorio::image_loader
{

// ImageLoader based on QImageReader, so it supports every format Qt's image plugins support.
class QtImageLoader : public ImageLoader
{
public:
    std::expected<ImageInfo, ImageLoadError> loadInfo(const QString& filePath) const override;
    QDateTime readCaptureDate(const QString& filePath) const override;
};

}  // namespace pictorio::image_loader
