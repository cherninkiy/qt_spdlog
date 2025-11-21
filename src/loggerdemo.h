#pragma once

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <QVariantMap>
#include <functional>

class LoggerDemo : public QObject
{
    Q_OBJECT

public:
    explicit LoggerDemo(QObject *parent = nullptr);
    ~LoggerDemo();

public slots:
    void checkLoggingSetup();
    void demonstrateAllScenarios();
    void startTimers();
    void stopTimers();
    void runSpecificTest(int testIndex);
    void showAvailableTests();

private slots:
    void onTimerTimeout();
    void simulateAsyncOperation();
    void onAllDemonstrationsFinished();

private:
    // Основные демонстрационные методы и "важное" логирование
    void demonstrateBasicLogging();
    // Работа с QString, временные строки
    void demonstrateQStringLogging();
    // Коллекции и спешанные типы
    void demonstrateCollectionAndMixedTypes();
    // Гибкая настройка формата вывода (Паттерны)
    void demonstrateFormatting();
    // Условное логирование
    void demonstrateConditionalLogging();
    // Сценарии с возможными ошибками
    void demonstrateErrorScenarios();
    // Логирование STD и QT исключений
    void demonstrateExceptionLogging();
    // Все сообщения на всех уровнях
    void demonstrateLoggingLevels();
    // Динамическое управление уровнем
    void demonstrateScopedLevel();
    // Демонстрация работы с разными логгерами
    void demonstrateCustomLoggers();
    // JSON логирование
    void demonstrateJsonLogging();
    // Интеграция с Qt (qDebug, qWarning)
    void demonstrateQtIntegration();
    // Бенчмарк Should Log для тяжелого объекта
    void demonstrateLoggingPerformance();
    // Логирование в многопоточном приложении (Thread-Local)
    void demonstrateThreadLocalLogging();
    // Бенчмарк Thread-Local для тяжелого объекта
    void demonstrateThreadLocalPerformance();
    // Бенчмарк Thread-Pool для тяжелого объекта
    void demonstrateThreadPoolPerformance();
    // Временный модуль
    void demonstrateScopedModule();
    // Бизнес демонстрация
    void demonstrateRealWorldScenarios();

    void initializeTestList();
    QString getDemoName(int index);

    // Вспомогательные методы
    QVariantMap createUserData();
    QVariantMap generateComplexData();
    QString generateModuleData(const QString& module);
    void simulateRiskyOperation();
    void simulateExpensiveOperation();
    void simulateFileOperation();
    void simulateDatabaseOperation();

    // Члены класса
    QVector<QString> m_testNames;
    QVector<std::function<void()>> m_demonstrations;
    QTimer* m_timer;
    QTimer* m_asyncTimer;
    QElapsedTimer m_benchmarkTimer;
    int m_counter = 0;
    int m_asyncOperationId = 0;
    bool m_demonstrationsFinished = false;

signals:
    void demonstrationsFinished();
    void demonstrationStarted(const QString& name);
    void demonstrationProgress(int current, int total);
};
