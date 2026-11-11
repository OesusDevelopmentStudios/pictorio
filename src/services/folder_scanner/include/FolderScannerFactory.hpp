/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <memory>

#include "FolderScanner.hpp"
#include "ImageLoader.hpp"

namespace pictorio::folder_scanner
{

// Creates the folder scanner used by the application (local file system, Qt's image plugins).
// The image loader provides the capture dates for SortOrder::Date.
std::unique_ptr<FolderScanner> createFolderScanner(std::unique_ptr<const image_loader::ImageLoader> imageLoader);

}  // namespace pictorio::folder_scanner
