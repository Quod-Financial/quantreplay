#ifndef SIMULATOR_LOG_TESTS_TEST_UTILS_RAPIDJSON_PRINTER_HPP_
#define SIMULATOR_LOG_TESTS_TEST_UTILS_RAPIDJSON_PRINTER_HPP_

#include <gmock/gmock.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace rapidjson {

inline auto PrintTo(const Document& document, std::ostream* os) -> void {
  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  document.Accept(writer);
  *os << buffer.GetString();
}

}  // namespace rapidjson

#endif  // SIMULATOR_LOG_TESTS_TEST_UTILS_RAPIDJSON_PRINTER_HPP_
