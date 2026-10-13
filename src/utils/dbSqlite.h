//
// Created by murph on 9/24/26.
//

#ifndef TGVOICEBOT_DBSQLITE_H
#define TGVOICEBOT_DBSQLITE_H

#include <fmt/format.h>
#include <sqlite3.h>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include "logger.h"

class SqliteError : public std::runtime_error {
 public:
  SqliteError(int code, const std::string& message) : std::runtime_error(message), m_code(code) {}

  int code() const { return m_code; }

 private:
  int m_code;
};

class Statement {
 public:
  ~Statement() = default;

  Statement(const Statement&) = delete;

  Statement& operator=(const Statement&) = delete;

  Statement(Statement&&) = default;

  Statement& operator=(Statement&&) = default;

  template <class... Args>
  Statement& bind_all(Args&&... args) {
    int idx = 1;
    (bind(idx++, std::forward<Args>(args)), ...);  //fold expression
    return *this;
  }

  Statement& bind(int idx, bool value);

  Statement& bind(int idx, int value);

  Statement& bind(int idx, std::int64_t value);

  Statement& bind(int idx, float value);

  Statement& bind(int idx, double value);

  Statement& bind(int idx, const std::string& value);

  Statement& bind(int idx, const char* value);

  Statement& bind(int idx, char* value);

  Statement& bind(int idx, std::nullptr_t);

  template <class T>
  Statement& bind(int idx, T value) = delete;  // == Все остальные типы запрещены

  bool step();

  void reset();

  bool isNull(int col) const;

  int columnCount() const;

  template <class T>
  T get(int col) const = delete;

 private:
  explicit Statement(sqlite3_stmt* stmt) : m_stmt(stmt) {}

  friend class dbSqlite;

  void check(int res, const char* where) const;  // Бросает SqliteError, если res != SQLITE_OK

  void checkColumn(int col) const;  // Бросает SqliteError, если col - invalid

  struct finalizer {
    void operator()(sqlite3_stmt* _stmt) const noexcept { sqlite3_finalize(_stmt); }
  };

  std::unique_ptr<sqlite3_stmt, finalizer> m_stmt;
};

class dbSqlite {
 public:
  explicit dbSqlite(const char* filename, int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

  dbSqlite(const dbSqlite&) = delete;

  dbSqlite& operator=(const dbSqlite&) = delete;

  Statement prepare(const std::string& sql);

  void exec(const std::string& sql);

 private:
  struct SqliteDeleter {
    void operator()(sqlite3* db) const noexcept {
      auto status = sqlite3_close_v2(db);
      if (status != SQLITE_OK) {
        const char* raw_err = sqlite3_errstr(status);
        raw_err = raw_err ? raw_err : "Unknown SQLite error status";

        LOG_CRITICAL(fmt::format("Sqlite error in ~dbSqlite(): {}", raw_err));
      }
    }
  };

  std::unique_ptr<sqlite3, SqliteDeleter> m_dbHandle;
};

template <>
bool Statement::get(int col) const;

template <>
int Statement::get(int col) const;

template <>
std::int64_t Statement::get(int col) const;

template <>
float Statement::get(int col) const;

template <>
double Statement::get(int col) const;

template <>
std::string Statement::get(int col) const;

#endif  //TGVOICEBOT_DBSQLITE_H
