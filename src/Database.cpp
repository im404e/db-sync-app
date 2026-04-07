#include "../headers/Database.h"
#include <crow/websocket.h>
#include <nlohmann/json.hpp>

nlohmann::json Database::getStreetTypesData() const{
    try {
        std::lock_guard<std::mutex> lock(db_mutex);
        pqxx::work w(connection);
        pqxx::result res = w.exec("SELECT * FROM address_street_type");
        w.commit();
        return resultToJson(res);
    } catch (std::exception &e) {
        std::stringstream error;
        error << "Database error in getStreetTypesData(): " << e.what();
        logger.log(error.str(), LogLevel::ERROR);
        return nlohmann::json::array();
    }
}

nlohmann::json Database::getPlaceTypesData() const{
    try {
        std::lock_guard<std::mutex> lock(db_mutex);
        pqxx::work w(connection);
        pqxx::result res = w.exec("SELECT * FROM address_place_type");
        w.commit();
        return resultToJson(res);
    } catch (std::exception &e) {
        std::stringstream error;
        error << "Database error in getPlaceTypesData(): " << e.what();
        logger.log(error.str(), LogLevel::ERROR);
        return nlohmann::json::array();
    }
}

nlohmann::json Database::getSoatoData() const{
    try {
        std::lock_guard<std::mutex> lock(db_mutex);
        pqxx::work w(connection);
        pqxx::result res = w.exec("SELECT * FROM address_soato");
        w.commit();
        return resultToJson(res);
    } catch (std::exception &e) {
        std::stringstream error;
        error << "Database error in getSoatoData(): " << e.what();
        logger.log(error.str(), LogLevel::ERROR);
        return nlohmann::json::array();
    }
}

nlohmann::json Database::getStreetSoatoId(const int id) const {
    try {
        std::lock_guard<std::mutex> lock(db_mutex);
        pqxx::work w(connection);
        pqxx::result res = w.exec_params("SELECT * FROM address_street WHERE soato_id=$1", id);
        w.commit();
        return resultToJson(res);
    } catch (std::exception &e) {
        std::stringstream error;
        error << "Database error in getStreetSoatoData(): " << e.what();
        logger.log(error.str(), LogLevel::ERROR);
        return nlohmann::json::array();
    }
}

nlohmann::json Database::getDelta(const std::string& table, const std::string& since) {
    try{
        std::lock_guard<std::mutex> lock(db_mutex);
        pqxx::work w(connection);
        pqxx::result res = w.exec_params(getQueryForTable(table), since);
        w.commit();
        return resultToJson(res);
    } catch (std::exception &e) {
        std::stringstream error;
        error << "Database error in getDelta() for table [" << table << "]: " << e.what();
        logger.log(error.str(), LogLevel::ERROR);
        return nlohmann::json::array();
    }
}

void Database::upsertAddressSoato(std::string& code_soato, std::optional<std::string> area,
    std::optional<std::string> district, std::string region, int place_type_id, std::string place) {
    try {
        std::lock_guard<std::mutex> lock(db_mutex);
        pqxx::work w(connection);
        w.exec_params(
        "INSERT INTO address_soato (code_soato, area, district, region, place_type_id, place, last_update) "
            "VALUES ($1, $2, $3, $4, $5, $6, CURRENT_TIMESTAMP) "
            "ON CONFLICT (code_soato) DO UPDATE SET "
            "area = EXCLUDED.area, "
            "district = EXCLUDED.district, "
            "region = EXCLUDED.region, "
            "place_type_id = EXCLUDED.place_type_id, "
            "place = EXCLUDED.place, "
            "last_update = CURRENT_TIMESTAMP "
            "WHERE "
            "address_soato.area IS DISTINCT FROM EXCLUDED.area OR "
            "address_soato.district IS DISTINCT FROM EXCLUDED.district OR "
            "address_soato.region IS DISTINCT FROM EXCLUDED.region OR "
            "address_soato.place_type_id IS DISTINCT FROM EXCLUDED.place_type_id OR "
            "address_soato.place IS DISTINCT FROM EXCLUDED.place",
            code_soato, area, district, region, place_type_id, place
        );
        std::stringstream message;
        message << "Upsert in address_soato table.\n\tcode_soato: " << code_soato
        << "\n\tarea: " << area.value_or(" ")
        << "\n\tdistrict: " << district.value_or(" ")
        << "\n\tregion: " << region
        << "\n\tplace_type_id: " << place_type_id
        << "\n\tplace: " << place;
        logger.log(message.str(), LogLevel::INFO);
        w.commit();
    } catch (const std::exception &e) {
        std::stringstream error;
        error << "Database error in upsertAddresSoato(): " << e.what();
        logger.log(error.str(), LogLevel::ERROR);
    }
}


void Database::upsertAddressStreet(int soato_id, int street_type_id, const std::string& name) {
    try {
        std::lock_guard<std::mutex> lock(db_mutex);
        pqxx::work w(connection);
        w.exec_params(
        "INSERT INTO address_street (soato_id, street_type_id, name, last_update) "
            "VALUES ($1, $2, $3, CURRENT_TIMESTAMP) "
            "ON CONFLICT (name, soato_id) "
                    "DO UPDATE SET "
                    "street_type_id = EXCLUDED.street_type_id, "
                    "last_update = CURRENT_TIMESTAMP "
                    "WHERE address_street.street_type_id IS DISTINCT FROM EXCLUDED.street_type_id",
        soato_id, street_type_id, name
        );
        std::stringstream message;
        message << "Upsert in address_street table:\n\tsoato_id: " << soato_id
        << "\n\tstreet_type_id: " << street_type_id
        << "\n\tname: " << name;
        logger.log(message.str(), LogLevel::INFO);
        w.commit();
    } catch (const std::exception &e) {
        std::stringstream error;
        error << "Database error in upsertAddresStreet(): " << e.what();
        logger.log(error.str(), LogLevel::ERROR);
    }
}

void Database::upsertAddress(int soato_id, int street_id,
    const std::string& house,
    std::optional<std::string> corps,
    std::optional<std::string> flat,
    std::optional<std::string> zip_code,
    std::optional<std::string> note) {
    try {
        std::lock_guard<std::mutex> lock(db_mutex);
        pqxx::work w(connection);
        w.exec_params(
        "INSERT INTO address (soato_id, street_id, house, corps, flat, zip_code, note, last_update) "
                    "VALUES ($1, $2, $3, $4, $5, $6, $7, CURRENT_TIMESTAMP) "
                    "ON CONFLICT (soato_id, street_id, house, corps, flat)"
                    "DO UPDATE SET "
                    "zip_code = EXCLUDED.zip_code, "
                    "note = EXCLUDED.note, "
                    "last_update = CURRENT_TIMESTAMP "
                    "WHERE "
                    "address.zip_code IS DISTINCT FROM EXCLUDED.zip_code OR "
                    "address.note IS DISTINCT FROM EXCLUDED.note",
            soato_id, street_id, house, corps, flat, zip_code, note
        );
        std::stringstream message;
        message << "Upsert in address table:\n\tsoato_id: " << soato_id
        << "\n\tstreet_id: " << street_id
        << "\n\thouse: " << house
        << "\n\tcorps: " << corps.value_or(" ")
        << "\n\tflat: " << flat.value_or(" ")
        << "\n\tzip_code: " << zip_code.value_or(" ")
        << "\n\tnote: " << note.value_or(" ");
        logger.log(message.str(), LogLevel::INFO);
        w.commit();
    } catch (const std::exception &e) {
        std::stringstream error;
        error << "Database error in upsertAddres(): " << e.what();
        logger.log(error.str(), LogLevel::ERROR);
    }
}

nlohmann::json Database::resultToJson(const pqxx::result& res) {
    nlohmann::json json_list = nlohmann::json::array();
    for (auto const &row: res) {
        nlohmann::json item;
        for (auto const &col: row) {
            item[col.name()] = col.c_str();
        }
        json_list.push_back(item);
    }
    return json_list;
}