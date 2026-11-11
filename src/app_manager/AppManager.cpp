/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include "app_manager/AppManager.hpp"

#include "FolderScannerFactory.hpp"
#include "ImageLoaderFactory.hpp"
#include "NavigatorFactory.hpp"
#include "SettingsFactory.hpp"
#include "SlideshowFactory.hpp"
#include "ViewTransformFactory.hpp"
#include "ViewerFactory.hpp"

namespace pictorio::app_manager
{

AppManager::AppManager(QObject* parent)
    : QObject(parent)
    , m_settings(settings::createSettings())
    , m_viewer(viewer::createViewer(image_loader::createImageLoader()))
    , m_viewTransform(view_transform::createViewTransform())
    , m_navigator(navigator::createNavigator(folder_scanner::createFolderScanner(image_loader::createImageLoader())))
    , m_slideshow(slideshow::createSlideshow(m_navigator.get(), m_settings.get()))
{
    connect(m_navigator.get(), &navigator::Navigator::imageRequested, m_viewer.get(), &viewer::Viewer::openImage);

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

navigator::Navigator* AppManager::navigator() const
{
    return m_navigator.get();
}

slideshow::Slideshow* AppManager::slideshow() const
{
    return m_slideshow.get();
}

}  // namespace pictorio::app_manager
