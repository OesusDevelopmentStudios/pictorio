/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <memory>

#include "Viewer.hpp"

namespace pictorio::viewer
{

std::unique_ptr<Viewer> createViewer(std::unique_ptr<image_loader::ImageLoader> imageLoader);

}  // namespace pictorio::viewer
