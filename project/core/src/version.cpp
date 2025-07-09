#include "core/version.hpp"

namespace simulator::core {

auto version() noexcept -> std::string_view {
#if !defined(SIM_VERSION_MAJOR)
#error "SIM_VERSION_MAJOR macro is not defined, can not define the version"
  return "";
#elif !defined(SIM_VERSION_REV_UNIX_TIMESTAMP)
#error \
    "SIM_VERSION_REV_UNIX_TIMESTAMP macro is not defined, can not define the version"
  return "";
#elif !defined(SIM_VERSION_REV_HASH)
#error "SIM_VERSION_REV_HASH macro is not defined, can not define the version"
  return "";
#elif !defined(SIM_VERSION_REV_HUMAN_READABLE_TIMESTAMP)
#error \
    "SIM_VERSION_REV_HUMAN_READABLE_TIMESTAMP macro is not defined, can not define the version"
  return "";
#else
  return SIM_VERSION_MAJOR "." SIM_VERSION_REV_UNIX_TIMESTAMP
                           "_" SIM_VERSION_REV_HASH
                           "(" SIM_VERSION_REV_HUMAN_READABLE_TIMESTAMP ")";
#endif
}

auto major_version() noexcept -> std::string_view {
#ifndef SIM_VERSION_MAJOR
#error \
    "SIM_VERSION_MAJOR macro is not defined, can not define the major version"
  return "";
#else
  return SIM_VERSION_MAJOR;
#endif
}

}  // namespace simulator::core