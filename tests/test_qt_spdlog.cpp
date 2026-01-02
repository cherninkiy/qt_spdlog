#include <QtTest/QtTest>
#include "qt_spdlog.h"
#include <spdlog/sinks/ostream_sink.h>
#include <sstream>
#include <stdexcept>

class TestQtSpdlog : public QObject
{
    Q_OBJECT

private slots:
    // Тесты форматирования
    void testFormatNum();
    void testFormatListNums();
    void testFormatVectorNums();
    void testFormatQStringList();
    void testFormatQByteArray();
    void testFormatQVariant();
    void testFormatQVariantList();
    void testFormatQVariantMap();

    // Тесты утилит
    void testFormatExceptionName();
    void testGetExceptionMessage();

    // Тесты JSON логирования
    void testJsonLog();

    // Тесты макросов (базовые)
    void testMacroTrace();
    void testMacroInfo();
    void testMacroError();
    void testMacroAlways();

    // Тесты thread-local функционала
    void testThreadLocalLogger();

    // Тесты скопов
    void testScopedModule();
    void testScopedLoggerLevel();

    // Инициализация и очистка
    void initTestCase();
    void cleanupTestCase();

private:
    std::ostringstream testStream;
    std::shared_ptr<spdlog::sinks::ostream_sink_mt> testSink;
    std::shared_ptr<spdlog::logger> testLogger;
};

void TestQtSpdlog::initTestCase()
{
    // Создаем тестовый логгер с выводом в строку
    testSink = std::make_shared<spdlog::sinks::ostream_sink_mt>(testStream);
    testLogger = std::make_shared<spdlog::logger>("test_logger", testSink);
    testLogger->set_pattern("%v"); // Только сообщение без метаданных
    testLogger->set_level(spdlog::level::trace);
    
    // Устанавливаем тестовый логгер по умолчанию
    spdlog::set_default_logger(testLogger);
}

void TestQtSpdlog::cleanupTestCase()
{
    // Сбрасываем логгер по умолчанию
    spdlog::set_default_logger(spdlog::stdout_color_mt("console"));
    testStream.clear();
}

void TestQtSpdlog::testFormatNum()
{
    // Тест целых чисел
    QCOMPARE(qt_spdlog::formatters::formatNum(42), QString("42"));
    QCOMPARE(qt_spdlog::formatters::formatNum(-100), QString("-100"));
    
    // Тест чисел с плавающей точкой
    QCOMPARE(qt_spdlog::formatters::formatNum(3.14159), QString("3.14159"));
    QCOMPARE(qt_spdlog::formatters::formatNum(-2.5), QString("-2.5"));
}

void TestQtSpdlog::testFormatListNums()
{
    QList<int> intList = {1, 2, 3, 4, 5};
    QCOMPARE(qt_spdlog::formatters::formatListNums(intList), QString("[1, 2, 3, 4, 5]"));
    
    QList<int> emptyList;
    QCOMPARE(qt_spdlog::formatters::formatListNums(emptyList), QString("[]"));
    
    QList<double> doubleList = {1.1, 2.2, 3.3};
    QCOMPARE(qt_spdlog::formatters::formatListNums(doubleList), QString("[1.1, 2.2, 3.3]"));
}

void TestQtSpdlog::testFormatVectorNums()
{
    QVector<int> intVector = {10, 20, 30};
    QCOMPARE(qt_spdlog::formatters::formatVectorNums(intVector), QString("[10, 20, 30]"));
    
    QVector<int> emptyVector;
    QCOMPARE(qt_spdlog::formatters::formatVectorNums(emptyVector), QString("[]"));
}

void TestQtSpdlog::testFormatQStringList()
{
    QStringList list = {"apple", "banana", "cherry"};
    QCOMPARE(qt_spdlog::formatters::formatQStringList(list), QString("[apple, banana, cherry]"));
    
    QStringList emptyList;
    QCOMPARE(qt_spdlog::formatters::formatQStringList(emptyList), QString("[]"));
}

void TestQtSpdlog::testFormatQByteArray()
{
    QByteArray data = "Hello World";
    
    // Текстовый режим
    QCOMPARE(qt_spdlog::formatters::formatQByteArray(data, false), QString("b'Hello World'"));
    
    // Hex режим
    QCOMPARE(qt_spdlog::formatters::formatQByteArray(data, true), QString("x'48656c6c6f20576f726c64'"));
    
    // Пустой массив
    QByteArray empty;
    QCOMPARE(qt_spdlog::formatters::formatQByteArray(empty, false), QString("b''"));
    QCOMPARE(qt_spdlog::formatters::formatQByteArray(empty, true), QString("x''"));
}

void TestQtSpdlog::testFormatQVariant()
{
    // Строка
    QVariant strVar = "Hello";
    QCOMPARE(qt_spdlog::formatters::formatQVariant(strVar), QString("Hello"));
    
    // Целое число
    QVariant intVar = 42;
    QCOMPARE(qt_spdlog::formatters::formatQVariant(intVar), QString("42"));
    
    // Вещественное число
    QVariant doubleVar = 3.14;
    QCOMPARE(qt_spdlog::formatters::formatQVariant(doubleVar), QString("3.14"));
    
    // Bool
    QVariant boolVar = true;
    QCOMPARE(qt_spdlog::formatters::formatQVariant(boolVar), QString("true"));
    
    // Null
    QVariant nullVar;
    QCOMPARE(qt_spdlog::formatters::formatQVariant(nullVar), QString("null"));
}

void TestQtSpdlog::testFormatQVariantList()
{
    QVariantList list = {1, "hello", 3.14, true};
    QString expected = "[1, hello, 3.14, true]";
    QCOMPARE(qt_spdlog::formatters::formatQVariantList(list), expected);
    
    QVariantList emptyList;
    QCOMPARE(qt_spdlog::formatters::formatQVariantList(emptyList), QString("[]"));
}

void TestQtSpdlog::testFormatQVariantMap()
{
    QVariantMap map;
    map["name"] = "John";
    map["age"] = 30;
    map["active"] = true;
    
    QString result = qt_spdlog::formatters::formatQVariantMap(map);
    // Порядок ключей может отличаться, поэтому проверяем наличие пар
    QVERIFY(result.contains("name: John"));
    QVERIFY(result.contains("age: 30"));
    QVERIFY(result.contains("active: true"));
    QVERIFY(result.startsWith("{"));
    QVERIFY(result.endsWith("}"));
    
    QVariantMap emptyMap;
    QCOMPARE(qt_spdlog::formatters::formatQVariantMap(emptyMap), QString("{}"));
}

void TestQtSpdlog::testFormatExceptionName()
{
    std::runtime_error runtimeError("test");
    std::invalid_argument invalidArgError("test");
    std::out_of_range outOfRangeError("test");
    
    QString runtimeName = qt_spdlog::utils::format_exception_name(typeid(runtimeError).name());
    QString invalidArgName = qt_spdlog::utils::format_exception_name(typeid(invalidArgError).name());
    QString outOfRangeName = qt_spdlog::utils::format_exception_name(typeid(outOfRangeError).name());
    
    QVERIFY(runtimeName.contains("runtime_error"));
    QVERIFY(invalidArgName.contains("invalid_argument"));
    QVERIFY(outOfRangeName.contains("out_of_range"));
}

void TestQtSpdlog::testGetExceptionMessage()
{
    std::runtime_error error("Test error message");
    QString message = qt_spdlog::utils::get_exception_message(error);
    QCOMPARE(message, QString("Test error message"));
}

void TestQtSpdlog::testJsonLog()
{
    testStream.str(""); // Очищаем поток
    
    QVariantMap fields;
    fields["user_id"] = 123;
    fields["action"] = "login";
    fields["success"] = true;
    
    qt_spdlog::json::json_info("User action", fields);
    
    QString output = QString::fromStdString(testStream.str());
    
    // Проверяем, что вывод содержит ожидаемые поля
    QVERIFY(output.contains("\"message\":\"User action\""));
    QVERIFY(output.contains("\"level\":\"info\""));
    QVERIFY(output.contains("\"user_id\":123"));
    QVERIFY(output.contains("\"action\":\"login\""));
    QVERIFY(output.contains("\"success\":true"));
    QVERIFY(output.contains("\"timestamp\""));
}

void TestQtSpdlog::testMacroTrace()
{
    testStream.str("");
    testLogger->set_level(spdlog::level::trace);
    
    QString testMessage = "Test trace message";
    QT_LOG_TRACE("{}", testMessage);
    
    QString output = QString::fromStdString(testStream.str());
    QCOMPARE(output.trimmed(), testMessage);
}

void TestQtSpdlog::testMacroInfo()
{
    testStream.str("");
    testLogger->set_level(spdlog::level::info);
    
    QString testMessage = "Test info message";
    QT_LOG_INFO("{}", testMessage);
    
    QString output = QString::fromStdString(testStream.str());
    QCOMPARE(output.trimmed(), testMessage);
}

void TestQtSpdlog::testMacroError()
{
    testStream.str("");
    testLogger->set_level(spdlog::level::err);
    
    QString testMessage = "Test error message";
    QT_LOG_ERROR("{}", testMessage);
    
    QString output = QString::fromStdString(testStream.str());
    QCOMPARE(output.trimmed(), testMessage);
}

void TestQtSpdlog::testMacroAlways()
{
    testStream.str("");
    testLogger->set_level(spdlog::level::off); // Устанавливаем off, но ALWAYS должен пройти
    
    QString testMessage = "Test always message";
    QT_LOG_ALWAYS("{}", testMessage);
    
    QString output = QString::fromStdString(testStream.str());
    QCOMPARE(output.trimmed(), testMessage);
}

void TestQtSpdlog::testThreadLocalLogger()
{
    // Тестируем thread-local модули
    qt_spdlog::set_current_module("test_module");
    QCOMPARE(qt_spdlog::get_current_module(), QString("test_module"));
    
    // Проверяем получение thread-local логгера
    auto logger1 = qt_spdlog::get_thread_local_logger();
    auto logger2 = qt_spdlog::get_thread_local_logger();
    QCOMPARE(logger1, logger2); // Должен быть тот же объект
    
    // Проверяем, что имя логгера содержит имя модуля
    std::string loggerName = logger1->name();
    QVERIFY(QString::fromStdString(loggerName).contains("test_module"));
    
    // Сбрасываем модуль
    qt_spdlog::set_current_module("");
}

void TestQtSpdlog::testScopedModule()
{
    QString originalModule = qt_spdlog::get_current_module();
    
    {
        auto scopedModule = qt_spdlog::module("scoped_test");
        QCOMPARE(qt_spdlog::get_current_module(), QString("scoped_test"));
        QCOMPARE(scopedModule.current_module(), QString("scoped_test"));
        QCOMPARE(scopedModule.previous_module(), originalModule);
    }
    
    // После выхода из области видимости должен восстановиться оригинальный модуль
    QCOMPARE(qt_spdlog::get_current_module(), originalModule);
}

void TestQtSpdlog::testScopedLoggerLevel()
{
    auto originalLevel = testLogger->level();
    
    {
        auto scoped = qt_spdlog::create_scoped_logger("debug");
        QVERIFY(static_cast<bool>(scoped));
        QCOMPARE(testLogger->level(), spdlog::level::debug);
    }
    
    // После выхода из области видимости должен восстановиться оригинальный уровень
    QCOMPARE(testLogger->level(), originalLevel);
}

QTEST_APPLESS_MAIN(TestQtSpdlog)
#include "test_qt_spdlog.moc"