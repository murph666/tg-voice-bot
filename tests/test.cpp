#include <gtest/gtest.h>
#include "utils/Config.h"
#include "utils/dbSqlite.h"

#include <cctype>

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

TEST(SqliteTest, PrepareInvalidSqlThrows) {
    dbSqlite db(":memory:");
    try {
        db.exec("SELEC 1"); // невалидный запрос
        FAIL();
    } catch (const SqliteError &e) {
        EXPECT_EQ(e.code(), SQLITE_ERROR);
    }
    EXPECT_THROW(db.prepare("SELECT id FROM nope"), SqliteError);  // таблицы нет
    EXPECT_THROW(db.prepare(""), SqliteError);
    EXPECT_THROW(db.prepare("SELECT 1; SELECT 2"), SqliteError);
    EXPECT_THROW(db.prepare("SELECT 1;; SELECT 2"), SqliteError);
    EXPECT_THROW(db.prepare("SELECT 1; ерунда"), SqliteError);
    EXPECT_THROW(db.prepare("CREATE TABLE t(id INTEGER); INSERT INTO t VALUES(1)"), SqliteError);
    EXPECT_NO_THROW(db.prepare("SELECT 1; -- комментарий"));
    EXPECT_NO_THROW(db.prepare("SELECT 1;   \n"));
}

TEST(SqliteTest, PrepareValidSql) {
//    dbSqlite db = dbSqlite();
    dbSqlite db(":memory:"); //Она создаётся пустой каждый раз и исчезает, когда объект уничтожается.
    db.exec("CREATE TABLE t(id INTEGER)");
    EXPECT_NO_THROW(db.prepare("SELECT id FROM t"));
}

TEST(SqliteTest, PrepareMultipleStatementsThrows) {
    dbSqlite db(":memory:");
    EXPECT_THROW(db.prepare("SELECT 1; SELECT 2"), SqliteError);
    EXPECT_NO_THROW(db.prepare("SELECT 1;   \n"));  // хвост из пробелов — это нормально
}

TEST(SqliteTest, StepCreatesTable) {
    dbSqlite db(":memory:");
    auto st = db.prepare("CREATE TABLE t(id INTEGER)");
    EXPECT_FALSE(st.step());
    EXPECT_NO_THROW(db.exec("INSERT INTO t VALUES(1)"));  // таблица существует
    EXPECT_NO_THROW(db.exec("INSERT INTO t VALUES(1)"));
}

TEST(SqliteBindTest, BindIntStoresInteger) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT 1 WHERE ?1 = 42 AND typeof(?1) = 'integer'");
    st.bind(1, 42);
    EXPECT_TRUE(st.step());
}

TEST(SqliteBindTest, BindInt64KeepsTelegramChatId) {
    dbSqlite db(":memory:");
    // id группы в Telegram не помещается в int
    auto st = db.prepare("SELECT 1 WHERE ?1 = -1001234567890 AND typeof(?1) = 'integer'");
    st.bind(1, std::int64_t{-1001234567890});
    EXPECT_TRUE(st.step());
}

TEST(SqliteBindTest, BindDoubleStoresReal) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT 1 WHERE ?1 = 3.25 AND typeof(?1) = 'real'");
    st.bind(1, 3.25);
    EXPECT_TRUE(st.step());
}

TEST(SqliteBindTest, BindFloatStoresReal) {
    dbSqlite db(":memory:");
    // 0.5 точно представимо и во float, и в double, поэтому сравнение на равенство корректно
    auto st = db.prepare("SELECT 1 WHERE ?1 = 0.5 AND typeof(?1) = 'real'");
    st.bind(1, 0.5f);
    EXPECT_TRUE(st.step());
}

TEST(SqliteBindTest, BindStdStringStoresText) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT 1 WHERE ?1 = 'Alice' AND typeof(?1) = 'text'");
    std::string name = "Alice";
    st.bind(1, name);
    EXPECT_TRUE(st.step());
}

TEST(SqliteBindTest, BindStringLiteral) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT 1 WHERE ?1 = 'Alice' AND typeof(?1) = 'text'");
    st.bind(1, "Alice");  // const char[6] -> const char*
    EXPECT_TRUE(st.step());
}

TEST(SqliteBindTest, BindCharPointer) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT 1 WHERE ?1 = 'Alice' AND typeof(?1) = 'text'");
    char buf[] = "Alice";
    char *p = buf;  // неконстантный char* — идёт через перегрузку bind(int, char*)
    st.bind(1, p);
    EXPECT_TRUE(st.step());
}

TEST(SqliteBindTest, BindEmptyStringIsTextNotNull) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT 1 WHERE ?1 = '' AND typeof(?1) = 'text'");
    st.bind(1, std::string{});
    EXPECT_TRUE(st.step());
}

TEST(SqliteBindTest, BindNullptrStoresNull) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT 1 WHERE ?1 IS NULL");
    st.bind(1, nullptr);
    EXPECT_TRUE(st.step());
}

TEST(SqliteBindTest, BindNullConstCharPointerStoresNull) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT 1 WHERE ?1 IS NULL");
    const char *nothing = nullptr;
    st.bind(1, nothing);  // sqlite3_bind_text с nullptr записывает NULL
    EXPECT_TRUE(st.step());
}

TEST(SqliteBindTest, WrongValueDoesNotMatch) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT 1 WHERE ?1 = 42");
    st.bind(1, 43);
    EXPECT_FALSE(st.step());
}

TEST(SqliteGetTest, IntRoundTrip) {
    dbSqlite db(":memory:");
    //Запрос не обращается ни к какой таблице, он просто возвращает одну строку с одним
    //столбцом, в котором лежит значение параметра.
    auto st = db.prepare("SELECT ?1");
    //
    st.bind(1, -42);
    ASSERT_TRUE(st.step());
    EXPECT_EQ(st.get<int>(0), -42);
}

TEST(SqliteGetTest, Int64RoundTripTelegramChatId) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT ?1");
    st.bind(1, std::int64_t{-1001234567890});
    ASSERT_TRUE(st.step());
    EXPECT_EQ(st.get<std::int64_t>(0), -1001234567890);
}

TEST(SqliteGetTest, Int64ExtremeValues) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT ?1, ?2");
    st.bind_all(INT64_MAX, INT64_MIN);
    ASSERT_TRUE(st.step());
    EXPECT_EQ(st.get<std::int64_t>(0), INT64_MAX);
    EXPECT_EQ(st.get<std::int64_t>(1), INT64_MIN);
}

TEST(SqliteGetTest, BoolRoundTrip) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT ?1, ?2");
    st.bind_all(true, false);
    ASSERT_TRUE(st.step());
    EXPECT_TRUE(st.get<bool>(0));
    EXPECT_FALSE(st.get<bool>(1));
}

TEST(SqliteGetTest, BoolFromAnyNonZeroIsTrue) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT 5, -1, 0");
    ASSERT_TRUE(st.step());
    EXPECT_TRUE(st.get<bool>(0));
    EXPECT_TRUE(st.get<bool>(1));
    EXPECT_FALSE(st.get<bool>(2));
}

TEST(SqliteGetTest, DoubleRoundTrip) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT ?1");
    st.bind(1, 3.141592653589793);
    ASSERT_TRUE(st.step());
    EXPECT_DOUBLE_EQ(st.get<double>(0), 3.141592653589793);
}

TEST(SqliteGetTest, FloatRoundTrip) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT ?1");
    st.bind(1, 1.25f);
    ASSERT_TRUE(st.step());
    EXPECT_FLOAT_EQ(st.get<float>(0), 1.25f);
}

TEST(SqliteGetTest, StringRoundTrip) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT ?1");
    st.bind(1, "O'Brien");
    ASSERT_TRUE(st.step());
    EXPECT_EQ(st.get<std::string>(0), "O'Brien");
}

TEST(SqliteGetTest, Utf8StringRoundTrip) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT ?1");
    const std::string text = "Привет, мир! 🎤";
    st.bind(1, text);
    ASSERT_TRUE(st.step());
    EXPECT_EQ(st.get<std::string>(0), text);
}

TEST(SqliteGetTest, StringWithEmbeddedZeroKeepsFullLength) {
    // Без sqlite3_column_bytes строка обрезалась бы на '\0'
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT ?1");
    const std::string text("ab\0cd", 5);
    st.bind(1, text);
    ASSERT_TRUE(st.step());
    auto got = st.get<std::string>(0);
    EXPECT_EQ(got.size(), 5u);
    EXPECT_EQ(got, text);
}

TEST(SqliteGetTest, EmptyStringRoundTrip) {
    dbSqlite db(":memory:");
    auto st = db.prepare("SELECT ?1");
    st.bind(1, std::string{});
    ASSERT_TRUE(st.step());
    EXPECT_FALSE(st.isNull(0));   // пустая строка — это значение, не NULL
    EXPECT_EQ(st.get<std::string>(0), "");
}

TEST(SqliteGetTest, StringWithSqlInjectionRoundTrip) {
    dbSqlite db(":memory:");
    db.exec("CREATE TABLE u(name TEXT)");
    db.prepare("INSERT INTO u VALUES(?)").bind(1, "x'); DROP TABLE u; --").step();

    auto sel = db.prepare("SELECT name FROM u");
    ASSERT_TRUE(sel.step());
    EXPECT_EQ(sel.get<std::string>(0), "x'); DROP TABLE u; --");
}