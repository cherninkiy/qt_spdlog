cmake_minimum_required(VERSION 3.16)
project(QtSpdlogDemo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test)

# Структура проекта
set(INCLUDE_DIR include)
set(SOURCE_DIR src)
set(TEST_DIR tests)
set(TEST_PROJECT_NAME QtSpdlogTests)

# Настройки spdlog
set(SPDLOG_BUILD_EXAMPLE OFF)
set(SPDLOG_BUILD_TESTS OFF)
set(SPDLOG_INSTALL OFF)

# Установка базовой директории для FetchContent
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)

include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.10.0
)
FetchContent_MakeAvailable(spdlog)

# Основное приложение
set(SOURCES
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/loggerdemo.cpp
)

set(HEADERS
    ${INCLUDE_DIR}/qt_spdlog.h
    ${SOURCE_DIR}/loggerdemo.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    spdlog::spdlog
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${SOURCE_DIR}
    ${INCLUDE_DIR}
)

# Тесты
if(Qt6Test_FOUND)
    set(TEST_SOURCES
        ${TEST_DIR}/test_qt_spdlog.cpp
    )

    add_executable(${TEST_PROJECT_NAME} ${TEST_SOURCES})

    target_link_libraries(${TEST_PROJECT_NAME}
        Qt6::Test
        Qt6::Core
        spdlog::spdlog
    )

    target_include_directories(${TEST_PROJECT_NAME}
        PRIVATE
        ${INCLUDE_DIR}
    )

    # Добавляем тест в CTest
    add_test(NAME ${TEST_PROJECT_NAME} COMMAND ${TEST_PROJECT_NAME})
endif()

# Настройки компилятора
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
    if(Qt6Test_FOUND)
        target_compile_options(${TEST_PROJECT_NAME} PRIVATE /W4 /permissive-)
    endif()
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    if(Qt6Test_FOUND)
        target_compile_options(${TEST_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-dangling-reference)
    if(Qt6Test_FOUND)
        target_compile_options(${TEST_PROJECT_NAME} PRIVATE -Wno-dangling-reference)
    endif()
endif()

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    if(Qt6Test_FOUND)
        target_compile_definitions(${TEST_PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()

    # Использование windeployqt для развертывания Qt библиотек
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin)

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE}
                \"$<TARGET_FILE_DIR:${PROJECT_NAME}>\"
            COMMENT "Deploying Qt libraries using windeployqt"
        )
    else()
        message(WARNING "windeployqt not found, Qt libraries won't be deployed automatically")
    endif()
endif()
