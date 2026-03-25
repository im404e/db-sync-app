#pragma once
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>

class Database {
private:
    mutable pqxx::connection connection;

public:
    Database(const std::string& options) : connection(options) {};
    nlohmann::json getStreetTypesData() const;
    nlohmann::json getPlaceTypesData() const;
    nlohmann::json getSoatoData() const;
    nlohmann::json getStreetsBySoatoId(const int id) const;
    nlohmann::json addAddressSoato(long long soato_id, std::string& place, int place_type_id, std::string& region);
    static nlohmann::json resultToJson(const pqxx::result& res);
};