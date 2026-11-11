/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <memory>

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

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

public:
    explicit AppManager(QObject* parent = nullptr);
    ~AppManager() override;

    viewer::Viewer* viewer() const;
    view_transform::ViewTransform* viewTransform() const;

private:
    std::unique_ptr<viewer::Viewer> m_viewer;
    std::unique_ptr<view_transform::ViewTransform> m_viewTransform;
};

}  // namespace pictorio::app_manager
