//
// Created by murph on 9/11/26.
//

#ifndef TGVOICEBOT_LOGGER_H
#define TGVOICEBOT_LOGGER_H

#include <spdlog/spdlog.h>

#include <cstddef>
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

class Logger {
 public:
  static void Init(const std::string path = "logs/app.log", const size_t max_length = 1024 * 1024 * 5,
                   const size_t lcount = 5) {
    auto _console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    _console->set_level(spdlog::level::warn);  // Console shows Warn, Error, Critical
    _console->set_pattern("%^[%L]%$ %v");      // Colored short log: "[I] Hello World"

    auto _file = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(path, max_length, lcount);
    _file->set_level(spdlog::level::trace);                     // File captures everything
    _file->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");  // Full timestamp & thread ID

    auto _logger = std::make_shared<spdlog::logger>("logger", spdlog::sinks_init_list{_file, _console});
    _logger->set_level(spdlog::level::trace);  // Logger level must be as low as the lowest sink

    spdlog::set_default_logger(_logger);
  }
};

#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__)

#endif  //TGVOICEBOT_LOGGER_H
