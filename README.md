# qt_spdlog

Интеграция spdlog с Qt для эффективного логирования в C++ приложениях.

Быстрый старт

```cpp
#include "qt_spdlog.h"

// Инициализация
auto logger = spdlog::stdout_color_mt("app");
spdlog::set_default_logger(logger);

// Уровни логирования
qt_spdlog::set_level("debug");
qt_spdlog::set_level(QtMsgType::QtWarningMsg);

// Форматы вывода
qt_spdlog::set_default_pattern();    // [time] [level] message
qt_spdlog::set_qt_style_pattern();   // Компактный формат

// Интеграция с Qt
qt_spdlog::setup_qt_message_handler(); // qDebug() → spdlog

// Базовое использование
QT_LOG_INFO("Запуск приложения");
QT_LOG_ERROR("Ошибка: {}", error_msg);
```

Основные макросы

```cpp
QT_LOG_TRACE(...)    // Детальная отладка
QT_LOG_DEBUG(...)    // Отладочная информация
QT_LOG_INFO(...)     // Информационные сообщения
QT_LOG_WARN(...)     // Предупреждения
QT_LOG_ERROR(...)    // Ошибки
QT_LOG_CRITICAL(...) // Критические ошибки
QT_LOG_ALWAYS(...)   // Всегда отображается
```

Логирование исключений

```cpp
try {
    throw std::runtime_error("Ошибка");
} catch (const std::exception& e) {
    QT_LOG_EXCEPTION_ERROR(e, "контекст");
}

// Qt исключения
QT_LOG_QEXCEPTION_WARN(e, "UI операция");
```


Поддерживаемые типы

   - Qt типы: QString, QByteArray, QVariant, QMap, QList, QVector

   - STL типы: std::string, контейнеры, исключения

Особенности

- Автоматическое форматирование Qt и STL типов
- Безопасность временных объектов
- Thread-local логирование  
- JSON логирование
- Scoped уровни и модули
- Интеграция с Qt Message System

Сборка

```cmake

cmake_minimum_required(VERSION 3.16)
project(MyApp)

# Зависимости
find_package(Qt6 REQUIRED COMPONENTS Core)

include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.10.0
)
FetchContent_MakeAvailable(spdlog)

# Целевое приложение
add_executable(my_app main.cpp)
target_link_libraries(my_app 
    Qt6::Core 
    spdlog::spdlog
)

# Включение qt_spdlog
target_include_directories(my_app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```
