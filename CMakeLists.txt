cmake_minimum_required(VERSION 3.16)
project(ModernWindow VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置自动生成moc文件
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 查找Qt包
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# 源文件列表
set(PROJECT_SOURCES
    main.cpp
    nmainwindow.cpp
    nmainwindow.h
    mainwindow.cpp
    mainwindow.h
)

# 平台特定源文件
if(WIN32)
    list(APPEND PROJECT_SOURCES
        windows_effect.cpp
        windows_effect.h
    )
endif()

# 创建可执行文件
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# 链接Qt库
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

# 平台特定链接
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE dwmapi user32 uxtheme)
endif()