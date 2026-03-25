#include "../headers/Database.h"
#include <iostream>
#include <crow/websocket.h>
#include <nlohmann/json.hpp>

nlohmann::json Database::getStreetTypesData() const{
    try {
        pqxx::work w(connection);
        pqxx::result res = w.exec("SELECT * FROM address_street_type");
        w.commit();
        return resultToJson(res);
    } catch (std::exception &e) {
        std::cerr << "Database error in getStreetTypesData(): " << e.what() << std::endl;
        return nlohmann::json::array();
    }
}

nlohmann::json Database::getPlaceTypesData() const{
    try {
        pqxx::work w(connection);
        pqxx::result res = w.exec("SELECT * FROM address_place_type");
        w.commit();
        return resultToJson(res);
    } catch (std::exception &e) {
        std::cerr << "Database error in getPlaceTypesData(): " << e.what() << std::endl;
        return nlohmann::json::array();
    }
}

nlohmann::json Database::getSoatoData() const{
    try {
        pqxx::work w(connection);
        pqxx::result res = w.exec("SELECT * FROM address_soato");
        w.commit();
        return resultToJson(res);
    } catch (std::exception &e) {
        std::cerr << "Database error in getSoatoData(): " << e.what() << std::endl;
        return nlohmann::json::array();
    }
}

nlohmann::json Database::getStreetsBySoatoId(const int id) const {
    try {
        pqxx::work w(connection);
        pqxx::result res = w.exec_params("SELECT * FROM address_street WHERE soato_id=$1", id);
        w.commit();
        return resultToJson(res);
    } catch (std::exception &e) {
        std::cerr << "Database error in getSoatoData(): " << e.what() << std::endl;
        return nlohmann::json::array();
    }
}

nlohmann::json Database::addAddressSoato(long long soato_id, std::string& place, int place_type_id, std::string& region) {
    try {
        pqxx::work w(connection);
        pqxx::result res = w.exec_params(
            "INSERT INTO address_soato (code_soato, place, place_type_id, region) VALUES ($1, $2, $3, $4);", soato_id, place, place_type_id, region);
        w.commit();
        return {{"output", "success"}};
    } catch (std::exception &e) {
        std::cerr << "Database error in addSoatoId(): " << e.what() << std::endl;
        return {{"output", "error"}};
    }
}

nlohmann::json Database::resultToJson(const pqxx::result& res) {
    nlohmann::json json_list = nlohmann::json::array();
    for (auto const &row: res) {
        nlohmann::json item;
        for (auto const &key: row) {
            item[key.name()] = key.c_str();
        }
        json_list.push_back(item);
    }
    return json_list;
}
