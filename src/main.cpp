/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios
 */

#include <cstdlib>

#include <KAboutData>
#include <KLocalizedQmlContext>
#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

#include "app_manager/AppManager.hpp"
#include "utils/AppIdentity.hpp"
#include "utils/CommandLine.hpp"

namespace
{

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

    pictorio::utils::registerApplicationIdentity();
    selectQuickControlsStyle();

    QCommandLineParser commandLineParser;
    pictorio::utils::addCommandLineOptions(commandLineParser);
    commandLineParser.process(application);  // exits on --help, --version and errors
    KAboutData::applicationData().processCommandLine(&commandLineParser);
    const pictorio::utils::CommandLineOptions commandLineOptions =
        pictorio::utils::readCommandLineOptions(commandLineParser, QDir::currentPath());

    pictorio::app_manager::AppManager appManager;
    if (!commandLineOptions.imagePaths.isEmpty())
    {
        appManager.viewer()->openImage(commandLineOptions.imagePaths.first());
    }

    QQmlApplicationEngine qmlEngine;
    KLocalization::setupLocalizedContext(&qmlEngine);  // makes KI18n.i18n*() work in QML

    // The QML module URI is the application ID (see ecm_add_qml_module in src/CMakeLists.txt).
    qmlEngine.loadFromModule(pictorio::utils::applicationId, QStringLiteral("Main"));

    const bool mainWindowLoaded = !qmlEngine.rootObjects().isEmpty();
    if (!mainWindowLoaded)
    {
        return EXIT_FAILURE;
    }

    return application.exec();
}
