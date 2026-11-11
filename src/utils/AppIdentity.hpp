/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#pragma once

#include <QByteArray>
#include <QString>

namespace pictorio::utils
{

// Reverse-DNS application ID. Used as the Flatpak app ID, the .desktop file name (the window manager
// uses it to find the app icon) and the URI of the QML module compiled into the executable.
inline const QString applicationId = QStringLiteral("io.github.oesusdevelopmentstudios.pictorio");

// Short internal name: translation catalogue (pictorio.mo) and config file (pictoriorc).
inline const QString applicationComponentName = QStringLiteral("pictorio");

// Used for D-Bus names. KAboutData defaults to kde.org, which would be wrong for a project outside KDE.
inline const QByteArray organizationDomain = QByteArrayLiteral("oesusdevelopmentstudios.github.io");

// Sets the translation domain, then registers the application name, version, license and IDs with
// KAboutData and QCoreApplication. Call once at startup, right after the QCoreApplication is created
// and before any other i18n() call.
void registerApplicationIdentity();

}  // namespace pictorio::utils
