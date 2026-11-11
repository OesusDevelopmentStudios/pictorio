// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

import QtQuick
import org.kde.kirigami as Kirigami
import org.kde.ki18n

// Shows the current image scaled to fit, or a message when there is none.
Kirigami.Page {
    id: page

    required property Viewer viewer

    title: viewer.hasImage ? viewer.fileName : KI18n.i18nc("@title", "Viewer")
    padding: 0

    Image {
        id: image

        anchors.fill: parent
        visible: page.viewer.hasImage

        source: page.viewer.imageSource
        fillMode: Image.PreserveAspectFit
        autoTransform: true  // apply the EXIF orientation
        asynchronous: true
        smooth: true
        mipmap: true
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
        visible: !page.viewer.hasImage && page.viewer.errorMessage.length === 0

        icon.name: "image-x-generic"
        text: KI18n.i18nc("@info:placeholder", "No image open")
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
        visible: page.viewer.errorMessage.length > 0 || image.status === Image.Error

        icon.name: "data-error"
        text: KI18n.i18nc("@info:placeholder %1 is a file name", "Cannot open %1", page.viewer.fileName)
        explanation: page.viewer.errorMessage
    }
}
