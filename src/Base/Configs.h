#pragma once

#include <string>

namespace cncpp {
// 连接配置
struct MysqlConfig {
  std::string host = {};
  std::string user = {};
  std::string password = {};
  std::string database = {};
  unsigned int port = 3306;
  const char *charset = "utf8mb4";
};
} // namespace cncpp