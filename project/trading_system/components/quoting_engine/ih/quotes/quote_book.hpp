#ifndef SIMULATOR_QUOTING_ENGINE_IH_QUOTES_QUOTE_BOOK_HPP_
#define SIMULATOR_QUOTING_ENGINE_IH_QUOTES_QUOTE_BOOK_HPP_

#include <memory>
#include <vector>

#include "idgen/market_entry_id.hpp"
#include "ih/quotes/quote.hpp"
#include "quoting_engine/quote_id_generator.hpp"

namespace simulator::trading_system::quoting_engine {

class QuoteBook {
 public:
  explicit QuoteBook(std::shared_ptr<QuoteIdGenerator> quote_id_generator);

  auto add(const std::vector<QuoteLevel>& bids,
           const std::vector<QuoteLevel>& asks) -> const Quote&;

  [[nodiscard]]
  auto latest() const -> const Quote*;

  [[nodiscard]]
  auto quotes() const -> const std::vector<Quote>& {
    return quotes_;
  }

 private:
  [[nodiscard]]
  auto identify(const std::vector<QuoteLevel>& levels)
      -> std::vector<PublishedLevel>;

  std::vector<Quote> quotes_;
  std::shared_ptr<QuoteIdGenerator> quote_id_generator_;
  idgen::MarketEntryIdContext entry_id_context_{
      idgen::make_market_entry_id_generation_ctx()};
};

}  // namespace simulator::trading_system::quoting_engine

#endif  // SIMULATOR_QUOTING_ENGINE_IH_QUOTES_QUOTE_BOOK_HPP_
