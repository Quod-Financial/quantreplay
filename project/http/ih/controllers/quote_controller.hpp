#ifndef SIMULATOR_HTTP_IH_CONTROLLERS_QUOTE_CONTROLLER_HPP_
#define SIMULATOR_HTTP_IH_CONTROLLERS_QUOTE_CONTROLLER_HPP_

#include <pistache/http_defs.h>

#include <memory>
#include <string>
#include <utility>

#include "ih/marshalling/json/quote.hpp"
#include "ih/redirect/redirection_processor.hpp"

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
  virtual auto publish_quote(const std::string& body) const -> Result = 0;
};

class QuoteControllerImpl final : public QuoteController {
 public:
  QuoteControllerImpl(
      std::shared_ptr<redirect::RedirectionProcessor> redirector,
      std::string venue_id);

  [[nodiscard]]
  auto publish_quote(const std::string& body) const -> Result override;

 private:
  [[nodiscard]]
  auto redirect(const std::string& venue_id, const std::string& body) const
      -> Result;

  [[nodiscard]]
  static auto publish(const json::QuotePublication& publication) -> Result;

  std::shared_ptr<redirect::RedirectionProcessor> redirector_;
  std::string venue_id_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_CONTROLLERS_QUOTE_CONTROLLER_HPP_
