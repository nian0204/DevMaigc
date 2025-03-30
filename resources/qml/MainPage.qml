import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15 as Material
import "./components"

Rectangle {
    anchors.fill: parent
    color: Material.color(Material.Background)
    padding: 24

    GridLayout {
        columns: 4
        rowSpacing: 24
        anchors.fill: parent

        Repeater {
            model: toolList // 假设 toolList 是从 C++ 层传递的模型
            delegate: FunctionalCard {
                title: modelData.title
                iconName: modelData.iconName
                onClicked: {
                    console.log("Tool clicked:", title);
                    stackView.push(createToolWidget()); // 假设 createToolWidget 是插件提供的方法
                }
            }
        }
    }
}
