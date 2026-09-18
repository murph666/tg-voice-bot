#include <gtest/gtest.h>
#include "utils/Config.h"

TEST(ConfigTest_nullValue, BadADMIN_TELEGRAM_ID) {
    auto fakeEnv = [](std::string_view n) -> std::optional<std::string> {
        if (n == "ADMIN_TELEGRAM_ID") return std::nullopt;
        if (n == "DB_PATH") return "voices.db";
        if (n == "BOT_TOKEN") return "abc:123";
        return std::nullopt;
    };
    Config c(fakeEnv);
    EXPECT_THROW(c.Load(), std::runtime_error);
}

TEST(ConfigTest_nullValue, BadDB_PATH) {
    auto fakeEnv = [](std::string_view n) -> std::optional<std::string> {
        if (n == "ADMIN_TELEGRAM_ID") return "1213";
        if (n == "DB_PATH") return std::nullopt;
        if (n == "BOT_TOKEN") return "abc:123";
        return std::nullopt;
    };
    Config c(fakeEnv);
    EXPECT_THROW(c.Load(), std::runtime_error);
}

TEST(ConfigTest_nullValue, BadBOT_TOKEN) {
    auto fakeEnv = [](std::string_view n) -> std::optional<std::string> {
        if (n == "ADMIN_TELEGRAM_ID") return "123";
        if (n == "DB_PATH") return "asd.db";
        if (n == "BOT_TOKEN") return std::nullopt;
        return std::nullopt;

    };
    Config c(fakeEnv);
    EXPECT_THROW(c.Load(), std::runtime_error);
}

TEST(ConfigTest_badValue, badADMIN_TELEGRAM_ID1) {
    auto fakeEnv = [](std::string_view n) -> std::optional<std::string> {
        if (n == "ADMIN_TELEGRAM_ID") return "0";
        if (n == "DB_PATH") return "asd.db";
        if (n == "BOT_TOKEN") return "asd:123";
        return std::nullopt;
    };
    Config c(fakeEnv);
    EXPECT_THROW(c.Load(), std::runtime_error);
}

TEST(ConfigTest_badValue, badADMIN_TELEGRAM_ID2) {
    auto fakeEnv = [](std::string_view n) -> std::optional<std::string> {
        if (n == "ADMIN_TELEGRAM_ID") return "qwerrty";
        if (n == "DB_PATH") return "asd.db";
        if (n == "BOT_TOKEN") return "asd:123";
        return std::nullopt;

    };
    Config c(fakeEnv);
    EXPECT_THROW(c.Load(), std::runtime_error);
}

TEST(ConfigTest_goodValues, GoodValues1) {
    auto fakeEnv = [](std::string_view n) -> std::optional<std::string> {
        if (n == "ADMIN_TELEGRAM_ID") return "12345";
        if (n == "DB_PATH") return "voices.db";
        if (n == "BOT_TOKEN") return "abc:123";
        return std::nullopt;
    };
    Config c(fakeEnv);
    EXPECT_NO_THROW(c.Load());

    EXPECT_EQ(c.getBotToken(), "abc:123");
    EXPECT_EQ(c.getDBPath(), "voices.db");
    EXPECT_EQ(c.m_adminID(), 12345);
}