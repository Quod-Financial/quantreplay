#ifndef SIMULATOR_APP_IH_PLATFORMS_PLATFORM_HPP_
#define SIMULATOR_APP_IH_PLATFORMS_PLATFORM_HPP_

namespace simulator {

struct Platform {
  Platform() = default;
  Platform(const Platform&) = default;
  Platform(Platform&&) noexcept = default;
  virtual ~Platform() = default;

  auto operator=(const Platform&) -> Platform& = default;
  auto operator=(Platform&&) noexcept -> Platform& = default;

  virtual auto launch() -> void = 0;

  virtual auto terminate() -> void = 0;
};

}  // namespace simulator

#endif  // SIMULATOR_APP_IH_PLATFORMS_PLATFORM_HPP_