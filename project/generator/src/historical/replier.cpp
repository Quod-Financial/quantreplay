#include "ih/historical/replier.hpp"

#include <cassert>
#include <exception>
#include <memory>
#include <utility>

#include "ih/historical/data/provider.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/processing/action_sink.hpp"
#include "ih/historical/scheduler.hpp"
#include "log/logging.hpp"

namespace simulator::generator::historical {

Replier::Replier(const data_layer::Datasource& datasource,
                 std::unique_ptr<ActionSink> processor)
    : historical_processor_{std::move(processor)} {
  assert(historical_processor_);

  actions_scheduler_ = std::make_unique<historical::ActionsScheduler>(
      create_provider(datasource));

  log::info(
      "historical data replier for the `{}' datasource (id: {}) initialized "
      "successfully",
      datasource.name(),
      datasource.datasource_id());
}

auto Replier::prepare() noexcept -> void { actions_scheduler_->initialize(); }

auto Replier::execute() -> void {
  actions_scheduler_->process_next_action([this](historical::Action action) {
    log::debug("historical replier is applying historical {}", action);
    historical_processor_->process(std::move(action));
  });
}

auto Replier::finished() const noexcept -> bool {
  return actions_scheduler_->finished();
}

auto Replier::next_exec_timeout() const -> std::chrono::microseconds {
  return actions_scheduler_->next_action_timeout();
}

auto Replier::create_provider(const data_layer::Datasource& datasource)
    -> std::unique_ptr<DataProvider> {
  auto provider_factory = std::make_unique<DataProvidersFactoryImpl>();
  std::unique_ptr<DataProvider> provider{nullptr};

  try {
    provider = provider_factory->create_provider(datasource);

    log::info("created a data provider for the `{}' datasource (id: {})",
              datasource.name(),
              datasource.datasource_id());
  } catch (const std::exception& exception) {
    log::err(
        "Failed to initialize a data provider for `{}' datasource "
        "(id: {}): {}",
        datasource.name(),
        datasource.datasource_id(),
        exception.what());
  }

  return provider;
}

}  // namespace simulator::generator::historical
