/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "FolderScannerFactory.hpp"

#include "impl/QtFolderScanner.hpp"

namespace pictorio::folder_scanner
{

std::unique_ptr<FolderScanner> createFolderScanner(std::unique_ptr<const image_loader::ImageLoader> imageLoader)
{
    return std::make_unique<QtFolderScanner>(std::move(imageLoader));
}

}  // namespace pictorio::folder_scanner
