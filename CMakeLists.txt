cmake_minimum_required(VERSION 3.5)

project(chat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Concurrent)

#定义头文件路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/protobuf)
include_directories(${PROJECT_SOURCE_DIR}/third)
include_directories(${PROJECT_SOURCE_DIR}/third/google/protobuf)

#定义所有编译的源文件
file(GLOB SRC "./src/*.*" "./src/*/*.*" "./*.ui" "./*.qrc" "./lib/*")
file(GLOB INC "./include/*.*" "./include/*/*.*" "./third/*.*")

set(PROJECT_SOURCES
        ${SRC}
        ${INC}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(chat
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET chat APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(chat SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(chat
            ${PROJECT_SOURCES}
        )
    endif()
endif()


find_library(var protobuf HINTS ${PROJECT_SOURCE_DIR}/lib)

target_link_libraries(chat PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Concurrent
    protobuf
)

set_target_properties(chat PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS chat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(chat)
endif()
