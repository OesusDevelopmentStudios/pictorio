/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <memory>

#include "ImageLoader.hpp"

namespace pictorio::image_loader
{

// Creates the image loader used by the application (based on Qt's image plugins).
std::unique_ptr<ImageLoader> createImageLoader();

}  // namespace pictorio::image_loader
