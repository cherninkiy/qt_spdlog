cmake_minimum_required(VERSION 3.16)
project(QtSpdlogDemo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core)

# Структура проекта
set(INCLUDE_DIR include)
set(SOURCE_DIR src)

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

# Настройки компилятора
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-dangling-reference)
endif()

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)

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
