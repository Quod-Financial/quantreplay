#include "ih/utils/path.hpp"

#include <fmt/format.h>

#include <regex>
#include <string>
#include <string_view>

namespace simulator::http {

auto cleanse_path_component(std::string_view value) -> std::string {
  static const std::regex dots{R"(\.)"};
  static const std::regex illegal{R"([/\\:*?"<>|])"};
  const std::string without_dots =
      std::regex_replace(std::string{value}, dots, "");
  return std::regex_replace(without_dots, illegal, "-");
}

auto make_dictionaries_filename(std::string_view venue_id,
                                std::string_view session_id) -> std::string {
  return fmt::format("{}_{}_Dictionaries.zip",
                     cleanse_path_component(venue_id),
                     cleanse_path_component(session_id));
}

}  // namespace simulator::http
