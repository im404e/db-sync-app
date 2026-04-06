#pragma once
#include <fstream>
#include <mutex>
#include "../headers/Utils.h"

enum LogLevel {
    INFO,
    WARN,
    ERROR
};

class Logger {
private:
    std::ofstream logFile;
    std::mutex log_mutex;

public:
    Logger(int port);
    void log(const std::string& message, LogLevel lvl);
    ~Logger() {
        if (logFile.is_open()) logFile.close();
    }
};