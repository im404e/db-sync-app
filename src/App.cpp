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

    /*CROW_ROUTE(app, "/api/add/address_soato").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        try {
            nlohmann::json json = nlohmann::json::parse(req.body);

            std::string code_soato = json.at("code_soato").get<std::string>();
            std::string place = json.at("place").get<std::string>();
            int place_type_id = json.at("place_type").get<int>();
            std::string region = json.at("region").get<std::string>();

            return db.addAddressSoato(code_soato, place, place_type_id, region).dump();
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            return nlohmann::json().dump();;
        }
    });

    CROW_ROUTE(app, "/api/add/address_street").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        try {
            nlohmann::json json = nlohmann::json::parse(req.body);

            int soato_id = json.at("soato_id").get<int>();
            int street_type_id = json.at("street_type_id").get<int>();
            std::string name = json.at("name").get<std::string>();

            return db.addAddressStreet(soato_id, street_type_id, name).dump();
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            return nlohmann::json().dump();
        }
    });

    CROW_ROUTE(app, "/api/add/address").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        try {
            nlohmann::json json = nlohmann::json::parse(req.body);
            int soato_id = json.at("soato_id").get<int>();
            int street_id = json.at("street_id").get<int>();
            std::string house = json.at("house").get<std::string>();
            std::string corps = json.at("corps").get<std::string>();
            std::string flat = json.at("flat").get<std::string>();

            return db.addAddress(soato_id, street_id, house, corps, flat).dump();
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            return nlohmann::json().dump();;
        }
    });*/

    CROW_ROUTE(app, "/api/sync/all")
    ([&sync_manager](const crow::request& req) {
        auto since = req.url_params.get("since");
        std::string time = since ? since : "1970-01-01 00:00:00";
        return sync_manager.getDelta(time).dump();
    });

    CROW_ROUTE(app, "/api/sync/start")
    ([&sync_manager]() {
        sync_manager.performSync();
        return crow::response(200, "Sync process started");
    });

    /*CROW_ROUTE(app, "/api/sync/<string>")
    ([&db](const crow::request& req, const std::string table) {
        try {
            auto since = req.url_params.get("since");
            std::string time = since ? since : "1970-01-01 00:00:00";

            return db.getDelta(table, time).dump();
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    });*/
}