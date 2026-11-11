// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 OesusDevelopmentStudios

// Lets the image components below use the page's ids.
pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami as Kirigami
import org.kde.ki18n

// Shows the current image with zoom and rotation, or a message when there is none.
// Previous / next step through the image's folder. During a slideshow the page presents the images:
// black background and a control bar that appears when the mouse moves.
Kirigami.Page {
    id: page

    required property Viewer viewer
    required property ViewTransform viewTransform
    required property Navigator navigator
    required property Slideshow slideshow

    readonly property bool presenting: slideshow.running

    title: {
        if (viewer.fileName.length === 0) {
            return KI18n.i18nc("@title", "Viewer");
        }
        if (navigator.currentIndex < 0) {
            return viewer.fileName;
        }
        return KI18n.i18nc("@title %1 is a file name, %2 its position in the folder, %3 the number of images",
                           "%1 (%2 of %3)", viewer.fileName, navigator.currentIndex + 1, navigator.imageCount);
    }
    padding: 0

    background: Rectangle {
        color: page.presenting ? "black" : Kirigami.Theme.backgroundColor
    }

    actions: [
        Kirigami.Action {
            icon.name: "go-previous"
            text: KI18n.i18nc("@action:intoolbar Previous image in the folder", "Previous")
            displayHint: Kirigami.DisplayHint.IconOnly
            shortcut: "Left"
            enabled: page.navigator.canGoToPrevious
            onTriggered: page.navigator.goToPrevious()
        },
        Kirigami.Action {
            icon.name: "go-next"
            text: KI18n.i18nc("@action:intoolbar Next image in the folder", "Next")
            displayHint: Kirigami.DisplayHint.IconOnly
            shortcut: "Right"
            enabled: page.navigator.canGoToNext
            onTriggered: page.navigator.goToNext()
        },
        Kirigami.Action {
            icon.name: "media-playback-start"
            text: KI18n.i18nc("@action:intoolbar", "Slideshow")
            enabled: page.viewer.hasImage

            Kirigami.Action {
                text: KI18n.i18nc("@action:inmenu Slideshow order", "By Name")
                onTriggered: page.slideshow.start(Slideshow.Order.ByName)
            }
            Kirigami.Action {
                text: KI18n.i18nc("@action:inmenu Slideshow order", "By Date Taken")
                onTriggered: page.slideshow.start(Slideshow.Order.ByDate)
            }
            Kirigami.Action {
                text: KI18n.i18nc("@action:inmenu Slideshow order", "In Random Order")
                onTriggered: page.slideshow.start(Slideshow.Order.Random)
            }
        },
        Kirigami.Action {
            icon.name: "zoom-out"
            text: KI18n.i18nc("@action:intoolbar", "Zoom Out")
            displayHint: Kirigami.DisplayHint.IconOnly
            shortcut: StandardKey.ZoomOut
            enabled: page.viewTransform.canZoomOut
            onTriggered: page.zoomAroundCenter(() => page.viewTransform.zoomOut())
        },
        Kirigami.Action {
            icon.name: "zoom-in"
            text: KI18n.i18nc("@action:intoolbar", "Zoom In")
            displayHint: Kirigami.DisplayHint.IconOnly
            shortcut: StandardKey.ZoomIn
            enabled: page.viewTransform.canZoomIn
            onTriggered: page.zoomAroundCenter(() => page.viewTransform.zoomIn())
        },
        Kirigami.Action {
            icon.name: "zoom-fit-best"
            text: KI18n.i18nc("@action:intoolbar Zoom so that the whole image is visible", "Fit")
            shortcut: "F"
            enabled: page.viewer.hasImage && !page.viewTransform.fitToWindow
            onTriggered: page.viewTransform.fitToWindow = true
        },
        Kirigami.Action {
            icon.name: "zoom-original"
            text: KI18n.i18nc("@action:intoolbar Zoom to 100 %", "Actual Size")
            shortcut: "Ctrl+0"
            enabled: page.viewer.hasImage && page.viewTransform.scale !== 1
            onTriggered: page.zoomAroundCenter(() => page.viewTransform.showActualSize())
        },
        Kirigami.Action {
            icon.name: "object-rotate-left"
            text: KI18n.i18nc("@action:intoolbar", "Rotate Left")
            displayHint: Kirigami.DisplayHint.IconOnly
            shortcut: "Ctrl+L"
            enabled: page.viewer.hasImage
            onTriggered: page.viewTransform.rotateCounterclockwise()
        },
        Kirigami.Action {
            icon.name: "object-rotate-right"
            text: KI18n.i18nc("@action:intoolbar", "Rotate Right")
            displayHint: Kirigami.DisplayHint.IconOnly
            shortcut: "Ctrl+R"
            enabled: page.viewer.hasImage
            onTriggered: page.viewTransform.rotateClockwise()
        }
    ]

    // Runs zoomAction while keeping the image point under viewportPoint where it is on screen.
    function zoomAround(viewportPoint: point, zoomAction: var) {
        const relativeX = (flickable.contentX + viewportPoint.x - imageFrame.x) / imageFrame.width;
        const relativeY = (flickable.contentY + viewportPoint.y - imageFrame.y) / imageFrame.height;

        zoomAction();  // updates the bindings below, including imageFrame's position and size

        const maximumX = flickable.contentWidth - flickable.width;
        const maximumY = flickable.contentHeight - flickable.height;
        flickable.contentX = Math.max(0, Math.min(imageFrame.x + relativeX * imageFrame.width - viewportPoint.x, maximumX));
        flickable.contentY = Math.max(0, Math.min(imageFrame.y + relativeY * imageFrame.height - viewportPoint.y, maximumY));
    }

    function zoomAroundCenter(zoomAction: var) {
        zoomAround(Qt.point(flickable.width / 2, flickable.height / 2), zoomAction);
    }

    Shortcut {
        sequences: [StandardKey.Cancel]
        enabled: page.presenting
        onActivated: page.slideshow.stop()
    }

    Shortcut {
        sequence: "Space"
        enabled: page.presenting
        onActivated: page.slideshow.paused ? page.slideshow.resume() : page.slideshow.pause()
    }

    Binding {
        target: page.viewTransform
        property: "viewportSize"
        value: Qt.size(flickable.width, flickable.height)
    }

    // Drag to pan. The content is at least as large as the viewport, so a small image stays centered.
    Flickable {
        id: flickable

        anchors.fill: parent
        visible: page.viewer.hasImage
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        contentWidth: Math.max(width, page.viewTransform.displayedSize.width)
        contentHeight: Math.max(height, page.viewTransform.displayedSize.height)

        // The rotated, scaled image as it appears on screen.
        Item {
            id: imageFrame

            x: (flickable.contentWidth - width) / 2
            y: (flickable.contentHeight - height) / 2
            width: page.viewTransform.displayedSize.width
            height: page.viewTransform.displayedSize.height

            Loader {
                id: imageLoader

                anchors.centerIn: parent
                width: page.viewTransform.imageSize.width * page.viewTransform.scale
                height: page.viewTransform.imageSize.height * page.viewTransform.scale
                rotation: page.viewTransform.rotationAngle

                // Image shows only the first frame of an animation.
                sourceComponent: page.viewer.imageAnimated ? animatedImage : stillImage
            }
        }

        // Handlers live in the content item, so event positions are in content coordinates.

        // Mouse wheel zooms around the cursor. WheelHandler accepts only mice by default, but on
        // Wayland a mouse wheel can arrive from a device reported as a touchpad.
        WheelHandler {
            target: null
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            onWheel: event => page.zoomAround(
                Qt.point(event.x - flickable.contentX, event.y - flickable.contentY),
                () => page.viewTransform.zoomBySteps(event.angleDelta.y / 120))  // 120 = one wheel notch
        }

        PinchHandler {
            id: pinchHandler

            target: null
            onScaleChanged: delta => page.zoomAround(
                Qt.point(pinchHandler.centroid.position.x - flickable.contentX,
                         pinchHandler.centroid.position.y - flickable.contentY),
                () => page.viewTransform.zoomBy(delta))
        }

        // Double-click switches between fit and 100 %.
        TapHandler {
            onDoubleTapped: eventPoint => page.zoomAround(
                Qt.point(eventPoint.position.x - flickable.contentX, eventPoint.position.y - flickable.contentY),
                () => page.viewTransform.fitToWindow ? page.viewTransform.showActualSize()
                                                     : page.viewTransform.fitToWindow = true)
        }
    }

    Component {
        id: stillImage

        Image {
            source: page.viewer.imageSource
            autoTransform: true  // apply the EXIF orientation
            asynchronous: true
            smooth: true
            mipmap: true
        }
    }

    Component {
        id: animatedImage

        AnimatedImage {
            source: page.viewer.imageSource
            smooth: true
        }
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
        visible: page.viewer.errorMessage.length > 0 || (imageLoader.item as Image)?.status === Image.Error

        icon.name: "data-error"
        text: KI18n.i18nc("@info:placeholder %1 is a file name", "Cannot open %1", page.viewer.fileName)
        explanation: page.viewer.errorMessage
    }

    SlideshowControls {
        anchors.fill: parent
        visible: page.presenting
        slideshow: page.slideshow
        navigator: page.navigator
    }
}
