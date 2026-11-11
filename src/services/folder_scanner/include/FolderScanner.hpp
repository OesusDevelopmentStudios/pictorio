/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <QString>
#include <QStringList>

namespace pictorio::folder_scanner
{

// Lists the images in a folder. Thread-safe, so it can run in the background.
class FolderScanner
{
public:
    virtual ~FolderScanner();

    // Absolute paths of the images directly in the folder (not in subfolders), in natural name order:
    // "img2" before "img10", case ignored. Images are recognized by their file extension.
    // Hidden files are skipped. Empty if the folder does not exist.
    virtual QStringList listImages(const QString& folderPath) const = 0;
};

}  // namespace pictorio::folder_scanner
