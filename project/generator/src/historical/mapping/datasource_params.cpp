#include "ih/historical/mapping/datasource_params.hpp"

namespace simulator::generator::historical {

const DatasourceParams DatasourceParams::Postgres{
    data_layer::Datasource::Format::Postgres, std::nullopt};
const DatasourceParams DatasourceParams::CsvNoHeader{
    data_layer::Datasource::Format::Csv, false};
const DatasourceParams DatasourceParams::CsvHasHeader{
    data_layer::Datasource::Format::Csv, true};

}  // namespace simulator::generator::historical
