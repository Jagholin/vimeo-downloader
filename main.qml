import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2

import "applogic.js" as Applogic

ApplicationWindow {
    visible: true
    width: 900
    height: 750
    title: qsTr("Vimeo downloader")

    FileDialog {
        id: saveFileDialog
        title: qsTr("Chose a location to save the file")
        folder: shortcuts.movies
        selectExisting: false
        selectMultiple: false
        selectFolder: false
        nameFilters: ["Video file (*.mp4)"]
    }

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Layout.alignment: Qt.AlignTop
            Label {
                text: qsTr("Insert data here")
            }
            TextField {
                Layout.fillWidth: true
                id: jsonData
            }
            Button {
                id: actionButton
                text: "Go"
                onClicked: {
                    Applogic.parseJSON(jsonData.text)
                }
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignTop
            Label {
                text: qsTr("Video URL")
                Layout.minimumWidth: 250
            }
            TextField {
                Layout.fillWidth: true
                id: videoUrl
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignTop
            Label {
                text: qsTr("Segments")
                Layout.minimumWidth: 250
            }
            TextField {
                Layout.fillWidth: true
                id: videoSegments
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignTop
            Label {
                text: qsTr("Width")
                Layout.minimumWidth: 250
            }
            TextField {
                Layout.fillWidth: true
                id: videoWidth
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignTop
            Label {
                text: qsTr("Height")
                Layout.minimumWidth: 250
            }
            TextField {
                Layout.fillWidth: true
                id: videoHeight
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignTop
            Label {
                text: qsTr("InitSegment")
                Layout.minimumWidth: 250
            }
            TextField {
                Layout.fillWidth: true
                id: videoInitSegment
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignTop
            Label {
                text: qsTr("video ID")
                Layout.minimumWidth: 250
            }
            TextField {
                Layout.fillWidth: true
                id: videoID
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignTop
            Label {
                text: qsTr("Audio URL")
                Layout.minimumWidth: 250
            }
            TextField {
                Layout.fillWidth: true
                id: audioUrl
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignTop
            Label {
                text: qsTr("Segments")
                Layout.minimumWidth: 250
            }
            TextField {
                Layout.fillWidth: true
                id: audioSegments
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignTop
            Label {
                text: qsTr("InitSegment")
                Layout.minimumWidth: 250
            }
            TextField {
                Layout.fillWidth: true
                id: audioInitSegment
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignTop
            Label {
                text: qsTr("audio ID")
                Layout.minimumWidth: 250
            }
            TextField {
                Layout.fillWidth: true
                id: audioID
            }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            id: trect

            Flickable {
                id: flick
                anchors.fill: parent

                width: trect.width
                height: trect.height
                x: 0
                y: 0

                contentWidth: stdOutput.paintedWidth
                contentHeight: stdOutput.paintedHeight

                function goToBottom() {
                    contentX = 0;
                    contentY = contentHeight - height;
                }

                TextArea {
                    anchors.fill: trect
                    id: stdOutput
                    text: "<Notifications and error messages appear here>"
                    readOnly: true
                    background: Rectangle {
                        anchors.fill: parent
                        color: "lightblue"
                        border.width: 1
                        border.color: "black"
                    }

                    onTextChanged: flick.goToBottom()
                }
            }
        }

    }

    footer: Label {
        id: downLabel
        text: qsTr("insert videodata JSON to continue")
    }

    Component.onCompleted: {
        downloadInitiator.stdOutput.connect(function(outputLine){
            stdOutput.insert(stdOutput.length, outputLine)
        });
        downloadInitiator.progressReport.connect(function(progressLine){
            downLabel.text = progressLine;
        });
    }
}
