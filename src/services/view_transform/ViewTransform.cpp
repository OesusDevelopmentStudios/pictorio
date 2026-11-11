/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "ViewTransform.hpp"

#include <algorithm>
#include <cmath>

namespace
{

constexpr int fullTurn = 360;
constexpr int quarterTurn = 90;

}  // namespace

namespace pictorio::view_transform
{

ViewTransform::ViewTransform(QObject* parent)
    : QObject(parent)
{}

ViewTransform::~ViewTransform() = default;

void ViewTransform::showImage(QSize imageSize)
{
    m_imageSize = imageSize;
    m_fitToWindow = true;
    m_userScale = 1.0;
    m_rotationAngle = 0;
    Q_EMIT transformChanged();
}

void ViewTransform::setViewportSize(QSizeF viewportSize)
{
    if (viewportSize == m_viewportSize)
    {
        return;
    }

    m_viewportSize = viewportSize;
    switchToFitIfWholeImageFits();
    Q_EMIT transformChanged();
}

void ViewTransform::setFitToWindow(bool fitToWindow)
{
    if (!fitToWindow)
    {
        showActualSize();
        return;
    }

    if (!m_fitToWindow)
    {
        m_fitToWindow = true;
        Q_EMIT transformChanged();
    }
}

void ViewTransform::zoomIn()
{
    zoomBySteps(1);
}

void ViewTransform::zoomOut()
{
    zoomBySteps(-1);
}

void ViewTransform::zoomBySteps(qreal steps)
{
    zoomBy(std::pow(zoomStep, steps));
}

void ViewTransform::zoomBy(qreal factor)
{
    const bool validFactor = std::isfinite(factor) && factor > 0;
    if (!hasImage() || !validFactor)
    {
        return;
    }

    setUserScale(scale() * factor);
}

void ViewTransform::showActualSize()
{
    if (!hasImage())
    {
        return;
    }

    setUserScale(1.0);
}

void ViewTransform::rotateClockwise()
{
    m_rotationAngle = (m_rotationAngle + quarterTurn) % fullTurn;
    switchToFitIfWholeImageFits();
    Q_EMIT transformChanged();
}

void ViewTransform::rotateCounterclockwise()
{
    m_rotationAngle = (m_rotationAngle + fullTurn - quarterTurn) % fullTurn;
    switchToFitIfWholeImageFits();
    Q_EMIT transformChanged();
}

QSize ViewTransform::imageSize() const
{
    return m_imageSize;
}

QSizeF ViewTransform::viewportSize() const
{
    return m_viewportSize;
}

bool ViewTransform::fitToWindow() const
{
    return m_fitToWindow;
}

qreal ViewTransform::scale() const
{
    return m_fitToWindow ? fitScale() : m_userScale;
}

int ViewTransform::rotationAngle() const
{
    return m_rotationAngle;
}

QSizeF ViewTransform::displayedSize() const
{
    return rotatedImageSize() * scale();
}

bool ViewTransform::canZoomIn() const
{
    return hasImage() && scale() < maximumScale;
}

bool ViewTransform::canZoomOut() const
{
    return hasImage() && !m_fitToWindow;
}

bool ViewTransform::hasImage() const
{
    return !m_imageSize.isEmpty();
}

QSizeF ViewTransform::rotatedImageSize() const
{
    const bool sidesSwapped = m_rotationAngle == quarterTurn || m_rotationAngle == 3 * quarterTurn;
    return sidesSwapped ? QSizeF(m_imageSize.transposed()) : QSizeF(m_imageSize);
}

qreal ViewTransform::fitScale() const
{
    if (!hasImage() || m_viewportSize.isEmpty())
    {
        return 1.0;
    }

    const QSizeF imageSize = rotatedImageSize();
    const qreal widthScale = m_viewportSize.width() / imageSize.width();
    const qreal heightScale = m_viewportSize.height() / imageSize.height();
    return std::min({1.0, widthScale, heightScale});
}

void ViewTransform::setUserScale(qreal scale)
{
    m_userScale = std::min(scale, maximumScale);
    m_fitToWindow = false;
    switchToFitIfWholeImageFits();
    Q_EMIT transformChanged();
}

void ViewTransform::switchToFitIfWholeImageFits()
{
    if (!m_fitToWindow && m_userScale <= fitScale())
    {
        m_fitToWindow = true;
    }
}

}  // namespace pictorio::view_transform
