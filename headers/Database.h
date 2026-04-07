#pragma once
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include "../headers/Logger.h"

class Database {
private:
    mutable pqxx::connection connection;
    Logger& logger;
    mutable std::mutex db_mutex;

public:
    Database(const std::string& options, Logger& logger) : connection(options), logger(logger) {};
    nlohmann::json getStreetTypesData() const;
    nlohmann::json getPlaceTypesData() const;
    nlohmann::json getSoatoData() const;
    nlohmann::json getStreetSoatoId(const int id) const;
    nlohmann::json getDelta(const std::string& table, const std::string& since);
    void upsertAddressSoato(std::string& code_soato, std::optional<std::string> area,
    std::optional<std::string> district, std::string region, int place_type_id, std::string place);
    void upsertAddressStreet(int soato_id, int street_type_id, const std::string& name);
    void upsertAddress(int soato_id, int street_id, const std::string& house, std::optional<std::string> corps,
        std::optional<std::string> flat, std::optional<std::string> zip_code, std::optional<std::string> note);
    static nlohmann::json resultToJson(const pqxx::result& res);

private:
    std::string getQueryForTable(const std::string& table) {
        if (table == "address_soato" || table == "address_street" || table == "address") {
            return "SELECT * FROM " + table + " WHERE last_update > $1::timestamp";
        }
        throw std::runtime_error("Invalid table name \"" + table+ '\"');
        return "";
    }
};