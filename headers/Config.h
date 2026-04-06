#pragma once
#include <nlohmann/json.hpp>

class Config {
private:
    nlohmann::json config;

public:
    Config(const std::string& config_file_path);
    std::string getDbOptionsAsString();
    nlohmann::json getConfigFile() const {return config;};
};