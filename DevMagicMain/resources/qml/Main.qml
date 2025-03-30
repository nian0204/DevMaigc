import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC
import QtQuick.Layouts 1.15

QQC.ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1280
    height: 720
    title: qsTr("DevMagic")
    color: "#f9fafb" // 背景颜色

    // 全局字体设置
    font.family: "Segoe UI"
    font.pointSize: 14

    // 当前选中的插件索引
    property int currentPluginIndex: -1

    // 顶部工具栏
    // QQC.ToolBar {
    //     id: toolbar
    //     anchors.top: parent.top
    //     width: parent.width
    //     height: 48
    //     background: Rectangle { // 工具栏背景颜色
    //         color: "#ffffff"
    //         border.color: "#e5e7eb"
    //     }

    //     RowLayout {
    //         anchors.fill: parent
    //         spacing: 16

    //         QQC.Button {
    //             text: qsTr("☰") // 菜单按钮
    //             onClicked: sidebar.visible = !sidebar.visible // 切换侧边栏显示/隐藏
    //             background: Rectangle {
    //                 color: "#6200EE" // Material Primary Color
    //                 radius: 4
    //             }
    //             palette.text: "#FFFFFF" // Material OnPrimary Color
    //         }

    //         QQC.Label {
    //             text: qsTr("DevMagic")
    //             font.bold: true
    //             color: "#374151" // 文字颜色
    //             Layout.fillWidth: true
    //             horizontalAlignment: Qt.AlignHCenter
    //         }
    //     }
    // }

    // 主界面布局
    RowLayout {
        anchors.fill: parent
        anchors.top: toolbar.bottom
        anchors.bottom: statusBar.top

        // 左侧边栏：插件列表
        Rectangle {
            id: sidebarBackground
            width: 200
            Layout.preferredWidth: 200
            Layout.fillHeight: true
            visible: true // 默认显示
            color: "#ffffff" // 背景颜色
            border.color: "#e5e7eb"

            Column {
                id: sidebar
                anchors.fill: parent
                spacing: 0

                ListView {
                    id: pluginList
                    anchors.fill: parent
                    model: availablePlugins
                    clip: true

                    delegate: Rectangle {
                        width: parent.width
                        height: 48
                        color: ListView.isCurrentItem ? "#f0f4f8" : "#ffffff" // 高亮当前项
                        border.color: "#e5e7eb"

                        RowLayout {
                            anchors.fill: parent
                            spacing: 8

                            Image {
                                source: modelData["toolIcon"] ? modelData["toolIcon"] : "" // 显示插件图标
                                width: 32
                                height: 32
                                fillMode: Image.PreserveAspectFit
                            }

                            QQC.Label {
                                text: modelData["toolName"]
                                font.pixelSize: 14
                                color: "#374151"
                                Layout.fillWidth: true
                                verticalAlignment: Qt.AlignVCenter
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                currentPluginIndex = index // 更新当前选中的插件索引
                                loader.sourceComponent = null // 清空旧内容
                                loader.source = "qrc:/qml/components/PluginWrapper.qml" // 加载通用插件容器
                            }
                        }
                    }
                }
            }
        }

        // 右侧主区域：插件页面
        Loader {
            id: loader
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    // 底部状态栏
    // Rectangle {
    //     id: statusBar
    //     anchors.bottom: parent.bottom
    //     width: parent.width
    //     height: 32
    //     color: "#ffffff" // 背景颜色
    //     border.color: "#e5e7eb"

    //     QQC.Label {
    //         text: qsTr("Version %1").arg(appVersion)
    //         anchors.centerIn: parent
    //         color: "#6b7280" // 文字颜色
    //     }
    // }
}
