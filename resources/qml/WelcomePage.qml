import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15 as Material
import "./components"

Rectangle {
    anchors.fill: parent
    color: "#f9fafb"
    padding: 32

    Column {
        anchors.centerIn: parent
        spacing: 16

        Image {
            source: "qrc:/icons/devmagic-logo.svg"
            width: 256
            height: 256
        }

        QQC.Label {
            text: qsTr("开发者的魔法工具箱")
            font.pixelSize: 28
            color: "#1a1a1a"
            fontWeight: Font.Bold
        }

        QQC.Label {
            text: qsTr("已加载 %1 个工具").arg(toolList.length)
            font.pixelSize: 14
            color: "#646c75"
            marginTop: 8
        }

        DevPathInfo { // 调试用路径显示
            visible: qmlEngine(engine).debugging
            appRoot: APP_ROOT
        }

        QQC.Button {
            text: qsTr("进入主界面")
            width: 200
            onClicked: stackView.currentIndex = 1
        }
    }
}
