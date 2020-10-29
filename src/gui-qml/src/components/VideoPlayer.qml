import QtMultimedia 5.12
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

ColumnLayout {
    property alias source: video.source
    property alias fillMode: video.fillMode
    property alias autoPlay: video.autoPlay

    onAutoPlayChanged: {
        if (autoPlay && video.playbackState != MediaPlayer.PlayingState) {
            video.play()
        }
    }

    Video {
        id: video

        Layout.fillHeight: true
        Layout.fillWidth: true
        notifyInterval: 100
        loops: MediaPlayer.Infinite
    }

    RowLayout {
        Layout.fillWidth: true

        ToolButton {
            icon.source: video.playbackState == MediaPlayer.PlayingState ? "/images/icons/pause.png" : "/images/icons/play.png"
            onClicked: video.playbackState == MediaPlayer.PlayingState ? video.pause() : video.play()
        }

        Slider {
            from: 0
            to: video.duration
            live: false
            value: video.position
            Layout.fillWidth: true

            onValueChanged: if (value !== video.position) video.seek(value)
        }
    }
}
