/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "ImageLoaderFactory.hpp"

#include "impl/QtImageLoader.hpp"

namespace pictorio::image_loader
{

std::unique_ptr<ImageLoader> createImageLoader()
{
    return std::make_unique<QtImageLoader>();
}

}  // namespace pictorio::image_loader
