/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <memory>

#include <QFutureWatcher>
#include <QObject>
#include <QString>
#include <QStringList>

#include "FolderScanner.hpp"

namespace pictorio::navigator
{

// Where the current image is in its folder, and moving to the next or previous image.
//
// Opening an image requests it at once and lists its folder in the background; until the list is
// ready, next and previous are not available. There is no wrap-around at either end.
class Navigator : public QObject
{
    Q_OBJECT

    struct FolderListing
    {
        QString folderPath;
        QStringList imagePaths;
    };

    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY positionChanged)
    Q_PROPERTY(int imageCount READ imageCount NOTIFY positionChanged)
    Q_PROPERTY(bool canGoToPrevious READ canGoToPrevious NOTIFY positionChanged)
    Q_PROPERTY(bool canGoToNext READ canGoToNext NOTIFY positionChanged)

public:
    explicit Navigator(std::unique_ptr<folder_scanner::FolderScanner> folderScanner, QObject* parent = nullptr);
    ~Navigator() override;

    void openImage(const QString& filePath);
    Q_INVOKABLE void goToPrevious();
    Q_INVOKABLE void goToNext();

    int currentIndex() const;  // -1 while the folder is being listed, or if the image is not in the list
    int imageCount() const;
    bool canGoToPrevious() const;
    bool canGoToNext() const;

Q_SIGNALS:
    void imageRequested(const QString& filePath);  // the image to show now
    void positionChanged();

private:
    void startListingFolder(const QString& folderPath);
    void takeFolderListing();
    void goToIndex(int index);

    // Shared with the background listing, which can outlive this object.
    std::shared_ptr<const folder_scanner::FolderScanner> m_folderScanner;
    QFutureWatcher<FolderListing> m_listingWatcher;

    QString m_currentPath;
    QString m_folderPath;  // the folder m_imagePaths belongs to
    QStringList m_imagePaths;
    int m_currentIndex = -1;
};

}  // namespace pictorio::navigator
