import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15 as Material

// 自定义功能卡片组件
QQC.Frame {
    id: functionalCard
    property string title: ""
    property string iconName: ""

    width: 280
    height: 200
    radius: 8
    border.color: Material.color(Material.OnSurface, 0.12)
    background: Material.color(Material.Surface)
    elevation: 2

    signal clicked()

    Column {
        anchors.centerIn: parent
        spacing: 12
        padding: 16

        Image { // 使用 Image 替代 QQC.Icon
            source: iconName
            width: 48
            height: 48
            fillMode: Image.PreserveAspectFit
            color: Material.color(Material.Primary)
        }

        QQC.Label {
            text: title
            font.bold: true
            color: Material.color(Material.OnSurface, 0.87)
            horizontalAlignment: Qt.AlignHCenter
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            functionalCard.clicked();
            parent.elevation = 4;
        }
        onReleased: parent.elevation = 2;
    }
}
