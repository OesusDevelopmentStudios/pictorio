/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <memory>

#include "FolderScanner.hpp"

namespace pictorio::folder_scanner
{

// Creates the folder scanner used by the application (local file system, Qt's image plugins).
std::unique_ptr<FolderScanner> createFolderScanner();

}  // namespace pictorio::folder_scanner
