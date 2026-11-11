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

// Lists the local folder with QDir and accepts the image types Qt's image plugins can read.
class QtFolderScanner : public FolderScanner
{
public:
    explicit QtFolderScanner(std::unique_ptr<const image_loader::ImageLoader> imageLoader);

    QStringList listImages(const QString& folderPath, const Sorting& sorting) const override;

private:
    void sortByDate(QStringList& imagePaths) const;

    std::unique_ptr<const image_loader::ImageLoader> m_imageLoader;
};

}  // namespace pictorio::folder_scanner
