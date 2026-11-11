/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "impl/QtFolderScanner.hpp"

#include <algorithm>
#include <numeric>

#include <QCollator>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QMimeDatabase>
#include <QRandomGenerator>
#include <QSet>
#include <QtConcurrentMap>

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

// Fisher-Yates with Qt's generator: std::shuffle may give a different order for the same seed
// depending on the standard library.
void shuffle(QStringList& filePaths, quint32 seed)
{
    QRandomGenerator generator(seed);
    for (qsizetype lastIndex = filePaths.size() - 1; lastIndex > 0; --lastIndex)
    {
        const auto swapIndex = static_cast<qsizetype>(generator.bounded(static_cast<quint64>(lastIndex) + 1));
        filePaths.swapItemsAt(lastIndex, swapIndex);
    }
}

}  // namespace

namespace pictorio::folder_scanner
{

QtFolderScanner::QtFolderScanner(std::unique_ptr<const image_loader::ImageLoader> imageLoader)
    : m_imageLoader(std::move(imageLoader))
{}

QStringList QtFolderScanner::listImages(const QString& folderPath, const Sorting& sorting) const
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

    // Name order first: date ties keep it, and the shuffle starts from the same order every time.
    sortNaturallyByName(imagePaths);
    switch (sorting.order)
    {
    case SortOrder::Name:
        break;
    case SortOrder::Date:
        sortByDate(imagePaths);
        break;
    case SortOrder::Random:
        shuffle(imagePaths, sorting.randomSeed);
        break;
    }

    return imagePaths;
}

void QtFolderScanner::sortByDate(QStringList& imagePaths) const
{
    // Reading the dates is the slow part (it opens every file), so it runs in parallel.
    const QList<QDateTime> dates = QtConcurrent::blockingMapped(imagePaths, [this](const QString& imagePath) {
        const QDateTime captureDate = m_imageLoader->readCaptureDate(imagePath);
        return captureDate.isValid() ? captureDate : QFileInfo(imagePath).lastModified();
    });

    QList<qsizetype> order(imagePaths.size());
    std::iota(order.begin(), order.end(), 0);
    std::ranges::stable_sort(order, [&](qsizetype left, qsizetype right) { return dates.at(left) < dates.at(right); });

    QStringList sortedPaths;
    sortedPaths.reserve(imagePaths.size());
    for (const qsizetype index : order)
    {
        sortedPaths.append(imagePaths.at(index));
    }

    imagePaths = std::move(sortedPaths);
}

}  // namespace pictorio::folder_scanner
