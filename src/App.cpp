#include "../headers/App.h"

void App::setup_routes(Database& db) {
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

    CROW_ROUTE(app, "/api/street_by_soato/<int>").methods(crow::HTTPMethod::GET)
    ([&db](int id) {
        return db.getStreetsBySoatoId(id).dump();
    });

    CROW_ROUTE(app, "/api/address_soato").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        try {
            nlohmann::json json = nlohmann::json::parse(req.body);

            long long code_soato = json["code_soato"].get<long long>();
            std::string place = json["place"].get<std::string>();
            int place_type_id = json["place_type"].get<int>();
            std::string region = json["region"].get<std::string>();

            return db.addAddressSoato(code_soato, place, place_type_id, region).dump();
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            return nlohmann::json({{"error", "bebebe"}}).dump();
        }
    });
}
