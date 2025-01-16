import QtQuick
import QtQml.Models
import QtQuick.Controls
import QtQuick.Layouts

import org.deepin.dtk 1.0

import agent

Instantiator {
    id: windowInstantiator

    model: Agent.model

    delegate: DialogWindow {
        id: window
        minimumWidth: 400
        visible: true
        required property var model

        ColumnLayout {
            id: content
            width: parent.width
            spacing: 10

            Label {
                Layout.preferredWidth: parent.width
                font: DTK.fontManager.t5
                wrapMode: Text.WordWrap
                horizontalAlignment: Qt.AlignHCenter

                text: model.description
            }
            Label {
                Layout.preferredWidth: parent.width
                font: DTK.fontManager.t5
                wrapMode: Text.WordWrap
                horizontalAlignment: Qt.AlignHCenter

                text: model.message
            }

            function triggerAction(actionIndex) {
                Agent.model.trigger(model.cookie, actionIndex)
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 10
                Layout.bottomMargin: 10

                spacing: 10

                Repeater {
                    model: {
                        return window.model.actions
                    }

                    Button {
                        text: modelData
                        Layout.fillWidth: true

                        onClicked: {
                            console.log(index)
                            content.triggerAction(index)
                        }
                    }
                }
            }
        }
    }
}
