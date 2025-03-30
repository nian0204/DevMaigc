#include <QApplication>
#include "jsonplugin.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 创建主窗口
    JsonToolWidget mainWindow;
    mainWindow.setWindowTitle("JSON 工具插件");
    mainWindow.resize(1200, 800); // 设置窗口大小
    mainWindow.show();

    return app.exec();
}
