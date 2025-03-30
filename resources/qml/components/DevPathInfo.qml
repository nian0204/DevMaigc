import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC

Rectangle {
    id: devPathInfo
    width: parent ? parent.width : 400 // 如果父对象存在，宽度与父对象一致
    height: 64
    color: "#ffffff" // 背景颜色
    border.color: "#e5e7eb" // 边框颜色
    radius: 4

    Row {
        spacing: 8
        anchors.fill: parent
        anchors.margins: 16 // 使用 anchors.margins 替代 margin

        Image {
            source: "qrc:/icons/folder.svg"
            width: 16
            height: 16
        }

        QQC.Label {
            text: qsTr("数据目录: %1").arg(appRoot)
            font.family: "Monospace"
            font.pixelSize: 12
            color: "#374151" // 文字颜色
        }
    }
}
