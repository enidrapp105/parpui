cmake_minimum_required(VERSION 3.16)

project(PARPUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)

FetchContent_Declare(
    parp
    GIT_REPOSITORY https://github.com/enidrapp105/parp
    GIT_TAG main
)

FetchContent_Declare(
    QHotkey
    GIT_REPOSITORY https://github.com/Skycoder42/QHotkey.git
    GIT_TAG master
)


FetchContent_MakeAvailable(parp QHotkey)
FetchContent_GetProperties(parp)
message(STATUS "parp source dir: ${parp_SOURCE_DIR}")


find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(SQLite3 REQUIRED)



qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(appPARPUI
    main.cpp
)

qt_add_qml_module(appPARPUI
    URI PARPUI
    QML_FILES
        Main.qml
        MenuContextBar.qml
        MenuHelp.qml
        MenuDevice.qml
        MenuSound.qml
        MenuFile.qml
        VolumeControl.qml
        CheckboxControl.qml
    SOURCES
        sqldatabase.h
        sqldatabase.cpp
        parpui.h
        backend.h
        backend.cpp
        globalshortcutmanager.h
        globalshortcutmanager.cpp
)

target_include_directories(appPARPUI PRIVATE ${SQLite3_INCLUDE_DIR})

target_include_directories(qhotkey
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/QHotkey>
)

target_compile_definitions(appPARPUI PRIVATE
    USE_CMAKE
    PARP_SOURCE_DIR="${parp_SOURCE_DIR}"
)

# file(COPY sounds DESTINATION ${CMAKE_BINARY_DIR})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appPARPUI PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appPARPUI
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appPARPUI
    PRIVATE
        Qt6::Concurrent
        Qt6::Quick
        parp
        QHotkey::QHotkey
        ${SQLite3_LIBRARIES}
)

install(TARGETS appPARPUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
