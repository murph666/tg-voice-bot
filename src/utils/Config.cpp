//
// Created by murph on 9/14/26.
//

#include "Config.h"

void Config::Load() {
  if (auto status = m_reader("ADMIN_TELEGRAM_ID"); status != std::nullopt) {
    if (auto res = EnvGetInt(status.value()); res != std::nullopt && res > 0) {
      m_config.adminID = res.value();
    } else {
      throw std::runtime_error("\"ADMIN_TELEGRAM_ID\" is not a number!");
    }
  } else {
    throw std::runtime_error("\"ADMIN_TELEGRAM_ID\" is not a valid!");
  }
  if (auto status = m_reader("DB_PATH"); status != std::nullopt) {
    m_config.dbPath = status.value();
  } else {
    throw std::runtime_error("\"DB_PATH\" is not a valid!");
  }
  if (auto status = m_reader("BOT_TOKEN"); status != std::nullopt) {
    m_config.bot_token = status.value();
  } else {
    throw std::runtime_error("\"BOT_TOKEN\" is not a valid!");
  }
}

Config::EnvString Config::getString(const char* key) {
  const char* value = std::getenv(key);
  if (value == nullptr)
    return std::nullopt;
  return std::string(value);
}

Config::EnvInt Config::EnvGetInt(std::string& env_string) {
  try {
    int64_t value = std::stol(env_string);
    return value;
  } catch (const std::invalid_argument& e) {
    return std::nullopt;
  }
}

std::optional<std::string> Config::RealGetenv(std::string_view name) {
  return getString(std::string(name).c_str());
}
