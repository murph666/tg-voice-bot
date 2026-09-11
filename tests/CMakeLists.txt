add_executable(SmokeTest test.cpp)
include(GoogleTest)

target_link_libraries(SmokeTest PRIVATE
        GTest::gtest
        GTest::gtest_main
)

# Вариант 1: Один тест (текущий)
#add_test(NAME SmokeTest1 COMMAND SmokeTest)

# Вариант 2: Каждый GTest отдельно (опционально)
# В этом случае нужно явно добавить GoogleTest -> смотри 2 строку
gtest_discover_tests(SmokeTest)  # Автоматически находит все TEST()