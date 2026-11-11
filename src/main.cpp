/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include <cstdlib>

#include <KAboutData>
#include <KLocalizedQmlContext>
#include <KLocalizedString>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

namespace
{
// Name of the translation catalogue (pictorio.mo) and of the config files (pictoriorc).
const auto applicationComponentName = QStringLiteral("pictorio");

// Must match the Flatpak app ID and the .desktop file name. The window manager uses it to find the app icon.
const auto applicationDesktopFileName = QStringLiteral("io.github.OesusDevelopmentStudios.Pictorio");

// URI of the QML module compiled into this executable (see ecm_add_qml_module in CMakeLists.txt).
const auto qmlModuleUri = QStringLiteral("io.github.OesusDevelopmentStudios.Pictorio");

// KAboutData defaults to kde.org, which would be wrong for a project outside KDE.
const auto organizationDomain = QByteArrayLiteral("OesusDevelopmentStudios.github.io");

// Fills in the application name, version and license shown in the About page,
// and registers them with Qt (QCoreApplication::applicationName() etc.).
void registerAboutData()
{
    KAboutData aboutData(applicationComponentName,
                         i18nc("@title", "Pictorio"),
                         QStringLiteral(PICTORIO_VERSION),
                         i18nc("@info:credit", "Image viewer and photo library"),
                         KAboutLicense::Unknown,
                         i18nc("@info:credit", "© 2026 OesusDevelopmentStudios"));
    aboutData.setLicense(KAboutLicense::GPL_V3, KAboutLicense::OrLaterVersions);
    aboutData.setDesktopFileName(applicationDesktopFileName);
    aboutData.setOrganizationDomain(organizationDomain);

    KAboutData::setApplicationData(aboutData);
}

// Uses the KDE look for buttons, menus etc., also outside Plasma.
// A style the user picked with the QT_QUICK_CONTROLS_STYLE environment variable takes precedence.
void selectQuickControlsStyle()
{
    const bool userChoseStyle = !qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE");
    if (!userChoseStyle)
    {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }
}
}  // namespace

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);

    // Must run before any i18n() call, including the ones in registerAboutData().
    KLocalizedString::setApplicationDomain(applicationComponentName.toUtf8());

    registerAboutData();
    selectQuickControlsStyle();

    QQmlApplicationEngine qmlEngine;
    KLocalization::setupLocalizedContext(&qmlEngine);  // makes KI18n.i18n*() work in QML
    qmlEngine.loadFromModule(qmlModuleUri, QStringLiteral("Main"));

    const bool mainWindowLoaded = !qmlEngine.rootObjects().isEmpty();
    if (!mainWindowLoaded)
    {
        return EXIT_FAILURE;
    }

    return application.exec();
}
