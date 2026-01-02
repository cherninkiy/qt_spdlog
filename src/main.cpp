#include <QCoreApplication>
#include <QTimer>
#include <QString>
#include <QCommandLineParser>
#include <iostream>
#include "qt_spdlog.h"
#include "loggerdemo.h"

#ifdef _WIN32
#include <windows.h>
#include <locale.h>
#else
#include <locale.h>
#include <stdlib.h>
#endif

bool setupConsoleEncoding()
{
#ifdef _WIN32
    if (SetConsoleOutputCP(65001) && SetConsoleCP(65001)) {
        setlocale(LC_ALL, ".UTF-8");
        return true;
    }
    return false;
#else
    setlocale(LC_ALL, "en_US.UTF-8");
    char* locale = setlocale(LC_ALL, NULL);
    QString currentLocale = locale ? locale : "unknown";
    return currentLocale.toLower().contains("utf-8");
#endif
}

bool initializeLogging(const QString& logger_name = "qt_app") {
    try {
        // Всегда создаем новый logger с именем
        auto logger = spdlog::stdout_color_mt(logger_name.toStdString());
        spdlog::set_default_logger(logger);

        qt_spdlog::set_level(QtMsgType::QtInfoMsg);
        qt_spdlog::set_qt_style_pattern();
        qt_spdlog::setup_qt_message_handler();
        qt_spdlog::setup_display_always();
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize logging: " << e.what() << std::endl;
        return false;
    }
}

void runInteractiveMode(LoggerDemo& loggerDemo)
{
    QTextStream stream(stdin);
    QString input;

    std::cout << "🎯 ДЕМО ЛОГГЕРА SPDLOG+QT\n";
    std::cout << "==========================\n";

    // Показываем список тестов сразу при входе
    loggerDemo.showAvailableTests();

    // Подключаем обработчики прогресса для интерактивного режима
    QObject::connect(&loggerDemo, &LoggerDemo::demonstrationStarted, [](const QString& name) {
        std::cout << "\n▶️  Запуск: " << name.toStdString() << std::endl;
    });

    QObject::connect(&loggerDemo, &LoggerDemo::demonstrationProgress, [](int current, int total) {
        std::cout << "📊 Прогресс: " << current << "/" << total << " ("
                  << (current * 100 / total) << "%)" << std::endl;
    });

    QObject::connect(&loggerDemo, &LoggerDemo::demonstrationsFinished, []() {
        std::cout << "✅ Все демонстрации завершены!" << std::endl;
    });

    while (true) {
        std::cout << "\nКоманды: 0-список, 1-17-тест, 99-все, 999-выход\n";
        std::cout << "Введите команду: ";

        stream.readLineInto(&input);
        int command = input.toInt();

        if (command == 999) {
            std::cout << "Выход...\n";
            break;
        }
        else if (command == 0) {
            loggerDemo.showAvailableTests();
        }
        else if (command == 99) {
            std::cout << "Запуск всех тестов...\n";
            loggerDemo.demonstrateAllScenarios();
        }
        else if (command >= 1 && command <= 18) {
            std::cout << "Запуск теста #" << command << "...\n";
            loggerDemo.runSpecificTest(command - 1);
        }
        else {
            std::cout << "Неизвестная команда\n";
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("LoggerDemo");
    QCoreApplication::setApplicationVersion("1.0");

    // Настройка кодировки консоли
    setupConsoleEncoding();

    // Инициализация логгирования
    if (!initializeLogging(app.applicationName())) {
        std::cerr << "CRITICAL: Failed to initialize logging!" << std::endl;
        return -1;
    }

    LoggerDemo loggerDemo;

    // Подключаем обработчики прогресса для всех режимов
    QObject::connect(&loggerDemo, &LoggerDemo::demonstrationStarted, [](const QString& name) {
        QT_LOG_INFO("🚀 Начало демонстрации:", name);
    });

    QObject::connect(&loggerDemo, &LoggerDemo::demonstrationProgress, [](int current, int total) {
        QT_LOG_DEBUG("📊 Прогресс демонстраций:", current, "из", total);
    });

    QObject::connect(&loggerDemo, &LoggerDemo::demonstrationsFinished, []() {
        QT_LOG_ALWAYS("✅ ВСЕ ДЕМОНСТРАЦИИ ЗАВЕРШЕНЫ!");
    });

    // Парсинг аргументов командной строки
    QCommandLineParser parser;
    parser.setApplicationDescription("Демонстрация логгера spdlog + Qt");
    parser.addHelpOption();
    parser.addVersionOption();

    // Опция для запуска конкретного теста
    QCommandLineOption testOption(QStringList() << "t" << "test",
                                  "Запустить конкретный тест (1-17)",
                                  "test_number");
    parser.addOption(testOption);

    // Опция для запуска всех тестов
    QCommandLineOption allTestsOption(QStringList() << "a" << "all",
                                      "Запустить все тесты");
    parser.addOption(allTestsOption);

    // Опция для показа списка тестов
    QCommandLineOption listOption(QStringList() << "l" << "list",
                                  "Показать список доступных тестов");
    parser.addOption(listOption);

    parser.process(app);

    // Обработка параметров командной строки
    if (parser.isSet(listOption)) {
        loggerDemo.showAvailableTests();
        return 0;
    }
    else if (parser.isSet(allTestsOption)) {
        std::cout << "Запуск всех тестов...\n";
        QT_LOG_ALWAYS("🎯 ЗАПУСК ВСЕХ ТЕСТОВ");
        loggerDemo.demonstrateAllScenarios();
        return 0;
    }
    else if (parser.isSet(testOption)) {
        bool ok;
        int testNumber = parser.value(testOption).toInt(&ok);
        if (ok && testNumber >= 1 && testNumber <= 17) {
            std::cout << "Запуск теста #" << testNumber << "...\n";
            QT_LOG_ALWAYS("🎯 ЗАПУСК ТЕСТА", testNumber);
            loggerDemo.runSpecificTest(testNumber - 1);
            return 0;
        } else {
            std::cerr << "Ошибка: номер теста должен быть от 1 до 17\n";
            return 1;
        }
    }

    // Запуск проверки настроек
    loggerDemo.checkLoggingSetup();

    // Если параметры не переданы - запускаем интерактивный режим
    QT_LOG_ALWAYS("🎮 ЗАПУСК ИНТЕРАКТИВНОГО РЕЖИМА");
    runInteractiveMode(loggerDemo);

    return 0;
}
