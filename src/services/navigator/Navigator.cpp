/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "Navigator.hpp"

#include <QDir>
#include <QFileInfo>
#include <QtConcurrentRun>

namespace
{

// Same form as the paths the folder scanner returns, so they can be compared.
QString normalizedPath(const QString& filePath)
{
    return QDir::cleanPath(QFileInfo(filePath).absoluteFilePath());
}

}  // namespace

namespace pictorio::navigator
{

Navigator::Navigator(std::unique_ptr<folder_scanner::FolderScanner> folderScanner, QObject* parent)
    : QObject(parent)
    , m_folderScanner(std::move(folderScanner))
{
    connect(&m_listingWatcher, &QFutureWatcher<FolderListing>::finished, this, &Navigator::takeFolderListing);
}

Navigator::~Navigator() = default;

void Navigator::openImage(const QString& filePath)
{
    m_currentPath = normalizedPath(filePath);
    Q_EMIT imageRequested(m_currentPath);

    const QString folderPath = QFileInfo(m_currentPath).path();
    if (folderPath == m_folderPath)
    {
        m_currentIndex = m_imagePaths.indexOf(m_currentPath);
    }
    else
    {
        m_folderPath.clear();
        m_imagePaths.clear();
        m_currentIndex = -1;
        startListingFolder(folderPath);
    }

    Q_EMIT positionChanged();
}

void Navigator::goToPrevious()
{
    if (canGoToPrevious())
    {
        goToIndex(m_currentIndex - 1);
    }
}

void Navigator::goToNext()
{
    if (canGoToNext())
    {
        goToIndex(m_currentIndex + 1);
    }
}

int Navigator::currentIndex() const
{
    return m_currentIndex;
}

int Navigator::imageCount() const
{
    return static_cast<int>(m_imagePaths.size());
}

bool Navigator::canGoToPrevious() const
{
    return m_currentIndex > 0;
}

bool Navigator::canGoToNext() const
{
    return m_currentIndex >= 0 && m_currentIndex < imageCount() - 1;
}

void Navigator::startListingFolder(const QString& folderPath)
{
    m_listingWatcher.setFuture(QtConcurrent::run([folderScanner = m_folderScanner, folderPath]() {
        return FolderListing{folderPath, folderScanner->listImages(folderPath)};
    }));
}

void Navigator::takeFolderListing()
{
    FolderListing listing = m_listingWatcher.result();

    // In the meantime an image in another folder may have been opened.
    if (listing.folderPath != QFileInfo(m_currentPath).path())
    {
        return;
    }

    m_folderPath = listing.folderPath;
    m_imagePaths = std::move(listing.imagePaths);
    m_currentIndex = m_imagePaths.indexOf(m_currentPath);
    Q_EMIT positionChanged();
}

void Navigator::goToIndex(int index)
{
    m_currentIndex = index;
    m_currentPath = m_imagePaths.at(index);
    Q_EMIT imageRequested(m_currentPath);
    Q_EMIT positionChanged();
}

}  // namespace pictorio::navigator
