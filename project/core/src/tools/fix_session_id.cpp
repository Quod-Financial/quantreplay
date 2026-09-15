#include "core/tools/fix_session_id.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace simulator::core {
namespace {

constexpr std::string_view CompIdSeparator{"->"};
constexpr char PartSeparator{':'};

}  // namespace

auto parse_fix_session_id(std::string_view text)
    -> std::optional<FixSessionId> {
  const auto begin_string_end = text.find(PartSeparator);
  if (begin_string_end == std::string_view::npos) {
    return std::nullopt;
  }

  const auto comp_ids_separator = text.find(CompIdSeparator, begin_string_end);
  if (comp_ids_separator == std::string_view::npos) {
    return std::nullopt;
  }

  const auto target_comp_id_begin =
      comp_ids_separator + CompIdSeparator.size();
  const auto qualifier_separator =
      text.find(PartSeparator, target_comp_id_begin);
  const bool qualified = qualifier_separator != std::string_view::npos;

  const std::string_view begin_string = text.substr(0, begin_string_end);
  const std::string_view sender_comp_id = text.substr(
      begin_string_end + 1, comp_ids_separator - begin_string_end - 1);
  const std::string_view target_comp_id = text.substr(
      target_comp_id_begin, qualifier_separator - target_comp_id_begin);
  const std::string_view session_qualifier =
      qualified ? text.substr(qualifier_separator + 1) : std::string_view{};

  if (begin_string.empty() || sender_comp_id.empty() ||
      target_comp_id.empty() || (qualified && session_qualifier.empty())) {
    return std::nullopt;
  }

  std::optional<FixSessionId> session_id{std::in_place};
  session_id->begin_string = std::string{begin_string};
  session_id->sender_comp_id = std::string{sender_comp_id};
  session_id->target_comp_id = std::string{target_comp_id};
  if (qualified) {
    session_id->session_qualifier = std::string{session_qualifier};
  }
  return session_id;
}

}  // namespace simulator::core
