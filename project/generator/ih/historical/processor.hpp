#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSOR_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSOR_HPP_

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "ih/context/instrument_context.hpp"
#include "ih/historical/data/record.hpp"

namespace simulator::generator::historical {

class Processor {
 public:
  Processor() = default;

  Processor(const Processor&) = default;
  auto operator=(const Processor&) -> Processor& = default;

  Processor(Processor&&) = default;
  auto operator=(Processor&&) -> Processor& = default;

  virtual ~Processor() = default;

  virtual auto process(historical::Action action) -> void = 0;
};

class ActionProcessor : public historical::Processor {
 public:
  using ContextPointer = std::shared_ptr<OrderInstrumentContext>;
  using Contexts = std::vector<ContextPointer>;

  class ContextsRegistry {
    using BySymbolRegistry =
        std::unordered_map<std::string_view,
                           std::shared_ptr<OrderInstrumentContext>>;

   public:
    ContextsRegistry() = delete;
    explicit ContextsRegistry(const Contexts& available_contexts) noexcept;

    auto resolve_context(const historical::Record& record) const
        -> ContextPointer;

   private:
    BySymbolRegistry registry_;
  };

  ActionProcessor() = delete;

  explicit ActionProcessor(const Contexts& available_contexts);

  auto process(historical::Action action) -> void override;

 private:
  auto process(historical::Record record) const -> void;
  ContextsRegistry ctx_registry_;
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSOR_HPP_
