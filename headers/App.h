#pragma once
#include <crow.h>
#include "Database.h"


class App {
private:
    crow::SimpleApp app;

public:
    void setup_routes(Database& db);
    void run(const int port) {app.port(port).multithreaded().run();};
    crow::SimpleApp& get_App() {return app;};
};