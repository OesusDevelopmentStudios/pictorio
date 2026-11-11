/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <memory>

#include "Slideshow.hpp"

namespace pictorio::slideshow
{

// The navigator and the settings are not owned and must outlive the slideshow.
std::unique_ptr<Slideshow> createSlideshow(navigator::Navigator* navigator, settings::Settings* settings);

}  // namespace pictorio::slideshow
