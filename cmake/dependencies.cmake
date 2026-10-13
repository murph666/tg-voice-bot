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
        OPTIONS "CURL_USE_LIBPSL OFF" "BUILD_CURL_EXE OFF" "BUILD_TESTING OFF"
        "BUILD_EXAMPLES OFF" "BUILD_LIBCURL_DOCS OFF" "BUILD_MISC_DOCS OFF"
        "ENABLE_CURL_MANUAL OFF"
)

CPMAddPackage(
        NAME googletest
        GIT_TAG v1.18.0
        GITHUB_REPOSITORY "google/googletest"
)
CPMAddPackage(
        NAME sqlite3
        URL https://www.sqlite.org/2026/sqlite-amalgamation-3530400.zip
        URL_HASH SHA3_256=628a44cfe82c66aed1ccbbe85a562d2e33ebe64b3288981ed76285612227934e
        DOWNLOAD_ONLY YES
)
if(sqlite3_ADDED)
    find_package(Threads REQUIRED)
    add_library(sqlite3 STATIC ${sqlite3_SOURCE_DIR}/sqlite3.c)
    target_link_libraries(sqlite3 PRIVATE Threads::Threads ${CMAKE_DL_LIBS})
    target_include_directories(sqlite3 PUBLIC ${sqlite3_SOURCE_DIR})
    set_target_properties(sqlite3 PROPERTIES POSITION_INDEPENDENT_CODE ON)
    add_library(SQLite::SQLite3 ALIAS sqlite3)
endif()

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


function(print_all_targets dir)
    get_property(targets DIRECTORY "${dir}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(t IN LISTS targets)
        get_target_property(type ${t} TYPE)
        message(STATUS "${t}  [${type}]")
    endforeach()
    get_property(subdirs DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
    foreach(s IN LISTS subdirs)
        print_all_targets("${s}")
    endforeach()
endfunction()

#print_all_targets(${CMAKE_SOURCE_DIR})