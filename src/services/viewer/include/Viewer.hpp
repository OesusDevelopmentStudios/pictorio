/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <memory>
#include <optional>

#include <QObject>
#include <QSize>
#include <QString>
#include <QUrl>

#include "ImageLoader.hpp"

namespace pictorio::viewer
{

// What the viewer page shows: the current image, or why it could not be opened.
class Viewer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool hasImage READ hasImage NOTIFY currentImageChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY currentImageChanged)
    Q_PROPERTY(QUrl imageSource READ imageSource NOTIFY currentImageChanged)
    Q_PROPERTY(QSize imageDisplaySize READ imageDisplaySize NOTIFY currentImageChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY currentImageChanged)

public:
    explicit Viewer(std::unique_ptr<image_loader::ImageLoader> imageLoader, QObject* parent = nullptr);
    ~Viewer() override;

    Q_INVOKABLE void openImage(const QString& filePath);

    bool hasImage() const;
    QString fileName() const;  // also set when opening failed, for the error message
    QUrl imageSource() const;
    QSize imageDisplaySize() const;
    QString errorMessage() const;  // empty when the image opened

Q_SIGNALS:
    void currentImageChanged();

private:
    std::unique_ptr<image_loader::ImageLoader> m_imageLoader;
    std::optional<image_loader::ImageInfo> m_currentImage;
    QString m_fileName;
    QString m_errorMessage;
};

}  // namespace pictorio::viewer
