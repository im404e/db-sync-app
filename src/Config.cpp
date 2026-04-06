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
    return "dbname=" + config.at("db").at("dbname").get<std::string>() +
        " user=" + config.at("db").at("user").get<std::string>() +
        " password=" + config.at("db").at("password").get<std::string>() +
        " host=" + config.at("db").at("host").get<std::string>() +
        " port=" + std::to_string(config.at("db").at("port").get<int>());
}