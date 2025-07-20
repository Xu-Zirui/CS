cmake_minimum_required(VERSION 3.16)

project(QtComputeShaderDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 使用 Qt6 包
find_package(Qt6 REQUIRED COMPONENTS Core Gui OpenGLWidgets Widgets)

qt_standard_project_setup()

# 添加资源文件
qt_add_resources(QRC_FILES shaders.qrc)

# 添加可执行文件
qt_add_executable(QtComputeShaderDemo
    main.cpp



    ${QRC_FILES}
    shaders.qrc
    shaders.qrc


    ComputePipeline.h
    ComputeShader.h
    SSBO.h
    demowidget.h demowidget.cpp
    demowidget.h demowidget.cpp
    GLBufferObject.h
    computepipeline.h computepipeline.cpp
    GLBufferObject.cpp
    csresourcemanager.h csresourcemanager.cpp
)

# 链接 Qt 库
target_link_libraries(QtComputeShaderDemo
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGLWidgets
)
