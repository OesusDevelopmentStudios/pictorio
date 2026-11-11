/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <memory>

#include "Navigator.hpp"

namespace pictorio::navigator
{

std::unique_ptr<Navigator> createNavigator(std::unique_ptr<folder_scanner::FolderScanner> folderScanner);

}  // namespace pictorio::navigator
