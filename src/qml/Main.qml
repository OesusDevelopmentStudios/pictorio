// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

import QtQuick
import org.kde.kirigami as Kirigami
import org.kde.ki18n

Kirigami.ApplicationWindow {
    id: root

    required property AppManager appManager

    title: appManager.viewer.hasImage
        ? KI18n.i18nc("@title:window %1 is a file name", "%1 — Pictorio", appManager.viewer.fileName)
        : KI18n.i18nc("@title:window", "Pictorio")

    // Kirigami grid units scale with the system font size.
    width: Kirigami.Units.gridUnit * 50
    height: Kirigami.Units.gridUnit * 35

    // A slideshow shows only the image: no toolbar, fullscreen.
    pageStack.globalToolBar.style: appManager.slideshow.running ? Kirigami.ApplicationHeaderStyle.None
                                                                : Kirigami.ApplicationHeaderStyle.Auto

    pageStack.initialPage: ViewerPage {
        viewer: root.appManager.viewer
        viewTransform: root.appManager.viewTransform
        navigator: root.appManager.navigator
        slideshow: root.appManager.slideshow
    }

    // Restored when the slideshow ends, e.g. a maximized window stays maximized.
    property int visibilityBeforeSlideshow: Window.Windowed
    property bool fullScreenForSlideshow: false

    Connections {
        target: root.appManager.slideshow

        function onStateChanged() {
            const running = root.appManager.slideshow.running;
            if (running && root.visibility !== Window.FullScreen) {
                root.visibilityBeforeSlideshow = root.visibility;
                root.fullScreenForSlideshow = true;
                root.showFullScreen();
            } else if (!running && root.fullScreenForSlideshow) {
                root.fullScreenForSlideshow = false;
                root.visibility = root.visibilityBeforeSlideshow;
            }
        }
    }
}
