// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.ki18n

// Control bar of a running slideshow. Covers the page but only watches the mouse, so the image below
// still gets all input. The bar and the mouse cursor appear when the mouse moves and hide again after
// a few seconds without movement.
Item {
    id: controls

    required property Slideshow slideshow
    required property Navigator navigator

    property bool barShown: false

    function revealBar() {
        barShown = true;
        hideTimer.restart();
    }

    onVisibleChanged: barShown = false

    HoverHandler {
        cursorShape: controls.barShown ? Qt.ArrowCursor : Qt.BlankCursor
        onPointChanged: controls.revealBar()
    }

    Timer {
        id: hideTimer

        interval: 3000
        // Keep the bar while the mouse is on it.
        onTriggered: controls.barShown = barHover.hovered
    }

    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Kirigami.Units.gridUnit
        width: barLayout.implicitWidth + Kirigami.Units.largeSpacing * 2
        height: barLayout.implicitHeight + Kirigami.Units.largeSpacing * 2

        opacity: controls.barShown ? 1 : 0
        visible: opacity > 0
        Behavior on opacity {
            NumberAnimation {
                duration: Kirigami.Units.longDuration
            }
        }

        radius: Kirigami.Units.cornerRadius
        color: Kirigami.Theme.backgroundColor

        HoverHandler {
            id: barHover
        }

        RowLayout {
            id: barLayout

            anchors.centerIn: parent
            spacing: Kirigami.Units.smallSpacing

            Controls.ToolButton {
                icon.name: "go-previous"
                text: KI18n.i18nc("@action:button Previous image", "Previous")
                display: Controls.AbstractButton.IconOnly
                enabled: controls.navigator.canGoToPrevious
                onClicked: controls.navigator.goToPrevious()

                Controls.ToolTip.text: text
                Controls.ToolTip.visible: hovered
                Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
            }

            Controls.ToolButton {
                icon.name: controls.slideshow.paused ? "media-playback-start" : "media-playback-pause"
                text: controls.slideshow.paused ? KI18n.i18nc("@action:button Slideshow", "Resume")
                                                : KI18n.i18nc("@action:button Slideshow", "Pause")
                display: Controls.AbstractButton.IconOnly
                onClicked: controls.slideshow.paused ? controls.slideshow.resume() : controls.slideshow.pause()

                Controls.ToolTip.text: text
                Controls.ToolTip.visible: hovered
                Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
            }

            Controls.ToolButton {
                icon.name: "go-next"
                text: KI18n.i18nc("@action:button Next image", "Next")
                display: Controls.AbstractButton.IconOnly
                enabled: controls.navigator.canGoToNext
                onClicked: controls.navigator.goToNext()

                Controls.ToolTip.text: text
                Controls.ToolTip.visible: hovered
                Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
            }

            Controls.Label {
                visible: controls.navigator.currentIndex >= 0
                text: KI18n.i18nc("@info %1 is the position of the image, %2 the number of images", "%1 of %2",
                                  controls.navigator.currentIndex + 1, controls.navigator.imageCount)
            }

            Kirigami.Separator {
                Layout.fillHeight: true
            }

            Controls.Label {
                text: KI18n.i18nc("@label:spinbox Time each image is shown", "Interval:")
            }

            // Plain digits, so the value can also be typed; the unit is a separate label.
            Controls.SpinBox {
                id: intervalSpinBox

                from: controls.slideshow.minimumIntervalSeconds
                to: controls.slideshow.maximumIntervalSeconds
                value: controls.slideshow.intervalSeconds
                editable: true
                validator: IntValidator {
                    bottom: intervalSpinBox.from
                    top: intervalSpinBox.to
                }
                textFromValue: (value, locale) => value.toString()
                valueFromText: (text, locale) => parseInt(text, 10)
                onValueModified: controls.slideshow.intervalSeconds = value
            }

            Controls.Label {
                text: KI18n.i18ncp("@label Unit of the slideshow interval", "second", "seconds", intervalSpinBox.value)
            }

            Kirigami.Separator {
                Layout.fillHeight: true
            }

            Controls.ToolButton {
                icon.name: "media-playback-stop"
                text: KI18n.i18nc("@action:button", "End Slideshow")
                onClicked: controls.slideshow.stop()
            }
        }
    }
}
