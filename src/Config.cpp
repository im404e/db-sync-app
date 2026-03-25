#include "../headers/Config.h"
#include <fstream>
#include <iostream>

Config::Config(const std::string& config_file_path) : config(config_file_path) {
    try {
        std::ifstream config_file(config_file_path);
        config = nlohmann::json::parse(config_file);
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

std::string Config::getDbOptionsAsString() {
    return "dbname=" + config["dbname"].get<std::string>() +
        " user=" + config["user"].get<std::string>() +
        " password=" + config["password"].get<std::string>() +
        " host=" + config["host"].get<std::string>() +
        " port=" + config["port"].get<std::string>();
}