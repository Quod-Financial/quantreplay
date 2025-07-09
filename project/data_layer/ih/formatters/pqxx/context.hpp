#ifndef SIMULATOR_DATA_LAYER_IH_FORMATTERS_PQXX_CONTEXT_HPP_
#define SIMULATOR_DATA_LAYER_IH_FORMATTERS_PQXX_CONTEXT_HPP_

#include <fmt/format.h>

#include "ih/common/queries/data_extractor.hpp"

template <>
struct fmt::formatter<simulator::data_layer::internal_pqxx::Context> {
  using Type = simulator::data_layer::internal_pqxx::Context;
  enum class Format : char { Connection = 'c', EntireContext = 'a' };

  constexpr auto parse(fmt::format_parse_context& context)
      -> decltype(context.begin()) {
    auto it = context.begin();
    const auto end = context.end();

    if (it != end && *it == static_cast<char>(Format::Connection)) {
      format_ = Format::Connection;
      it++;
    }
    if (it != end && *it == static_cast<char>(Format::EntireContext)) {
      format_ = Format::EntireContext;
      it++;
    }
    return it;
  }

  template <typename Context>
  auto format(const Type& pqxx_context, Context& fmt_context) const
      -> decltype(fmt_context.out()) {
    if (format_ == Format::Connection) {
      format_connection(pqxx_context, fmt_context);
    } else {
      format_context(pqxx_context, fmt_context);
    }
    return fmt_context.out();
  }

 private:
  template <typename Context>
  auto format_connection(const Type& pqxx_context, Context& fmt_context) const
      -> void {
    fmt::format_to(
        fmt_context.out(), "{}", pqxx_context.get_connection_string());
  }

  template <typename Context>
  auto format_context(const Type& pqxx_context, Context& fmt_context) const
      -> void {
    fmt::format_to(fmt_context.out(), "pqxx::Context={{ Connection=\"");
    format_connection(pqxx_context, fmt_context);
    fmt::format_to(fmt_context.out(), "\" }}");
  }

  Format format_{Format::EntireContext};
};

#endif  // SIMULATOR_DATA_LAYER_IH_FORMATTERS_PQXX_CONTEXT_HPP_
