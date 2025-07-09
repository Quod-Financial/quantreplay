#ifndef SIMULATOR_GENERATOR_IH_RANDOM_INSTRUMENT_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_INSTRUMENT_GENERATOR_HPP_

#include <chrono>
#include <memory>

#include "ih/adaptation/generated_message.hpp"
#include "ih/context/instrument_context.hpp"
#include "ih/random/algorithm/generation_algorithm.hpp"
#include "ih/utils/executable.hpp"

namespace simulator::generator::random {

class OrderGenerator : public Executable {
 public:
  OrderGenerator() = delete;

  OrderGenerator(
      std::shared_ptr<OrderInstrumentContext> instrument_context,
      std::unique_ptr<GenerationAlgorithm> random_generation_algorithm);

  // An empty implementation.
  // InstrumentGenerator has nothing to prepare before starting generation
  auto prepare() noexcept -> void override;

  auto execute() -> void override;

  auto finished() const noexcept -> bool override;

  [[nodiscard]]
  auto next_exec_timeout() const -> std::chrono::microseconds override;

 private:
  auto init_execution_rate() -> void;

  GeneratedMessage generated_message_;

  std::shared_ptr<OrderInstrumentContext> instrument_context_;

  std::unique_ptr<GenerationAlgorithm> generation_algorithm_;

  std::chrono::microseconds execution_rate_{0};
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_INSTRUMENT_GENERATOR_HPP_
