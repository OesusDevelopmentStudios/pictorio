// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

import QtQuick
import org.kde.kirigami as Kirigami
import org.kde.ki18n

Kirigami.ApplicationWindow {
    id: root

    title: KI18n.i18nc("@title:window", "Pictorio")

    // Kirigami grid units scale with the system font size.
    width: Kirigami.Units.gridUnit * 50
    height: Kirigami.Units.gridUnit * 35

    pageStack.initialPage: Kirigami.Page {
        title: KI18n.i18nc("@title", "Viewer")

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            width: parent.width - Kirigami.Units.gridUnit * 4

            icon.name: "image-x-generic"
            text: KI18n.i18nc("@info:placeholder", "No image open")
        }
    }
}
