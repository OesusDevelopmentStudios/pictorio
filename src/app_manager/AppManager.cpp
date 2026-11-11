/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "app_manager/AppManager.hpp"

#include "ImageLoaderFactory.hpp"
#include "ViewerFactory.hpp"

namespace pictorio::app_manager
{

AppManager::AppManager(QObject* parent)
    : QObject(parent)
    , m_viewer(viewer::createViewer(image_loader::createImageLoader()))
{}

AppManager::~AppManager() = default;

viewer::Viewer* AppManager::viewer() const
{
    return m_viewer.get();
}

}  // namespace pictorio::app_manager
