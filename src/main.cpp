//
// Created by murph on 8/7/26.
//

#include <iostream>
#include "utils/Config.h"
#include "utils/logger.h"

int main(int argc, char* argv[]) {
  Logger::Init();
  Config config_obj = Config();
  try {
    config_obj.Load();
  } catch (const std::runtime_error& e) {
    LOG_CRITICAL(e.what());
    return -1;
  };

  return 0;
}
