cmake_minimum_required(VERSION 3.26)
project(DevMagic VERSION 1.0.0 LANGUAGES CXX)

#### **核心配置：指定LLVM-Mingw**
set(CMAKE_CXX_COMPILER "Z:/software/qt/Tools/llvm-mingw1706_64/bin/clang++.exe" CACHE PATH "C++ Compiler")
set(CMAKE_C_COMPILER "Z:/software/qt/Tools/llvm-mingw1706_64/bin/clang.exe" CACHE PATH "C Compiler")

#### **指定Qt6路径（LLVM-Mingw专用）**
set(Qt6_DIR "Z:/software/qt/6.8.3/llvm-mingw_64/lib/cmake/Qt6" CACHE PATH "Qt6 LLVM-Mingw路径")
find_package(Qt6 REQUIRED COMPONENTS
    Core Gui Widgets Network
)

#### **C++标准与编译选项**
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(
    -Wall -Wextra -Wpedantic
    -fms-extensions  # 启用Microsoft扩展（如__stdcall）
)

#### **链接器配置（LLVM-Mingw专用）**
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")  # 使用LLD链接器

#### **源文件与资源**
set(SOURCES
    src/main.cpp
    src/devplugin.h
    src/mainwindow.h
    src/mainwindow.cpp
)

set(CMAKE_AUTOMOC ON)        # 启用元对象编译
set(CMAKE_INCLUDE_CURRENT_DIR ON) # 包含当前目录

#### **主程序目标**
add_executable(DevMagic
    ${SOURCES}
    src/configmanager.h src/configmanager.cpp
    src/pluginsmanager.h src/pluginsmanager.cpp
    src/application.h
    src/settingsdialog.h src/settingsdialog.cpp
    src/pluginmanagerdialog.h src/pluginmanagerdialog.cpp
    src/listwidegt.h src/listwidegt.cpp
    src/scheckboxheaderview.h

)
target_link_libraries(DevMagic PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)
target_include_directories(DevMagic PRIVATE
    src
    ${Qt6Core_INCLUDE_DIRS}
)

# 移除宏定义，改用目标属性
set_target_properties(DevMagic PROPERTIES
    APP_NAME "DevMagic"
)
