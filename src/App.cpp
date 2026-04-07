#include "../headers/App.h"
#include "../headers/Logger.h"

void App::setup_routes(Database& db, SyncManager& sync_manager, Logger& log) {
    Logger& logger = log;

    CROW_ROUTE(app, "/api/street_types").methods(crow::HTTPMethod::GET)
    ([&db]() {
        return db.getStreetTypesData().dump();
    });

    CROW_ROUTE(app, "/api/place_types").methods(crow::HTTPMethod::GET)
    ([&db]() {
        return db.getPlaceTypesData().dump();
    });

    CROW_ROUTE(app, "/api/soato").methods(crow::HTTPMethod::GET)
    ([&db]() {
        return db.getSoatoData().dump();
    });

    CROW_ROUTE(app, "/api/street_soato/<int>").methods(crow::HTTPMethod::GET)
    ([&db](int id) {
        return db.getStreetSoatoId(id).dump();
    });

    CROW_ROUTE(app, "/api/add/address_soato").methods(crow::HTTPMethod::POST)
    ([&db, &logger](const crow::request& req) {
        try {
            nlohmann::json json = nlohmann::json::parse(req.body);

            std::string code_soato = json.at("code_soato").get<std::string>();
            std::optional<std::string> area;
            if (json.contains("area") && !json["area"].is_null()) {
                area = json.at("area").get<std::string>();
            } else {
                area = std::nullopt;
            }
            std::optional<std::string> district;
            if (json.contains("district") && !json["district"].is_null()) {
                district = json.at("district").get<std::string>();
            } else {
                district = std::nullopt;
            }
            std::string region = json.at("region").get<std::string>();
            int place_type_id = json.at("place_type").get<int>();
            std::string place = json.at("place").get<std::string>();

            std::stringstream message;
            message << "Request for upsert in address_soato.";
            logger.log(message.str(), LogLevel::WARN);

            db.upsertAddressSoato(code_soato, area, district, region, place_type_id, place);

            return crow::response(200, "OK");
        } catch (std::exception& e) {
            std::stringstream error;
            error << "Error occurred while trying to upsert row in address_soato table." << e.what();
            logger.log(error.str(), LogLevel::ERROR);

            return crow::response(500, e.what());;
        }
    });

    CROW_ROUTE(app, "/api/add/address_street").methods(crow::HTTPMethod::POST)
    ([&db, &logger](const crow::request& req) {
        try {
            nlohmann::json json = nlohmann::json::parse(req.body);

            int soato_id = json.at("soato_id").get<int>();
            int street_type_id = json.at("street_type_id").get<int>();
            std::string name = json.at("name").get<std::string>();

            std::stringstream message;
            message << "Request for upsert in address_street.";
            logger.log(message.str(), LogLevel::WARN);

            db.upsertAddressStreet(soato_id, street_type_id, name);

            return crow::response(200, "OK");
        } catch (std::exception& e) {
            std::stringstream error;
            error << "Error occurred while trying to upsert row in address_street table." << e.what();
            logger.log(error.str(), LogLevel::ERROR);

            return crow::response(500, e.what());;
        }
    });

    CROW_ROUTE(app, "/api/add/address").methods(crow::HTTPMethod::POST)
    ([&db, &logger](const crow::request& req) {
        try {
            nlohmann::json json = nlohmann::json::parse(req.body);
            int soato_id = json.at("soato_id").get<int>();
            int street_id = json.at("street_id").get<int>();
            std::string house = json.at("house").get<std::string>();
            std::optional<std::string> corps;
            if (json.contains("corps") && !json["corps"].is_null()) {
                corps = json.at("corps").get<std::string>();
            } else {
                corps = std::nullopt;
            }
            std::optional<std::string> flat;
            if (json.contains("flat") && !json["flat"].is_null()) {
                flat = json.at("flat").get<std::string>();
            } else {
                flat = std::nullopt;
            }
            std::optional<std::string> zip_code;
            if (json.contains("zip_code") && !json["zip_code"].is_null()) {
                zip_code = json.at("zip_code").get<std::string>();
            } else {
                zip_code = std::nullopt;
            }
            std::optional<std::string> note;
            if (json.contains("note") && !json["note"].is_null()) {
                note = json.at("note").get<std::string>();
            } else {
                note = std::nullopt;
            }

            std::stringstream message;
            message << "Request for upsert in address.";
            logger.log(message.str(), LogLevel::WARN);

            db.upsertAddress(soato_id, street_id, house, corps, flat, zip_code, note);

            return crow::response(200, "OK");
        } catch (std::exception& e) {
            std::stringstream error;
            error << "Error occurred while trying to upsert row in address table." << e.what();
            logger.log(error.str(), LogLevel::ERROR);

            return crow::response(500, e.what());;
        }
    });


    CROW_ROUTE(app, "/api/sync/all").methods(crow::HTTPMethod::GET)
    ([&sync_manager, &logger](const crow::request& req) {
        std::stringstream message;
        message << "Request for get delta all.";
        logger.log(message.str(), LogLevel::WARN);

        auto since = req.url_params.get("since");
        std::string time = since ? since : "1970-01-01 00:00:00";

        return sync_manager.getDelta(time).dump();
    });

    CROW_ROUTE(app, "/api/sync/start").methods(crow::HTTPMethod::GET)
    ([&sync_manager, &logger]() {
        std::stringstream message;
        message << "Request for sync start.";
        logger.log(message.str(), LogLevel::WARN);

        sync_manager.performSync();

        return crow::response(200, "Sync process started");
    });
}