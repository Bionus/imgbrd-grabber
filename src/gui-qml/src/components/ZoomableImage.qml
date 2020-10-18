import QtQuick 2.12
import QtQuick.Controls 2.5

Flickable {
    id: flickable

    property alias source: image.source
    property alias status: image.status
    property alias progress: image.progress

    property int currentZoomStep: 0
    property real currentZoom: 1.0
    property var zoomSteps: [1.0, 1.75, 2.5]
    property var zoomScrollStep: 0.2

    contentWidth: imageContainer.width
    contentHeight: imageContainer.height

    Text {
        text: "zoom: " + currentZoom + " / step: " + currentZoomStep
        height: 50
    }

    Item {
        id: imageContainer

        width: Math.max(image.width * image.scale, flickable.width)
        height: Math.max(image.height * image.scale, flickable.height)

        Image {
            id: image

            source: fileUrl
            fillMode: Image.PreserveAspectFit
            anchors.centerIn: parent

            onStatusChanged: {
                if (status == Image.Ready) {
                    scale = Math.min(flickable.width / width, flickable.height / height);
                    pinchArea.minScale = scale
                    pinchArea.maxScale = scale * zoomSteps[zoomSteps.length - 1]
                    flickable.returnToBounds()
                }
            }
        }
    }

    PinchArea {
        id: pinchArea

        property real minScale: 1.0
        property real maxScale: 2.0

        anchors.fill: parent
        pinch.target: image
        pinch.minimumScale: minScale
        pinch.maximumScale: maxScale

        onPinchFinished: flickable.returnToBounds()
    }

    MouseArea {
        anchors.fill: parent

        onDoubleClicked: {
            currentZoomStep = (currentZoomStep + 1) % zoomSteps.length
            currentZoom = zoomSteps[currentZoomStep]

            image.scale = pinchArea.minScale * currentZoom

            if (currentZoomStep == 0) {
                flickable.returnToBounds()
            }
        }

        onWheel: {
            var newZoom = currentZoom + zoomScrollStep * (wheel.angleDelta.y / 120)
            currentZoom = Math.max(1.0, Math.min(zoomSteps[zoomSteps.length - 1], newZoom))

            if (wheel.angleDelta.y > 0) {
                while (currentZoomStep < zoomSteps.length - 1 && currentZoom >= zoomSteps[currentZoomStep + 1]) {
                    currentZoomStep++;
                }
            } else {
                while (currentZoomStep > 0 && currentZoom <= zoomSteps[currentZoomStep - 1]) {
                    currentZoomStep--;
                }
            }

            image.scale = pinchArea.minScale * currentZoom
        }
    }
}
