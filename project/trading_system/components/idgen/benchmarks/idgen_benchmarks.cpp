#include <benchmark/benchmark.h>

#include "cfg/api/cfg.hpp"
#include "common/attributes.hpp"
#include "idgen/execution_id.hpp"
#include "idgen/instrument_id.hpp"
#include "idgen/order_id.hpp"

namespace {

namespace idgen = simulator::trading_system::idgen;

void setup([[maybe_unused]] const benchmark::State& state) {
  using namespace simulator::cfg;

  // Currently we have no other options to disable logging in runtime,
  // to be updated, once configuration/logging implementation is redesigned
  simulator::cfg::init();
  auto& log_cfg = const_cast<LogConfiguration&>(simulator::cfg::log());
  log_cfg.level = "ERROR";
  log_cfg.maxFiles = 0;
  log_cfg.maxSize = 0;
}

auto BM_make_instrument_id_generation_ctx(benchmark::State& state) -> void {
  for (auto _ : state) {
    auto ctx = idgen::make_instrument_id_generation_ctx();
    benchmark::DoNotOptimize(ctx);
  }
}

auto BM_reset_instrument_id_generation_ctx(benchmark::State& state) -> void {
  auto context = idgen::make_instrument_id_generation_ctx();

  for (auto _ : state) {
    idgen::reset_generation_ctx(context);
  }
}

auto BM_generate_new_instrument_id(benchmark::State& state) -> void {
  auto context = idgen::make_instrument_id_generation_ctx();

  for (auto _ : state) {
    auto listing_id = idgen::generate_new_id(context);
    benchmark::DoNotOptimize(listing_id);
  }
}

auto BM_make_order_id_generation_ctx(benchmark::State& state) -> void {
  for (auto _ : state) {
    auto ctx = idgen::make_order_id_generation_ctx();
    benchmark::DoNotOptimize(ctx);
  }
}

auto BM_generate_new_order_id(benchmark::State& state) -> void {
  auto context = idgen::make_order_id_generation_ctx();

  for (auto _ : state) {
    auto order_id = idgen::generate_new_id(context);
    benchmark::DoNotOptimize(order_id);
  }
}

auto BM_make_execution_id_generation_ctx(benchmark::State& state) -> void {
  const simulator::trading_system::OrderId exec_order_id{240205103011000042};

  for (auto _ : state) {
    auto ctx = idgen::make_execution_id_generation_ctx(exec_order_id);
    benchmark::DoNotOptimize(ctx);
  }
}

auto BM_generate_new_execution_id(benchmark::State& state) -> void {
  const simulator::trading_system::OrderId exec_order_id{240205103011000042};
  auto context = idgen::make_execution_id_generation_ctx(exec_order_id);

  for (auto _ : state) {
    auto execution_id = idgen::generate_new_id(context);
    benchmark::DoNotOptimize(execution_id);
  }
}

}  // namespace

BENCHMARK(BM_make_instrument_id_generation_ctx)->Setup(setup);
BENCHMARK(BM_reset_instrument_id_generation_ctx)->Setup(setup);
BENCHMARK(BM_generate_new_instrument_id)->Setup(setup);
BENCHMARK(BM_make_order_id_generation_ctx)->Setup(setup);
BENCHMARK(BM_generate_new_order_id)->Setup(setup);
BENCHMARK(BM_make_execution_id_generation_ctx)->Setup(setup);
BENCHMARK(BM_generate_new_execution_id)->Setup(setup);

BENCHMARK_MAIN();