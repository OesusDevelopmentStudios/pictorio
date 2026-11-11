/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "SlideshowFactory.hpp"

namespace pictorio::slideshow
{

std::unique_ptr<Slideshow> createSlideshow(navigator::Navigator* navigator)
{
    return std::make_unique<Slideshow>(navigator);
}

}  // namespace pictorio::slideshow
