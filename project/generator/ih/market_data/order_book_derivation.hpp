#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_ORDER_BOOK_DERIVATION_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_ORDER_BOOK_DERIVATION_HPP_

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <optional>

#include "core/domain/attributes.hpp"
#include "ih/context/component_context.hpp"
#include "ih/historical/processor.hpp"
#include "ih/market_data/subscription_requirement.hpp"
#include "protocol/app/market_data_snapshot.hpp"

namespace simulator::generator::mdata {

class OrderBookDerivation {
 public:
  OrderBookDerivation() = delete;

  OrderBookDerivation(std::shared_ptr<historical::Processor> processor,
                      std::shared_ptr<ComponentContext> context);

  auto add(const MdRequestId& request_id,
           OrderDerivationRequirement requirement) -> void;

  auto process(const protocol::MarketDataSnapshot& snapshot) -> void;

 private:
  struct Binding {
    OrderDerivationRequirement requirement;
    std::uint64_t converted_records = 0;
  };

  [[nodiscard]]
  auto convert(const protocol::MarketDataSnapshot& snapshot)
      -> std::optional<historical::Record>;

  std::map<MdRequestId, Binding> bindings_;
  std::shared_ptr<historical::Processor> processor_;
  std::shared_ptr<ComponentContext> context_;
  std::mutex mutex_;
};

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_ORDER_BOOK_DERIVATION_HPP_
