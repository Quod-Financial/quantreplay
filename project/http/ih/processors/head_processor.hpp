#ifndef SIMULATOR_HTTP_IH_PROCESSORS_HEAD_PROCESSOR_HPP_
#define SIMULATOR_HTTP_IH_PROCESSORS_HEAD_PROCESSOR_HPP_

#include <pistache/http.h>
#include <pistache/http_header.h>
#include <pistache/router.h>

#include <memory>
#include <string>
#include <vector>

#include "ih/config_provider.hpp"
#include "ih/redirect/redirection_processor.hpp"
#include "ih/redirect/result.hpp"

namespace simulator::http {

class HeadProcessor {
 public:
  virtual ~HeadProcessor() = default;

  virtual auto get_data_dictionaries(const Pistache::Rest::Request& request,
                                     Pistache::Http::ResponseWriter response)
      -> void = 0;
};

class HeadProcessorImpl final : public HeadProcessor {
 public:
  HeadProcessorImpl(std::shared_ptr<redirect::RedirectionProcessor> redirector,
                    std::shared_ptr<ConfigProvider> config_provider);

  auto get_data_dictionaries(const Pistache::Rest::Request& request,
                             Pistache::Http::ResponseWriter response)
      -> void override;

 private:
  [[nodiscard]]
  auto has_session(const std::string& session_id) const -> bool;

  auto relay_data_dictionaries(const Pistache::Rest::Request& request,
                               Pistache::Http::ResponseWriter& response,
                               const std::string& venue_id,
                               const std::string& session_id) const -> void;

  auto redirect(const Pistache::Rest::Request& request,
                const std::string& instance_id) const -> redirect::Result;

  static auto respond(const Pistache::Rest::Request& request,
                      Pistache::Http::ResponseWriter& response,
                      Pistache::Http::Code code) -> void;

  static auto respond_ok(
      const Pistache::Rest::Request& request,
      Pistache::Http::ResponseWriter& response,
      const std::vector<std::shared_ptr<Pistache::Http::Header::Header>>&
          headers) -> void;

  std::shared_ptr<redirect::RedirectionProcessor> redirector_;
  std::shared_ptr<ConfigProvider> config_provider_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_PROCESSORS_HEAD_PROCESSOR_HPP_
