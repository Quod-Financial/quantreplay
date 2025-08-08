#include "ih/trading_system_facade.hpp"

#include "cfg/api/cfg.hpp"
#include "ih/state_persistence/serializer.hpp"
#include "ih/tools/instrument_resolver.hpp"
#include "ih/tools/loaders.hpp"
#include "ih/tools/trading_engine_factory.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system {

TradingSystemFacade::TradingSystemFacade(Config config,
                                         instrument::Cache instruments)
    : thread_pool_(runtime::ThreadPool::create_simple_thread_pool()),
      event_loop_(runtime::Loop::create_one_second_rate_loop()),
      instruments_(std::move(instruments)),
      config_(std::move(config)),
      instrument_resolver_(create_cached_instrument_resolver(instruments_)),
      repository_accessor_(RepositoryAccessor::create(engines_repository_)),
      execution_system_(*instrument_resolver_, *repository_accessor_),
      event_controller_(ies::Controller(event_loop_)),
      persistence_controller_{config_,
                              execution_system_,
                              std::make_unique<JsonSerializer>(),
                              cfg::venue().name,
                              instruments_.retrieve_instruments()} {
  log::debug("creating trading system facade");

  init_trading_engines();
  launch_ies();
  persistence_controller_.recover();

  log::info("trading system facade created");
}

auto TradingSystemFacade::execute(protocol::OrderPlacementRequest request)
    -> void {
  log::debug("trading system received {}", request);
  execution_system_.execute_request(request);
}

auto TradingSystemFacade::execute(protocol::OrderModificationRequest request)
    -> void {
  log::debug("trading system received {}", request);
  execution_system_.execute_request(request);
}

auto TradingSystemFacade::execute(protocol::OrderCancellationRequest request)
    -> void {
  log::debug("trading system received {}", request);
  execution_system_.execute_request(request);
}

auto TradingSystemFacade::execute(protocol::MarketDataRequest request) -> void {
  log::debug("trading system received {}", request);
  execution_system_.execute_request(std::move(request));
}

auto TradingSystemFacade::execute(
    const protocol::SecurityStatusRequest& request) -> void {
  log::debug("trading system received {}", request);
  execution_system_.execute_request(request);
}

auto TradingSystemFacade::execute(
    const protocol::InstrumentStateRequest& request,
    [[maybe_unused]] protocol::InstrumentState& reply) -> void {
  log::debug("trading system received {}", request);
  execution_system_.execute_request(request, reply);
}

auto TradingSystemFacade::execute(const protocol::HaltPhaseRequest& request,
                                  protocol::HaltPhaseReply& reply) -> void {
  event_controller_.process(request, reply);
}

auto TradingSystemFacade::execute(const protocol::ResumePhaseRequest& request,
                                  protocol::ResumePhaseReply& reply) -> void {
  event_controller_.process(request, reply);
}

auto TradingSystemFacade::execute(
    const protocol::StoreMarketStateRequest& request,
    [[maybe_unused]] protocol::StoreMarketStateReply& reply) -> void {
  log::debug("trading system received {}", request);
  reply.result = persistence_controller_.store();
}

auto TradingSystemFacade::execute(
    const protocol::RecoverMarketStateRequest& request,
    [[maybe_unused]] protocol::RecoverMarketStateReply& reply) -> void {
  log::debug("trading system received {}", request);
  auto [code, error_message] = persistence_controller_.recover();
  reply.result = code;
  reply.error_message = std::move(error_message);
}

auto TradingSystemFacade::react_on(
    const protocol::SessionTerminatedEvent& event) -> void {
  log::debug("trading system is notified about {}", event);
  execution_system_.handle(event);
}

auto TradingSystemFacade::terminate() -> void {
  persistence_controller_.store();
  event_loop_.terminate();
  thread_pool_.await();
}

auto TradingSystemFacade::init_trading_engines() -> void {
  // Load all cached instruments
  std::vector<Instrument> instruments = instruments_.retrieve_instruments();

  // Create a matching engine factory
  std::unique_ptr<TradingEngineFactory> engine_factory =
      create_matching_engine_factory(config_, thread_pool_);

  for (auto& instrument : instruments) {
    // Add a trading engine for each instrument to the repository
    engines_repository_.add_engine(
        instrument, engine_factory->create_trading_engine(instrument));
  }

  log::debug("initialized trading engines");
}

auto TradingSystemFacade::launch_ies() -> void {
  event_controller_
      .bind<event::Tick>([this](const auto& event) { process(event); })
      .bind<event::PhaseTransition>(
          [this](const auto& event) { process(event); });

  event_controller_.set_tz_clock(config_.timezone_clock());
  event_controller_.schedule_phases(config_.trading_phases_schedule());

  event_loop_.run();

  log::debug("started internal event system");
}

auto TradingSystemFacade::process(const event::Tick& event) -> void {
  engines_repository_.for_each_engine(
      [&event](TradingEngine& engine) { engine.handle(event); });
}

auto TradingSystemFacade::process(const event::PhaseTransition& event) -> void {
  engines_repository_.for_each_engine(
      [&event](TradingEngine& engine) { engine.handle(event); });
}

}  // namespace simulator::trading_system