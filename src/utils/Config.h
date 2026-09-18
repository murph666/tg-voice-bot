//
// Created by murph on 9/14/26.
//

#ifndef TGVOICEBOT_CONFIG_H
#define TGVOICEBOT_CONFIG_H

#include <cstdint>
#include <functional>
#include <optional>
#include <string>

struct appConfiguration {
  std::string bot_token;
  int64_t adminID{};
  std::string dbPath;
};

class Config {
 public:
  using EnvReader = std::function<std::optional<std::string>(std::string_view)>;
  using EnvString = std::optional<std::string>;
  using EnvInt = std::optional<int64_t>;

  explicit Config(EnvReader reader = RealGetenv) : m_reader(std::move(reader)) {}

  void Load();

  ~Config() = default;

  std::string getBotToken() const { return m_config.bot_token; };

  std::string getDBPath() const { return m_config.dbPath; };

  int64_t m_adminID() const { return m_config.adminID; };

 private:
  appConfiguration m_config{};

  EnvReader m_reader{};

  static EnvString getString(const char* key);

  static EnvInt EnvGetInt(std::string& env_string);

  static std::optional<std::string> RealGetenv(std::string_view name);
};

#endif  //TGVOICEBOT_CONFIG_H
