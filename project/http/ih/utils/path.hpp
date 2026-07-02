#ifndef SIMULATOR_HTTP_IH_UTILS_PATH_HPP_
#define SIMULATOR_HTTP_IH_UTILS_PATH_HPP_

#include <string>
#include <string_view>

namespace simulator::http {

// Removes dot characters and replaces filesystem-illegal characters with a
// dash so the value is safe to use as a path component.
[[nodiscard]]
auto cleanse_path_component(std::string_view value) -> std::string;

// Builds the attachment file name for the data dictionaries archive of the
// given venue and session.
[[nodiscard]]
auto make_dictionaries_filename(std::string_view venue_id,
                                std::string_view session_id) -> std::string;

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_UTILS_PATH_HPP_
