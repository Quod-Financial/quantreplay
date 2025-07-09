#include "ih/historical/data/provider.hpp"

#include <cassert>
#include <memory>
#include <optional>
#include <utility>

#include "data_layer/api/models/datasource.hpp"
#include "ih/historical/adapters/data_access_adapter.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/data/time.hpp"
#include "log/logging.hpp"

namespace simulator::generator::historical {

auto DataProvider::prepare(historical::DataAccessAdapter& adapter)
    -> std::size_t {
  std::size_t records_accepted = 0;
  adapter.accept([this, &records_accepted](historical::Record record) {
    add(std::move(record));
    ++records_accepted;
  });
  return records_accepted;
}

auto DataProvider::pull_action(const DataProvider::ActionPuller& puller)
    -> void {
  if (is_empty()) {
    throw std::logic_error{
        "unable to pull action from data provider - "
        "data provider has no data to provide"};
  }

  if (!has_time_offset()) {
    initialize_time_offset();
  }

  Action::Builder pulled_action_builder{};
  pull_into(pulled_action_builder);

  auto action = Action::Builder::construct(std::move(pulled_action_builder));
  puller(std::move(action));
}

auto DataProvider::has_time_offset() const noexcept -> bool {
  return opt_time_offset_.has_value();
}

auto DataProvider::get_time_offset() const noexcept -> historical::Duration {
  return opt_time_offset_.value_or(historical::Duration{0});
}

auto DataProvider::set_time_offset(historical::Duration offset) noexcept
    -> void {
  opt_time_offset_ = offset;
}

auto FiniteProvider::is_empty() const noexcept -> bool {
  return records_.empty();
}

auto FiniteProvider::initialize_time_offset() noexcept -> void {
  if (is_empty()) {
    return;
  }

  assert(!records_.empty());
  const auto& next_rec_time = records_.front().receive_time();
  const auto time_offset = historical::time::make_offset(next_rec_time);
  DataProvider::set_time_offset(time_offset);
}

auto FiniteProvider::add(historical::Record record) -> void {
  records_.emplace_back(std::move(record));
}

auto FiniteProvider::pull_into(
    historical::Action::Builder& pulled_action_builder) -> void {
  // Should be checked in parent template method
  assert(has_time_offset());
  assert(!records_.empty());

  const auto time_offset = get_time_offset();
  std::optional<historical::Timepoint> prev_rec_time{};
  while (!records_.empty()) {
    const auto next_rec_time = records_.front().receive_time();
    if (prev_rec_time.value_or(next_rec_time) != next_rec_time) {
      break;
    }

    prev_rec_time = next_rec_time;
    auto next_rec = std::move(records_.front());
    records_.pop_front();

    pulled_action_builder.add(std::move(next_rec), time_offset);
  }
}

auto RepeatingProvider::is_empty() const noexcept -> bool {
  return records_.empty() && processed_records_.empty();
}

auto RepeatingProvider::initialize_time_offset() noexcept -> void {
  if (is_empty()) {
    return;
  }

  if (records_.empty()) {
    assert(!processed_records_.empty());
    std::swap(records_, processed_records_);
  }

  assert(!records_.empty());
  const auto& next_rec_time = records_.front().receive_time();
  const auto time_offset = historical::time::make_offset(next_rec_time);
  DataProvider::set_time_offset(time_offset);
}

auto RepeatingProvider::add(historical::Record record) -> void {
  records_.emplace_back(std::move(record));
}

auto RepeatingProvider::pull_into(
    historical::Action::Builder& pulled_action_builder) -> void {
  // Should be checked in parent template method
  assert(!is_empty());

  if (records_.empty()) {
    assert(!processed_records_.empty());
    std::swap(records_, processed_records_);
    initialize_time_offset();
  }

  assert(has_time_offset());
  assert(!records_.empty());

  const auto time_offset = get_time_offset();
  std::optional<historical::Timepoint> prev_rec_time{};
  while (!records_.empty()) {
    const auto next_rec_time = records_.front().receive_time();
    if (prev_rec_time.value_or(next_rec_time) != next_rec_time) {
      break;
    }

    prev_rec_time = next_rec_time;

    auto next_rec = std::move(records_.front());
    records_.pop_front();
    processed_records_.push_back(next_rec);

    pulled_action_builder.add(std::move(next_rec), time_offset);
  }
}

DataProvidersFactoryImpl::DataProvidersFactoryImpl()
    : DataProvidersFactoryImpl(
          std::make_unique<DataAccessAdapterFactoryImpl>()) {}

DataProvidersFactoryImpl::DataProvidersFactoryImpl(
    std::unique_ptr<DataAccessAdapterFactory> data_adapters_factory) noexcept
    : data_adapter_factory{std::move(data_adapters_factory)} {
  assert(data_adapter_factory);
}

auto DataProvidersFactoryImpl::create_provider(
    const data_layer::Datasource& datasource) const
    -> std::unique_ptr<DataProvider> {
  auto data_adapter = data_adapter_factory->create_data_adapter(datasource);
  auto data_provider = [&]() -> std::unique_ptr<DataProvider> {
    if (datasource.repeat_flag().value_or(false)) {
      return std::make_unique<RepeatingProvider>();
    }
    return std::make_unique<FiniteProvider>();
  }();

  assert(data_adapter);
  assert(data_provider);

  const std::size_t records_read = data_provider->prepare(*data_adapter);

  log::info(
      "created a data provider for a `{}' datasource (DatasourceID: "
      "{} connection: {}) with {} historical records prepared",
      datasource.name(),
      datasource.datasource_id(),
      datasource.connection(),
      records_read);

  return data_provider;
}

}  // namespace simulator::generator::historical
