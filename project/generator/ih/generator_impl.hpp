#ifndef SIMULATOR_GENERATOR_IH_GENERATOR_IMPL_HPP_
#define SIMULATOR_GENERATOR_IH_GENERATOR_IMPL_HPP_

#include <memory>
#include <unordered_map>
#include <vector>

#include "data_layer/api/database/context.hpp"
#include "ih/context/generation_manager.hpp"
#include "ih/context/instrument_context.hpp"
#include "ih/factory/executable_factory.hpp"
#include "ih/random/instrument_generator.hpp"
#include "ih/utils/executor.hpp"

namespace simulator::generator {

class GeneratorImpl final {
 public:
  GeneratorImpl() = delete;

  GeneratorImpl(const data_layer::Venue& target_venue,
                data_layer::database::Context database_context);

  GeneratorImpl(const GeneratorImpl&) = delete;
  auto operator=(const GeneratorImpl&) -> GeneratorImpl& = delete;

  GeneratorImpl(GeneratorImpl&&) = delete;
  auto operator=(GeneratorImpl&&) -> GeneratorImpl& = delete;

  ~GeneratorImpl() noexcept;

  auto status() -> bool;

  auto suspend() -> void;

  auto resume() -> void;

  auto start() -> void;

  auto terminate() noexcept -> void;

  auto process_reply(std::uint64_t instrument_id, const GeneratedMessage& reply)
      -> void;

 private:
  auto initialize_instruments() -> void;

  auto initialize_instrument(const data_layer::Listing& listing) -> void;

  auto initialize_random_generation_executors() -> void;

  auto initialize_historical_executor() -> void;

  auto terminate_generator() noexcept -> void;

  std::vector<std::unique_ptr<Executor>> listings_random_generators_;
  std::vector<std::shared_ptr<OrderInstrumentContext>> order_listings_contexts_;
  std::unordered_map<std::uint64_t, std::shared_ptr<OrderInstrumentContext>>
      context_lookup_;

  data_layer::database::Context database_context_;

  std::shared_ptr<GenerationManager> generation_manager_;

  std::unique_ptr<Executor> historical_replier_;
  std::unique_ptr<InstrumentRandomGeneratorFactory> rnd_executor_factory_;

  std::unique_ptr<InstrumentHistoricalReplierFactory> hist_executor_factory_;

  bool was_terminated_{false};
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_GENERATOR_IMPL_HPP_
