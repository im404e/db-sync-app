#pragma once
#include <string>
#include <thread>
#include <atomic>
#include "Database.h"
#include "../headers/Logger.h"

class SyncManager {
private:
    Database& db;
    std::string last_sync_time = "1970-01-01 00:00:00";
    std::string remote_url;
    std::thread syncThread;
    std::atomic<bool> isRunning = false;
    Logger& logger;

public:
    SyncManager(Database& db, const std::string& remote_url, Logger& logger)
    : db(db), remote_url(remote_url), logger(logger){};
    ~SyncManager();
    nlohmann::json getDelta(const std::string& last_time);
    void performSync();
    void applySyncData(const nlohmann::json& data);
    void startAutoSync(int intervalSeconds);
    void stopAutoSync();
    void syncLoop(int intervalSeconds);
};