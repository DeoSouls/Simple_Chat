# tests/CMakeLists.txt

# Включение тестирования
enable_testing()

# Поиск пакета Qt Test
find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)

# Добавление исполняемого файла для тестов как консольного приложения
qt_add_executable(SimpleChatTests
    MANUAL_FINALIZATION
    test_chatcontent.cpp
    test_chatcontent.h
    # Добавьте другие тестовые файлы здесь, если есть
)

# Установка включаемых директорий
target_include_directories(SimpleChatTests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/MainWindow
    ${CMAKE_SOURCE_DIR}/LoginForm
    ${CMAKE_SOURCE_DIR}/ChatForm
    ${CMAKE_SOURCE_DIR}/ChatForm/SwitchChatContainer
    ${CMAKE_SOURCE_DIR}/ChatForm/ChatContent
    ${CMAKE_SOURCE_DIR}/ChatForm/UserChoiceContainer
)

# Линковка с Qt Test, Qt Widgets и основной библиотекой
target_link_libraries(SimpleChatTests PRIVATE
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::WebSockets
    SimpleChatLib
)

# Установка типа исполняемого файла как консольного
set_target_properties(SimpleChatTests PROPERTIES WIN32_EXECUTABLE OFF)

# Добавление тестов в CTest
add_test(NAME SimpleChatTests COMMAND SimpleChatTests)
