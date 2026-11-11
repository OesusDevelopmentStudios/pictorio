/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <memory>

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "Navigator.hpp"
#include "Slideshow.hpp"
#include "ViewTransform.hpp"
#include "Viewer.hpp"

namespace pictorio::app_manager
{

// Creates the services through their factories and makes them available to QML. Owns no logic.
class AppManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Created in main.cpp")

    Q_PROPERTY(pictorio::viewer::Viewer* viewer READ viewer CONSTANT)
    Q_PROPERTY(pictorio::view_transform::ViewTransform* viewTransform READ viewTransform CONSTANT)
    Q_PROPERTY(pictorio::navigator::Navigator* navigator READ navigator CONSTANT)
    Q_PROPERTY(pictorio::slideshow::Slideshow* slideshow READ slideshow CONSTANT)

public:
    explicit AppManager(QObject* parent = nullptr);
    ~AppManager() override;

    viewer::Viewer* viewer() const;
    view_transform::ViewTransform* viewTransform() const;
    navigator::Navigator* navigator() const;
    slideshow::Slideshow* slideshow() const;

private:
    std::unique_ptr<viewer::Viewer> m_viewer;
    std::unique_ptr<view_transform::ViewTransform> m_viewTransform;
    std::unique_ptr<navigator::Navigator> m_navigator;
    std::unique_ptr<slideshow::Slideshow> m_slideshow;  // uses m_navigator, so declared after it
};

}  // namespace pictorio::app_manager
