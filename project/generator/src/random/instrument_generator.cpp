#include "ih/random/instrument_generator.hpp"

#include <fmt/chrono.h>

#include <cassert>
#include <chrono>
#include <cmath>
#include <memory>
#include <ratio>

#include "ih/adaptation/protocol_conversion.hpp"
#include "ih/constants.hpp"
#include "log/logging.hpp"
#include "middleware/routing/trading_request_channel.hpp"

namespace simulator::generator::random {
namespace {

auto send_message(const GeneratedMessage& message,
                  const InstrumentDescriptor& instrument) -> void {
  try {
    if (message.message_type == MessageType::NewOrderSingle) {
      middleware::send_trading_request(
          convert_to_order_placement_request(message, instrument));
    } else if (message.message_type == MessageType::OrderCancelReplaceRequest) {
      middleware::send_trading_request(
          convert_to_order_modification_request(message, instrument));
    } else if (message.message_type == MessageType::OrderCancelRequest) {
      middleware::send_trading_request(
          convert_to_order_cancellation_request(message, instrument));
    }
  } catch (const middleware::ChannelUnboundError&) {
    log::err(
        "failed to send message from random generator - trading request "
        "channel is not bound");
  }
}

}  // namespace

OrderGenerator::OrderGenerator(
    std::shared_ptr<OrderInstrumentContext> instrument_context,
    std::unique_ptr<GenerationAlgorithm> random_generation_algorithm)
    : instrument_context_{std::move(instrument_context)},
      generation_algorithm_{std::move(random_generation_algorithm)} {
  assert(instrument_context_);
  assert(generation_algorithm_);

  init_execution_rate();

  const auto& listing = instrument_context_->get_instrument();
  log::info(
      "successfully initialized random orders generator for `{}' instrument "
      "(id: {}), execution rate is set to {}",
      listing.symbol(),
      listing.listing_id(),
      execution_rate_);
}

auto OrderGenerator::finished() const noexcept -> bool {
  // A random instrument generator is an infinite process.
  return false;
}

auto OrderGenerator::prepare() noexcept -> void {}

auto OrderGenerator::execute() -> void {
  const auto& listing = instrument_context_->get_instrument();
  log::debug(
      "executing random order message generation operation for `{}' (id: {})",
      listing.symbol(),
      listing.listing_id());

  const bool publish = generation_algorithm_->generate(generated_message_);
  if (!publish) {
    return;
  }

  send_message(generated_message_,
               instrument_context_->get_instrument_descriptor());
}

auto OrderGenerator::next_exec_timeout() const -> std::chrono::microseconds {
  return execution_rate_;
}

auto OrderGenerator::init_execution_rate() -> void {
  constexpr double norm_numerator = 3.0;
  constexpr double norm_denominator = 2.0;

  const auto& instrument = instrument_context_->get_instrument();
  const auto update_rate = instrument.random_orders_rate().value_or(
      constant::DefaultListingRandomOrdersRate);
  assert(update_rate > 0);

  const double normalize_coefficient =
      update_rate * norm_numerator / norm_denominator;
  assert(normalize_coefficient > 0.0);

  const auto normalized_rate =
      std::chrono::microseconds{static_cast<std::uint32_t>(
          std::round(std::micro::den / normalize_coefficient))};

  log::debug(
      "normalized random generation execution rate for `{}' instrument "
      "(id: {}) is calculated as 1 random message per {} "
      "(rate normalization coefficient is {})",
      instrument.symbol(),
      instrument.listing_id(),
      normalized_rate,
      normalize_coefficient);

  execution_rate_ = normalized_rate;
}

}  // namespace simulator::generator::random
