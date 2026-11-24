#ifndef SIMULATOR_HTTP_IH_CONTROLLERS_QUOTE_CONTROLLER_HPP_
#define SIMULATOR_HTTP_IH_CONTROLLERS_QUOTE_CONTROLLER_HPP_

#include <pistache/http_defs.h>

#include <optional>
#include <string>
#include <utility>

#include "core/domain/attributes.hpp"
#include "ih/marshalling/json/quote.hpp"

namespace simulator::http {

class QuoteController {
 public:
  using Result = std::pair<Pistache::Http::Code, std::string>;

  QuoteController() = default;
  QuoteController(const QuoteController&) = default;
  QuoteController(QuoteController&&) noexcept = default;
  virtual ~QuoteController() = default;

  auto operator=(const QuoteController&) -> QuoteController& = default;
  auto operator=(QuoteController&&) noexcept -> QuoteController& = default;

  [[nodiscard]]
  virtual auto publication_venue_id(const std::string& body) const
      -> std::optional<std::string> = 0;

  [[nodiscard]]
  virtual auto publish_quote(const std::string& body) const -> Result = 0;

  [[nodiscard]]
  virtual auto cancel_quote(const QuoteId& quote_id) const -> Result = 0;
};

class QuoteControllerImpl final : public QuoteController {
 public:
  [[nodiscard]]
  auto publication_venue_id(const std::string& body) const
      -> std::optional<std::string> override;

  [[nodiscard]]
  auto publish_quote(const std::string& body) const -> Result override;

  [[nodiscard]]
  auto cancel_quote(const QuoteId& quote_id) const -> Result override;

 private:
  [[nodiscard]]
  static auto publish(const json::QuotePublication& publication) -> Result;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_CONTROLLERS_QUOTE_CONTROLLER_HPP_
