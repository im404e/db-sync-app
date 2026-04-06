#pragma once
#include <crow.h>
#include "Database.h"
#include "SyncManager.h"

class App {
private:
    crow::SimpleApp app;

public:
    void setup_routes(Database& db, SyncManager& sync_manager, Logger& log);
    void run(const int port) {app.port(port).multithreaded().run();};
    crow::SimpleApp& get_App() {return app;};
};