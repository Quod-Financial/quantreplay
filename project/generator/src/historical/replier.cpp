#include "ih/historical/replier.hpp"

#include <exception>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "ih/historical/data/provider.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/processor.hpp"
#include "ih/historical/scheduler.hpp"
#include "log/logging.hpp"

namespace simulator::generator::historical {

Replier::Replier(const Datasources& datasources,
                 const InstrumentsContexts& instruments_contexts) {
  if (instruments_contexts.empty()) {
    throw std::invalid_argument{
        "can not initialize historical replier without instruments "
        "contexts"};
  }

  historical_processor_ =
      std::make_unique<ActionProcessor>(instruments_contexts);

  // We'll redesign the approach of datasources initialization when
  // a reading from multiple datasources will be required.
  // It may require parallel initialization of data_providers and
  // data_adapters.
  auto provider = create_provider(datasources);

  actions_scheduler_ =
      std::make_unique<historical::ActionsScheduler>(std::move(provider));

  log::info("historical data replier initialized successfully");
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

auto Replier::create_provider(const Datasources& datasources)
    -> std::unique_ptr<DataProvider> {
  if (datasources.empty()) {
    log::info(
        "no data sources configured, skipping historical replying "
        "configuration");
    return nullptr;
  }

  const auto& datasource = datasources.front();
  auto provider_factory = std::make_unique<DataProvidersFactoryImpl>();
  std::unique_ptr<DataProvider> provider{nullptr};

  try {
    provider = provider_factory->create_provider(datasource);

    log::info("created a data provider for the `{}' datasource (id: {})",
              datasource.name(),
              datasource.datasource_id());
  } catch (const std::exception& exception) {
    log::warn(
        "Failed to initialize a data provider for `{}' datasource "
        "(id: {}): {}",
        datasource.name(),
        datasource.datasource_id(),
        exception.what());
  }

  return provider;
}

}  // namespace simulator::generator::historical
