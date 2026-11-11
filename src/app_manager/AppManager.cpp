/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "app_manager/AppManager.hpp"

#include "ImageLoaderFactory.hpp"
#include "ViewTransformFactory.hpp"
#include "ViewerFactory.hpp"

namespace pictorio::app_manager
{

AppManager::AppManager(QObject* parent)
    : QObject(parent)
    , m_viewer(viewer::createViewer(image_loader::createImageLoader()))
    , m_viewTransform(view_transform::createViewTransform())
{
    // Every newly opened image starts fitted to the window and not rotated.
    connect(m_viewer.get(), &viewer::Viewer::currentImageChanged, m_viewTransform.get(), [this]() {
        m_viewTransform->showImage(m_viewer->imageDisplaySize());
    });
}

AppManager::~AppManager() = default;

viewer::Viewer* AppManager::viewer() const
{
    return m_viewer.get();
}

view_transform::ViewTransform* AppManager::viewTransform() const
{
    return m_viewTransform.get();
}

}  // namespace pictorio::app_manager
