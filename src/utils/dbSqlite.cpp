//
// Created by murph on 9/24/26.
//

#include "dbSqlite.h"

dbSqlite::dbSqlite(const char* filename, int flags) {
  sqlite3* raw = nullptr;
  auto status = sqlite3_open_v2(filename, &raw, flags, nullptr);
  m_dbHandle.reset(raw);
  if (status != SQLITE_OK) {
    std::string error = fmt::format("Sqlite error in dbSqlite(): {}", sqlite3_errmsg(m_dbHandle.get()));
    throw SqliteError(status, error);
  }
}

Statement dbSqlite::prepare(const std::string& sql) {
  sqlite3_stmt* raw = nullptr;
  const char* tail = nullptr;
  int status = sqlite3_prepare_v2(m_dbHandle.get(), sql.c_str(), -1, &raw, &tail);
  if (status != SQLITE_OK) {
    throw SqliteError(status, fmt::format("Failed to prepare statement: {}", sqlite3_errmsg(m_dbHandle.get())));
  }

  if (raw == nullptr) {
    // Пустая строка или только комментарий: status == OK, но запроса нет
    throw SqliteError(SQLITE_MISUSE, "Failed to prepare statement: SQL is empty");
  }

  Statement stmt{raw};
  // После первого запроса должны остаться только пробелы и переводы строк
  status = sqlite3_prepare_v2(m_dbHandle.get(), tail, -1, &raw, nullptr);
  if (status == SQLITE_OK && raw != nullptr) {
    // в тейле есть запрос
    sqlite3_finalize(raw);
    throw SqliteError(SQLITE_MISUSE, "Failed to prepare statement: SQL tail has another request");
  }

  if (status != SQLITE_OK) {
    throw SqliteError(status, fmt::format("Failed to prepare statement: {}", sqlite3_errmsg(m_dbHandle.get())));
  }

  return stmt;
}

void dbSqlite::exec(const std::string& sql) {
  int status = sqlite3_exec(m_dbHandle.get(), sql.c_str(), nullptr, nullptr, nullptr);
  if (status != SQLITE_OK) {
    std::string error = fmt::format("Sqlite error in exec(): {}", sqlite3_errmsg(m_dbHandle.get()));
    throw SqliteError(status, error);
  }
}

bool Statement::step() {
  const int res = sqlite3_step(m_stmt.get());
  if (res == SQLITE_ROW) {
    return true;
  } else if (res == SQLITE_DONE) {
    return false;
  } else {
    const std::string error = fmt::format("SQLite Statement error in step(): {} (code: {})",
                                          sqlite3_errmsg(sqlite3_db_handle(m_stmt.get())), res);
    throw SqliteError(res, error);
  }
}

void Statement::reset() {
  // Код возврата игнорируем намеренно: если предыдущий step() упал,
  // sqlite3_reset вернёт ту же ошибку повторно, а step() её уже сообщил.
  sqlite3_reset(m_stmt.get());
}

void Statement::check(int res, const char* where) const {
  if (res != SQLITE_OK) {
    throw SqliteError(res, fmt::format("SQLite error in {}: {} (code: {})", where,
                                       sqlite3_errmsg(sqlite3_db_handle(m_stmt.get())), res));
  }
}

void Statement::checkColumn(int col) const {
  const int currentCount = sqlite3_data_count(m_stmt.get());

  if (currentCount == 0)
    throw SqliteError(SQLITE_MISUSE, "Check column fail: invalid row");

  if (col < 0 || col >= currentCount)
    throw SqliteError(SQLITE_RANGE, fmt::format("Check column {} fail: out of range[0, {})", col, currentCount));
}

bool Statement::isNull(int col) const {
  checkColumn(col);
  return sqlite3_column_type(m_stmt.get(), col) == SQLITE_NULL;
}

int Statement::columnCount() const {
  return sqlite3_column_count(m_stmt.get());
}

Statement& Statement::bind(int idx, bool value) {
  return bind(idx, static_cast<int>(value));
}

Statement& Statement::bind(int idx, int value) {
  check(sqlite3_bind_int(m_stmt.get(), idx, value), "bind(int)");
  return *this;
}

Statement& Statement::bind(int idx, std::int64_t value) {
  check(sqlite3_bind_int64(m_stmt.get(), idx, value), "bind(int64)");
  return *this;
}

Statement& Statement::bind(int idx, float value) {
  return bind(idx, static_cast<double>(value));
}

Statement& Statement::bind(int idx, double value) {
  check(sqlite3_bind_double(m_stmt.get(), idx, value), "bind(double)");
  return *this;
}

Statement& Statement::bind(int idx, const std::string& value) {
  check(sqlite3_bind_text(m_stmt.get(), idx, value.data(), static_cast<int>(value.size()), SQLITE_TRANSIENT),
        "bind(string)");
  return *this;
}

Statement& Statement::bind(int idx, const char* value) {
  check(sqlite3_bind_text(m_stmt.get(), idx, value, -1, SQLITE_TRANSIENT), "bind(char *)");
  return *this;
}

Statement& Statement::bind(int idx, char* value) {
  return bind(idx, static_cast<const char*>(value));
}

Statement& Statement::bind(int idx, std::nullptr_t) {
  check(sqlite3_bind_null(m_stmt.get(), idx), "bind(null)");
  return *this;
}

template <>
bool Statement::get(int col) const {
  return Statement::get<int>(col) != 0;
}

template <>
int Statement::get(int col) const {
  checkColumn(col);
  return sqlite3_column_int(m_stmt.get(), col);
}

template <>
int64_t Statement::get(int col) const {
  checkColumn(col);
  return sqlite3_column_int64(m_stmt.get(), col);
}

template <>
float Statement::get(int col) const {
  checkColumn(col);
  return static_cast<float>(sqlite3_column_double(m_stmt.get(), col));
}

template <>
double Statement::get(int col) const {
  checkColumn(col);
  return sqlite3_column_double(m_stmt.get(), col);
}

template <>
std::string Statement::get(int col) const {
  checkColumn(col);
  auto test = sqlite3_column_text(m_stmt.get(), col);
  if (!test) {
    return {};
  }
  return std::string(reinterpret_cast<const char*>(test), sqlite3_column_bytes(m_stmt.get(), col));
}