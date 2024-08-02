#include "logger.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream> 
#include <thread>


Logger::Logger() : stopFlag(false) {}

Logger::~Logger() {
    stop();
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const std::string& sender, const std::string& message) {
    std::unique_lock<std::mutex> lock(mtx);
    LogMessage logMessage{sender, message, getCurrentDateTime()};
    logQueue.push(logMessage);
    cv.notify_one();
}

void Logger::log(const std::string& message) {
    std::unique_lock<std::mutex> lock(mtx);
    LogMessage logMessage{"DEBUG", message, getCurrentDateTime()};
    logQueue.push(logMessage);
    cv.notify_one();
}

void Logger::start() {
    stopFlag = false;

    std::string logFileName = getLogFileName();
    logFile.open(logFileName, std::ios::out | std::ios::app);

    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file: " << logFileName << std::endl;
        return;
    }

    workerThread = std::thread(&Logger::processQueue, this);
    sched_param sch_params;
    sch_params.sched_priority = 1; /* MIN = 99 */
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &sch_params);
}

void Logger::stop() {
    {
        std::unique_lock<std::mutex> lock(mtx);
        stopFlag = true;
    }
    cv.notify_all();

    if (workerThread.joinable()) {
        workerThread.join();
    }

    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::processQueue() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !logQueue.empty() || stopFlag; });

        while (!logQueue.empty()) {
            LogMessage logMessage = logQueue.front();
            logQueue.pop();
            lock.unlock();

            std::string formattedMessage = logMessage.dateTime + " [" + logMessage.sender + "] " + logMessage.message + "\n";
            if (logFile.is_open()) {
                logFile << formattedMessage;
                logFile.flush();
            }

            lock.lock();
        }

        if (stopFlag && logQueue.empty()) {
            break;
        }
    }
}

std::string Logger::getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Logger::getLogFileName() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream ss;
    ss << "log_" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M-%S") << ".txt";
    return ss.str();
}
