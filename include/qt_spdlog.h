#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/fmt/bundled/format.h>
#include <QString>
#include <QMap>
#include <QList>
#include <QVector>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QStringList>
#include <QByteArray>
#include <QDateTime>
#include <QException>
#include <QThread>
#include <QThreadStorage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <type_traits>
#include <tuple>
#include <thread>
#include <iostream>

// Использовать для настройки spdlog
// Напр.
// #define SPDLOG_LEVEL_NAMES { "trace", "debug", "info", "warn", "error", "critical", "always" }
// #define SPDLOG_SHORT_LEVEL_NAMES { "T", "D", "I", "W", "E", "C", "A" }
#include <spdlog/tweakme.h>


// Предварительные объявления форматтеров
namespace qt_spdlog::formatters {
template<typename T>
QString formatNum(const T& value);
template<typename T>
QString formatListNums(const QList<T>& list);
template<typename T>
QString formatVectorNums(const QVector<T>& vec);
template<typename T>
QString formatMapStrNums(const QMap<QString, T>& map);
template<typename T>
QString formatMapNumStrs(const QMap<T, QString>& map);
QString formatQStringList(const QStringList& list);
QString formatQByteArray(const QByteArray& data, bool showHex = false);
QString formatQVariant(const QVariant& variant);
QString formatQVariantList(const QVariantList& list);
QString formatQVariantMap(const QVariantMap& map);
}

// Сначала специализации formatter ДО их использования
namespace fmt {

template <>
struct formatter<QVariant> : formatter<std::string> {
    auto format(const QVariant& variant, format_context& ctx) {
        auto str = qt_spdlog::formatters::formatQVariant(variant).toStdString();
        return formatter<std::string>::format(str, ctx);
    }
};

template <>
struct formatter<QVariantList> : formatter<std::string> {
    auto format(const QVariantList& list, format_context& ctx) {
        auto str = qt_spdlog::formatters::formatQVariantList(list).toStdString();
        return formatter<std::string>::format(str, ctx);
    }
};

template <>
struct formatter<QVariantMap> : formatter<std::string> {
    auto format(const QVariantMap& map, format_context& ctx) {
        auto str = qt_spdlog::formatters::formatQVariantMap(map).toStdString();
        return formatter<std::string>::format(str, ctx);
    }
};

template <>
struct formatter<QStringList> : formatter<std::string> {
    auto format(const QStringList& list, format_context& ctx) {
        auto str = qt_spdlog::formatters::formatQStringList(list).toStdString();
        return formatter<std::string>::format(str, ctx);
    }
};

} // namespace fmt


namespace qt_spdlog {

namespace info {

inline const char* version() { return "1.0.0"; }

inline const char* author() { return "https://github.com/cherninkiy"; }

} // namespace info

namespace formatters {

// Шаблонный форматтер для числовых типов
template<typename T>
inline QString formatNum(const T& value) {
    if constexpr (std::is_floating_point_v<T>) {
        return QString::number(value, 'g', 6);
    } else {
        return QString::number(value);
    }
}

// Форматтер для QList числовых типов
template<typename T>
inline QString formatListNums(const QList<T>& list) {
    if (list.isEmpty()) {
        return "[]";
    }

    QString result = "[";
    for (int i = 0; i < list.size(); ++i) {
        if (i > 0) result += ", ";
        result += formatNum(list[i]);
    }
    result += "]";
    return result;
}

// Форматтер для QVector числовых типов
template<typename T>
inline QString formatVectorNums(const QVector<T>& vec) {
    if (vec.isEmpty()) {
        return "[]";
    }

    QString result = "[";
    for (int i = 0; i < vec.size(); ++i) {
        if (i > 0) result += ", ";
        result += formatNum(vec[i]);
    }
    result += "]";
    return result;
}

// Форматтер для QMap<QString, int>
template<typename T>
inline QString formatMapStrNums(const QMap<QString, T>& map) {
    if (map.isEmpty()) {
        return "{}";
    }

    QString result = "{";
    auto keys = map.keys();
    for (int i = 0; i < keys.size(); ++i) {
        if (i > 0) result += ", ";
        result += keys[i] + ": " + QString::number(map.value(keys[i]));
    }
    result += "}";
    return result;
}

// Форматтер для QMap<int, QString>
template<typename T>
inline QString formatMapNumStrs(const QMap<T, QString>& map) {
    if (map.isEmpty()) {
        return "{}";
    }

    QString result = "{";
    auto keys = map.keys();
    for (int i = 0; i < keys.size(); ++i) {
        if (i > 0) result += ", ";
        result += QString::number(keys[i]) + ": " + map[keys[i]];
    }
    result += "}";
    return result;
}

// Форматтер для QStringList
inline QString formatQStringList(const QStringList& list) {
    if (list.isEmpty()) {
        return "[]";
    }
    return "[" + list.join(", ") + "]";
}

// Форматтер для QByteArray (hex представление)
inline QString formatQByteArray(const QByteArray& data, bool showHexString) {
    if (data.isEmpty()) {
        return showHexString ? "x''" : "b''";
    }

    if (showHexString) {
        // Непрерывная hex-строка с префиксом x'
        return "x'" + data.toHex() + "'";
    } else {
        // Текстовый режим с префиксом b' и экранированием (как в Python)
        QString result = "b'";
        for (char c : data) {
            if (c >= 32 && c <= 126 && c != '\'' && c != '\\') {
                result += c;
            } else {
                result += "\\x" + QString::number(static_cast<unsigned char>(c), 16).rightJustified(2, '0');
            }
        }
        result += "'";
        return result;
    }
}

// Форматтер для QVariant
inline QString formatQVariant(const QVariant& variant) {
    switch (variant.typeId()) {
    case QMetaType::QString:
        return variant.toString();
    case QMetaType::Int:
        return QString::number(variant.toInt());
    case QMetaType::Double:
        return QString::number(variant.toDouble());
    case QMetaType::Bool:
        return variant.toBool() ? "true" : "false";
    case QMetaType::QStringList:
        return formatQStringList(variant.toStringList());
    case QMetaType::QVariantList:
        return formatQVariantList(variant.toList());
    case QMetaType::QVariantMap:
        return formatQVariantMap(variant.toMap());
    case QMetaType::QByteArray:
        return QString(variant.toByteArray());
    default:
        if (variant.isNull()) {
            return "null";
        }
        return variant.toString();
    }
}

// Форматтер для QVariantList
inline QString formatQVariantList(const QVariantList& list) {
    if (list.isEmpty()) {
        return "[]";
    }

    QString result = "[";
    for (int i = 0; i < list.size(); ++i) {
        if (i > 0) result += ", ";
        result += formatQVariant(list[i]);
    }
    result += "]";
    return result;
}

// Форматтер для QVariantMap
inline QString formatQVariantMap(const QVariantMap& map) {
    if (map.isEmpty()) {
        return "{}";
    }

    QString result = "{";
    auto keys = map.keys();
    for (int i = 0; i < keys.size(); ++i) {
        if (i > 0) result += ", ";
        result += keys[i] + ": " + formatQVariant(map[keys[i]]);
    }
    result += "}";
    return result;
}

} // namespace formatters

namespace utils {

// Конвертер
template<typename T>
constexpr auto convert_arg(T&& arg) {
    if constexpr (std::is_same_v<std::decay_t<T>, QString>) {
        // toUtf8() возвращает QByteArray
        return std::forward<T>(arg).toUtf8();
    } else if constexpr (std::is_same_v<std::decay_t<T>, QByteArray>) {
        // Передаем QByteArray как есть
        return std::forward<T>(arg);
    } else {
        // Остальные типы передаем без изменений
        return std::forward<T>(arg);
    }
}

// Вспомогательная функция для извлечения данных
template<typename T>
constexpr auto get_log_arg(const T& arg) {
    if constexpr (std::is_same_v<std::decay_t<T>, QByteArray>) {
        // Для QByteArray возвращаем указатель на данные
        return arg.constData();
    } else {
        // Для всех остальных типов возвращаем как есть
        return arg;
    }
}

// Безопасная версия с гарантией времени жизни
template<typename LoggerFunc, typename... Args>
constexpr void log_with_conversion(LoggerFunc&& logger_func, Args&&... args) {
    // Статическая проверка типов
    static_assert((
                      (!std::is_pointer_v<std::decay_t<Args>> ||
                       std::is_same_v<std::decay_t<Args>, const char*> ||
                       std::is_same_v<std::decay_t<Args>, char*>) && ...
                      ), "Raw pointers (except char* and const char*) are not safe for logging");

    // Создаем кортеж с конвертированными аргументами
    // Они будут жить до конца выполнения лямбды
    auto converted = std::make_tuple(convert_arg(std::forward<Args>(args))...);

    // Извлекаем данные из конвертированных аргументов
    std::apply([&](const auto&... safe_args) {
        logger_func(get_log_arg(safe_args)...);
    }, converted);
}

inline QString format_exception_name(const char* name) {
    QString result(name);

    // Базовые замены для читаемости
    result.replace("St13runtime_error", "std::runtime_error");
    result.replace("St16invalid_argument", "std::invalid_argument");
    result.replace("St12out_of_range", "std::out_of_range");
    result.replace("St11logic_error", "std::logic_error");
    result.replace("St12length_error", "std::length_error");
    result.replace("NSt8ios_base7failureE", "std::ios::failure");
    result.replace("NSt8ios_base7failureB5cxx11E", "std::ios::failure");

    // Убираем лишние префиксы
    if (result.startsWith("St")) {
        result = result.mid(2);
    }

    return result;
}

inline QString get_exception_message(const std::exception& e) {
    return QString::fromUtf8(e.what());
}

inline QString get_exception_message(const QException& e) {
    return QString::fromUtf8(e.what());
}

} // namespace utils

namespace details {

// Единая карта для преобразования строк <-> уровни spdlog
inline const QMap<QString, spdlog::level::level_enum>& get_string_to_level_map() {
    static const QMap<QString, spdlog::level::level_enum> map = {
        {"trace", spdlog::level::trace},
        {"debug", spdlog::level::debug},
        {"info", spdlog::level::info},
        {"warn", spdlog::level::warn},
        {"warning", spdlog::level::warn},
        {"error", spdlog::level::err},
        {"critical", spdlog::level::critical},
        {"off", spdlog::level::off},
        {"always", spdlog::level::off}
    };
    return map;
}

inline const QMap<spdlog::level::level_enum, QString>& get_level_to_string_map() {
    static const QMap<spdlog::level::level_enum, QString> map = {
        {spdlog::level::trace, "trace"},
        {spdlog::level::debug, "debug"},
        {spdlog::level::info, "info"},
        {spdlog::level::warn, "warn"},
        {spdlog::level::err, "error"},
        {spdlog::level::critical, "critical"},
        {spdlog::level::off, "off"}
    };
    return map;
}

// Единая карта для преобразования QtMsgType <-> уровни spdlog
inline const QMap<QtMsgType, spdlog::level::level_enum>& get_qt_to_spdlog_map() {
    static const QMap<QtMsgType, spdlog::level::level_enum> map = {
                                                                   {QtMsgType::QtDebugMsg, spdlog::level::debug},
                                                                   {QtMsgType::QtInfoMsg, spdlog::level::info},
                                                                   {QtMsgType::QtWarningMsg, spdlog::level::warn},
                                                                   {QtMsgType::QtCriticalMsg, spdlog::level::err},
                                                                   {QtMsgType::QtFatalMsg, spdlog::level::critical},
                                                                   };
    return map;
}

inline const QMap<spdlog::level::level_enum, QtMsgType>& get_spdlog_to_qt_map() {
    static const QMap<spdlog::level::level_enum, QtMsgType> map = {
        {spdlog::level::trace, QtMsgType::QtDebugMsg},
        {spdlog::level::debug, QtMsgType::QtDebugMsg},
        {spdlog::level::info, QtMsgType::QtInfoMsg},
        {spdlog::level::warn, QtMsgType::QtWarningMsg},
        {spdlog::level::err, QtMsgType::QtCriticalMsg},
        {spdlog::level::critical, QtMsgType::QtFatalMsg},
        {spdlog::level::off, QtMsgType::QtFatalMsg}
    };
    return map;
}

} // namespace details

namespace scoped {

class ScopedLoggerLevel {
private:
    std::shared_ptr<spdlog::logger> logger_;
    spdlog::level::level_enum original_level_;
    bool active_;

public:
    // Конструктор с именем логгера
    ScopedLoggerLevel(const std::string& logger_name, spdlog::level::level_enum level)
        : active_(false) {

        logger_ = logger_name.empty() ? spdlog::default_logger() : spdlog::get(logger_name);

        // Если логгер с указанным именем не существует, fallback на default
        if (!logger_) {
            logger_ = spdlog::default_logger();
        }

        if (logger_) {
            original_level_ = logger_->level();
            logger_->set_level(level);
            active_ = true;
        }
    }

    ~ScopedLoggerLevel() {
        if (active_ && logger_) {
            logger_->set_level(original_level_);
        }
    }

    explicit operator bool() const { return active_; }

    // Запрещаем копирование и перемещение
    ScopedLoggerLevel(const ScopedLoggerLevel&) = delete;
    ScopedLoggerLevel& operator=(const ScopedLoggerLevel&) = delete;
    ScopedLoggerLevel(ScopedLoggerLevel&&) = delete;
    ScopedLoggerLevel& operator=(ScopedLoggerLevel&&) = delete;
};
}

namespace json {

// Функции для JSON логирования
inline void json_log(spdlog::level::level_enum level, const QString& message, const QVariantMap& fields = {}) {
    QJsonObject jsonObj;
    jsonObj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODateWithMs);
    jsonObj["level"] = QString::fromStdString(spdlog::level::to_string_view(level).data());
    jsonObj["message"] = message;

    if (!fields.isEmpty()) {
        jsonObj["fields"] = QJsonObject::fromVariantMap(fields);
    }

    QJsonDocument doc(jsonObj);
    QString jsonStr = doc.toJson(QJsonDocument::Compact);

    spdlog::default_logger()->log(level, jsonStr.toStdString());
}

// Удобные обертки
inline void json_info(const QString& message, const QVariantMap& fields = {}) {
    json_log(spdlog::level::info, message, fields);
}

inline void json_error(const QString& message, const QVariantMap& fields = {}) {
    json_log(spdlog::level::err, message, fields);
}

inline void json_warn(const QString& message, const QVariantMap& fields = {}) {
    json_log(spdlog::level::warn, message, fields);
}

inline void json_debug(const QString& message, const QVariantMap& fields = {}) {
    json_log(spdlog::level::debug, message, fields);
}

} // namespace json

// ============================================================================
// УПРАВЛЕНИЕ УРОВНЯМИ
// ============================================================================

// Преобразование строки в уровень spdlog
inline spdlog::level::level_enum string_to_level(const QString& level_str) {
    auto& map = details::get_string_to_level_map();
    QString lower = level_str.toLower();

    if (map.contains(lower)) {
        return map.value(lower);
    }

    // Если уровень не найден, пробуем преобразовать через spdlog
    try {
        std::string std_str = level_str.toStdString();
        return spdlog::level::from_str(std_str);
    }
    catch (...) {
        return spdlog::level::info; // уровень по умолчанию
    }
}

// Преобразование уровня spdlog в строку
inline QString level_to_string(spdlog::level::level_enum level) {
    auto& map = details::get_level_to_string_map();

    if (map.contains(level)) {
        return map.value(level);
    }

    return QString::fromStdString(spdlog::level::to_short_c_str(level));
}

// Преобразование QtMsgType в уровень spdlog
inline spdlog::level::level_enum qt_to_spdlog_level(QtMsgType qt_level) {
    auto& map = details::get_qt_to_spdlog_map();
    return map.value(qt_level, spdlog::level::info);
}

inline bool set_level(const QString& level) {
    const auto& level_map = details::get_string_to_level_map();
    auto it = level_map.find(level.toLower());
    if (it != level_map.end()) {
        spdlog::set_level(it.value());
        return true;
    }
    std::cerr << "Unknown log level: " << level.toStdString() << std::endl;
    return false;
}

inline QString get_level() {
    auto level = spdlog::get_level();
    const auto& level_map = details::get_level_to_string_map();
    auto it = level_map.find(level);
    return it != level_map.end() ? it.value() : "unknown";
}

inline bool set_level(QtMsgType level) {
    const auto& level_map = details::get_qt_to_spdlog_map();
    auto it = level_map.find(level);
    if (it != level_map.end()) {
        spdlog::set_level(it.value());
        return true;
    }
    std::cerr << "Unknown Qt message type: " << static_cast<int>(level) << std::endl;
    return false;
}

inline QtMsgType get_qt_level() {
    auto level = spdlog::get_level();
    const auto& level_map = details::get_spdlog_to_qt_map();
    auto it = level_map.find(level);
    return it != level_map.end() ? it.value() : QtMsgType::QtInfoMsg;
}

inline bool is_valid_level(const QString& level) {
    const auto& level_map = details::get_string_to_level_map();
    return level_map.contains(level.toLower());
}

inline QString get_level_display_name(const QString& level_str) {
    spdlog::level::level_enum level = string_to_level(level_str);
    return level_to_string(level);
}

inline QStringList get_level_aliases(const QString& canonical_level) {
    auto& map = details::get_string_to_level_map();
    QStringList aliases;

    spdlog::level::level_enum target_level = string_to_level(canonical_level);

    for (auto it = map.begin(); it != map.end(); ++it) {
        if (it.value() == target_level && it.key() != canonical_level) {
            aliases.append(it.key());
        }
    }

    return aliases;
}

// Получение отображаемого имени с учетом алиасов
inline QString get_level_display_name_with_aliases(const QString& level_str) {
    QString canonical = get_level_display_name(level_str);
    QStringList aliases = get_level_aliases(canonical);

    if (aliases.isEmpty()) {
        return canonical;
    }

    return QString("%1 (алиасы: %2)").arg(canonical).arg(aliases.join(", "));
}

inline QStringList get_available_levels() {
    const auto& level_map = details::get_string_to_level_map();
    return level_map.keys();
}

inline QStringList get_canonical_levels() {
    auto& map = details::get_level_to_string_map();
    QStringList levels;
    levels.reserve(map.size());

    // Сохраняем порядок от trace до off
    for (int i = static_cast<int>(spdlog::level::trace);
         i <= static_cast<int>(spdlog::level::off);
         ++i) {
        auto level = static_cast<spdlog::level::level_enum>(i);
        if (map.contains(level)) {
            levels.append(map.value(level));
        }
    }

    return levels;
}

inline QMap<QString, QString> get_levels_with_display_names() {
    QMap<QString, QString> result;
    const auto& level_map = details::get_string_to_level_map();

    for (auto it = level_map.begin(); it != level_map.end(); ++it) {
        result[it.key()] = get_level_display_name(it.key());
    }

    return result;
}

inline void setup_display_always() {
    auto& registry = spdlog::details::registry::instance();

    // Применяем функцию ко всем зарегистрированным логгерам
    registry.apply_all([](const std::shared_ptr<spdlog::logger>& logger) {
        // Для каждого логгера настраиваем цвета во всех console sink'ах
        for (auto& sink : logger->sinks()) {
            if (auto console_sink = std::dynamic_pointer_cast<spdlog::sinks::stdout_color_sink_mt>(sink)) {

#ifdef _WIN32
                // Для Windows использовать Windows API цвета
                console_sink->set_color(spdlog::level::off, 0x5); // cyan
#else
                // Для Linux/Mac использовать ANSI коды
                console_sink->set_color(spdlog::level::off, "\033[1;36m"); // bright cyan
#endif
                // #define FOREGROUND_BLUE 0x1
                // #define FOREGROUND_GREEN 0x2
                // #define FOREGROUND_RED 0x4
                // #define FOREGROUND_INTENSITY 0x8
                // #define BACKGROUND_BLUE 0x10
                // #define BACKGROUND_GREEN 0x20
                // #define BACKGROUND_RED 0x40
                // #define BACKGROUND_INTENSITY 0x80

                // default win console level colors
                // colors_[level::trace] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;     // white
                // colors_[level::debug] = FOREGROUND_GREEN | FOREGROUND_BLUE;                      // cyan
                // colors_[level::info] = FOREGROUND_GREEN;                                         // green
                // colors_[level::warn] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; // intense yellow
                // colors_[level::err] = FOREGROUND_RED | FOREGROUND_INTENSITY;                     // intense red
                // colors_[level::critical] =
                //     BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; // intense white on red background
                // colors_[level::off] = 0;

                // console_sink->set_color(spdlog::level::off, "\033[1;36m");      // ALWAYS - яркий cyan
                // console_sink->set_color(spdlog::level::critical, "\033[1;31m"); // CRITICAL - яркий red
                // console_sink->set_color(spdlog::level::err, "\033[1;31m");      // ERROR - яркий red
                // console_sink->set_color(spdlog::level::warn, "\033[1;33m");     // WARN - яркий yellow
                // console_sink->set_color(spdlog::level::info, "\033[1;32m");     // INFO - яркий green
                // console_sink->set_color(spdlog::level::debug, "\033[1;34m");    // DEBUG - яркий blue
                // console_sink->set_color(spdlog::level::trace, "\033[1;37m");    // TRACE - яркий white

            }
        }
    });
}

// ============================================================================
// ВРЕМЕННЫЕ ЛОГГЕРЫ
// ============================================================================

// Для основного логгера "qt" с уровнем из строки
inline scoped::ScopedLoggerLevel create_scoped_logger(const QString& level_name) {
    if (!is_valid_level(level_name)) {
        // Возвращаем неактивный объект
        return scoped::ScopedLoggerLevel("", spdlog::level::info);
    }
    spdlog::level::level_enum level = string_to_level(level_name);
    return scoped::ScopedLoggerLevel("qt", level);
}

// Для основного логгера "qt" с уровнем spdlog
inline scoped::ScopedLoggerLevel create_scoped_logger(spdlog::level::level_enum level) {
    return scoped::ScopedLoggerLevel("qt", level);
}

// Для конкретного логгера по имени с уровнем из строки
inline scoped::ScopedLoggerLevel create_scoped_logger(const QString& logger_name, const QString& level_name) {
    if (!is_valid_level(level_name)) {
        return scoped::ScopedLoggerLevel("", spdlog::level::info);
    }
    spdlog::level::level_enum level = string_to_level(level_name);
    return scoped::ScopedLoggerLevel(logger_name.toStdString(), level);
}

// Для конкретного логгера с уровнем spdlog
inline scoped::ScopedLoggerLevel create_scoped_logger(const QString& logger_name, spdlog::level::level_enum level) {
    return scoped::ScopedLoggerLevel(logger_name.toStdString(), level);
}

// ============================================================================
// THREAD-LOCAL ЛОГГЕРЫ
// ============================================================================

// Простое thread-local хранилище для модуля
inline QThreadStorage<QString>& get_module_storage() {
    static QThreadStorage<QString> storage;
    return storage;
}

// Thread-local storage для имени модуля
inline QString get_current_module_name() {
    auto& storage = get_module_storage();
    if (storage.hasLocalData()) {
        return storage.localData();
    }
    return "unknown";
}

inline void set_current_module_name(const QString& module) {
    auto& storage = get_module_storage();
    storage.setLocalData(module);
}

// QThreadStorage для thread-local логгеров
inline QThreadStorage<std::shared_ptr<spdlog::logger>>& get_logger_storage() {
    static QThreadStorage<std::shared_ptr<spdlog::logger>> storage;
    return storage;
}

inline std::shared_ptr<spdlog::logger> get_thread_local_logger() {
    auto& storage = get_logger_storage();

    if (!storage.hasLocalData()) {
        // Используем хэш std::thread::id для уникальности
        auto thread_id = std::this_thread::get_id();
        std::hash<std::thread::id> hasher;
        auto thread_hash = hasher(thread_id);

        auto current_module = get_current_module_name().toStdString();
        auto logger_name = current_module + "_" + std::to_string(thread_hash);

        auto logger = spdlog::default_logger()->clone(logger_name);
        storage.setLocalData(logger);
    }

    return storage.localData();
}

// Алиасы для удобства
inline QString get_current_module() {
    return get_current_module_name();
}

inline void set_current_module(const QString& module) {
    set_current_module_name(module);
}

namespace scoped {

// RAII-обертка для временной смены модуля
class ScopedModule {
public:
    ScopedModule(const QString& module)
        : m_previous_module(get_current_module_name())
        , m_new_module(module) {
        set_current_module_name(module);
    }

    ~ScopedModule() {
        set_current_module_name(m_previous_module);
    }

    // Методы для получения информации
    QString previous_module() const { return m_previous_module; }
    QString current_module() const { return m_new_module; }

private:
    QString m_previous_module;
    QString m_new_module;
};

}

// Основной фабричный метод
inline scoped::ScopedModule module(const QString& module_name) {
    return scoped::ScopedModule(module_name);
}

// ============================================================================
// УПРАВЛЕНИЕ ПАТТЕРНАМИ
// ============================================================================

// Предустановленные паттерны
namespace patterns {
inline const QString DEFAULT = "%^[%T] [%l]%$ %v";
inline const QString SIMPLE = "[%H:%M:%S] [%l] %v";
inline const QString DETAILED = "%^[%Y-%m-%d %H:%M:%S.%e] [%l] [%n]%$ %v";
inline const QString LOCATION = "%^[%Y-%m-%d %H:%M:%S.%e] [%l] [TID=%t] [%s:%#] [%!]%$ %v";
inline const QString QT_STYLE = "%^[%T] [%l]%$ %v";
inline const QString THREAD_ID = "%^[%T] [%l] [TID=%t]%$ %v";
}

inline bool set_pattern(const QString& pattern) {
    try {
        spdlog::set_pattern(pattern.toUtf8().constData());
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to set log pattern '" << pattern.toStdString() << "': " << e.what() << std::endl;
        return false;
    }
    catch (...) {
        std::cerr << "Failed to set log pattern '" << pattern.toStdString() << "': unknown error" << std::endl;
        return false;
    }
}

inline bool set_default_pattern() {
    return set_pattern(patterns::DEFAULT);
}

inline bool set_simple_pattern() {
    return set_pattern(patterns::SIMPLE);
}

inline bool set_detailed_pattern() {
    return set_pattern(patterns::DETAILED);
}

inline bool set_location_pattern() {
    return set_pattern(patterns::LOCATION);
}

inline bool set_qt_style_pattern() {
    return set_pattern(patterns::QT_STYLE);
}

inline bool set_thread_id_pattern() {
    return set_pattern(patterns::THREAD_ID);
}

// ============================================================================
// ИНТЕГРАЦИЯ С QT MESSAGE HANDLER
// ============================================================================

inline void setup_qt_message_handler(bool preserve_original = true) {
    static QtMessageHandler original_handler = nullptr;

    if (preserve_original && !original_handler) {
        original_handler = qInstallMessageHandler(nullptr);
    }

    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        try {
            spdlog::level::level_enum level;
            switch(type) {
            case QtDebugMsg: level = spdlog::level::debug; break;
            case QtInfoMsg: level = spdlog::level::info; break;
            case QtWarningMsg: level = spdlog::level::warn; break;
            case QtCriticalMsg: level = spdlog::level::err; break;
            case QtFatalMsg: level = spdlog::level::critical; break;
            default: level = spdlog::level::info; break;  // для будущих типов
            }

            std::string message = msg.toStdString();
            spdlog::default_logger()->log(level, message);

            if (type == QtFatalMsg) {
                std::abort();
            }

        } catch (const std::exception& e) {
            std::cerr << "Qt logging failed: " << e.what() << std::endl;
            if (original_handler) {
                original_handler(type, context, msg);
            }
        }
    });
}

} // namespace qt_spdlog


// ============================================================================
// УНИВЕРСАЛЬНЫЙ ШАБЛОН ДЛЯ ЛЮБОГО ЛОГГЕРА
// ============================================================================

#ifdef QT_LOG_INTERNAL
#undef QT_LOG_INTERNAL
#endif

#define QT_LOG_INTERNAL(logger_ptr, level_name, level_enum, ...) \
do { \
        auto _logger = (logger_ptr); \
        if (_logger->should_log(spdlog::level::level_enum)) { \
            qt_spdlog::utils::log_with_conversion( \
                                                   [_logger](auto... converted_args) { \
                                                           _logger->level_name(converted_args...); \
                                                   }, __VA_ARGS__); \
    } \
} while(0)

// ============================================================================
// УПРОЩЕННЫЕ МАКРОСЫ НА ОСНОВЕ УНИВЕРСАЛЬНОГО ШАБЛОНА
// ============================================================================

// Default logger

#ifdef QT_LOG_TRACE
#undef QT_LOG_TRACE
#endif
#ifdef QT_LOG_DEBUG
#undef QT_LOG_DEBUG
#endif
#ifdef QT_LOG_INFO
#undef QT_LOG_INFO
#endif
#ifdef QT_LOG_WARN
#undef QT_LOG_WARN
#endif
#ifdef QT_LOG_ERROR
#undef QT_LOG_ERROR
#endif
#ifdef QT_LOG_CRITICAL
#undef QT_LOG_CRITICAL
#endif

#define QT_LOG_TRACE(...)    QT_LOG_INTERNAL(spdlog::default_logger(), trace, trace, __VA_ARGS__)
#define QT_LOG_DEBUG(...)    QT_LOG_INTERNAL(spdlog::default_logger(), debug, debug, __VA_ARGS__)
#define QT_LOG_INFO(...)     QT_LOG_INTERNAL(spdlog::default_logger(), info, info, __VA_ARGS__)
#define QT_LOG_WARN(...)     QT_LOG_INTERNAL(spdlog::default_logger(), warn, warn, __VA_ARGS__)
#define QT_LOG_ERROR(...)    QT_LOG_INTERNAL(spdlog::default_logger(), error, err, __VA_ARGS__)
#define QT_LOG_CRITICAL(...) QT_LOG_INTERNAL(spdlog::default_logger(), critical, critical, __VA_ARGS__)

// Custom logger

#ifdef QT_LOGGER_TRACE
#undef QT_LOGGER_TRACE
#endif
#ifdef QT_LOGGER_DEBUG
#undef QT_LOGGER_DEBUG
#endif
#ifdef QT_LOGGER_INFO
#undef QT_LOGGER_INFO
#endif
#ifdef QT_LOGGER_WARN
#undef QT_LOGGER_WARN
#endif
#ifdef QT_LOGGER_ERROR
#undef QT_LOGGER_ERROR
#endif
#ifdef QT_LOGGER_CRITICAL
#undef QT_LOGGER_CRITICAL
#endif

#define QT_LOGGER_TRACE(logger, ...)    QT_LOG_INTERNAL(logger, trace, trace, __VA_ARGS__)
#define QT_LOGGER_DEBUG(logger, ...)    QT_LOG_INTERNAL(logger, debug, debug, __VA_ARGS__)
#define QT_LOGGER_INFO(logger, ...)     QT_LOG_INTERNAL(logger, info, info, __VA_ARGS__)
#define QT_LOGGER_WARN(logger, ...)     QT_LOG_INTERNAL(logger, warn, warn, __VA_ARGS__)
#define QT_LOGGER_ERROR(logger, ...)    QT_LOG_INTERNAL(logger, error, err, __VA_ARGS__)
#define QT_LOGGER_CRITICAL(logger, ...) QT_LOG_INTERNAL(logger, critical, critical, __VA_ARGS__)

// ============================================================================
// МАКРОСЫ ДЛЯ УСЛОВНОГО ЛОГИРОВАНИЯ
// ============================================================================

#ifdef QT_LOG_IF_TRACE
#undef QT_LOG_IF_TRACE
#endif
#ifdef QT_LOG_IF_DEBUG
#undef QT_LOG_IF_DEBUG
#endif
#ifdef QT_LOG_IF_INFO
#undef QT_LOG_IF_INFO
#endif
#ifdef QT_LOG_IF_WARN
#undef QT_LOG_IF_WARN
#endif
#ifdef QT_LOG_IF_ERROR
#undef QT_LOG_IF_ERROR
#endif
#ifdef QT_LOG_IF_CRITICAL
#undef QT_LOG_IF_CRITICAL
#endif

#define QT_LOG_IF_TRACE(condition, ...) \
    do { if (condition) QT_LOG_TRACE(__VA_ARGS__); } while(0)

#define QT_LOG_IF_DEBUG(condition, ...) \
    do { if (condition) QT_LOG_DEBUG(__VA_ARGS__); } while(0)

#define QT_LOG_IF_INFO(condition, ...) \
        do { if (condition) QT_LOG_INFO(__VA_ARGS__); } while(0)

#define QT_LOG_IF_WARN(condition, ...) \
    do { if (condition) QT_LOG_WARN(__VA_ARGS__); } while(0)

#define QT_LOG_IF_ERROR(condition, ...) \
        do { if (condition) QT_LOG_ERROR(__VA_ARGS__); } while(0)

#define QT_LOG_IF_CRITICAL(condition, ...) \
    do { if (condition) QT_LOG_CRITICAL(__VA_ARGS__); } while(0)

// ============================================================================
// THREAD-LOCAL МАКРОСЫ
// ============================================================================

// Thread-local макросы
#define QT_LOG_TRACE_TS(...) \
        QT_LOG_INTERNAL(qt_spdlog::get_thread_local_logger(), trace, trace, __VA_ARGS__)

#define QT_LOG_DEBUG_TS(...) \
        QT_LOG_INTERNAL(qt_spdlog::get_thread_local_logger(), debug, debug, __VA_ARGS__)

#define QT_LOG_INFO_TS(...) \
        QT_LOG_INTERNAL(qt_spdlog::get_thread_local_logger(), info, info, __VA_ARGS__)

#define QT_LOG_WARN_TS(...) \
        QT_LOG_INTERNAL(qt_spdlog::get_thread_local_logger(), warn, warn, __VA_ARGS__)

#define QT_LOG_ERROR_TS(...) \
        QT_LOG_INTERNAL(qt_spdlog::get_thread_local_logger(), error, err, __VA_ARGS__)

#define QT_LOG_CRITICAL_TS(...) \
        QT_LOG_INTERNAL(qt_spdlog::get_thread_local_logger(), critical, critical, __VA_ARGS__)

// ============================================================================
// МАКРОСЫ ДЛЯ УРОВНЯ ALWAYS (всегда отображается)
// ============================================================================

#ifdef QT_LOG_ALWAYS
#undef QT_LOG_ALWAYS
#endif
#ifdef QT_LOG_IF_ALWAYS
#undef QT_LOG_IF_ALWAYS
#endif
#ifdef QT_LOG_ALWAYS_TS
#undef QT_LOG_ALWAYS_TS
#endif

#define QT_LOG_ALWAYS(...) \
        do { \
            auto _logger = spdlog::default_logger(); \
            /* Принудительно логируем, игнорируя текущий уровень */ \
            qt_spdlog::utils::log_with_conversion( \
                                                   [_logger](auto... converted_args) { \
                                                           _logger->log(spdlog::level::off, converted_args...); \
                                                   }, __VA_ARGS__); \
    } while(0)

#define QT_LOG_IF_ALWAYS(condition, ...) \
    do { if (condition) QT_LOG_ALWAYS(__VA_ARGS__); } while(0)

// Thread-local версия
#define QT_LOG_ALWAYS_TS(...) \
        do { \
            auto _logger = qt_spdlog::get_thread_local_logger(); \
            qt_spdlog::utils::log_with_conversion( \
                                                   [_logger](auto... converted_args) { \
                                                           _logger->log(spdlog::level::off, converted_args...); \
                                                   }, __VA_ARGS__); \
    } while(0)

// ============================================================================
// МАКРОСЫ ОТЛАДОЧНОЙ ИНФОРМАЦИЕЙ НО БЕЗ ФОРМАТИРОВАНИЯ
// ============================================================================

#ifdef QT_LOG_TRACE_LOCATION
#undef QT_LOG_TRACE_LOCATION
#endif
#ifdef QT_LOG_DEBUG_LOCATION
#undef QT_LOG_DEBUG_LOCATION
#endif
#ifdef QT_LOG_INFO_LOCATION
#undef QT_LOG_INFO_LOCATION
#endif
#ifdef QT_LOG_WARN_LOCATION
#undef QT_LOG_WARN_LOCATION
#endif
#ifdef QT_LOG_ERROR_LOCATION
#undef QT_LOG_ERROR_LOCATION
#endif
#ifdef QT_LOG_CRITICAL_LOCATION
#undef QT_LOG_CRITICAL_LOCATION
#endif

// Макросы с сообщением но без форматирования (только static string)
#define QT_LOG_TRACE_LOCATION()    SPDLOG_LOGGER_CALL(spdlog::default_logger(), spdlog::level::trace, "")
#define QT_LOG_DEBUG_LOCATION()    SPDLOG_LOGGER_CALL(spdlog::default_logger(), spdlog::level::debug, "")
#define QT_LOG_INFO_LOCATION()     SPDLOG_LOGGER_CALL(spdlog::default_logger(), spdlog::level::info, "")
#define QT_LOG_WARN_LOCATION()     SPDLOG_LOGGER_CALL(spdlog::default_logger(), spdlog::level::warn, "")
#define QT_LOG_ERROR_LOCATION()    SPDLOG_LOGGER_CALL(spdlog::default_logger(), spdlog::level::err, "")
#define QT_LOG_CRITICAL_LOCATION() SPDLOG_LOGGER_CALL(spdlog::default_logger(), spdlog::level::critical, "")

// Thread-local версии
#define QT_LOG_TRACE_LOCATION_TS()    SPDLOG_LOGGER_CALL(qt_spdlog::get_thread_local_logger(), spdlog::level::trace, "")
#define QT_LOG_DEBUG_LOCATION_TS()    SPDLOG_LOGGER_CALL(qt_spdlog::get_thread_local_logger(), spdlog::level::debug, "")
#define QT_LOG_INFO_LOCATION_TS()      SPDLOG_LOGGER_CALL(qt_spdlog::get_thread_local_logger(), spdlog::level::info, "")
#define QT_LOG_WARN_LOCATION_TS()     SPDLOG_LOGGER_CALL(qt_spdlog::get_thread_local_logger(), spdlog::level::warn, "")
#define QT_LOG_ERROR_LOCATION_TS()    SPDLOG_LOGGER_CALL(qt_spdlog::get_thread_local_logger(), spdlog::level::err, "")
#define QT_LOG_CRITICAL_LOCATION_TS() SPDLOG_LOGGER_CALL(qt_spdlog::get_thread_local_logger(), spdlog::level::critical, "")

#ifdef QT_LOG_TRACE_LOCATION_MSG
#undef QT_LOG_TRACE_LOCATION_MSG
#endif
#ifdef QT_LOG_DEBUG_LOCATION_MSG
#undef QT_LOG_DEBUG_LOCATION_MSG
#endif
#ifdef QT_LOG_INFO_LOCATION_MSG
#undef QT_LOG_INFO_LOCATION_MSG
#endif
#ifdef QT_LOG_WARN_LOCATION_MSG
#undef QT_LOG_WARN_LOCATION_MSG
#endif
#ifdef QT_LOG_ERROR_LOCATION_MSG
#undef QT_LOG_ERROR_LOCATION_MSG
#endif
#ifdef QT_LOG_CRITICAL_LOCATION_MSG
#undef QT_LOG_CRITICAL_LOCATION_MSG
#endif

// Макросы с сообщением но без форматирования (только static string)
#define QT_LOG_TRACE_LOCATION_MSG(msg)    SPDLOG_LOGGER_CALL(spdlog::default_logger(), spdlog::level::trace, msg)
#define QT_LOG_DEBUG_LOCATION_MSG(msg)    SPDLOG_LOGGER_CALL(spdlog::default_logger(), spdlog::level::debug, msg)
#define QT_LOG_INFO_LOCATION_MSG(msg)     SPDLOG_LOGGER_CALL(spdlog::default_logger(), spdlog::level::info, msg)
#define QT_LOG_WARN_LOCATION_MSG(msg)     SPDLOG_LOGGER_CALL(spdlog::default_logger(), spdlog::level::warn, msg)
#define QT_LOG_ERROR_LOCATION_MSG(msg)    SPDLOG_LOGGER_CALL(spdlog::default_logger(), spdlog::level::err, msg)
#define QT_LOG_CRITICAL_LOCATION_MSG(msg) SPDLOG_LOGGER_CALL(spdlog::default_logger(), spdlog::level::critical, msg)

// Thread-local версии
#define QT_LOG_TRACE_LOCATION_MSG_TS(msg)    SPDLOG_LOGGER_CALL(qt_spdlog::get_thread_local_logger(), spdlog::level::trace, msg)
#define QT_LOG_DEBUG_LOCATION_MSG_TS(msg)    SPDLOG_LOGGER_CALL(qt_spdlog::get_thread_local_logger(), spdlog::level::debug, msg)
#define QT_LOG_INFO_LOCATION_MSG_TS(msg)     SPDLOG_LOGGER_CALL(qt_spdlog::get_thread_local_logger(), spdlog::level::info, msg)
#define QT_LOG_WARN_LOCATION_MSG_TS(msg)     SPDLOG_LOGGER_CALL(qt_spdlog::get_thread_local_logger(), spdlog::level::warn, msg)
#define QT_LOG_ERROR_LOCATION_MSG_TS(msg)    SPDLOG_LOGGER_CALL(qt_spdlog::get_thread_local_logger(), spdlog::level::err, msg)
#define QT_LOG_CRITICAL_LOCATION_MSG_TS(msg) SPDLOG_LOGGER_CALL(qt_spdlog::get_thread_local_logger(), spdlog::level::critical, msg)


// ============================================================================
// МАКРОСЫ ДЛЯ JSON ЛОГИРОВАНИЯ
// ============================================================================

#ifdef QT_LOG_TRACE_JSON
#undef QT_LOG_TRACE_JSON
#endif
#ifdef QT_LOG_DEBUG_JSON
#undef QT_LOG_DEBUG_JSON
#endif
#ifdef QT_LOG_INFO_JSON
#undef QT_LOG_INFO_JSON
#endif
#ifdef QT_LOG_WARN_JSON
#undef QT_LOG_WARN_JSON
#endif
#ifdef QT_LOG_ERROR_JSON
#undef QT_LOG_ERROR_JSON
#endif
#ifdef QT_LOG_CRITICAL_JSON
#undef QT_LOG_CRITICAL_JSON
#endif

#define QT_LOG_TRACE_JSON(message, fields)    qt_spdlog::json::json_log(spdlog::level::trace, message, fields)
#define QT_LOG_DEBUG_JSON(message, fields)    qt_spdlog::json::json_log(spdlog::level::debug, message, fields)
#define QT_LOG_INFO_JSON(message, fields)     qt_spdlog::json::json_log(spdlog::level::info, message, fields)
#define QT_LOG_WARN_JSON(message, fields)     qt_spdlog::json::json_log(spdlog::level::warn, message, fields)
#define QT_LOG_ERROR_JSON(message, fields)    qt_spdlog::json::json_log(spdlog::level::err, message, fields)
#define QT_LOG_CRITICAL_JSON(message, fields) qt_spdlog::json::json_log(spdlog::level::critical, message, fields)

// Упрощенные версии без полей
#ifdef QT_LOG_TRACE_JSON_MSG
#undef QT_LOG_TRACE_JSON_MSG
#endif
#ifdef QT_LOG_DEBUG_JSON_MSG
#undef QT_LOG_DEBUG_JSON_MSG
#endif
#ifdef QT_LOG_INFO_JSON_MSG
#undef QT_LOG_INFO_JSON_MSG
#endif
#ifdef QT_LOG_WARN_JSON_MSG
#undef QT_LOG_WARN_JSON_MSG
#endif
#ifdef QT_LOG_ERROR_JSON_MSG
#undef QT_LOG_ERROR_JSON_MSG
#endif
#ifdef QT_LOG_CRITICAL_JSON_MSG
#undef QT_LOG_CRITICAL_JSON_MSG
#endif

#define QT_LOG_TRACE_JSON_MSG(message)    qt_spdlog::json::json_log(spdlog::level::trace, message)
#define QT_LOG_DEBUG_JSON_MSG(message)    qt_spdlog::json::json_log(spdlog::level::debug, message)
#define QT_LOG_INFO_JSON_MSG(message)     qt_spdlog::json::json_log(spdlog::level::info, message)
#define QT_LOG_WARN_JSON_MSG(message)     qt_spdlog::json::json_log(spdlog::level::warn, message)
#define QT_LOG_ERROR_JSON_MSG(message)    qt_spdlog::json::json_log(spdlog::level::err, message)
#define QT_LOG_CRITICAL_JSON_MSG(message) qt_spdlog::json::json_log(spdlog::level::critical, message)

// Условное JSON логирование
#ifdef QT_LOG_IF_TRACE_JSON
#undef QT_LOG_IF_TRACE_JSON
#endif
#ifdef QT_LOG_IF_DEBUG_JSON
#undef QT_LOG_IF_DEBUG_JSON
#endif
#ifdef QT_LOG_IF_INFO_JSON
#undef QT_LOG_IF_INFO_JSON
#endif
#ifdef QT_LOG_IF_WARN_JSON
#undef QT_LOG_IF_WARN_JSON
#endif
#ifdef QT_LOG_IF_ERROR_JSON
#undef QT_LOG_IF_ERROR_JSON
#endif
#ifdef QT_LOG_IF_CRITICAL_JSON
#undef QT_LOG_IF_CRITICAL_JSON
#endif

#define QT_LOG_IF_TRACE_JSON(condition, message, fields) \
do { if (condition) QT_LOG_TRACE_JSON(message, fields); } while(0)

#define QT_LOG_IF_DEBUG_JSON(condition, message, fields) \
    do { if (condition) QT_LOG_DEBUG_JSON(message, fields); } while(0)

#define QT_LOG_IF_INFO_JSON(condition, message, fields) \
    do { if (condition) QT_LOG_INFO_JSON(message, fields); } while(0)

#define QT_LOG_IF_WARN_JSON(condition, message, fields) \
        do { if (condition) QT_LOG_WARN_JSON(message, fields); } while(0)

#define QT_LOG_IF_ERROR_JSON(condition, message, fields) \
    do { if (condition) QT_LOG_ERROR_JSON(message, fields); } while(0)

#define QT_LOG_IF_CRITICAL_JSON(condition, message, fields) \
        do { if (condition) QT_LOG_CRITICAL_JSON(message, fields); } while(0)

// ============================================================================
// МАКРОСЫ ДЛЯ ИСКЛЮЧЕНИЙ
// ============================================================================

#ifdef QT_LOG_EXCEPTION
#undef QT_LOG_EXCEPTION
#endif
#ifdef QT_LOG_EXCEPTION_WARN
#undef QT_LOG_EXCEPTION_WARN
#endif
#ifdef QT_LOG_EXCEPTION_ERROR
#undef QT_LOG_EXCEPTION_ERROR
#endif
#ifdef QT_LOG_EXCEPTION_CRITICAL
#undef QT_LOG_EXCEPTION_CRITICAL
#endif

// Базовый макрос для std исключений
#define QT_LOG_EXCEPTION(level, exception, context) \
QT_LOG_##level("{}: {} [контекст: {}]", \
               qt_spdlog::utils::format_exception_name(typeid(exception).name()), \
               qt_spdlog::utils::get_exception_message(exception), \
               context)

// Уровневые макросы для std исключений
#define QT_LOG_EXCEPTION_WARN(exception, context)          QT_LOG_EXCEPTION(WARN, exception, context)
#define QT_LOG_EXCEPTION_ERROR(exception, context)         QT_LOG_EXCEPTION(ERROR, exception, context)
#define QT_LOG_EXCEPTION_CRITICAL(exception, context)      QT_LOG_EXCEPTION(CRITICAL, exception, context)

// Специализированные макросы для std исключений
#ifdef QT_LOG_IF_EXCEPTION
#undef QT_LOG_IF_EXCEPTION
#endif
#ifdef QT_LOG_IF_EXCEPTION_WARN
#undef QT_LOG_IF_EXCEPTION_WARN
#endif
#ifdef QT_LOG_IF_EXCEPTION_ERROR
#undef QT_LOG_IF_EXCEPTION_ERROR
#endif
#ifdef QT_LOG_IF_EXCEPTION_CRITICAL
#undef QT_LOG_IF_EXCEPTION_CRITICAL
#endif

#define QT_LOG_IF_EXCEPTION(level, condition, exception, context) \
do { \
        if (condition) { \
            QT_LOG_EXCEPTION(level, exception, context); \
    } \
} while(0)

#define QT_LOG_IF_EXCEPTION_WARN(condition, exception, context)        QT_LOG_IF_EXCEPTION(WARN, condition, exception, context)
#define QT_LOG_IF_EXCEPTION_ERROR(condition, exception, context)       QT_LOG_IF_EXCEPTION(ERROR, condition, exception, context)
#define QT_LOG_IF_EXCEPTION_CRITICAL(condition, exception, context)    QT_LOG_IF_EXCEPTION(CRITICAL, condition, exception, context)

// Макросы для Qt исключений
#ifdef QT_LOG_QEXCEPTION
#undef QT_LOG_QEXCEPTION
#endif
#ifdef QT_LOG_QEXCEPTION_WARN
#undef QT_LOG_QEXCEPTION_WARN
#endif
#ifdef QT_LOG_QEXCEPTION_ERROR
#undef QT_LOG_QEXCEPTION_ERROR
#endif
#ifdef QT_LOG_QEXCEPTION_CRITICAL
#undef QT_LOG_QEXCEPTION_CRITICAL
#endif

// Базовый макрос для исключений
#define QT_LOG_QEXCEPTION(level, exception, context) \
QT_LOG_##level("QException: {} [контекст: {}]", \
               qt_spdlog::utils::get_exception_message(exception), \
               context)

// Уровневые макросы для std исключений
#define QT_LOG_QEXCEPTION_WARN(exception, context)         QT_LOG_QEXCEPTION(WARN, exception, context)
#define QT_LOG_QEXCEPTION_ERROR(exception, context)        QT_LOG_QEXCEPTION(ERROR, exception, context)
#define QT_LOG_QEXCEPTION_CRITICAL(exception, context)     QT_LOG_QEXCEPTION(CRITICAL, exception, context)

// Специализированные макросы для Qt исключений
#ifdef QT_LOG_IF_QEXCEPTION
#undef QT_LOG_IF_QEXCEPTION
#endif
#ifdef QT_LOG_IF_QEXCEPTION_WARN
#undef QT_LOG_IF_QEXCEPTION_WARN
#endif
#ifdef QT_LOG_IF_QEXCEPTION_ERROR
#undef QT_LOG_IF_QEXCEPTION_ERROR
#endif
#ifdef QT_LOG_IF_QEXCEPTION_CRITICAL
#undef QT_LOG_IF_QEXCEPTION_CRITICAL
#endif

#define QT_LOG_IF_QEXCEPTION(level, condition, exception, context) \
do { \
        if (condition) { \
            QT_LOG_QEXCEPTION(level, exception, context); \
    } \
} while(0)

#define QT_LOG_IF_QEXCEPTION_WARN(condition, exception, context)       QT_LOG_IF_QEXCEPTION(WARN, condition, exception, context)
#define QT_LOG_IF_QEXCEPTION_ERROR(condition, exception, context)      QT_LOG_IF_QEXCEPTION(ERROR, condition, exception, context)
#define QT_LOG_IF_QEXCEPTION_CRITICAL(condition, exception, context)   QT_LOG_IF_QEXCEPTION(CRITICAL, condition, exception, context)

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ МАКРОСЫ ДЛЯ УРОВНЕЙ
// ============================================================================

#ifdef QT_LOG_LEVEL_TRACE
#undef QT_LOG_LEVEL_TRACE
#endif
#ifdef QT_LOG_LEVEL_DEBUG
#undef QT_LOG_LEVEL_DEBUG
#endif
#ifdef QT_LOG_LEVEL_INFO
#undef QT_LOG_LEVEL_INFO
#endif
#ifdef QT_LOG_LEVEL_WARN
#undef QT_LOG_LEVEL_WARN
#endif
#ifdef QT_LOG_LEVEL_ERROR
#undef QT_LOG_LEVEL_ERROR
#endif
#ifdef QT_LOG_LEVEL_CRITICAL
#undef QT_LOG_LEVEL_CRITICAL
#endif
#ifdef QT_LOG_LEVEL_OFF
#undef QT_LOG_LEVEL_OFF
#endif

#define QT_LOG_LEVEL_TRACE    spdlog::level::trace
#define QT_LOG_LEVEL_DEBUG    spdlog::level::debug
#define QT_LOG_LEVEL_INFO     spdlog::level::info
#define QT_LOG_LEVEL_WARN     spdlog::level::warn
#define QT_LOG_LEVEL_ERROR    spdlog::level::err
#define QT_LOG_LEVEL_CRITICAL spdlog::level::critical
#define QT_LOG_LEVEL_OFF      spdlog::level::off

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ МАКРОСЫ ДЛЯ ФОРМАТИРОВАНИЯ
// ============================================================================

#define QT_FORMAT_CSTR(msg, ...) QString(msg).arg(__VA_ARGS__).toStdString().c_str()
#define QT_ERROR_CSTR(msg, e) QString(msg).arg(e.what()).toStdString().c_str()

