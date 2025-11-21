#include "loggerdemo.h"
#include "qt_spdlog.h"
#include <QRandomGenerator>
#include <QDateTime>
#include <QThread>
#include <QTimer>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

LoggerDemo::LoggerDemo(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_asyncTimer(new QTimer(this))
{
    initializeTestList();

    // Настройка таймеров
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &LoggerDemo::onTimerTimeout);

    m_asyncTimer->setInterval(500);
    connect(m_asyncTimer, &QTimer::timeout, this, &LoggerDemo::simulateAsyncOperation);
}

LoggerDemo::~LoggerDemo()
{
    stopTimers();
}

void LoggerDemo::initializeTestList()
{
    m_testNames = {
        "1. Базовое логирование + ALWAYS уровень",
        "2. Работа с QString и временными объектами",
        "3. Коллекции и смешанные типы",
        "4. Форматирование вывода (паттерны)",
        "5. Условное логирование",
        "6. Сценарии с ошибками и исключениями",
        "7. Логирование STD и QT исключений",
        "8. JSON логирование",
        "9. Все уровни логирования",
        "10. Scoped уровень (временное изменение)",
        "11. Кастомные логгеры",
        "12. Интеграция с Qt (qDebug, qWarning)",
        "13. Производительность логирования",
        "14. Thread-local логирование",
        "15. Временные модули (Scoped Module)",
        "16. Производительность thread-local",
        "17. Производительность thread-pool",
        "18. Реальные сценарии (бизнес-логика)"
    };

    m_demonstrations = {
        [this]() { demonstrateBasicLogging(); },
        [this]() { demonstrateQStringLogging(); },
        [this]() { demonstrateCollectionAndMixedTypes(); },
        [this]() { demonstrateFormatting(); },
        [this]() { demonstrateConditionalLogging(); },
        [this]() { demonstrateErrorScenarios(); },
        [this]() { demonstrateExceptionLogging(); },
        [this]() { demonstrateJsonLogging(); },
        [this]() { demonstrateLoggingLevels(); },
        [this]() { demonstrateScopedLevel(); },
        [this]() { demonstrateCustomLoggers(); },
        [this]() { demonstrateQtIntegration(); },
        [this]() { demonstrateLoggingPerformance(); },
        [this]() { demonstrateThreadLocalLogging(); },
        [this]() { demonstrateScopedModule(); },
        [this]() { demonstrateThreadLocalPerformance(); },
        [this]() { demonstrateThreadPoolPerformance(); },
        [this]() { demonstrateRealWorldScenarios(); }
    };
}

QString LoggerDemo::getDemoName(int index)
{
    if (index >= 0 && index < m_testNames.size()) {
        return m_testNames[index];
    }
    return "Неизвестный тест";
}

// ============================================================================
// ОСНОВНЫЕ СЛОТЫ
// ============================================================================

void LoggerDemo::checkLoggingSetup()
{
    QT_LOG_ALWAYS("=== ПРОВЕРКА НАСТРОЕК ЛОГГИРОВАНИЯ ===");
    QT_LOG_INFO("Версия библиотеки: {}", qt_spdlog::info::version());
    QT_LOG_INFO("Автор: {}", qt_spdlog::info::author());
    QT_LOG_INFO("Текущий уровень логирования: {}", qt_spdlog::get_level());
    QT_LOG_INFO("Канонические уровни: {}", qt_spdlog::get_canonical_levels().join(", ")); // ← ИЗМЕНИТЬ
    QT_LOG_ALWAYS("=== ПРОВЕРКА ЗАВЕРШЕНА ===\n");
}

void LoggerDemo::demonstrateAllScenarios()
{
    m_demonstrationsFinished = false;

    for (int i = 0; i < m_demonstrations.size(); ++i) {
        emit demonstrationStarted(m_testNames[i]);
        emit demonstrationProgress(i + 1, m_demonstrations.size());

        m_demonstrations[i]();

        // Небольшая пауза между демонстрациями
        QThread::msleep(100);
    }

    m_demonstrationsFinished = true;
    emit demonstrationsFinished();
}

void LoggerDemo::startTimers()
{
    m_timer->start();
    m_asyncTimer->start();
    QT_LOG_INFO("Таймеры запущены");
}

void LoggerDemo::stopTimers()
{
    m_timer->stop();
    m_asyncTimer->stop();
}

void LoggerDemo::runSpecificTest(int testIndex)
{
    if (testIndex >= 0 && testIndex < m_demonstrations.size()) {
        emit demonstrationStarted(m_testNames[testIndex]);
        m_demonstrations[testIndex]();
        emit demonstrationsFinished();
    } else {
        QT_LOG_ERROR("Неверный индекс теста: {}", testIndex);
    }
}

void LoggerDemo::showAvailableTests()
{
    QT_LOG_ALWAYS("=== ДОСТУПНЫЕ ТЕСТЫ ===");
    for (int i = 0; i < m_testNames.size(); ++i) {
        QT_LOG_ALWAYS("{}", m_testNames[i]);
    }
    QT_LOG_ALWAYS("=======================\n");
}

// ============================================================================
// СЛОТЫ ТАЙМЕРОВ
// ============================================================================

void LoggerDemo::onTimerTimeout()
{
    m_counter++;
    QT_LOG_DEBUG("Таймер сработал: {} раз", m_counter);

    // Каждые 5 секунд имитируем разные операции
    if (m_counter % 5 == 0) {
        simulateFileOperation();
    }
    if (m_counter % 7 == 0) {
        simulateDatabaseOperation();
    }
    if (m_counter % 10 == 0) {
        simulateExpensiveOperation();
    }
}

void LoggerDemo::onAllDemonstrationsFinished()
{
    QT_LOG_ALWAYS("Все демонстрации завершены!");
}

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ
// ============================================================================

QVariantMap LoggerDemo::createUserData()
{
    return QVariantMap{
        {"id", QRandomGenerator::global()->bounded(1000)},
        {"name", "Иван Иванов"},
        {"email", "ivan@example.com"},
        {"age", 30},
        {"active", true},
        {"last_login", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")},
        {"roles", QVariantList{"user", "editor"}}
    };
}

QVariantMap LoggerDemo::generateComplexData()
{
    // Имитация генерации сложных данных (дорогая операция)
    QVariantMap data;
    data["id"] = QRandomGenerator::global()->bounded(1000);
    data["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    data["values"] = QVariantList{1, 2, 3, 4, 5};
    data["metadata"] = QVariantMap{
        {"source", "demo"},
        {"version", "1.0"},
        {"checksum", QRandomGenerator::global()->bounded(10000)},
        {"tags", QVariantList{"important", "test", "demo"}}
    };

    // Добавляем вложенные данные для демонстрации
    data["nested"] = QVariantMap{
        {"level1", QVariantMap{
                       {"level2", QVariantMap{
                                      {"level3", "deeply_nested_data"}
                                  }}
                   }}
    };

    return data;
}

QString LoggerDemo::generateModuleData(const QString& module)
{
    return QString("%1_data_%2")
    .arg(module)
        .arg(QRandomGenerator::global()->bounded(1000), 3, 10, QChar('0'));
}

void LoggerDemo::simulateAsyncOperation()
{
    m_asyncOperationId++;
    QT_LOG_TRACE("[ASYNC] Асинхронная операция #{} запущена", m_asyncOperationId);

    // Случайный результат операции
    int resultType = QRandomGenerator::global()->bounded(3);
    switch (resultType) {
    case 0:
        QT_LOG_INFO("[ASYNC] Операция #{} успешно завершена", m_asyncOperationId);
        break;
    case 1:
        QT_LOG_WARN("[ASYNC] Операция #{} завершена с предупреждениями",
                    m_asyncOperationId);
        break;
    case 2:
        QT_LOG_ERROR("[ASYNC] Операция #{} завершена с ошибкой",
                     m_asyncOperationId);
        QT_LOG_ALWAYS("[ASYNC] ALWAYS: Операция #{} завершена с ошибкой", m_asyncOperationId);
        break;
    }
}

void LoggerDemo::simulateRiskyOperation() {
    QT_LOG_TRACE_LOCATION(); // location без сообщения

    // 50% шанс выбросить исключение
    if (QRandomGenerator::global()->bounded(2)) {
        throw std::runtime_error("Исключение в рискованной операции");
    }

    QT_LOG_TRACE_LOCATION_MSG("Рискованная операция завершена успешно");
}

void LoggerDemo::simulateExpensiveOperation()
{
    QT_LOG_DEBUG("Начало дорогой операции...");

    QElapsedTimer timer;
    timer.start();

    // Имитация тяжелых вычислений
    QThread::msleep(200 + QRandomGenerator::global()->bounded(100));
    auto result = generateComplexData();

    QT_LOG_INFO("Дорогая операция завершена за {} мс, размер данных: {} байт",
                timer.elapsed(), result.size());
}

void LoggerDemo::simulateFileOperation()
{
    QString filename = QString("test_file_%1.txt").arg(QRandomGenerator::global()->bounded(1000));
    QT_LOG_INFO("[FILE] Чтение файла: {}", filename);

    // Имитация операции с файлом
    QTimer::singleShot(300, [filename]() {
        bool success = QRandomGenerator::global()->bounded(2);
        if (success) {
            QT_LOG_DEBUG("[FILE] Файл {} прочитан успешно", filename);
        } else {
            QT_LOG_ERROR("[FILE] Ошибка чтения файла {}", filename);
            QT_LOG_ALWAYS("[FILE] ALWAYS: Критическая ошибка чтения файла");
        }
    });
}

void LoggerDemo::simulateDatabaseOperation()
{
    int userId = QRandomGenerator::global()->bounded(1000);
    QT_LOG_DEBUG("[DB] Запрос пользователя: ID={}", userId);

    // Имитация запроса к базе данных
    QTimer::singleShot(400, [userId]() {
        bool found = QRandomGenerator::global()->bounded(2);
        if (found) {
            QT_LOG_INFO("[DB] Пользователь {} найден", userId);
        } else {
            QT_LOG_WARN("[DB] Пользователь {} не найден", userId);
        }
    });
}

// ============================================================================
// МЕТОДЫ ДЕМОНСТРАЦИЙ (ПУСТЫЕ - ДЛЯ РЕАЛИЗАЦИИ)
// ============================================================================

void LoggerDemo::demonstrateBasicLogging()
{
    auto levelGuard = qt_spdlog::create_scoped_logger("trace");

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ БАЗОВОГО ЛОГИРОВАНИЯ ===");

    // 1. Демонстрация всех уровней логирования
    QT_LOG_ALWAYS("1. Все уровни логирования:");
    QT_LOG_TRACE("Это сообщение уровня TRACE");
    QT_LOG_DEBUG("Это сообщение уровня DEBUG");
    QT_LOG_INFO("Это сообщение уровня INFO");
    QT_LOG_WARN("Это сообщение уровня WARN");
    QT_LOG_ERROR("Это сообщение уровня ERROR");
    QT_LOG_CRITICAL("Это сообщение уровня CRITICAL");

    // 2. Демонстрация ALWAYS уровня (работает даже при level=off)
    QT_LOG_ALWAYS("2. ALWAYS уровень - выводится всегда:");
    QT_LOG_ALWAYS("Это сообщение ALWAYS уровня");
    QT_LOG_ALWAYS("Оно будет видно даже если установлен уровень 'off'");

    // 3. Разные типы данных
    QT_LOG_ALWAYS("3. Разные типы данных:");
    QT_LOG_INFO("Строка: {}", "Hello World");
    QT_LOG_INFO("Число: {}", 42);
    QT_LOG_INFO("Дробное число: {}", 3.14159);
    QT_LOG_INFO("Булево значение: {}", true);

    // 4. Множественные аргументы
    QT_LOG_ALWAYS("4. Множественные аргументы:");
    QT_LOG_INFO("Пользователь: {}, Возраст: {}, Активен: {}", "Иван", 25, true);
    QT_LOG_DEBUG("Координаты: x={}, y={}, z={}", 10.5, 20.3, 5.7);

    // 5. Специальные символы и форматирование
    QT_LOG_ALWAYS("5. Специальные символы:");
    QT_LOG_INFO("Путь: C:\\Program Files\\MyApp");
    QT_LOG_INFO("JSON: {\"name\": \"John\", \"age\": 30}");
    QT_LOG_INFO("Многострочный текст: Первая строка\nВторая строка");

    // 6. Демонстрация производительности - много сообщений
    QT_LOG_ALWAYS("6. Производительность - 10 быстрых сообщений:");
    for (int i = 0; i < 10; ++i) {
        QT_LOG_DEBUG("Быстрое сообщение #{}", i + 1);
    }

    QT_LOG_ALWAYS("=== БАЗОВОЕ ЛОГИРОВАНИЕ ЗАВЕРШЕНО ===\n");
}

void LoggerDemo::demonstrateQStringLogging()
{
    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ РАБОТЫ С QString ===");

    // 1. Обычные QString переменные
    QT_LOG_ALWAYS("1. Обычные QString переменные:");
    QString regularString = "Обычная QString переменная";
    QT_LOG_INFO("Обычная QString: {}", regularString);

    QString unicodeString = "QString с юникодом: Привет мир! 🌍";
    QT_LOG_INFO("Unicode QString: {}", unicodeString);

    // 2. Временные QString (rvalue)
    QT_LOG_ALWAYS("2. Временные QString (rvalue):");
    QT_LOG_WARN("Временный QString: {}", QString("Временная строка"));

    // 3. QString с форматированием
    QT_LOG_ALWAYS("3. QString с форматированием:");
    QString formatted = QString("Форматированная: %1, %2").arg("значение1").arg(123);
    QT_LOG_INFO("Форматированный QString: {}", formatted);

    // 4. Конвертация чисел в QString
    QT_LOG_ALWAYS("4. Конвертация чисел в QString:");
    QString numberString = QString::number(3.14159, 'f', 3);
    QT_LOG_INFO("Число как QString: {}", numberString);

    QString intString = QString::number(42);
    QT_LOG_INFO("Целое число как QString: {}", intString);

    // 5. Множественные QString аргументы
    QT_LOG_ALWAYS("5. Множественные QString аргументы:");
    QString name = "Анна";
    QString role = "администратор";
    int accessLevel = 5;

    QT_LOG_INFO("Пользователь: {}, Роль: {}, Уровень доступа: {}",
                name, role, accessLevel);

    // 6. Длинные QString (проверка управления памятью)
    QT_LOG_ALWAYS("6. Длинные QString:");
    QString longString;
    for (int i = 0; i < 10; ++i) {
        longString += QString("Часть %1 ").arg(i + 1);
    }
    QT_LOG_INFO("Длинная строка: {}", longString);

    // 7. Временные QString в циклах (проверка времени жизни)
    QT_LOG_ALWAYS("7. Временные QString в циклах:");
    for (int i = 0; i < 5; ++i) {
        QT_LOG_WARN("Итерация {}: {}", i + 1, QString("Временная строка %1").arg(i + 1));
    }

    // 8. Смешанные типы с QString
    QT_LOG_ALWAYS("8. Смешанные типы с QString:");
    QT_LOG_INFO("Смешанные: QString + int: {}, bool: {}",
                QString("Результат"), 100, true);

    // 9. QStringList
    QT_LOG_ALWAYS("9. QStringList:");
    QStringList stringList = {"элемент1", "элемент2", "элемент3"};
    QT_LOG_INFO("QStringList: {}", stringList.join(", "));

    // 10. Проверка времени жизни временных объектов
    QT_LOG_ALWAYS("10. Проверка времени жизни временных объектов:");
    {
        QString tempString = "Временный объект в блоке";
        QT_LOG_WARN("Внутри блока: {}", tempString);
    } // tempString уничтожается здесь, но лог должен быть безопасным

    QT_LOG_INFO("После блока - строка уничтожена, но лог корректен");

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ QString ЗАВЕРШЕНА ===\n");
}

void LoggerDemo::demonstrateCollectionAndMixedTypes()
{
    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ КОЛЛЕКЦИЙ И СМЕШАННЫХ ТИПОВ ===");

    // 1. QStringList
    QT_LOG_ALWAYS("1. QStringList:");
    QStringList fruits = {"яблоко", "апельсин", "банан", "виноград"};
    QT_LOG_INFO("Фрукты: {}", fruits);
    QT_LOG_WARN("Количество фруктов: {}", fruits.size());

    // 2. QList и QVector
    QT_LOG_ALWAYS("2. QList и QVector:");
    QList<int> intList = {1, 2, 3, 4, 5};
    QT_LOG_INFO("QList<int>: {}", qt_spdlog::formatters::formatListNums(intList));

    QVector<double> doubleVector = {-1.0, -0.5, 0.0, 1.0};
    QT_LOG_INFO("QVector<double>: {}", qt_spdlog::formatters::formatVectorNums(doubleVector));

    // 3. QMap (QMap<int, QString>, QMap<QString, float>)
    QT_LOG_ALWAYS("3. QMap:");
    QMap<int, QString> idToName = {
        {1, "Первый"},
        {2, "Второй"},
        {3, "Третий"},
        {4, "Четвертый"}
    };
    QT_LOG_INFO("ID -> Имя: {}", qt_spdlog::formatters::formatMapNumStrs(idToName));

    QMap<QString, float> studentGrades = {
        {"Анна", 95.4},
        {"Борис", 87.4},
        {"Виктор", 92.0},
        {"Дарья", 78.1}
    };
    QT_LOG_INFO("Оценки студентов: {}", qt_spdlog::formatters::formatMapStrNums(studentGrades));

    // 4. QVariantMap и сложные структуры данных
    QT_LOG_ALWAYS("4. QVariantMap и сложные структуры:");
    QVariantMap userData = createUserData();
    QT_LOG_INFO("Данные пользователя: {}", userData);

    QVariantMap complexData = generateComplexData();
    QT_LOG_WARN("Сложные данные: {}", complexData);

    // 5. Смешанные типы в коллекциях
    QT_LOG_ALWAYS("5. Смешанные типы в коллекциях:");
    QVariantList mixedList = {
        "строка",
        42,
        3.14,
        true,
        QVariantList{"вложенный", "список"},
        QVariantMap{{"ключ", "значение"}}
    };
    QT_LOG_INFO("Смешанный список: {}", mixedList);

    // 6. Вложенные структуры
    QT_LOG_ALWAYS("6. Вложенные структуры:");
    QVariantMap nestedData = {
        {"user", QVariantMap{
                     {"profile", QVariantMap{
                                     {"name", "Мария"},
                                     {"age", 28}
                                 }},
                     {"preferences", QVariantMap{
                                         {"notifications", true},
                                         {"theme", "light"}
                                     }}
                 }},
        {"system", QVariantMap{
                       {"version", "1.0.0"},
                       {"timestamp", QDateTime::currentDateTime().toString()}
                   }}
    };
    QT_LOG_INFO("Вложенные данные: {}", nestedData);

    // 7. Большие коллекции (производительность)
    QT_LOG_ALWAYS("7. Большие коллекции:");
    QVariantList largeList;
    for (int i = 0; i < 50; ++i) {
        largeList.append(QString("Элемент %1").arg(i + 1));
    }
    QT_LOG_WARN("Большой список (50 элементов): {}", largeList);

    // 8. Специальные значения
    QT_LOG_ALWAYS("8. Специальные значения:");
    QVariantList specialValues = {
        QVariant(), // null
        QString(""),
        0,
        false,
        QVariantList{},
        QVariantMap{}
    };
    QT_LOG_INFO("Специальные значения: {}", specialValues);

    // 9. Комбинирование с обычными типами
    QT_LOG_ALWAYS("9. Комбинирование с обычными типами:");
    QString message = "Результат операции";
    QVariantMap result = {
        {"status", "success"},
        {"data", QVariantList{1, 2, 3}},
        {"message", message}
    };
    QT_LOG_INFO("Сообщение: {}, Результат: {}", message, result);

    // 10. QByteArray
    QT_LOG_ALWAYS("10. QByteArray:");
    QByteArray byteData = "Raw byte data";
    QT_LOG_INFO("QByteArray: {}", qt_spdlog::formatters::formatQByteArray(byteData));
    QT_LOG_INFO("QByteArray (hex): {}", qt_spdlog::formatters::formatQByteArray(byteData, true));

    QByteArray hexData = QByteArray::fromHex("48656c6c6f20576f726c64");
    QT_LOG_WARN("Hex QByteArray: {}", hexData);
    QT_LOG_WARN("Hex QByteArray (text): {}", qt_spdlog::formatters::formatQByteArray(hexData, false));

    QByteArray binaryData = QByteArray::fromHex("0102030405060708");
    QT_LOG_INFO("Binary QByteArray: {}", qt_spdlog::formatters::formatQByteArray(binaryData, true));

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ КОЛЛЕКЦИЙ ЗАВЕРШЕНА ===\n");
}

void LoggerDemo::demonstrateFormatting()
{
    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ ФОРМАТИРОВАНИЯ ВЫВОДА ===");

    auto levelGuard = qt_spdlog::create_scoped_logger("trace");

    // Сохраняем оригинальный паттерн чтобы восстановить потом
    QString originalPattern = "%^[%T] [%l]%$ %v";

    // 1. Простой паттерн (только время и сообщение)
    QT_LOG_ALWAYS("1. Простой паттерн (только время и сообщение):");
    qt_spdlog::set_simple_pattern();
    QT_LOG_INFO("Простое сообщение");
    QT_LOG_WARN("Предупреждение с простым форматом");

    // 2. Подробный паттерн (с датой, файлом, строкой)
    QT_LOG_ALWAYS("2. Подробный паттерн (с датой, файлом, строкой):");
    qt_spdlog::set_detailed_pattern();
    QT_LOG_INFO("Детальное сообщение");
    QT_LOG_ERROR("Ошибка с детальной информацией");

    // 3. Trace паттерн (добавляет имя функции)
    QT_LOG_ALWAYS("3. Trace паттерн (добавляет имя функции):");
    qt_spdlog::set_location_pattern();
    QT_LOG_TRACE_LOCATION(); // Только location
    QT_LOG_TRACE_LOCATION_MSG("Trace с функцией и сообщением");

    // 4. Qt-style паттерн (компактный, как в Qt)
    QT_LOG_ALWAYS("4. Qt-style паттерн (компактный, как в Qt):");
    qt_spdlog::set_qt_style_pattern();
    QT_LOG_INFO("Qt-style сообщение");
    QT_LOG_WARN("Предупреждение в Qt-стиле");

    // 5. Кастомный паттерн
    QT_LOG_ALWAYS("5. Кастомный паттерн (только уровень и сообщение):");
    qt_spdlog::set_pattern("%^[%l]%$ %v");
    QT_LOG_INFO("Кастомное сообщение");
    QT_LOG_ERROR("Ошибка в кастомном формате");

    // 6. Паттерн с id потока
    QT_LOG_ALWAYS("6. Паттерн с id потока:");
    qt_spdlog::set_thread_id_pattern();
    QT_LOG_INFO("Сообщение thread ID");
    QT_LOG_DEBUG("Отладка с информацией о потоке");

    // 7. Цветной паттерн (разные цвета для разных уровней)
    QT_LOG_ALWAYS("7. Цветной паттерн:");
    qt_spdlog::set_pattern("%^[%T] [%l]%$ %v");
    QT_LOG_TRACE("Trace с цветом");
    QT_LOG_DEBUG("Debug с цветом");
    QT_LOG_INFO("Info с цветом");
    QT_LOG_WARN("Warn с цветом");
    QT_LOG_ERROR("Error с цветом");
    QT_LOG_CRITICAL("Critical с цветом");
    QT_LOG_ALWAYS("Always с цветом");

    // 8. Паттерн без цветов
    QT_LOG_ALWAYS("8. Паттерн без цветов:");
    qt_spdlog::set_pattern("[%T] [%l] %v");
    QT_LOG_INFO("Сообщение без цветов");
    QT_LOG_WARN("Предупреждение без цветов");

    // 9. Восстанавливаем оригинальный паттерн
    QT_LOG_ALWAYS("9. Восстановление оригинального паттерна:");
    qt_spdlog::set_pattern(originalPattern);
    QT_LOG_INFO("Сообщение с восстановленным форматом");
    QT_LOG_ALWAYS("Все форматы проверены!");

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ ФОРМАТИРОВАНИЯ ЗАВЕРШЕНА ===\n");
}

void LoggerDemo::demonstrateErrorScenarios()
{
    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ СЦЕНАРИЕВ С ОШИБКАМИ И ИСКЛЮЧЕНИЯМИ ===");

    // Включаем trace уровень для location макросов
    auto levelGuard = qt_spdlog::create_scoped_logger("trace");
    qt_spdlog::set_location_pattern();

    // 1. Логирование в блоке try/catch
    QT_LOG_ALWAYS("1. Логирование в блоке try/catch:");
    try {
        QT_LOG_TRACE_LOCATION_MSG("Начало опасной операции");
        simulateRiskyOperation();
        QT_LOG_TRACE_LOCATION_MSG("Операция завершена успешно");
    }
    catch (const std::exception& e) {
        QT_LOG_ERROR_LOCATION_MSG(QT_ERROR_CSTR("Поймано исключение: %1", e));
    }

    // 2. Ошибки форматирования
    QT_LOG_ALWAYS("2. Ошибки форматирования:");
    QT_LOG_INFO("Ручной перехват - мы сами бросаем и ловим fmt::format_error");
    try {
        // Вместо прямого вызова QT_LOG_INFO, сымитируем ошибку форматирования
        throw fmt::format_error("argument not found");
    }
    catch (const fmt::format_error& e) {
        QT_LOG_ERROR_LOCATION_MSG(QT_ERROR_CSTR("Ошибка форматирования: %1", e));
    }

    QT_LOG_INFO("Spdlog автоматически обрабатывает ошибки форматирования:");
    try {
        // Намеренно неправильное форматирование
        QT_LOG_INFO("Неправильное форматирование: {} {}", 42); // Недостаточно аргументов
    }
    catch (const fmt::format_error& e) {
        QT_LOG_ERROR_LOCATION_MSG(QT_ERROR_CSTR("Ошибка форматирования: %1", e));
    }

    // 3. Ошибки валидации
    QT_LOG_ALWAYS("3. Ошибки валидации:");
    try {
        QT_LOG_DEBUG_LOCATION_MSG("Начало проверки валидации");
        QT_LOG_INFO_LOCATION_MSG("Проверка пограничных значений");

        // Проверка работы с пограничными значениями
        QVariant invalidVariant;
        QT_LOG_INFO_LOCATION_MSG(QT_FORMAT_CSTR("Invalid variant: %1",
                                                qt_spdlog::formatters::formatQVariant(invalidVariant)));

        QByteArray emptyBytes;
        QT_LOG_INFO_LOCATION_MSG(QT_FORMAT_CSTR("Empty bytes: %1",
                                                qt_spdlog::formatters::formatQByteArray(emptyBytes)));

        // Имитация ошибки валидации
        throw std::runtime_error("Ошибка проверки данных");
    }
    catch (const std::exception& e) {
        QT_LOG_ERROR_LOCATION_MSG(QT_ERROR_CSTR("Поймано исключение: %1", e));
    }

    // 4. Исключения в асинхронных операциях
    QT_LOG_ALWAYS("4. Исключения в асинхронных операциях:");
    QEventLoop loop;
    QTimer::singleShot(500, [this, &loop]() { // Увеличил до 500 мс
        try {
            QT_LOG_TRACE_LOCATION_MSG("Асинхронная операция начата");
            throw std::runtime_error("Исключение в асинхронной операции");
        }
        catch (const std::exception& e) {
            QT_LOG_ERROR_LOCATION_MSG(QT_ERROR_CSTR("Асинхронная ошибка: %1", e));
            loop.quit(); // Выходим из event loop
        }
    });
    loop.exec(); // Ждем завершения асинхронной операции

    // 5. Логирование в деструкторах
    QT_LOG_ALWAYS("5. Логирование в деструкторах:");
    {
        class TempObject {
        public:
            ~TempObject() {
                try {
                    QT_LOG_DEBUG_LOCATION();
                    if (QRandomGenerator::global()->bounded(2)) {
                        throw std::logic_error("Ошибка в деструкторе");
                    }
                }
                catch (const std::exception& e) {
                    QT_LOG_ERROR_LOCATION_MSG(QT_ERROR_CSTR("Ошибка в деструкторе: %1", e));
                }
            }
        };

        TempObject obj;
        QT_LOG_INFO_LOCATION_MSG("TempObject создан, скоро будет уничтожен");
    }

    // 6. Рекурсивные вызовы логирования
    QT_LOG_ALWAYS("6. Рекурсивные вызовы логирования:");
    std::function<void(int)> recursiveLog;
    recursiveLog = [&recursiveLog](int depth) {
        if (depth <= 0) return;
        QT_LOG_TRACE_LOCATION_MSG("Рекурсивный вызов");
        recursiveLog(depth - 1);
    };

    try {
        recursiveLog(3);
        QT_LOG_INFO_LOCATION_MSG("Рекурсивное логирование завершено успешно");
    }
    catch (const std::exception& e) {
        QT_LOG_ERROR_LOCATION_MSG(QT_ERROR_CSTR("Ошибка рекурсивного логирования: %1", e));
    }

    // 7. Восстановление
    QT_LOG_ALWAYS("7. Восстановление оригинального паттерна:");
    qt_spdlog::set_default_pattern();
    QT_LOG_INFO("Обычное сообщение после ошибок");
    QT_LOG_ALWAYS("Все сценарии с ошибками проверены!");

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ СЦЕНАРИЕВ С ОШИБКАМИ ЗАВЕРШЕНА ===\n");
}

void LoggerDemo::demonstrateCustomLoggers()
{
    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ РАБОТЫ С РАЗНЫМИ ЛОГГЕРАМИ ===");

    // 1. Создание кастомных логгеров с разными именами
    QT_LOG_ALWAYS("1. Создание кастомных логгеров:");

    auto networkLogger = spdlog::stdout_color_mt("network");
    QT_LOGGER_INFO(networkLogger, "Создан network логгерт");

    auto databaseLogger = spdlog::stdout_color_mt("database");
    QT_LOGGER_INFO(databaseLogger, "Создан database логгер");

    auto businessLogger = spdlog::stdout_color_mt("business");
    QT_LOGGER_INFO(businessLogger, "Создан business логгер");

    // 2. Получение логгеров по имени
    QT_LOG_ALWAYS("2. Получение логгеров по имени:");
    auto existingNetworkLogger = spdlog::get("network");
    auto existingBusinessLogger = spdlog::get("business");

    if (existingNetworkLogger && existingBusinessLogger) {
        QT_LOGGER_INFO(existingNetworkLogger, "Существующий network логгер работает");
        QT_LOGGER_INFO(existingBusinessLogger, "Существующий business логгер работает");
    }

    // 3. Разные настройки для разных логгеров
    QT_LOG_ALWAYS("3. Разные настройки для разных логгеров:");

    networkLogger->set_level(spdlog::level::debug);
    networkLogger->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] [%n] [%l]%$ %v");
    QT_LOGGER_DEBUG(networkLogger, "networkLogger -> debug");

    databaseLogger->set_level(spdlog::level::warn);
    databaseLogger->set_pattern("%^[%H:%M:%S] [%n] [%l]%$ %v");
    QT_LOGGER_WARN(databaseLogger, "databaseLogger -> warn");

    businessLogger->set_level(spdlog::level::info);
    businessLogger->set_pattern("%^[%n] [%l]%$ %v");
    QT_LOGGER_INFO(businessLogger, "businessLogger -> info");

    // Network logger (debug уровень)
    QT_LOGGER_TRACE(networkLogger, "Network trace - не должен отобразиться");
    QT_LOGGER_DEBUG(networkLogger, "Network debug сообщение");
    QT_LOGGER_INFO(networkLogger, "Network info: подключение установлено");
    QT_LOGGER_WARN(networkLogger, "Network warning: медленное соединение");
    QT_LOGGER_ERROR(networkLogger, "Network error: таймаут соединения");

    // Database logger (warn уровень)
    QT_LOGGER_DEBUG(databaseLogger, "Database debug - не должен отобразиться");
    QT_LOGGER_INFO(databaseLogger, "Database info - не должен отобразиться");
    QT_LOGGER_WARN(databaseLogger, "Database warning: медленный запрос");
    QT_LOGGER_ERROR(databaseLogger, "Database error: ошибка constraint");

    // 4. Временное изменение уровня для конкретного логгера
    QT_LOG_ALWAYS("4. Временное изменение уровня для network логгера:");
    {
        auto scopedLevel = qt_spdlog::create_scoped_logger("business", "trace");
        if (scopedLevel) {
            QT_LOGGER_TRACE(businessLogger, "Business trace - теперь отображается!");
            QT_LOGGER_DEBUG(businessLogger, "Business debug с временным trace уровнем");
        }
    }

    // Проверяем что уровень восстановился
    QT_LOGGER_TRACE(businessLogger, "Business trace - снова не должен отображаться");
    QT_LOGGER_DEBUG(businessLogger, "Business debug - все еще работает");

    QT_LOG_ALWAYS("Все кастомные логгеры работают корректно!");

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ КАСТОМНЫХ ЛОГГЕРОВ ЗАВЕРШЕНА ===\n");
}

void LoggerDemo::demonstrateJsonLogging()
{
    QT_LOG_ALWAYS("=== JSON ЛОГИРОВАНИ ===");

    // 1. Базовое JSON логирование
    QT_LOG_ALWAYS("1. Базовое JSON логирование:");
    QT_LOG_INFO_JSON_MSG("Приложение запущено");
    QT_LOG_DEBUG_JSON_MSG("Отладочная информация");
    QT_LOG_WARN_JSON_MSG("Предупреждение системы");
    QT_LOG_ERROR_JSON_MSG("Ошибка в модуле");

    // 2. JSON с бизнес-контекстом
    QT_LOG_ALWAYS("2. Бизнес-события с контекстом:");

    // Пользовательская сессия
    QVariantMap loginFields = {
        {"user_id", 1542},
        {"username", "ivan.petrov"},
        {"ip_address", "192.168.1.100"},
        {"user_agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36"},
        {"session_id", "sess_a1b2c3d4e5"}
    };
    QT_LOG_INFO_JSON("Пользователь успешно аутентифицирован", loginFields);

    // Финансовая операция
    QVariantMap paymentFields = {
        {"transaction_id", "txn_8472abc"},
        {"amount", 2500.50},
        {"currency", "RUB"},
        {"from_account", "40817810500001234567"},
        {"to_account", "40817810500009876543"},
        {"fee", 0.0},
        {"status", "completed"}
    };
    QT_LOG_INFO_JSON("Перевод средств выполнен", paymentFields);

    // 3. Системный мониторинг в JSON
    QT_LOG_ALWAYS("3. Системные метрики в JSON:");

    QVariantMap metricsFields = {
        {"memory_used_mb", 512},
        {"memory_total_mb", 8192},
        {"cpu_percent", 45},
        {"disk_free_gb", 125},
        {"active_connections", 84},
        {"response_time_avg_ms", 124}
    };
    QT_LOG_INFO_JSON("Системные метрики", metricsFields);

    // 4. Ошибки с детальным контекстом
    QT_LOG_ALWAYS("4. Структурированные ошибки:");

    QVariantMap errorFields = {
        {"error_code", "DB_CONNECTION_FAILED"},
        {"component", "database"},
        {"operation", "user_query"},
        {"database_url", "postgresql://localhost:5432/app_db"},
        {"query_timeout_ms", 5000},
        {"retry_count", 3},
        {"last_error", "Connection refused"}
    };
    QT_LOG_ERROR_JSON("Не удалось подключиться к базе данных", errorFields);

    // 5. Аудиторские логи (compliance)
    QT_LOG_ALWAYS("5. Аудиторские логи:");

    QVariantMap auditFields = {
        {"event_type", "user_permission_changed"},
        {"admin_user", "alexey.admin"},
        {"target_user", "ivan.petrov"},
        {"permission", "financial_operations"},
        {"old_value", "denied"},
        {"new_value", "allowed"},
        {"reason", "Повышение в должности"}
    };
    QT_LOG_WARN_JSON("Изменение прав доступа пользователя", auditFields);

    // 6. Производительность и бенчмарки
    QT_LOG_ALWAYS("6. Метрики производительности:");

    QElapsedTimer perfTimer;
    perfTimer.start();

    // Имитация тяжелой операции
    QThread::msleep(150);
    qint64 elapsed = perfTimer.elapsed();

    QVariantMap perfFields = {
        {"operation", "complex_calculation"},
        {"duration_ms", elapsed},
        {"iterations", 1000},
        {"memory_peak_mb", 45},
        {"status", "completed"}
    };
    QT_LOG_DEBUG_JSON("Производительность операции", perfFields);

    // 7. Многопоточное JSON логирование
    QT_LOG_ALWAYS("7. Многопоточное JSON логирование:");

    QVector<QFuture<void>> jsonFutures;
    for (int i = 0; i < 2; ++i) {
        jsonFutures.append(QtConcurrent::run([i]() {
            QVariantMap threadFields = {
                {"thread_id", i},
                {"operation", "data_processing"},
                {"items_processed", 100 * (i + 1)},
                {"thread_name", QString("WorkerThread%1").arg(i)}
            };
            QT_LOG_INFO_JSON(QString("Поток %1 завершил обработку").arg(i), threadFields);
        }));
    }

    for (auto& future : jsonFutures) {
        future.waitForFinished();
    }

    // 8. Комбинация обычного и JSON логирования
    QT_LOG_ALWAYS("8. Гибридное логирование:");

    QT_LOG_INFO("Обычное логирование для разработки");

    QVariantMap hybridFields = {
        {"deployment", "production"},
        {"version", "2.1.0"},
        {"environment", "prod"},
        {"region", "eu-west-1"}
    };
    QT_LOG_INFO_JSON("Продакшен событие", hybridFields);

    QT_LOG_ALWAYS("=== JSON ЛОГИРОВАНИЕ ЗАВЕРШЕНО ===");
}

void LoggerDemo::demonstrateConditionalLogging()
{
    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ УСЛОВНОГО ЛОГИРОВАНИЯ ===");

    // 1. Базовое условное логирование
    QT_LOG_ALWAYS("1. Базовое условное логирование:");

    bool isDebugMode = true;
    bool hasErrors = false;
    int retryCount = 3;

    QT_LOG_IF_DEBUG(isDebugMode, "Отладочная информация не выводится");
    QT_LOG_IF_INFO(!hasErrors, "Приложение работает без ошибок");
    QT_LOG_IF_WARN(retryCount > 2, "Много повторных попыток: {}", retryCount);

    // 2. Условное логирование в циклах
    QT_LOG_ALWAYS("2. Условное логирование в циклах:");

    QVector<int> data = {1, 2, 3, 4, 5, 10, 15, 20};
    for (int i = 0; i < data.size(); ++i) {
        QT_LOG_IF_DEBUG(isDebugMode, "Обработка элемента {}: {}", i, data[i]);
        QT_LOG_IF_WARN(data[i] > 10, "Большое значение: {}", data[i]);
    }

    // 3. Условное логирование с сложными условиями
    QT_LOG_ALWAYS("3. Сложные условия:");

    QString username = "admin";
    int loginAttempts = 2;
    bool isBlocked = false;

    QT_LOG_IF_INFO(!username.isEmpty() && !isBlocked,
                   "Пользователь {} входит в систему", username);
    QT_LOG_IF_WARN(loginAttempts >= 3,
                   "Много попыток входа для пользователя: {}", username);
    QT_LOG_IF_ERROR(isBlocked,
                    "Пользователь {} заблокирован", username);

    // 4. Условное логирование производительности
    QT_LOG_ALWAYS("4. Логирование производительности:");

    QElapsedTimer timer;
    timer.start();

    simulateExpensiveOperation();

    qint64 elapsed = timer.elapsed();
    QT_LOG_IF_WARN(elapsed > 100, "Медленная операция: {} мс", elapsed);
    QT_LOG_IF_DEBUG(!hasErrors, "Время операции: {} мс", elapsed);

    // 5. Условное логирование с ALWAYS уровнем
    QT_LOG_ALWAYS("5. Условное ALWAYS логирование:");

    bool criticalError = false;
    bool systemStartup = true;

    QT_LOG_IF_ALWAYS(systemStartup, "=== СИСТЕМА ЗАПУЩЕНА ===");
    QT_LOG_IF_ALWAYS(criticalError, "!!! КРИТИЧЕСКАЯ ОШИБКА !!!");

    // 6. Условное логирование с проверкой ресурсов
    QT_LOG_ALWAYS("6. Проверка ресурсов:");

    qint64 freeMemory = 512 * 1024 * 1024; // 512 MB
    qint64 freeDiskSpace = 2 * 1024 * 1024 * 1024LL; // 2 GB

    QT_LOG_IF_WARN(freeMemory < 100 * 1024 * 1024, "Мало памяти: {} MB", freeMemory / (1024 * 1024));
    QT_LOG_IF_ERROR(freeDiskSpace < 500 * 1024 * 1024, "Мало места на диске: {} GB", freeDiskSpace / (1024 * 1024 * 1024));
    QT_LOG_IF_INFO(freeMemory > 500 * 1024 * 1024, "Память в норме: {} MB", freeMemory / (1024 * 1024));

    // 7. Условное логирование в бизнес-логике
    QT_LOG_ALWAYS("7. Бизнес-логика:");

    double accountBalance = 1500.0;
    double withdrawalAmount = 2000.0;

    QT_LOG_IF_INFO(withdrawalAmount <= accountBalance,
                   "Списание средств: {} со счета {}", withdrawalAmount, accountBalance);
    QT_LOG_IF_ERROR(withdrawalAmount > accountBalance,
                    "Недостаточно средств: запрошено {}, доступно {}", withdrawalAmount, accountBalance);

    // 8. Комбинированные условия
    QT_LOG_ALWAYS("8. Комбинированные условия:");

    bool isProduction = false;
    bool enableLogging = true;
    int logLevel = 2;

    QT_LOG_IF_DEBUG(isDebugMode && enableLogging,
                    "Расширенное логирование включено");
    QT_LOG_IF_INFO(enableLogging && logLevel >= 1,
                   "Базовое логирование активно");
    QT_LOG_IF_WARN(!isProduction && enableLogging,
                   "Внимание: логирование в development режиме");

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ УСЛОВНОГО ЛОГИРОВАНИЯ ЗАВЕРШЕНА ===\n");
}

void LoggerDemo::demonstrateQtIntegration()
{
    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ ИНТЕГРАЦИИ С QT MESSAGE SYSTEM ===");

    // Сохраняем текущие настройки
    QString originalPattern = "%^[%T] [%l]%$ %v";
    auto originalLevel = qt_spdlog::get_level();

    // 1. Настройка интеграции
    QT_LOG_ALWAYS("1. Настройка интеграции с Qt:");
    qt_spdlog::set_qt_style_pattern();
    QT_LOG_INFO("Установлен паттерн для Qt сообщений");
    qt_spdlog::setup_qt_message_handler(true);
    QT_LOG_INFO("Qt message handler установлен");

    // 2. Демонстрация Qt сообщений
    QT_LOG_ALWAYS("2. Демонстрация Qt сообщений через spdlog:");

    qDebug() << "qDebug сообщение";
    qInfo() << "qInfo сообщение";
    qWarning() << "qWarning сообщение";
    qCritical() << "qCritical сообщение";

    // 3. Qt сообщения с форматированием
    QT_LOG_ALWAYS("3. Qt сообщения с форматированием:");

    qDebug("Форматированное qDebug: %s %d", "строка", 42);
    qInfo("Форматированное qInfo: %.2f", 3.14159);
    qWarning("Форматированное qWarning: %d + %d = %d", 2, 2, 4);
    qCritical("Форматированное qCritical: ошибка в %s", "функции");

    // 4. Демонстрация фильтрации по уровням
    QT_LOG_ALWAYS("4. Фильтрация Qt сообщений по уровням:");

    QT_LOG_INFO("Установка уровня 'warn' - только warning и выше");
    qt_spdlog::set_level("warn");

    qDebug() << "Это qDebug НЕ должен отобразиться";
    qInfo() << "Это qInfo НЕ должен отобразиться";
    qWarning() << "Это qWarning ДОЛЖЕН отобразиться";
    qCritical() << "Это qCritical ДОЛЖЕН отобразиться";

    // 5. Восстановление уровня и демонстрация всех сообщений
    QT_LOG_ALWAYS("5. Все Qt сообщения при уровне 'debug':");
    qt_spdlog::set_level("debug");

    qDebug() << "Это qDebug теперь видно";
    qInfo() << "Это qInfo теперь видно";
    qWarning() << "Это qWarning теперь видно";
    qCritical() << "Это qCritical теперь видно";

    // 6. Многопоточные Qt сообщения
    QT_LOG_ALWAYS("6. Многопоточные Qt сообщения:");

    QVector<QFuture<void>> futures;
    for (int i = 0; i < 3; ++i) {
        futures.append(QtConcurrent::run([i]() {
            qDebug() << "Поток" << i << "- qDebug сообщение";
            qInfo() << "Поток" << i << "- qInfo сообщение";
            qWarning() << "Поток" << i << "- qWarning сообщение";
        }));
    }

    for (auto& future : futures) {
        future.waitForFinished();
    }

    QT_LOG_INFO("Все многопоточные Qt сообщения завершены");

    // 7. Специальные Qt типы
    QT_LOG_ALWAYS("7. Qt сообщения со специальными типами:");

    QStringList stringList = {"item1", "item2", "item3"};
    qDebug() << "QStringList:" << stringList;

    QVariantMap variantMap = {{"key1", "value1"}, {"key2", 42}};
    qInfo() << "QVariantMap:" << variantMap;

    QByteArray byteArray = "Hello Qt";
    qDebug() << "QByteArray:" << byteArray;

    QUrl url("https://example.com/path");
    qInfo() << "QUrl:" << url;

    // 8. Комбинация Qt и spdlog сообщений
    QT_LOG_ALWAYS("8. Комбинация Qt и spdlog сообщений:");

    qDebug() << "Qt debug до spdlog";
    QT_LOG_DEBUG("Spdlog debug сообщение");
    qInfo() << "Qt info после spdlog";
    QT_LOG_INFO("Spdlog info сообщение");
    qWarning() << "Qt warning вместе с spdlog";
    QT_LOG_WARN("Spdlog warning сообщение");

    // 9. Восстановление оригинальных настроек
    QT_LOG_ALWAYS("9. Восстановление оригинальных настроек:");

    qt_spdlog::set_pattern(originalPattern);
    qt_spdlog::set_level(originalLevel);

    QT_LOG_INFO("Паттерн восстановлен: {}", originalPattern);
    QT_LOG_INFO("Уровень восстановлен: {}", qt_spdlog::get_level());

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ ИНТЕГРАЦИИ С QT ЗАВЕРШЕНА ===\n");
}

void LoggerDemo::demonstrateLoggingPerformance()
{
    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ ПРОИЗВОДИТЕЛЬНОСТИ ЛОГИРОВАНИЯ ===");

    // 1. Бенчмарк обычного логирования
    QT_LOG_ALWAYS("1. Сравнение обычных строк и QString:");

    const int ITERATIONS = 1000000;
    QT_LOG_ALWAYS("Количество итераций: {}", ITERATIONS);

    QElapsedTimer timer;

    // Бенчмарк обычных C-строк
    timer.start();
    for (int i = 0; i < ITERATIONS; ++i) {
        QT_LOG_DEBUG("Обычная строка #{}", i);
    }
    qint64 cstringTime = timer.elapsed();

    // Бенчмарк QString
    timer.restart();
    for (int i = 0; i < ITERATIONS; ++i) {
        QString message = QString("QString сообщение #%1").arg(i);
        QT_LOG_DEBUG("{}", message);
    }
    qint64 qstringTime = timer.elapsed();

    // Сравнение результатов
    QT_LOG_INFO("Время {} обычных строк: {} мс ({:.2f} мкс/сообщение)",
                ITERATIONS, cstringTime, (static_cast<double>(cstringTime) / ITERATIONS) * 1000);
    QT_LOG_INFO("Время {} QString: {} мс ({:.2f} мкс/сообщение)",
                ITERATIONS, qstringTime, (static_cast<double>(qstringTime) / ITERATIONS) * 1000);

    // Разница в производительности
    double difference = (static_cast<double>(qstringTime) / cstringTime) * 100;
    QT_LOG_INFO("QString медленнее на: {:.1f}%", difference - 100);


    // 2. Бенчмарк should_log для разных уровней
    QT_LOG_ALWAYS("2. Бенчмарк should_log с дорогой операцией:");

    const int COMPLEX_ITERATIONS = 100000;
    QT_LOG_ALWAYS("Количество итераций: {}", COMPLEX_ITERATIONS);

    // Без проверки should_log
    timer.start();
    for (int i = 0; i < COMPLEX_ITERATIONS; ++i) {
        auto complexData = generateComplexData(); // Дорогая операция
        QT_LOG_DEBUG("Сложные данные: {}", complexData);
    }
    qint64 noCheckTime = timer.elapsed();

    // С проверкой should_log
    timer.restart();
    for (int i = 0; i < COMPLEX_ITERATIONS; ++i) {
        if (spdlog::default_logger()->should_log(spdlog::level::debug)) {
            auto complexData = generateComplexData(); // Дорогая операция
            QT_LOG_DEBUG("Сложные данные: {}", complexData);
        }
    }
    qint64 withCheckTime = timer.elapsed();

    // Сравнение
    QT_LOG_INFO("Без should_log: {} мс (генерируются все данные)", noCheckTime);
    QT_LOG_INFO("С should_log: {} мс (данные генерируются только при необходимости)", withCheckTime);

    double savings = (static_cast<double>(noCheckTime - withCheckTime) / noCheckTime) * 100;
    QT_LOG_INFO("Экономия времени: {:.1f}%", savings);

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ ПРОИЗВОДИТЕЛЬНОСТИ ЗАВЕРШЕНА ===\n");
}

void LoggerDemo::demonstrateLoggingLevels()
{
    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ ВСЕХ УРОВНЕЙ ЛОГИРОВАНИЯ ===");

    // Сохраняем текущий уровень
    auto originalLevel = qt_spdlog::get_level();
    QT_LOG_INFO("Текущий уровень: {}", originalLevel);

    // 1. Уровень TRACE (самый детальный)
    QT_LOG_ALWAYS("1. Уровень TRACE (самый детальный):");
    qt_spdlog::set_level("trace");

    QT_LOG_TRACE("Сообщение уровня TRACE");
    QT_LOG_DEBUG("Сообщение уровня DEBUG");
    QT_LOG_INFO("Сообщение уровня INFO");
    QT_LOG_WARN("Сообщение уровня WARN");
    QT_LOG_ERROR("Сообщение уровня ERROR");
    QT_LOG_CRITICAL("Сообщение уровня CRITICAL");
    QT_LOG_ALWAYS("Сообщение уровня ALWAYS");

    // 2. Уровень DEBUG
    QT_LOG_ALWAYS("2. Уровень DEBUG:");
    qt_spdlog::set_level("debug");

    QT_LOG_TRACE("TRACE - не должен отобразиться");
    QT_LOG_DEBUG("Сообщение уровня DEBUG");
    QT_LOG_INFO("Сообщение уровня INFO");
    QT_LOG_WARN("Сообщение уровня WARN");
    QT_LOG_ERROR("Сообщение уровня ERROR");
    QT_LOG_CRITICAL("Сообщение уровня CRITICAL");
    QT_LOG_ALWAYS("Сообщение уровня ALWAYS");

    // 3. Уровень INFO
    QT_LOG_ALWAYS("3. Уровень INFO:");
    qt_spdlog::set_level("info");

    QT_LOG_TRACE("TRACE - не должен отобразиться");
    QT_LOG_DEBUG("DEBUG - не должен отобразиться");
    QT_LOG_INFO("Сообщение уровня INFO");
    QT_LOG_WARN("Сообщение уровня WARN");
    QT_LOG_ERROR("Сообщение уровня ERROR");
    QT_LOG_CRITICAL("Сообщение уровня CRITICAL");
    QT_LOG_ALWAYS("Сообщение уровня ALWAYS");

    // 4. Уровень WARN
    QT_LOG_ALWAYS("4. Уровень WARN:");
    qt_spdlog::set_level("warn");

    QT_LOG_TRACE("TRACE - не должен отобразиться");
    QT_LOG_DEBUG("DEBUG - не должен отобразиться");
    QT_LOG_INFO("INFO - не должен отобразиться");
    QT_LOG_WARN("Сообщение уровня WARN");
    QT_LOG_ERROR("Сообщение уровня ERROR");
    QT_LOG_CRITICAL("Сообщение уровня CRITICAL");
    QT_LOG_ALWAYS("Сообщение уровня ALWAYS");

    // 5. Уровень ERROR
    QT_LOG_ALWAYS("5. Уровень ERROR:");
    qt_spdlog::set_level("error");

    QT_LOG_TRACE("TRACE - не должен отобразиться");
    QT_LOG_DEBUG("DEBUG - не должен отобразиться");
    QT_LOG_INFO("INFO - не должен отобразиться");
    QT_LOG_WARN("WARN - не должен отобразиться");
    QT_LOG_ERROR("Сообщение уровня ERROR");
    QT_LOG_CRITICAL("Сообщение уровня CRITICAL");
    QT_LOG_ALWAYS("Сообщение уровня ALWAYS");

    // 6. Уровень CRITICAL
    QT_LOG_ALWAYS("6. Уровень CRITICAL:");
    qt_spdlog::set_level("critical");

    QT_LOG_TRACE("TRACE - не должен отобразиться");
    QT_LOG_DEBUG("DEBUG - не должен отобразиться");
    QT_LOG_INFO("INFO - не должен отобразиться");
    QT_LOG_WARN("WARN - не должен отобразиться");
    QT_LOG_ERROR("ERROR - не должен отобразиться");
    QT_LOG_CRITICAL("Сообщение уровня CRITICAL");
    QT_LOG_ALWAYS("Сообщение уровня ALWAYS");

    // 7. Уровень OFF
    QT_LOG_ALWAYS("7. Уровень OFF:");
    qt_spdlog::set_level("off");

    QT_LOG_TRACE("TRACE - не должен отобразиться");
    QT_LOG_DEBUG("DEBUG - не должен отобразиться");
    QT_LOG_INFO("INFO - не должен отобразиться");
    QT_LOG_WARN("WARN - не должен отобразиться");
    QT_LOG_ERROR("ERROR - не должен отобразиться");
    QT_LOG_CRITICAL("CRITICAL - не должен отобразиться");
    QT_LOG_ALWAYS("Сообщение уровня ALWAYS (всегда отображается!)");

    // 8. Демонстрация алиасов уровней
    QT_LOG_ALWAYS("8. Алиасы уровней:");

    QT_LOG_INFO("Уровень 'warning' (алиас для 'warn'):");
    qt_spdlog::set_level("warning");
    QT_LOG_WARN("Сообщение WARN при уровне 'warning'");
    QT_LOG_INFO("INFO - не должен отобразиться");

    QT_LOG_INFO("Уровень 'always' (алиас для 'off'):");
    qt_spdlog::set_level("always");
    QT_LOG_ALWAYS("ALWAYS сообщение при уровне 'always'");
    QT_LOG_INFO("INFO - не должен отобразиться");

    // 9. Восстановление оригинального уровня
    QT_LOG_ALWAYS("9. Восстановление оригинального уровня:");
    qt_spdlog::set_level(originalLevel);
    QT_LOG_INFO("Уровень восстановлен: {}", qt_spdlog::get_level());

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ УРОВНЕЙ ЛОГИРОВАНИЯ ЗАВЕРШЕНА ===\n");
}

void LoggerDemo::demonstrateScopedLevel()
{
    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ SCOPED УРОВНЯ (RAII) ===");

    auto originalLevel = qt_spdlog::get_level();
    QT_LOG_INFO("Начальный уровень: {}", originalLevel);

    // 1. Базовый scoped уровень
    QT_LOG_ALWAYS("1. Базовый scoped уровень:");

    QT_LOG_INFO("До scoped блока - уровень: {}", qt_spdlog::get_level());
    {
        auto scopedLevel = qt_spdlog::create_scoped_logger("trace");
        if (scopedLevel) {
            QT_LOG_INFO("Внутри scoped блока - уровень: {}", qt_spdlog::get_level());
            QT_LOG_TRACE("TRACE сообщение теперь видно!");
            QT_LOG_DEBUG("DEBUG сообщение теперь видно!");
        }
    }
    QT_LOG_INFO("После scoped блока - уровень: {}", qt_spdlog::get_level());

    // 2. Scoped уровень для конкретного логгера
    QT_LOG_ALWAYS("2. Scoped уровень для конкретного логгера:");

    auto customLogger = spdlog::stdout_color_mt("scoped_test");
    customLogger->set_level(spdlog::level::warn);

    QT_LOGGER_INFO(customLogger, "До scoped - это сообщение не должно отобразиться");
    {
        auto scopedCustom = qt_spdlog::create_scoped_logger("scoped_test", "debug");
        if (scopedCustom) {
            QT_LOGGER_DEBUG(customLogger, "Внутри scoped - DEBUG сообщение видно!");
            QT_LOGGER_INFO(customLogger, "Внутри scoped - INFO сообщение видно!");
        }
    }
    QT_LOGGER_INFO(customLogger, "После scoped - снова не должно отобразиться");

    // 3. Вложенные scoped уровни
    QT_LOG_ALWAYS("3. Вложенные scoped уровни:");

    QT_LOG_INFO("Уровень 0: {}", qt_spdlog::get_level());
    {
        auto level1 = qt_spdlog::create_scoped_logger("debug");
        QT_LOG_INFO("Уровень 1: {}", qt_spdlog::get_level());
        QT_LOG_DEBUG("DEBUG сообщение на уровне 1");

        {
            auto level2 = qt_spdlog::create_scoped_logger("trace");
            QT_LOG_INFO("Уровень 2: {}", qt_spdlog::get_level());
            QT_LOG_TRACE("TRACE сообщение на уровне 2");

            {
                auto level3 = qt_spdlog::create_scoped_logger("error");
                QT_LOG_INFO("Уровень 3: {}", qt_spdlog::get_level());
                QT_LOG_ERROR("ERROR сообщение на уровне 3");
                QT_LOG_TRACE("TRACE сообщение - не должно отобразиться");
            }

            QT_LOG_INFO("Уровень 2 восстановлен: {}", qt_spdlog::get_level());
            QT_LOG_TRACE("TRACE сообщение снова видно!");
        }

        QT_LOG_INFO("Уровень 1 восстановлен: {}", qt_spdlog::get_level());
        QT_LOG_DEBUG("DEBUG сообщение все еще видно");
    }
    QT_LOG_INFO("Исходный уровень восстановлен: {}", qt_spdlog::get_level());

    // 4. Scoped уровень в функциях
    QT_LOG_ALWAYS("4. Scoped уровень в функциях:");

    auto functionWithScopedLevel = []() {
        auto functionLevel = qt_spdlog::create_scoped_logger("trace");
        QT_LOG_TRACE("TRACE из функции");
        QT_LOG_DEBUG("DEBUG из функции");
        return qt_spdlog::get_level();
    };

    QT_LOG_INFO("Уровень до функции: {}", qt_spdlog::get_level());
    auto levelInFunction = functionWithScopedLevel();
    QT_LOG_INFO("Уровень в функции: {}", levelInFunction);
    QT_LOG_INFO("Уровень после функции: {}", qt_spdlog::get_level());

    // 5. Scoped уровень с обработкой ошибок
    QT_LOG_ALWAYS("5. Scoped уровень с обработкой ошибок:");

    {
        auto errorScoped = qt_spdlog::create_scoped_logger("invalid_level_name");
        if (!errorScoped) {
            QT_LOG_ERROR("Scoped logger не создан - неверное имя уровня");
        } else {
            QT_LOG_INFO("Это сообщение не должно отобразиться");
        }
    }

    // 6. Scoped уровень для отладки сложных операций
    QT_LOG_ALWAYS("6. Scoped уровень для отладки сложных операций:");

    QT_LOG_INFO("Начало сложной операции...");
    {
        auto debugScoped = qt_spdlog::create_scoped_logger("debug");
        QT_LOG_DEBUG("Начало отладочного блока");

        // Имитация сложной операции с детальным логированием
        for (int i = 0; i < 3; ++i) {
            QT_LOG_DEBUG("Итерация {}: данные = {}", i, generateComplexData());
            QThread::msleep(10);
        }

        QT_LOG_DEBUG("Конец отладочного блока");
    }
    QT_LOG_INFO("Сложная операция завершена");

    // 7. Восстановление оригинального уровня
    QT_LOG_ALWAYS("7. Восстановление оригинального уровня:");
    qt_spdlog::set_level(originalLevel);
    QT_LOG_INFO("Финальный уровень: {}", qt_spdlog::get_level());

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ SCOPED УРОВНЯ ЗАВЕРШЕНА ===\n");
}

void LoggerDemo::demonstrateThreadLocalLogging()
{
    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ THREAD-LOCAL ЛОГИРОВАНИЯ ===");

    // 1. Базовое thread-local логирование
    QT_LOG_ALWAYS("1. Базовое thread-local логирование:");

    QT_LOG_INFO("Основной поток - default логгер");
    QT_LOG_INFO_TS("Основной поток - thread-local логгер");

    QT_LOG_INFO("Имя модуля в основном потоке: {}", qt_spdlog::get_current_module());

    // 2. Смена модуля в основном потоке
    QT_LOG_ALWAYS("2. Смена модуля в основном потоке:");

    qt_spdlog::set_current_module("MainModule");
    QT_LOG_INFO_TS("Сообщение из MainModule");

    {
        auto scopedModule = qt_spdlog::module("TemporaryModule");
        QT_LOG_INFO_TS("Сообщение из TemporaryModule");
    }

    QT_LOG_INFO_TS("Снова MainModule");

    // 3. Создание нескольких потоков
    QT_LOG_ALWAYS("3. Многопоточное логирование:");

    QVector<QFuture<void>> futures;
    const int THREAD_COUNT = 5;

    for (int i = 0; i < THREAD_COUNT; ++i) {
        futures.append(QtConcurrent::run([i]() {
            // Устанавливаем уникальный модуль для каждого потока
            qt_spdlog::set_current_module(QString("Worker%1").arg(i));

            // Логируем через thread-local логгер
            QT_LOG_INFO_TS("Поток запущен");
            QT_LOG_DEBUG_TS("Отладочная информация потока");

            // Имитация работы
            QThread::msleep(50 + i * 10);

            QT_LOG_WARN_TS("Поток завершает работу");
            QT_LOG_INFO_TS("Результат работы потока: {}", i * 100);
        }));
    }

    // Ждем завершения всех потоков
    for (auto& future : futures) {
        future.waitForFinished();
    }

    QT_LOG_INFO("Все потоки завершены");

    // 4. Thread-local логгеры с разными настройками
    QT_LOG_ALWAYS("4. Thread-local логгеры с разными настройками:");


    QFuture<void> customLoggerFuture = QtConcurrent::run([]() {
        qt_spdlog::set_current_module("CustomLogger");

        // Получаем thread-local логгер и настраиваем его
        auto threadLogger = qt_spdlog::get_thread_local_logger();
        threadLogger->set_level(spdlog::level::debug);
        threadLogger->set_pattern("%^[%T] [%n] [%t] [%l]%$ %v");

        QT_LOG_DEBUG_TS("Debug сообщение с кастомным паттерном");
        QT_LOG_INFO_TS("Info сообщение с кастомным паттерном");
        QT_LOG_WARN_TS("Warn сообщение с кастомным паттерном");
    });

    customLoggerFuture.waitForFinished();

    // 5. Проверка изоляции thread-local логгеров
    QT_LOG_ALWAYS("5. Проверка изоляции thread-local логгеров:");

    QT_LOG_INFO("Основной поток - default логгер (паттерн без thread ID)");
    QT_LOG_INFO_TS("Основной поток - thread-local логгер (паттерн с thread ID)");

    // 6. Scoped модули в потоках
    QT_LOG_ALWAYS("6. Scoped модули в потоках:");

    QFuture<void> scopedModuleFuture = QtConcurrent::run([]() {
        qt_spdlog::set_current_module("InitialModule");
        QT_LOG_INFO_TS("Начало в InitialModule");

        {
            auto scopedModule = qt_spdlog::module("ScopedModule");
            QT_LOG_INFO_TS("Внутри scoped модуля");
            QT_LOG_DEBUG_TS("Отладка в scoped модуле");
        }

        QT_LOG_INFO_TS("Возврат в InitialModule");
    });

    scopedModuleFuture.waitForFinished();

    // 8. Восстановление основного модуля
    QT_LOG_ALWAYS("8. Восстановление основного модуля:");
    qt_spdlog::set_current_module("default");
    QT_LOG_INFO_TS("Возврат к default модулю");

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ THREAD-LOCAL ЛОГИРОВАНИЯ ЗАВЕРШЕНА ===\n");
}

void LoggerDemo::demonstrateScopedModule()
{
    // Сохраняем оригинальный паттерн
    QString originalPattern = "%^[%T] [%l]%$ %v";

    // Устанавливаем паттерн с модулем и потоком для наглядности
    qt_spdlog::set_pattern("%^[%T] [%l] [%n] [%t]%$ %v");

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ ВРЕМЕННЫХ МОДУЛЕЙ (SCOPED MODULE) ===");

    // Устанавливаем начальный модуль для наглядности
    qt_spdlog::set_current_module("MainApp");
    QString originalModule = qt_spdlog::get_current_module();
    QT_LOG_INFO("Исходный модуль: {}", originalModule);

    // 1. Базовое использование scoped модуля
    QT_LOG_ALWAYS("1. Базовое использование scoped модуля:");

    QT_LOG_INFO_TS("Сообщение до scoped модуля");

    {
        auto module = qt_spdlog::module("Database");
        QT_LOG_INFO_TS("Запрос к базе данных");
        QT_LOG_DEBUG_TS("Подключение к PostgreSQL");
        QT_LOG_INFO_TS("Выполнение SQL запроса");
    }

    QT_LOG_INFO_TS("Сообщение после scoped модуля - автоматически восстановлен исходный модуль");

    // 2. Вложенные scoped модули
    QT_LOG_ALWAYS("2. Вложенные scoped модули:");

    {
        auto outerModule = qt_spdlog::module("Network");
        QT_LOG_INFO_TS("Начало сетевой операции");

        {
            auto innerModule = qt_spdlog::module("HTTP");
            QT_LOG_INFO_TS("HTTP запрос к API");
            QT_LOG_DEBUG_TS("Headers: Content-Type: application/json");

            {
                auto deepestModule = qt_spdlog::module("JSON");
                QT_LOG_INFO_TS("Парсинг JSON ответа");
                QT_LOG_DEBUG_TS("Поле 'status': 'success'");
            }

            QT_LOG_INFO_TS("Возврат в HTTP модуль");
        }

        QT_LOG_INFO_TS("Возврат в Network модуль");
    }

    QT_LOG_INFO_TS("Возврат в исходный модуль");

    // 3. Scoped модули в многопоточном контексте
    QT_LOG_ALWAYS("3. Scoped модули в многопоточном контексте:");

    QVector<QFuture<void>> futures;

    for (int i = 0; i < 3; ++i) {
        futures.append(QtConcurrent::run([i]() {
            // Каждый поток начинает с default модуля
            QT_LOG_INFO_TS("Поток {} запущен", i);

            {
                auto module = qt_spdlog::module(QString("Worker%1").arg(i));
                QT_LOG_INFO_TS("Поток {} начал работу в своем модуле", i);

                // Имитация работы
                for (int j = 0; j < 2; ++j) {
                    QT_LOG_DEBUG_TS("Поток {} - итерация {}", i, j);
                    QThread::msleep(10);
                }

                QT_LOG_INFO_TS("Поток {} завершил работу в своем модуле", i);
            }

            // Автоматическое восстановление модуля
            QT_LOG_INFO_TS("Поток {} вернулся в default модуль", i);
        }));
    }

    // Ждем завершения потоков
    for (auto& future : futures) {
        future.waitForFinished();
    }

    QT_LOG_INFO("Все потоки завершили работу с scoped модулями");

    // 4. Сочетание scoped модулей и scoped уровней
    QT_LOG_ALWAYS("4. Сочетание scoped модулей и scoped уровней:");

    {
        auto levelGuard = qt_spdlog::create_scoped_logger("debug");
        auto moduleGuard = qt_spdlog::module("Security");

        QT_LOG_TRACE_TS("Security trace - теперь видно!");
        QT_LOG_DEBUG_TS("Проверка прав доступа");
        QT_LOG_INFO_TS("Пользователь аутентифицирован");
        QT_LOG_WARN_TS("Подозрительная активность");
    }

    QT_LOG_INFO_TS("Уровень и модуль автоматически восстановлены");

    // 5. Обработка ошибок с scoped модулями
    QT_LOG_ALWAYS("5. Обработка ошибок с scoped модулями:");

    try {
        auto errorModule = qt_spdlog::module("ErrorHandling");
        QT_LOG_INFO_TS("Начало обработки ошибок");

        // Имитация ошибки
        throw std::runtime_error("Критическая ошибка в модуле ErrorHandling");
    }
    catch (const std::exception& e) {
        // Модуль автоматически восстановился даже при исключении!
        QT_LOG_ERROR_TS("Поймано исключение: {}", e.what());
        QT_LOG_INFO_TS("Модуль автоматически восстановлен после исключения: {}",
                       qt_spdlog::get_current_module());
    }

    // 6. Длительные операции с scoped модулями
    QT_LOG_ALWAYS("6. Длительные операции с scoped модулями:");

    {
        auto longOpModule = qt_spdlog::module("LongOperation");
        QT_LOG_INFO_TS("Начало длительной операции");

        for (int i = 0; i < 3; ++i) {
            QT_LOG_INFO_TS("Прогресс: {}/3", i + 1);
            QThread::msleep(50);
        }

        QT_LOG_INFO_TS("Длительная операция завершена");
    }

    QT_LOG_INFO_TS("Автоматический возврат из LongOperation модуля");

    // 7. Проверка восстановления исходного модуля
    QT_LOG_ALWAYS("7. Проверка восстановления исходного модуля:");

    QString finalModule = qt_spdlog::get_current_module();
    QT_LOG_INFO("Финальный модуль: {}", finalModule);
    QT_LOG_INFO("Исходный модуль: {}", originalModule);

    if (finalModule == originalModule) {
        QT_LOG_ALWAYS("✅ Модуль успешно восстановлен!");
    } else {
        QT_LOG_ERROR("❌ Ошибка восстановления модуля!");
    }

    // Восстанавливаем оригинальный паттерн
    qt_spdlog::set_pattern(originalPattern);

    QT_LOG_INFO("Паттерн восстановлен: {}", originalPattern);

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ SCOPED МОДУЛЕЙ ЗАВЕРШЕНА ===\n");
}

void LoggerDemo::demonstrateThreadLocalPerformance()
{
    QT_LOG_ALWAYS("=== ПРОИЗВОДИТЕЛЬНОСТЬ THREAD-LOCAL ЛОГИРОВАНИЯ ===");

    const int THREAD_COUNT = 4;
    const int PER_THREAD_ITERATIONS = 10000;
    const int MULTI_ITERATIONS = 2500; // 4 потока × 2500 = 10000 сообщений

    // 1. Сравнение производительности thread-local и обычного логирования
    QT_LOG_ALWAYS("1. Сравнение thread-local и обычного логирования:");

    // Бенчмарк обычного логирования (default логгер)
    QElapsedTimer defaultTimer;
    defaultTimer.start();

    for (int i = 0; i < PER_THREAD_ITERATIONS; ++i) {
        QT_LOG_INFO("Обычное логирование #{}", i);
    }

    qint64 defaultTime = defaultTimer.elapsed();

    // Бенчмарк thread-local логирования
    QElapsedTimer threadLocalTimer;
    threadLocalTimer.start();

    for (int i = 0; i < PER_THREAD_ITERATIONS; ++i) {
        QT_LOG_INFO_TS("Thread-local логирование #{}", i);
    }

    qint64 threadLocalTime = threadLocalTimer.elapsed();

    // 2. Многопоточный бенчмарк
    QT_LOG_ALWAYS("2. Многопоточный бенчмарк:");

    QVector<QFuture<qint64>> multiFutures;

    // Thread-local версия
    QElapsedTimer multiThreadLocalTimer;
    multiThreadLocalTimer.start();

    for (int i = 0; i < THREAD_COUNT; ++i) {
        multiFutures.append(QtConcurrent::run([i, MULTI_ITERATIONS]() {
            qt_spdlog::set_current_module(QString("MultiTest%1").arg(i));

            QElapsedTimer threadTimer;
            threadTimer.start();

            for (int j = 0; j < MULTI_ITERATIONS; ++j) {
                QT_LOG_INFO_TS("Многопоточное thread-local #{}", j);
            }

            return threadTimer.elapsed();
        }));
    }

    qint64 multiThreadLocalTotal = 0;
    for (int i = 0; i < multiFutures.size(); ++i) {
        qint64 threadTime = multiFutures[i].result();
        multiThreadLocalTotal += threadTime;
    }

    qint64 multiThreadLocalTime = multiThreadLocalTimer.elapsed();

    // Обычное логирование в многопоточном режиме
    multiFutures.clear();
    QElapsedTimer multiDefaultTimer;
    multiDefaultTimer.start();

    for (int i = 0; i < THREAD_COUNT; ++i) {
        multiFutures.append(QtConcurrent::run([i, MULTI_ITERATIONS]() {
            QElapsedTimer threadTimer;
            threadTimer.start();

            for (int j = 0; j < MULTI_ITERATIONS; ++j) {
                QT_LOG_INFO("Многопоточное обычное #{}", j);
            }

            return threadTimer.elapsed();
        }));
    }

    qint64 multiDefaultTotal = 0;
    for (int i = 0; i < multiFutures.size(); ++i) {
        qint64 threadTime = multiFutures[i].result();
        multiDefaultTotal += threadTime;
    }

    qint64 multiDefaultTime = multiDefaultTimer.elapsed();

    // Восстановление основного модуля
    qt_spdlog::set_current_module("default");
    QT_LOG_INFO_TS("Возврат к default модулю");

    // 3. Результаты сравнения (ОДИНАКОВО С THREADPOOL)
    QT_LOG_ALWAYS("3. Результаты сравнения:");

    // Однопоточное сравнение
    QT_LOG_INFO("Обычное логирование: {} сообщений за {} мс",
                PER_THREAD_ITERATIONS, defaultTime);
    QT_LOG_INFO("Thread-local логирование: {} сообщений за {} мс",
                PER_THREAD_ITERATIONS, threadLocalTime);

    if (defaultTime > 0) {
        double difference = (static_cast<double>(threadLocalTime) / defaultTime) * 100;
        QT_LOG_INFO("Thread-local {} на: {:.1f}%",
                    threadLocalTime > defaultTime ? "медленнее" : "быстрее",
                    std::abs(difference - 100));
    }

    // Многопоточное сравнение
    QT_LOG_INFO("Многопоточное обычное: {} сообщений за {} мс",
                THREAD_COUNT * MULTI_ITERATIONS, multiDefaultTime);
    QT_LOG_INFO("Многопоточное thread-local: {} сообщений за {} мс",
                THREAD_COUNT * MULTI_ITERATIONS, multiThreadLocalTime);

    if (multiDefaultTime > 0) {
        double multiDifference = (static_cast<double>(multiThreadLocalTime) / multiDefaultTime) * 100;
        QT_LOG_INFO("Многопоточное thread-local {} на: {:.1f}%",
                    multiThreadLocalTime > multiDefaultTime ? "медленнее" : "быстрее",
                    std::abs(multiDifference - 100));
    }

    // Сравнение суммарного времени потоков
    QT_LOG_INFO("Многопоточное обычное суммарное время потоков: {} мс", multiDefaultTotal);
    QT_LOG_INFO("Многопоточное thread-local суммарное время потоков: {} мс", multiThreadLocalTotal);

    if (multiDefaultTotal > 0) {
        double totalDifference = (static_cast<double>(multiThreadLocalTotal) / multiDefaultTotal) * 100;
        QT_LOG_INFO("Суммарное время потоков thread-local {} на: {:.1f}%",
                    multiThreadLocalTotal > multiDefaultTotal ? "больше" : "меньше",
                    std::abs(totalDifference - 100));
    }

    // 4. Анализ эффективности (ОДИНАКОВО С THREADPOOL)
    QT_LOG_ALWAYS("4. Анализ эффективности:");

    double multiDefaultEfficiency = (static_cast<double>(multiDefaultTotal) / multiDefaultTime) * 100;
    double multiThreadLocalEfficiency = (static_cast<double>(multiThreadLocalTotal) / multiThreadLocalTime) * 100;

    QT_LOG_INFO("Эффективность многопоточного обычного: {:.1f}%", multiDefaultEfficiency);
    QT_LOG_INFO("Эффективность многопоточного thread-local: {:.1f}%", multiThreadLocalEfficiency);

    // 5. Дополнительные метрики (ОДИНАКОВО С THREADPOOL)
    QT_LOG_ALWAYS("5. Дополнительные метрики:");

    double defaultAvgPerMessage = static_cast<double>(defaultTime) / PER_THREAD_ITERATIONS;
    double threadLocalAvgPerMessage = static_cast<double>(threadLocalTime) / PER_THREAD_ITERATIONS;
    double multiDefaultAvgPerMessage = static_cast<double>(multiDefaultTime) / (THREAD_COUNT * MULTI_ITERATIONS);
    double multiThreadLocalAvgPerMessage = static_cast<double>(multiThreadLocalTime) / (THREAD_COUNT * MULTI_ITERATIONS);

    QT_LOG_INFO("Обычное среднее время на сообщение: {:.3f} мкс", defaultAvgPerMessage * 1000);
    QT_LOG_INFO("Thread-local среднее время на сообщение: {:.3f} мкс", threadLocalAvgPerMessage * 1000);
    QT_LOG_INFO("Многопоточное обычное среднее время на сообщение: {:.3f} мкс", multiDefaultAvgPerMessage * 1000);
    QT_LOG_INFO("Многопоточное thread-local среднее время на сообщение: {:.3f} мкс", multiThreadLocalAvgPerMessage * 1000);

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ THREAD-LOCAL ЛОГИРОВАНИЯ ЗАВЕРШЕНА ===\n");
}

void LoggerDemo::demonstrateThreadPoolPerformance()
{
    QT_LOG_ALWAYS("=== ПРОИЗВОДИТЕЛЬНОСТЬ THREAD-POOL ЛОГИРОВАНИЯ ===");

    const int THREAD_COUNT = 4;
    const int PER_THREAD_ITERATIONS = 10000;
    const int MULTI_ITERATIONS = 2500; // 4 потока × 2500 = 10000 сообщений

    // 1. Бенчмарк ThreadPool логирования
    QT_LOG_ALWAYS("1. Бенчмарк ThreadPool логирования:");

    QElapsedTimer threadPoolTimer;
    threadPoolTimer.start();

    QVector<QFuture<qint64>> threadPoolFutures;

    // ThreadPool версия с QtConcurrent
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threadPoolFutures.append(QtConcurrent::run([i, MULTI_ITERATIONS]() {
            qt_spdlog::set_current_module(QString("ThreadPool%1").arg(i));

            QElapsedTimer threadTimer;
            threadTimer.start();

            for (int j = 0; j < MULTI_ITERATIONS; ++j) {
                QT_LOG_INFO_TS("ThreadPool логирование #{}", j);
            }

            return threadTimer.elapsed();
        }));
    }

    qint64 threadPoolTotal = 0;
    for (int i = 0; i < threadPoolFutures.size(); ++i) {
        qint64 threadTime = threadPoolFutures[i].result();
        threadPoolTotal += threadTime;
    }

    qint64 threadPoolTime = threadPoolTimer.elapsed();

    // 2. Сравнение с многопоточной версией из demonstrateThreadLocalPerformance
    QT_LOG_ALWAYS("2. Сравнение ThreadPool и многопоточного логирования:");

    // Повторяем многопоточный бенчмарк для точного сравнения
    QVector<QFuture<qint64>> multiFutures;
    QElapsedTimer multiTimer;
    multiTimer.start();

    for (int i = 0; i < THREAD_COUNT; ++i) {
        multiFutures.append(QtConcurrent::run([i, MULTI_ITERATIONS]() {
            qt_spdlog::set_current_module(QString("MultiThread%1").arg(i));

            QElapsedTimer threadTimer;
            threadTimer.start();

            for (int j = 0; j < MULTI_ITERATIONS; ++j) {
                QT_LOG_INFO_TS("Многопоточное логирование #{}", j);
            }

            return threadTimer.elapsed();
        }));
    }

    qint64 multiTotal = 0;
    for (int i = 0; i < multiFutures.size(); ++i) {
        qint64 threadTime = multiFutures[i].result();
        multiTotal += threadTime;
    }

    qint64 multiTime = multiTimer.elapsed();

    // Восстановление основного модуля
    qt_spdlog::set_current_module("default");
    QT_LOG_INFO_TS("Возврат к default модулю");

    // 3. Сравнение результатов
    QT_LOG_ALWAYS("3. Результаты сравнения:");

    QT_LOG_INFO("ThreadPool: {} сообщений за {} мс",
                THREAD_COUNT * MULTI_ITERATIONS, threadPoolTime);
    QT_LOG_INFO("Многопоточное: {} сообщений за {} мс",
                THREAD_COUNT * MULTI_ITERATIONS, multiTime);

    // Сравнение общего времени
    if (multiTime > 0) {
        double difference = (static_cast<double>(threadPoolTime) / multiTime) * 100;
        QT_LOG_INFO("ThreadPool {} на: {:.1f}%",
                    threadPoolTime > multiTime ? "медленнее" : "быстрее",
                    std::abs(difference - 100));
    }

    // Сравнение суммарного времени потоков
    QT_LOG_INFO("ThreadPool суммарное время потоков: {} мс", threadPoolTotal);
    QT_LOG_INFO("Многопоточное суммарное время потоков: {} мс", multiTotal);

    if (multiTotal > 0) {
        double totalDifference = (static_cast<double>(threadPoolTotal) / multiTotal) * 100;
        QT_LOG_INFO("Суммарное время потоков ThreadPool {} на: {:.1f}%",
                    threadPoolTotal > multiTotal ? "больше" : "меньше",
                    std::abs(totalDifference - 100));
    }

    // 4. Анализ эффективности
    QT_LOG_ALWAYS("4. Анализ эффективности:");

    double threadPoolEfficiency = (static_cast<double>(threadPoolTotal) / threadPoolTime) * 100;
    double multiEfficiency = (static_cast<double>(multiTotal) / multiTime) * 100;

    QT_LOG_INFO("Эффективность ThreadPool: {:.1f}%", threadPoolEfficiency);
    QT_LOG_INFO("Эффективность многопоточного: {:.1f}%", multiEfficiency);

    // 5. Дополнительные метрики
    QT_LOG_ALWAYS("5. Дополнительные метрики:");

    double threadPoolAvgPerMessage = static_cast<double>(threadPoolTime) / (THREAD_COUNT * MULTI_ITERATIONS);
    double multiAvgPerMessage = static_cast<double>(multiTime) / (THREAD_COUNT * MULTI_ITERATIONS);

    QT_LOG_INFO("ThreadPool среднее время на сообщение: {:.3f} мкс", threadPoolAvgPerMessage * 1000);
    QT_LOG_INFO("Многопоточное среднее время на сообщение: {:.3f} мкс", multiAvgPerMessage * 1000);

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ THREAD-POOL ЛОГИРОВАНИЯ ЗАВЕРШЕНА ===\n");
}

void LoggerDemo::demonstrateRealWorldScenarios()
{
    QT_LOG_ALWAYS("=== РЕАЛЬНЫЕ БИЗНЕС-СЦЕНАРИИ ЛОГИРОВАНИЯ ===");

    // 1. Сценарий: Запуск приложения
    QT_LOG_ALWAYS("1. Запуск и инициализация приложения:");

    QT_LOG_ALWAYS("🚀 ЗАПУСК ПРИЛОЖЕНИЯ");
    QT_LOG_INFO("Версия: 2.1.0");
    QT_LOG_INFO("Сборка: {}", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm"));
    QT_LOG_INFO("Архитектура: x86_64");
    QT_LOG_INFO("Qt версия: {}", qVersion());

    {
        auto initModule = qt_spdlog::module("Initialization");
        QT_LOG_INFO_TS("Загрузка конфигурации...");
        QThread::msleep(100);
        QT_LOG_INFO_TS("Инициализация базы данных...");
        QThread::msleep(150);
        QT_LOG_INFO_TS("Настройка сетевых соединений...");
        QThread::msleep(200);
        QT_LOG_WARN_TS("Конфигурационный файл не найден, используются значения по умолчанию");
        QT_LOG_INFO_TS("Инициализация завершена за 450 мс");
    }

    // 2. Сценарий: Пользовательская сессия
    QT_LOG_ALWAYS("2. Пользовательская сессия:");

    QString username = "ivan.petrov";
    int userId = 1542;

    QT_LOG_INFO("👤 Вход пользователя: {} (ID: {})", username, userId);

    {
        auto sessionModule = qt_spdlog::module("Session");
        QT_LOG_INFO_TS("Аутентификация пользователя...");
        QThread::msleep(50);

        QT_LOG_IF_DEBUG(true, "Токен доступа: a1b2c3d4e5f6");
        QT_LOG_INFO_TS("Проверка прав доступа...");

        if (userId > 1000) {
            QT_LOG_WARN_TS("Пользователь имеет расширенные права");
        }

        QT_LOG_INFO_TS("Сессия создана, время жизни: 3600 сек");
    }

    // 3. Сценарий: Финансовая операция
    QT_LOG_ALWAYS("3. Финансовая операция:");

    double balance = 12500.75;
    double amount = 2500.0;
    QString fromAccount = "40817810500001234567";
    QString toAccount = "40817810500009876543";

    QT_LOG_INFO("💰 Перевод средств: {} → {}", fromAccount, toAccount);

    {
        auto financeModule = qt_spdlog::module("Finance");
        QT_LOG_INFO_TS("Начало финансовой операции");
        QT_LOG_DEBUG_TS("Сумма: {}, Баланс: {}", amount, balance);

        if (amount > balance) {
            QT_LOG_ERROR_TS("Недостаточно средств: запрошено {}, доступно {}", amount, balance);
            QT_LOG_ALWAYS("❌ ОПЕРАЦИЯ ОТКЛОНЕНА: Недостаточно средств");
        } else {
            QT_LOG_INFO_TS("Средства доступны, выполнение списания...");
            QThread::msleep(100);
            QT_LOG_INFO_TS("Зачисление на счет получателя...");
            QThread::msleep(100);
            QT_LOG_INFO_TS("Комиссия: 0 руб.");
            QT_LOG_ALWAYS("✅ ОПЕРАЦИЯ УСПЕШНА: Перевод {} руб. выполнен", amount);
        }
    }

    // 4. Сценарий: Системные мониторинг
    QT_LOG_ALWAYS("4. Системный мониторинг:");

    qint64 memoryUsage = 512 * 1024 * 1024; // 512 MB
    qint64 diskFree = 15 * 1024 * 1024 * 1024LL; // 15 GB
    int cpuUsage = 45; // 45%
    int activeConnections = 127;

    QT_LOG_IF_WARN(memoryUsage > 500 * 1024 * 1024,
                   "⚠️  Высокое использование памяти: {} MB", memoryUsage / (1024 * 1024));
    QT_LOG_IF_INFO(diskFree > 10 * 1024 * 1024 * 1024LL,
                   "💾 Свободно места на диске: {} GB", diskFree / (1024 * 1024 * 1024));
    QT_LOG_IF_WARN(cpuUsage > 80,
                   "🔥 Высокая загрузка CPU: {}%", cpuUsage);
    QT_LOG_INFO("🌐 Активные подключения: {}", activeConnections);

    // 5. Сценарий: Обработка заказа
    QT_LOG_ALWAYS("5. Обработка заказа:");

    int orderId = 8472;
    QString customer = "ООО 'Ромашка'";
    double orderAmount = 15420.50;

    QT_LOG_INFO("📦 Обработка заказа #{} от {}", orderId, customer);

    {
        auto orderModule = qt_spdlog::module("OrderProcessing");

        QT_LOG_INFO_TS("Проверка доступности товаров...");
        QThread::msleep(80);
        QT_LOG_DEBUG_TS("Сумма заказа: {} руб.", orderAmount);

        QT_LOG_INFO_TS("Расчет стоимости доставки...");
        QThread::msleep(60);
        QT_LOG_INFO_TS("Доставка: 500 руб.");

        QT_LOG_INFO_TS("Формирование документов...");
        QThread::msleep(120);
        QT_LOG_WARN_TS("Требуется ручная проверка менеджером");

        QT_LOG_ALWAYS("✅ ЗАКАЗ #{} ПРИНЯТ В ОБРАБОТКУ", orderId);
    }

    // 6. Сценарий: Ошибка в системе
    QT_LOG_ALWAYS("6. Обработка системной ошибки:");

    try {
        auto errorModule = qt_spdlog::module("ErrorHandler");
        QT_LOG_ERROR_TS("Обнаружена критическая ошибка в модуле платежей");
        QT_LOG_DEBUG_TS("Код ошибки: 0x80070005");
        QT_LOG_WARN_TS("Попытка автоматического восстановления...");

        // Имитация ошибки
        throw std::runtime_error("Доступ к базе данных отклонен: неверные учетные данные");
    }
    catch (const std::exception& e) {
        QT_LOG_CRITICAL("💥 КРИТИЧЕСКАЯ ОШИБКА: {}", e.what());
        QT_LOG_ALWAYS("🔄 ЗАПУСК ПРОЦЕДУРЫ ВОССТАНОВЛЕНИЯ");

        {
            auto recoveryModule = qt_spdlog::module("Recovery");
            QT_LOG_WARN_TS("Использование резервного соединения...");
            QThread::msleep(200);
            QT_LOG_INFO_TS("Восстановление сессии пользователя...");
            QT_LOG_ALWAYS("✅ СИСТЕМА ВОССТАНОВЛЕНА");
        }
    }

    // 7. Сценарий: Завершение работы
    QT_LOG_ALWAYS("7. Завершение работы приложения:");

    QT_LOG_ALWAYS("🛑 ЗАВЕРШЕНИЕ РАБОТЫ ПРИЛОЖЕНИЯ");

    {
        auto shutdownModule = qt_spdlog::module("Shutdown");
        QT_LOG_INFO_TS("Сохранение пользовательских данных...");
        QThread::msleep(100);
        QT_LOG_INFO_TS("Закрытие сетевых соединений...");
        QThread::msleep(150);
        QT_LOG_WARN_TS("Принудительное завершение 3 активных сессий");
        QT_LOG_INFO_TS("Освобождение системных ресурсов...");
        QT_LOG_ALWAYS("✅ ПРИЛОЖЕНИЕ УСПЕШНО ЗАВЕРШЕНО");
    }

    // 8. Сводная статистика
    QT_LOG_ALWAYS("8. Сводная статистика работы:");

    int totalOperations = 47;
    int successfulOps = 45;
    int failedOps = 2;
    double successRate = (static_cast<double>(successfulOps) / totalOperations) * 100;

    QT_LOG_INFO("📊 СТАТИСТИКА ЗА СЕССИЮ:");
    QT_LOG_INFO("   Всего операций: {}", totalOperations);
    QT_LOG_INFO("   Успешных: {}", successfulOps);
    QT_LOG_INFO("   Неудачных: {}", failedOps);
    QT_LOG_INFO("   Успешность: {:.1f}%", successRate);

    if (successRate < 95.0) {
        QT_LOG_WARN("⚠️  Низкий процент успешных операций");
    } else {
        QT_LOG_INFO("✅ Отличная стабильность системы");
    }

    QT_LOG_ALWAYS("=== РЕАЛЬНЫЕ СЦЕНАРИИ ЗАВЕРШЕНЫ ===\n");
}

void LoggerDemo::demonstrateExceptionLogging()
{
    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ ЛОГИРОВАНИЯ STD И QT ИСКЛЮЧЕНИЙ ===");

    // 1. Разные типы std исключений
    QT_LOG_ALWAYS("1. Разные типы std исключений:");

    try {
        throw std::runtime_error("Ошибка выполнения: сервер недоступен");
    } catch (const std::exception& e) {
        QT_LOG_EXCEPTION_WARN(e, "проверка соединения");
        QT_LOG_EXCEPTION_ERROR(e, "основная операция");
        QT_LOG_EXCEPTION_CRITICAL(e, "критическая система");
    }

    // 2. Qt исключения
    QT_LOG_ALWAYS("2. Qt исключения:");

    try {
        throw QException();
    } catch (const QException& e) {
        QT_LOG_QEXCEPTION_WARN(e, "второстепенная Qt операция");
        QT_LOG_QEXCEPTION_ERROR(e, "основная Qt операция");
        QT_LOG_QEXCEPTION_CRITICAL(e, "критическая Qt операция");
    }

    // 3. Условное логирование std исключений
    QT_LOG_ALWAYS("3. Условное логирование std исключений:");

    bool is_production = true;
    bool enable_debug = false;
    bool system_critical = true;

    try {
        throw std::ios_base::failure("Ошибка ввода-вывода: диск переполнен");
    } catch (const std::exception& e) {
        // Используем переменные вместо прямых выражений с !
        bool is_development = !is_production;
        QT_LOG_IF_EXCEPTION_WARN(is_development, e, "мониторинг диска");
        QT_LOG_IF_EXCEPTION_ERROR(is_production, e, "мониторинг диска");
    }

    try {
        throw std::logic_error("Сложная логическая ошибка");
    } catch (const std::exception& e) {
        QT_LOG_IF_EXCEPTION_WARN(enable_debug, e, "детальная отладка");
    }

    try {
        throw std::runtime_error("Ошибка безопасности: сбой аутентификации");
    } catch (const std::exception& e) {
        QT_LOG_IF_EXCEPTION_CRITICAL(system_critical, e, "модуль безопасности");
    }

    // 4. Условное логирование Qt исключений
    QT_LOG_ALWAYS("4. Условное логирование Qt исключений:");

    bool gui_initialized = false;
    bool critical_gui = true;

    try {
        throw QException();
    } catch (const QException& e) {
        // Используем переменные вместо прямых выражений с !
        bool gui_not_initialized = !gui_initialized;
        QT_LOG_IF_QEXCEPTION_WARN(gui_not_initialized, e, "инициализация GUI");
        QT_LOG_IF_QEXCEPTION_ERROR(gui_initialized, e, "работа GUI");
    }

    try {
        throw QException();
    } catch (const QException& e) {
        QT_LOG_IF_QEXCEPTION_CRITICAL(critical_gui, e, "основное окно приложения");
    }

    // 5. Смешанные сценарии - обработка файлов
    QT_LOG_ALWAYS("5. Смешанные сценарии - обработка файлов:");

    auto processConfigFile = [](const QString& filename) {
        try {
            if (filename.isEmpty()) {
                throw std::invalid_argument("Имя файла не может быть пустым");
            }

            QFile file(filename);
            if (!file.exists()) {
                throw std::runtime_error("Файл не существует: " + filename.toStdString());
            }

            // Имитация Qt исключения при работе с файлом
            throw QException();

        } catch (const QException& e) {
            bool file_operation_failed = true;
            QT_LOG_IF_QEXCEPTION_ERROR(file_operation_failed, e, "Qt операции с файлом: " + filename);

        } catch (const std::runtime_error& e) {
            bool file_not_found = true;
            QT_LOG_IF_EXCEPTION_WARN(file_not_found, e, "поиск файла: " + filename);

        } catch (const std::invalid_argument& e) {
            bool validation_error = true;
            QT_LOG_IF_EXCEPTION_WARN(validation_error, e, "валидация имени файла");
        }
    };

    processConfigFile("");                    // Пустое имя файла
    processConfigFile("nonexistent.json");    // Несуществующий файл

    QT_LOG_ALWAYS("=== ДЕМОНСТРАЦИЯ ЛОГИРОВАНИЯ ИСКЛЮЧЕНИЙ ЗАВЕРШЕНА ===\n");
}
