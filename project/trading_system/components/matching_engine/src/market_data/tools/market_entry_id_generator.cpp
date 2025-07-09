#include "ih/market_data/tools/market_entry_id_generator.hpp"

#include "idgen/market_entry_id.hpp"

namespace simulator::trading_system::matching_engine::mdata {

namespace {

class IdgenImplementation : public MarketEntryIdGenerator {
 public:
  IdgenImplementation()
      : generation_ctx_(idgen::make_market_entry_id_generation_ctx()) {}

  auto operator()() -> MarketEntryId override {
    return idgen::generate_new_id(generation_ctx_);
  }

 private:
  idgen::MarketEntryIdContext generation_ctx_;
};

}  // namespace

auto MarketEntryIdGenerator::create()
    -> std::unique_ptr<MarketEntryIdGenerator> {
  return std::make_unique<IdgenImplementation>();
}

}  // namespace simulator::trading_system::matching_engine::mdata