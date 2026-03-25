cmake_minimum_required(VERSION 4.1)
project(testdbconnection)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PostgreSQL REQUIRED)
find_package(Crow CONFIG REQUIRED)
find_package(nlohmann_json 3.12.0 REQUIRED)

find_package(PkgConfig REQUIRED)
set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/opt/libpq/lib/pkgconfig:/opt/homebrew/opt/libpqxx/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

pkg_check_modules(PQXX REQUIRED IMPORTED_TARGET libpqxx)

add_executable(testdbconnection
        src/main.cpp
        src/Config.cpp
        headers/Config.h
        src/App.cpp
        headers/App.h
        src/Database.cpp
        headers/Database.h
)

target_link_libraries(testdbconnection PRIVATE
        PostgreSQL::PostgreSQL
        PkgConfig::PQXX
        Crow::Crow
        nlohmann_json::nlohmann_json
)