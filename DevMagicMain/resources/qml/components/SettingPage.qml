import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15 as Material

// 设置界面组件
Rectangle {
    color: Material.color(Material.Background)
    padding: 24

    ColumnLayout {
        spacing: 16
        anchors.fill: parent

        QQC.Switch {
            id: darkModeSwitch
            text: qsTr("深色模式")
            checked: devConfig.theme === "dark"
            onCheckedChanged: devConfig.setTheme(checked ? "dark" : "light")
        }

        QQC.ComboBox {
            id: languageCombo
            model: [qsTr("English"), qsTr("简体中文")]
            currentIndex: devConfig.language === "zh_CN" ? 1 : 0
            onCurrentIndexChanged: devConfig.setLanguage(index === 1 ? "zh_CN" : "en_US")
        }
    }
}
