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
            std::optional<std::string> area = json.contains("area") ? std::make_optional(json.at("area").get<std::string>()) : std::nullopt;
            std::optional<std::string> district = json.contains("district") ? std::make_optional(json.at("district").get<std::string>()) : std::nullopt;
            std::string region = json.at("region").get<std::string>();
            int place_type_id = json.at("place_type").get<int>();
            std::string place = json.at("place").get<std::string>();

            db.upsertAddressSoato(code_soato, area, district, region, place_type_id, place);

            std::stringstream message;
            message << "Success upsert in address_soato:\n\tcode_soato" << code_soato
            << "\n\t area: " << area.value_or(" ")
            << "\n\t district: " << district.value_or(" ")
            << "\n\t region: " << region
            << "\n\t place: " << place_type_id
            << "\n\t place: " << place;
            logger.log(message.str(), LogLevel::INFO);

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

            db.upsertAddressStreet(soato_id, street_type_id, name);

            std::stringstream message;
            message << "Success upsert in address_street:\n\tsoato_id" << soato_id
            << "\n\tstreet_type_id: " << street_type_id
            << "\n\tname: " << name;
            logger.log(message.str(), LogLevel::INFO);

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
            std::optional<std::string> corps = json.contains("corps") ? std::make_optional(json.at("corps").get<std::string>()) : std::nullopt;
            std::optional<std::string> flat = json.contains("flat") ? std::make_optional(json.at("flat").get<std::string>()) : std::nullopt;
            std::optional<std::string> zip_code = json.contains("zip_code") ? std::make_optional(json.at("zip_code").get<std::string>()) : std::nullopt;
            std::optional<std::string> note = json.contains("note") ? std::make_optional(json.at("note").get<std::string>()) : std::nullopt;

            db.upsertAddress(soato_id, street_id, house, corps, flat, zip_code, note);

            std::stringstream message;
            message << "Success upsert in address_street:\n\tsoato_id" << soato_id
            << "\n\tstreet_id: " << street_id
            << "\n\thouse: " << house
            << "\n\tcorps: " << corps.value_or(" ")
            << "\n\t flat: " << flat.value_or(" ")
            << "\n\t zip_code: " << zip_code.value_or(" ")
            << "\n\tnote: " << note.value_or(" ");
            logger.log(message.str(), LogLevel::INFO);

            return crow::response(200, "OK");
        } catch (std::exception& e) {
            std::stringstream error;
            error << "Error occurred while trying to upsert row in address table." << e.what();
            logger.log(error.str(), LogLevel::ERROR);
            return crow::response(500, e.what());;
        }
    });


    CROW_ROUTE(app, "/api/sync/all").methods(crow::HTTPMethod::GET)
    ([&sync_manager](const crow::request& req) {
        auto since = req.url_params.get("since");
        std::string time = since ? since : "1970-01-01 00:00:00";
        return sync_manager.getDelta(time).dump();
    });

    CROW_ROUTE(app, "/api/sync/start").methods(crow::HTTPMethod::GET)
    ([&sync_manager]() {
        sync_manager.performSync();
        return crow::response(200, "Sync process started");
    });
}