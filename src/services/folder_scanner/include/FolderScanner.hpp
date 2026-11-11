/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <QString>
#include <QStringList>
#include <QtTypes>

namespace pictorio::folder_scanner
{

enum class SortOrder
{
    Name,    // natural order: "img2" before "img10", case ignored
    Date,    // oldest first: when the photo was taken, else the file modification time; ties by name
    Random,  // shuffled; the same seed always gives the same order
};

struct Sorting
{
    SortOrder order = SortOrder::Name;
    quint32 randomSeed = 0;  // used only by SortOrder::Random
};

// Lists the images in a folder. Thread-safe, so it can run in the background.
class FolderScanner
{
public:
    virtual ~FolderScanner();

    // Absolute paths of the images directly in the folder (not in subfolders). Images are recognized
    // by their file extension; hidden files are skipped. Empty if the folder does not exist.
    virtual QStringList listImages(const QString& folderPath, const Sorting& sorting) const = 0;
};

}  // namespace pictorio::folder_scanner
