import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC
import QtQuick.Layouts 1.15

Rectangle {
    color: "#ffffff"
    border.color: "#e5e7eb"

    // 嵌入插件的 QWidget
    Item {
        id: widgetContainer
        anchors.fill: parent

        Component.onCompleted: {
            const pluginIndex = mainWindow.currentPluginIndex;
            const pluginWidget = availablePlugins[pluginIndex].toolWidget;
            if (pluginWidget) {
                // 将 QWidget 嵌入到 QML 中
                const window = mainWindow.windowHandle;
                window && pluginWidget.winId() && window.setTransientParent(pluginWidget);
                pluginWidget.setParent(widgetContainer);
                pluginWidget.resize(widgetContainer.width, widgetContainer.height);
            }
        }
    }
}
