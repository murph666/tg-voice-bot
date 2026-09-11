include_guard(GLOBAL)

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

CPMAddPackage(
        NAME Boost
        VERSION 1.86.0
        URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.xz
        URL_HASH SHA256=2c5ec5edcdff47ff55e27ed9560b0a0b94b07bd07ed9928b476150e16b0efc57
        OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_SKIP_INSTALL_RULES ON" "BOOST_ENABLE_COMPATIBILITY_TARGETS ON" # Set `OFF` for installation
        "BUILD_SHARED_LIBS OFF" "BOOST_INCLUDE_LIBRARIES container\\\;asio\\\;beast" # Note the escapes!
)

CPMAddPackage(
        NAME curl
        GIT_TAG curl-8_21_0
        GITHUB_REPOSITORY "curl/curl"
        OPTIONS "CURL_USE_LIBPSL OFF"
)

CPMAddPackage(
        NAME googletest
        GIT_TAG v1.18.0
        GITHUB_REPOSITORY "google/googletest"
)


CPMAddPackage(
        NAME nlohmann_json
        GITHUB_REPOSITORY nlohmann/json
        GIT_TAG v3.11.3
)

CPMAddPackage(
        NAME tgbot-cpp
        GITHUB_REPOSITORY reo7sp/tgbot-cpp
        GIT_TAG v1.12.2
        OPTIONS "BUILD_SHARED_LIBS ON" "CMAKE_SKIP_INSTALL_RULES ON"
)

CPMAddPackage(
        NAME spdlog
        VERSION 1.12.0
        GITHUB_REPOSITORY "gabime/spdlog"
        OPTIONS "SPDLOG_FMT_EXTERNAL OFF"
)
