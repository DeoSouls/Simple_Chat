cmake_minimum_required(VERSION 3.16)

project(SimpleChat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
        src/main.cpp
        MainWindow/mainwindow.cpp
        MainWindow/mainwindow.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SimpleChat
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        LoginForm/loginform.h LoginForm/loginform.cpp
        ChatForm/chatform.h ChatForm/chatform.cpp
        LoginForm/loginbutton.h LoginForm/loginbutton.cpp
        ChatForm/SwitchChatContainer/switchchatcontainer.h ChatForm/SwitchChatContainer/switchchatcontainer.cpp
        ChatForm/SwitchChatContainer/switchchatbutton.h ChatForm/SwitchChatContainer/switchchatbutton.cpp
        ChatForm/ChatContent/chatcontent.h ChatForm/ChatContent/chatcontent.cpp
        ChatForm/ChatContent/InputMessage/inputmessage.h ChatForm/ChatContent/InputMessage/inputmessage.cpp
        ChatForm/ChatContent/MessageContainer/messagecontainer.h ChatForm/ChatContent/MessageContainer/messagecontainer.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SimpleChat APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(SimpleChat SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SimpleChat
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(SimpleChat PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SimpleChat)
endif()
set_target_properties(SimpleChat PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SimpleChat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SimpleChat)
endif()
