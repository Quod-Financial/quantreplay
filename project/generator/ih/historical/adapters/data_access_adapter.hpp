#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_ADAPTERS_DATA_ACCESS_ADAPTER_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_ADAPTERS_DATA_ACCESS_ADAPTER_HPP_

#include <functional>
#include <memory>

#include "ih/historical/data/record.hpp"

namespace simulator::data_layer {

class Datasource;

}  // namespace simulator::data_layer

namespace simulator::generator::historical {

class DataAccessAdapter {
 public:
  using RecordVisitor = std::function<void(historical::Record)>;

  DataAccessAdapter() = default;

  DataAccessAdapter(const DataAccessAdapter&) = default;
  auto operator=(const DataAccessAdapter&) -> DataAccessAdapter& = default;

  DataAccessAdapter(DataAccessAdapter&&) = default;
  auto operator=(DataAccessAdapter&&) -> DataAccessAdapter& = default;

  virtual ~DataAccessAdapter() = default;

  auto accept(const RecordVisitor& visitor) noexcept -> void;

 private:
  [[nodiscard]]
  virtual auto has_next_record() const noexcept -> bool = 0;

  virtual auto parse_next_record(Record::Builder& builder) -> void = 0;
};

class DataAccessAdapterFactory {
 public:
  DataAccessAdapterFactory() = default;

  DataAccessAdapterFactory(const DataAccessAdapterFactory&) = default;
  auto operator=(const DataAccessAdapterFactory&)
      -> DataAccessAdapterFactory& = default;

  DataAccessAdapterFactory(DataAccessAdapterFactory&&) = default;
  auto operator=(DataAccessAdapterFactory&&)
      -> DataAccessAdapterFactory& = default;

  virtual ~DataAccessAdapterFactory() = default;

  [[nodiscard]]
  virtual auto create_data_adapter(const data_layer::Datasource& datasource)
      const -> std::unique_ptr<DataAccessAdapter> = 0;
};

class DataAccessAdapterFactoryImpl : public DataAccessAdapterFactory {
 public:
  [[nodiscard]]
  auto create_data_adapter(const data_layer::Datasource& datasource) const
      -> std::unique_ptr<DataAccessAdapter> override;

 private:
  static auto create_csv_reader(const data_layer::Datasource& datasource)
      -> std::unique_ptr<DataAccessAdapter>;

  static auto create_postgres_db_reader(
      const data_layer::Datasource& datasource)
      -> std::unique_ptr<DataAccessAdapter>;
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_ADAPTERS_DATA_ACCESS_ADAPTER_HPP_
