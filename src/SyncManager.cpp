#include "../headers/SyncManager.h"
#include <cpr/cpr.h>
#include <iostream>
#include "../headers/Utils.h"

void SyncManager::startAutoSync(int intervalSeconds) {
    if (isRunning) return;
    isRunning = true;
    syncThread = std::thread(&SyncManager::syncLoop, this, intervalSeconds);
}

void SyncManager::syncLoop(int intervalSeconds) {
    while (isRunning) {
        logger.log("Auto-sync started.", LogLevel::INFO);
        performSync();

        for (int i = 0; i < intervalSeconds && isRunning; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void SyncManager::stopAutoSync() {
    if (isRunning) {
        isRunning = false;
        if (syncThread.joinable()) {
            syncThread.join();
        }
    }
}

SyncManager::~SyncManager() {
    stopAutoSync();
}

void SyncManager::performSync() {
    std::string since = last_sync_time;

    auto r = cpr::Get(cpr::Url{remote_url + "/api/sync/all"},
                      cpr::Parameters{{"since", since}});

    if (r.status_code == 200) {
        try {
            auto data = nlohmann::json::parse(r.text);
            applySyncData(data);
            last_sync_time = data["generated_at"].get<std::string>();
            std::stringstream message;
            message << "Sync successful. Time: " << last_sync_time;
            logger.log(message.str(), LogLevel::INFO);
        } catch (const std::exception& e) {
            std::stringstream error;
            error << "Sync error: " << e.what();
            logger.log(error.str(), LogLevel::ERROR);
        }
    } else {
        std::stringstream error;
        error << "Response Error! Status code: " << r.status_code << ". Error: " << "" << r.error.message;
        logger.log(error.str(), LogLevel::ERROR);
    }
}

void SyncManager::applySyncData(const nlohmann::json &data) {
    if (data.contains("address_soato") && data["address_soato"].is_array()) {
        for (const auto item: data["address_soato"]) {
            std::string code_soato = item.at("code_soato").get<std::string>();
            db.upsertAddressSoato(
                code_soato,
                item.value("area", std::string{""}),
                item.value("district", std::string{""}),
                item.at("region").get<std::string>(),
                std::stoi(item.at("place_type_id").get<std::string>()),
                item.at("place").get<std::string>()
            );
        }
        std::stringstream message;
        message << "Successfully synced address_soato. " << data["address_soato"].size() << " rows has been updated.";
        logger.log(message.str(), LogLevel::INFO);
    }

    if (data.contains("address_street") && data["address_street"].is_array()) {
        for (const auto item: data["address_street"]) {
            db.upsertAddressStreet(
                std::stoi(item.at("soato_id").get<std::string>()),
                std::stoi(item.at("street_type_id").get<std::string>()),
                item.at("name").get<std::string>()
            );
        }
        std::stringstream message;
        message << "Successfully synced address_street. " << data["address_street"].size() << " rows has been updated.";
        logger.log(message.str(), LogLevel::INFO);
    }

    if (data.contains("address") && data["address"].is_array()) {
        for (const auto item: data["address"]) {
            db.upsertAddress(
                std::stoi(item.at("soato_id").get<std::string>()),
                std::stoi(item.at("street_id").get<std::string>()),
                std::stoi(item.at("house").get<std::string>()),
                item.value("corps", std::string{""}),
                item.value("flat", std::string{""}),
                item.value("zip_code", std::string{""}),
                item.value("note", std::string{""})
            );
        }
        std::stringstream message;
        message << "Successfully synced address. " << data["address"].size() << " rows has been updated.";
        logger.log(message.str(), LogLevel::INFO);
    }
}


nlohmann::json SyncManager::getDelta(const std::string &last_time) {
    nlohmann::json delta;

    delta["address_soato"] = db.getDelta("address_soato", last_time);
    delta["address_street"] = db.getDelta("address_street", last_time);
    delta["address"] = db.getDelta("address", last_time);
    delta["generated_at"] = Utils::getCurrentTimeStamp();

    return delta;
}
