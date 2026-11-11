/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "impl/QtFolderScanner.hpp"

#include <algorithm>

#include <QCollator>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QMimeDatabase>
#include <QSet>

namespace
{

// Image types Qt can read. Only image/* types: Qt can also read e.g. PDF pages, which do not belong
// in an image folder view.
QSet<QString> readableImageMimeTypes()
{
    QSet<QString> mimeTypes;
    for (const QByteArray& mimeType : QImageReader::supportedMimeTypes())
    {
        if (mimeType.startsWith("image/"))
        {
            mimeTypes.insert(QString::fromLatin1(mimeType));
        }
    }

    return mimeTypes;
}

// By extension only: reading the content of every file would be slow in large folders.
bool isReadableImage(const QFileInfo& fileInfo, const QSet<QString>& readableMimeTypes)
{
    static const QMimeDatabase mimeDatabase;
    const QMimeType mimeType = mimeDatabase.mimeTypeForFile(fileInfo, QMimeDatabase::MatchExtension);
    if (readableMimeTypes.contains(mimeType.name()))
    {
        return true;
    }

    const QStringList aliases = mimeType.aliases();
    return std::ranges::any_of(aliases, [&](const QString& alias) { return readableMimeTypes.contains(alias); });
}

// The paths share their folder, so comparing whole paths orders them by file name.
void sortNaturallyByName(QStringList& filePathsInOneFolder)
{
    QCollator collator;
    collator.setNumericMode(true);
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    std::ranges::sort(filePathsInOneFolder,
                      [&](const QString& left, const QString& right) { return collator.compare(left, right) < 0; });
}

}  // namespace

namespace pictorio::folder_scanner
{

QStringList QtFolderScanner::listImages(const QString& folderPath) const
{
    const QDir folder(folderPath);
    const QFileInfoList entries = folder.entryInfoList(QDir::Files | QDir::Readable);
    const QSet<QString> readableMimeTypes = readableImageMimeTypes();

    QStringList imagePaths;
    for (const QFileInfo& entry : entries)
    {
        if (isReadableImage(entry, readableMimeTypes))
        {
            imagePaths.append(entry.absoluteFilePath());
        }
    }

    sortNaturallyByName(imagePaths);
    return imagePaths;
}

}  // namespace pictorio::folder_scanner
