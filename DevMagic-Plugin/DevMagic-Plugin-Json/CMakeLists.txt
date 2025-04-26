cmake_minimum_required(VERSION 3.22)
project(LandcDevMagicPluginJson LANGUAGES CXX)

#### **核心配置：指定LLVM-Mingw**
set(CMAKE_CXX_COMPILER "Z:/software/qt/Tools/llvm-mingw1706_64/bin/clang++.exe" CACHE PATH "C++ Compiler")
set(CMAKE_C_COMPILER "Z:/software/qt/Tools/llvm-mingw1706_64/bin/clang.exe" CACHE PATH "C Compiler")


#### **指定Qt6路径（LLVM-Mingw专用）**
set(Qt6_DIR "Z:/software/qt/6.8.3/llvm-mingw_64/lib/cmake/Qt6" CACHE PATH "Qt6 LLVM-Mingw路径")
find_package(Qt6 REQUIRED COMPONENTS
    Core Gui Widgets Qml Quick QuickControls2 Network Xml
)
#### **C++标准与编译选项**
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(
    -Wall -Wextra -Wpedantic
    -fms-extensions  # 启用Microsoft扩展（如__stdcall）
)

# ========== 源文件与头文件 ==========
set(SOURCES
    src/main.cpp
    src/jsonplugin.cpp  # 唯一编译单元
    src/jsoneditor.cpp
    src/expandbutton.cpp
)

set(CMAKE_AUTOMOC ON)        # 启用元对象编译
set(CMAKE_AUTORCC ON)        # 自动处理QRC资源
set(CMAKE_INCLUDE_CURRENT_DIR ON) # 包含当前目录


# 2. 定义资源清单（包含图标和元数据）
qt6_add_resources(RESOURCES
    "resources/icons.qrc"
)

# ========== 插件目标配置 ==========
# add_library(LandcDevMagicPluginJson SHARED ${SOURCES} ${RESOURCES}
#     src/linenumberarea.h
#     src/jsoneditor.cpp

#     src/javaconverter.h
#     src/goconverter.h
#     src/xmlconverter.h
#     src/jsonhighlighter.h
#     src/expandbutton.h
# )

add_executable(LandcDevMagicPluginJson ${SOURCES} ${RESOURCES}
    src/main.cpp
    src/linenumberarea.h
    src/jsoneditor.cpp

    src/jsoneditor.h
    src/javaconverter.h
    src/goconverter.h
    src/xmlconverter.h
    src/jsonhighlighter.h
    src/expandbutton.h
    src/expandbutton.cpp
    src/jsonhighlighter.cpp
    include/devplugin.h
)

# 设置头文件目录（必须在 add_library 之后）
target_include_directories(LandcDevMagicPluginJson PRIVATE
    src
    include
)

# 设置输出名称和目录
# set_target_properties(LandcDevMagicPluginJson PROPERTIES
#     OUTPUT_NAME "LandcDevMagicPluginJson"  # 最终DLL/so名称
#     LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/plugins"
#     CXX_VISIBILITY_PRESET hidden  # 隐藏非导出符号
# )

# ========== 链接与编译选项 ==========
target_link_libraries(LandcDevMagicPluginJson PRIVATE
    Qt6::Widgets
    Qt6::Xml
    Qt6::Core
)

# target_sources(LandcDevMagicPluginJson PRIVATE
#     ${QRC_RESOURCES}
# )
# install(TARGETS LandcDevMagicPluginJson
#     RUNTIME DESTINATION "plugins"  COMPONENT runtime  # Windows DLL
#     LIBRARY DESTINATION "plugins"  COMPONENT runtime  # Linux/macOS 动态库
#     ARCHIVE DESTINATION "lib"      COMPONENT dev      # 静态库（可选）
# )
