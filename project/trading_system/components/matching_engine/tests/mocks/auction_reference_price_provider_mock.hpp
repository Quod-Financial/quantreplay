#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_AUCTION_REFERENCE_PRICE_PROVIDER_MOCK_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_AUCTION_REFERENCE_PRICE_PROVIDER_MOCK_HPP_

#include <gmock/gmock.h>

#include <optional>

#include "core/domain/attributes.hpp"
#include "ih/common/abstractions/auction_reference_price_provider.hpp"

namespace simulator::trading_system::matching_engine::test {

struct AuctionReferencePriceProviderMock
    : public AuctionReferencePriceProvider {
  MOCK_METHOD(std::optional<Price>,
              last_open_phase_traded_price,
              (),
              (const, override));
  MOCK_METHOD(std::optional<Price>, closing_price, (), (const, override));
};

}  // namespace simulator::trading_system::matching_engine::test

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_AUCTION_REFERENCE_PRICE_PROVIDER_MOCK_HPP_
