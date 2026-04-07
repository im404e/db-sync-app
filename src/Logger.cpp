#include "../headers/Logger.h"
#include <iostream>

Logger::Logger(int port) {
    std::string time = Utils::getCurrentTimeStamp();
    for (char& c : time) {if (c == ':' || c == ' ' || c == '.') c = '-';}

    std::string file_name = "app-" + std::to_string(port) + "-" + time + ".log";
    logFile.open(file_name, std::ios::app);

    if (!logFile.is_open()) {
        std::cerr << "Error: Log file " << file_name  << " hasn't opened." << std::endl;
    }
}

void Logger::log(const std::string& message, LogLevel lvl) {
    std::lock_guard<std::mutex> lock(log_mutex);
    std::string output_message;
    switch (lvl) {
        case INFO:
            output_message = "[INFO]";
            break;
        case ERROR:
            output_message = "[ERROR]";
            break;
    }
    output_message += "["+ Utils::getCurrentTimeStamp() + "]:" + message;
    std::cout << output_message << std::endl;

    if (logFile.is_open()) {
        logFile << output_message << std::endl;
        logFile.flush();
    }
}