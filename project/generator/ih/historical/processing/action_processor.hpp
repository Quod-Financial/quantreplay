#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSING_ACTION_PROCESSOR_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSING_ACTION_PROCESSOR_HPP_

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "ih/context/instrument_context.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/processing/action_sink.hpp"
#include "ih/historical/processing/record_sink.hpp"

namespace simulator::generator::historical {

class ActionProcessor : public historical::ActionSink,
                        public historical::RecordSink {
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

  auto process(historical::Record record) -> void override;

 private:
  ContextsRegistry ctx_registry_;
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSING_ACTION_PROCESSOR_HPP_
