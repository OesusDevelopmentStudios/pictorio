/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "ViewerFactory.hpp"

namespace pictorio::viewer
{

std::unique_ptr<Viewer> createViewer(std::unique_ptr<image_loader::ImageLoader> imageLoader)
{
    return std::make_unique<Viewer>(std::move(imageLoader));
}

}  // namespace pictorio::viewer
