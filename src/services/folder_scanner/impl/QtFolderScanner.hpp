/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include "FolderScanner.hpp"

namespace pictorio::folder_scanner
{

// Lists the local folder with QDir and accepts the image types Qt's image plugins can read.
class QtFolderScanner : public FolderScanner
{
public:
    QStringList listImages(const QString& folderPath) const override;
};

}  // namespace pictorio::folder_scanner
