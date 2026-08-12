#include "ih/pqxx/result/detail/basic_row_parser.hpp"

#include <date/date.h>

#include <sstream>

namespace simulator::data_layer::internal_pqxx::detail {

auto BasicRowParser::parse_timestamp(const std::string& encoded_timestamp)
    -> std::optional<core::sys_us> {
  std::istringstream stream{encoded_timestamp};
  core::sys_us time;
  stream >> date::parse("%F %T", time);

  if (stream.fail() || stream.bad()) {
    return std::nullopt;
  }
  return std::make_optional(time);
}

}  // namespace simulator::data_layer::internal_pqxx::detail
