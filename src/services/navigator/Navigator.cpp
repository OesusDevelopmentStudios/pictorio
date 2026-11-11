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
        clearListing();
        startListingFolder(folderPath);
    }

    Q_EMIT positionChanged();
}

void Navigator::setSorting(const folder_scanner::Sorting& sorting)
{
    if (sorting == m_sorting)
    {
        return;
    }

    m_sorting = sorting;
    if (m_currentPath.isEmpty())
    {
        return;
    }

    clearListing();
    startListingFolder(QFileInfo(m_currentPath).path());
    Q_EMIT positionChanged();
}

void Navigator::setWrapAround(bool wrapAround)
{
    if (wrapAround != m_wrapAround)
    {
        m_wrapAround = wrapAround;
        Q_EMIT positionChanged();
    }
}

void Navigator::goToPrevious()
{
    if (canGoToPrevious())
    {
        goToIndex((m_currentIndex + imageCount() - 1) % imageCount());
    }
}

void Navigator::goToNext()
{
    if (canGoToNext())
    {
        goToIndex((m_currentIndex + 1) % imageCount());
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
    if (m_currentIndex < 0 || imageCount() < 2)
    {
        return false;
    }

    return m_wrapAround || m_currentIndex > 0;
}

bool Navigator::canGoToNext() const
{
    if (m_currentIndex < 0 || imageCount() < 2)
    {
        return false;
    }

    return m_wrapAround || m_currentIndex < imageCount() - 1;
}

void Navigator::startListingFolder(const QString& folderPath)
{
    m_listingWatcher.setFuture(QtConcurrent::run([folderScanner = m_folderScanner, folderPath, sorting = m_sorting]() {
        return FolderListing{folderPath, sorting, folderScanner->listImages(folderPath, sorting)};
    }));
}

void Navigator::clearListing()
{
    m_folderPath.clear();
    m_imagePaths.clear();
    m_currentIndex = -1;
}

void Navigator::takeFolderListing()
{
    FolderListing listing = m_listingWatcher.result();

    // In the meantime an image in another folder may have been opened, or the sorting changed.
    if (listing.folderPath != QFileInfo(m_currentPath).path() || listing.sorting != m_sorting)
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
