#include "ih/quotes/quote_book.hpp"

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <utility>

#include "core/tools/time.hpp"
#include "idgen/market_entry_id.hpp"

namespace simulator::trading_system::quoting_engine {

QuoteBook::QuoteBook(std::shared_ptr<QuoteIdGenerator> quote_id_generator)
    : quote_id_generator_{std::move(quote_id_generator)} {
  if (quote_id_generator_ == nullptr) {
    throw std::invalid_argument{"quote id generator cannot be null"};
  }
}

auto QuoteBook::add(const std::vector<QuoteLevel>& bids,
                    const std::vector<QuoteLevel>& asks) -> const Quote& {
  return quotes_.emplace_back(
      Quote{.identifier = quote_id_generator_->generate(),
            .publication_time = core::get_current_system_time(),
            .bids = identify(bids),
            .asks = identify(asks)});
}

auto QuoteBook::identify(const std::vector<QuoteLevel>& levels)
    -> std::vector<PublishedLevel> {
  std::vector<PublishedLevel> published;
  published.reserve(levels.size());
  std::ranges::transform(
      levels, std::back_inserter(published), [this](const QuoteLevel& level) {
        return PublishedLevel{
            .entry_id = idgen::generate_new_id(entry_id_context_),
            .price = level.price,
            .quantity = level.quantity};
      });
  return published;
}

auto QuoteBook::latest() const -> const Quote* {
  return quotes_.empty() ? nullptr : &quotes_.back();
}

}  // namespace simulator::trading_system::quoting_engine
