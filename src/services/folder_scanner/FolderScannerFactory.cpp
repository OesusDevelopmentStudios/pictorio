/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "FolderScannerFactory.hpp"

#include "impl/QtFolderScanner.hpp"

namespace pictorio::folder_scanner
{

std::unique_ptr<FolderScanner> createFolderScanner()
{
    return std::make_unique<QtFolderScanner>();
}

}  // namespace pictorio::folder_scanner
