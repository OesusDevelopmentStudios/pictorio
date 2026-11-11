/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "ViewTransform.hpp"
#include "Viewer.hpp"

// Makes service classes known to QML without the services depending on QML. QML only receives
// these objects from the AppManager; it cannot create them.

namespace pictorio::app_manager
{

struct ForeignViewer
{
    Q_GADGET
    QML_FOREIGN(pictorio::viewer::Viewer)
    QML_NAMED_ELEMENT(Viewer)
    QML_UNCREATABLE("Provided by AppManager")
};

struct ForeignViewTransform
{
    Q_GADGET
    QML_FOREIGN(pictorio::view_transform::ViewTransform)
    QML_NAMED_ELEMENT(ViewTransform)
    QML_UNCREATABLE("Provided by AppManager")
};

}  // namespace pictorio::app_manager
