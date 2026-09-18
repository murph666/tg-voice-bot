add_executable(SmokeTest
        test.cpp
        ../src/utils/Config.cpp
)

target_include_directories(SmokeTest PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/utils
)
target_precompile_headers(SmokeTest PRIVATE ../src/pch.h)

include(GoogleTest)

target_link_libraries(SmokeTest PRIVATE
        GTest::gtest
        GTest::gtest_main
        spdlog::spdlog
)

# Вариант 1: Один тест (текущий)
#add_test(NAME SmokeTest1 COMMAND SmokeTest)

# Вариант 2: Каждый GTest отдельно (опционально)
# В этом случае нужно явно добавить GoogleTest -> смотри 2 строку
gtest_discover_tests(SmokeTest)  # Автоматически находит все TEST()