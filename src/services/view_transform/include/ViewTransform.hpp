/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <QObject>
#include <QSize>
#include <QSizeF>

namespace pictorio::view_transform
{

// How the current image is shown: zoom and rotation of the view (the file is never changed).
//
// The zoom either fits the image into the viewport or is a scale chosen by the user. A chosen scale is
// always larger than the fit scale; zooming out further, or enlarging the window, switches back to fit.
// Fitting never enlarges an image beyond 100 %.
class ViewTransform : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QSize imageSize READ imageSize NOTIFY transformChanged)
    Q_PROPERTY(QSizeF viewportSize READ viewportSize WRITE setViewportSize NOTIFY transformChanged)
    Q_PROPERTY(bool fitToWindow READ fitToWindow WRITE setFitToWindow NOTIFY transformChanged)
    Q_PROPERTY(qreal scale READ scale NOTIFY transformChanged)
    Q_PROPERTY(int rotationAngle READ rotationAngle NOTIFY transformChanged)
    Q_PROPERTY(QSizeF displayedSize READ displayedSize NOTIFY transformChanged)
    Q_PROPERTY(bool canZoomIn READ canZoomIn NOTIFY transformChanged)
    Q_PROPERTY(bool canZoomOut READ canZoomOut NOTIFY transformChanged)

public:
    static constexpr qreal maximumScale = 16.0;
    static constexpr qreal zoomStep = 1.25;  // scale factor of one zoom step

    explicit ViewTransform(QObject* parent = nullptr);
    ~ViewTransform() override;

    // Starts showing a new image: fit to window, not rotated. An empty size means no image.
    void showImage(QSize imageSize);

    void setViewportSize(QSizeF viewportSize);
    void setFitToWindow(bool fitToWindow);  // false shows the actual size

    Q_INVOKABLE void zoomIn();
    Q_INVOKABLE void zoomOut();
    Q_INVOKABLE void zoomBySteps(qreal steps);  // one step per mouse wheel notch; negative zooms out
    Q_INVOKABLE void zoomBy(qreal factor);      // e.g. a pinch gesture
    Q_INVOKABLE void showActualSize();          // 100 %
    Q_INVOKABLE void rotateClockwise();
    Q_INVOKABLE void rotateCounterclockwise();

    QSize imageSize() const;  // before rotation, in pixels
    QSizeF viewportSize() const;
    bool fitToWindow() const;
    qreal scale() const;           // 1.0 = 100 %
    int rotationAngle() const;     // clockwise: 0, 90, 180 or 270
    QSizeF displayedSize() const;  // the rotated, scaled image on screen
    bool canZoomIn() const;
    bool canZoomOut() const;

Q_SIGNALS:
    void transformChanged();

private:
    bool hasImage() const;
    QSizeF rotatedImageSize() const;
    qreal fitScale() const;
    void setUserScale(qreal scale);
    void switchToFitIfWholeImageFits();

    QSize m_imageSize;
    QSizeF m_viewportSize;
    bool m_fitToWindow = true;
    qreal m_userScale = 1.0;  // used when not fitting
    int m_rotationAngle = 0;
};

}  // namespace pictorio::view_transform
