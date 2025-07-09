#include <memory>
#include <utility>

#include "ih/chained_mux.hpp"
#include "ih/loop_impl.hpp"
#include "ih/mux_impl.hpp"
#include "ih/one_second_rate_loop.hpp"
#include "ih/simple_thread_pool.hpp"
#include "ih/thread_pool_impl.hpp"
#include "runtime/loop.hpp"
#include "runtime/mux.hpp"
#include "runtime/thread_pool.hpp"

namespace simulator::trading_system::runtime {

auto Loop::create_one_second_rate_loop() -> Loop {
  return Loop(std::make_unique<OneSecondRateLoop>());
}

Loop::Loop(std::unique_ptr<Implementation> impl) : impl_{std::move(impl)} {}

Loop::Loop(Loop&&) noexcept = default;

Loop::~Loop() noexcept = default;

auto Loop::operator=(Loop&&) noexcept -> Loop& = default;

auto Loop::add(std::function<void()> task) -> void {
  impl_->add(std::move(task));
}

auto Loop::run() -> void { impl_->run(); }

auto Loop::terminate() -> void { impl_->terminate(); }

auto Mux::create_chained_mux(Service& executor) -> Mux {
  return Mux(std::make_unique<ChainedMux>(executor));
}

Mux::Mux(std::unique_ptr<Implementation> impl) : impl_(std::move(impl)) {}

Mux::Mux(Mux&&) noexcept = default;

Mux::~Mux() noexcept = default;

auto Mux::operator=(Mux&&) noexcept -> Mux& = default;

auto Mux::execute(std::function<void()> task) -> void {
  impl_->post(std::move(task));
}

auto ThreadPool::create_simple_thread_pool(std::size_t threads) -> ThreadPool {
  return ThreadPool(std::make_unique<SimpleThreadPool>(threads));
}

ThreadPool::ThreadPool(std::unique_ptr<Implementation> impl)
    : impl_{std::move(impl)} {}

ThreadPool::ThreadPool(ThreadPool&&) noexcept = default;

ThreadPool::~ThreadPool() noexcept = default;

auto ThreadPool::operator=(ThreadPool&&) noexcept -> ThreadPool& = default;

auto ThreadPool::await() noexcept -> void { impl_->await(); }

auto ThreadPool::execute(std::function<void()> task) -> void {
  impl_->enqueue(std::move(task));
}

}  // namespace simulator::trading_system::runtime