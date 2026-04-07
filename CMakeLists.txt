cmake_minimum_required(VERSION 4.1)
project(db-sync-app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/libpq")
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")

find_package(PostgreSQL REQUIRED)
find_package(Crow CONFIG REQUIRED)
find_package(nlohmann_json 3.12.0 REQUIRED)
find_package(PkgConfig REQUIRED)
set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/opt/libpq/lib/pkgconfig:/opt/homebrew/opt/libpqxx/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

pkg_check_modules(PQXX REQUIRED IMPORTED_TARGET libpqxx)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 1.14.1)
FetchContent_MakeAvailable(cpr)

add_executable(db-sync-app
        src/main.cpp
        src/Config.cpp
        headers/Config.h
        src/App.cpp
        headers/App.h
        src/Database.cpp
        headers/Database.h
        src/SyncManager.cpp
        headers/SyncManager.h
        src/Logger.cpp
        headers/Logger.h
        headers/Utils.h
)

target_link_libraries(db-sync-app PRIVATE
        PostgreSQL::PostgreSQL
        PkgConfig::PQXX
        Crow::Crow
        nlohmann_json::nlohmann_json
        cpr::cpr
)