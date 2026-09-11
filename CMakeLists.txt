cmake_minimum_required(VERSION 3.31)

project(TGVoiceBot VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/dependencies.cmake)
include(cmake/CodeQuality.cmake)

enable_testing()
#include(tests/CMakeLists.txt) - неправильно
add_subdirectory(tests) # ← ИСПРАВЛЕНО: вместо include()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(TGVoiceBot)
set_target_properties(TGVoiceBot PROPERTIES CXX_SCAN_FOR_MODULES OFF) #без этого падает lint

target_sources(TGVoiceBot PRIVATE
                src/main.cpp)

target_link_libraries(TGVoiceBot PRIVATE TgBot::TgBot spdlog::spdlog)
