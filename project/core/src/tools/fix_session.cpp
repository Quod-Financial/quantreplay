#include "core/tools/fix_session.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace simulator::core {
namespace {

constexpr std::string_view CompIdSeparator{"->"};
constexpr char PartSeparator{':'};
constexpr char ValueSeparator{','};
constexpr std::string_view Whitespaces{" \t\n\v\f\r"};

[[nodiscard]]
auto trim(std::string_view text) -> std::string_view {
  const auto first = text.find_first_not_of(Whitespaces);
  if (first == std::string_view::npos) {
    return {};
  }
  const auto last = text.find_last_not_of(Whitespaces);
  return text.substr(first, last - first + 1);
}

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

auto parse_fix_session_reference(std::string_view text)
    -> std::optional<FixSessionReference> {
  const auto separator = text.find(ValueSeparator);

  auto session_id = parse_fix_session_id(trim(text.substr(0, separator)));
  if (!session_id.has_value()) {
    return std::nullopt;
  }

  std::optional<FixSessionReference> reference{std::in_place};
  reference->session_id = std::move(*session_id);

  if (separator != std::string_view::npos) {
    const std::string_view settings_file = trim(text.substr(separator + 1));
    if (settings_file.empty() ||
        settings_file.find(ValueSeparator) != std::string_view::npos) {
      return std::nullopt;
    }
    reference->settings_file = settings_file;
  }

  return reference;
}

}  // namespace simulator::core
