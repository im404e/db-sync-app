#include "crow.h"
#include "../headers/Config.h"
#include "../headers/App.h"

int main()
{
    Config config("../config.json");

    Database db(config.getDbOptionsAsString());

    App app;
    app.setup_routes(db);
    app.run(18080);
}