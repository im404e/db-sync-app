#include "../headers/Config.h"
#include "../headers/App.h"
#include "../headers/SyncManager.h"

int main(int argc, char* argv[])
{
    std::string configPath = "../config.json";
    if (argc > 1) {
        configPath = argv[1];
    }

    try {
        Config config(configPath);

        int port = config.getConfigFile().at("server").at("port").get<int>();
        Logger logger(port);
        logger.log("Loading config from: " + configPath, LogLevel::INFO);

        Database db(config.getDbOptionsAsString(), logger);

        std::string remote_url = config.getConfigFile().at("server").at("remote_url").get<std::string>();
        SyncManager syncManager(db, remote_url, logger);

        syncManager.startAutoSync(60);

        App app;
        app.setup_routes(db, syncManager, logger);
        logger.log("Server starting on port " + std::to_string(port) + "...", LogLevel::INFO) ;
        app.run(port);

        return 0;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}