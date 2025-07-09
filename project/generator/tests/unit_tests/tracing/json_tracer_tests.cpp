#include <gtest/gtest.h>

#include "ih/tracing/tracing.hpp"

namespace simulator::generator::trace::test {
namespace {

TEST(Generator_Trace_JsonTracer, Trace_NoSteps) {
  constexpr std::string_view expected_trace{R"({"algorithmTrace":[]})"};

  JsonTracer tracer{};
  const std::string actual_trace = TraceFormatter::format(std::move(tracer));

  EXPECT_EQ(actual_trace, expected_trace);
}

TEST(Generator_Trace_JsonTracer, Trace_SingleStep) {
  constexpr std::string_view expected_trace{
      R"({"algorithmTrace":[)"
      R"({"step":1,"action":"firstStep","input":[],"output":[]})"
      "]}"};

  JsonTracer tracer{};
  auto step = make_step(tracer, "firstStep");
  tracer.trace(std::move(step));
  const std::string actual_trace = TraceFormatter::format(std::move(tracer));

  EXPECT_EQ(actual_trace, expected_trace);
}

TEST(Generator_Trace_JsonTracer, Trace_MultipleSteps) {
  constexpr std::string_view expected_trace{
      R"({"algorithmTrace":[)"
      R"({"step":1,"action":"firstStep","input":[],"output":[]},)"
      R"({"step":2,"action":"secondStep","input":[],"output":[]},)"
      R"({"step":3,"action":"thirdStep","input":[],"output":[]})"
      "]}"};

  JsonTracer tracer{};

  auto step = make_step(tracer, "firstStep");
  tracer.trace(std::move(step));

  step = make_step(tracer, "secondStep");
  tracer.trace(std::move(step));

  step = make_step(tracer, "thirdStep");
  tracer.trace(std::move(step));

  const std::string actual_trace = TraceFormatter::format(std::move(tracer));

  EXPECT_EQ(actual_trace, expected_trace);
}

TEST(Generator_Trace_JsonTracer, Trace_Step_SingeInput) {
  constexpr int value = 12;

  constexpr std::string_view expected_trace{
      R"({"algorithmTrace":[)"
      R"({"step":1,"action":"firstStep",)"
      R"("input":[)"
      R"({"someValue":12,"comment":"a comment"})"
      "],"
      R"("output":[]})"
      "]}"};

  JsonTracer tracer{};
  auto step = make_step(tracer, "firstStep");
  step.trace_input(make_commented_value("someValue", value, "a comment"));
  tracer.trace(std::move(step));

  const std::string actual_trace = TraceFormatter::format(std::move(tracer));

  EXPECT_EQ(actual_trace, expected_trace);
}

TEST(Generator_Trace_JsonTracer, Trace_Step_MultipleInputs) {
  constexpr double double_value = 12.9;

  constexpr std::string_view expected_trace{
      R"({"algorithmTrace":[)"
      R"({"step":1,"action":"firstStep",)"
      R"("input":[)"
      R"({"doubleValue":12.9,"comment":"a comment"},)"
      R"({"booleanValue":true,"comment":"a comment to boolean"},)"
      R"({"stringViewValue":"string_view"})"
      "],"
      R"("output":[])"
      "}"
      "]}"};

  JsonTracer tracer{};
  auto step = make_step(tracer, "firstStep");
  step.trace_input(
      make_commented_value("doubleValue", double_value, "a comment"));
  step.trace_input(
      make_commented_value("booleanValue", true, "a comment to boolean"));
  step.trace_input(
      make_value("stringViewValue", std::string_view{"string_view"}));

  tracer.trace(std::move(step));

  const std::string actual_trace = TraceFormatter::format(std::move(tracer));
  EXPECT_EQ(actual_trace, expected_trace);
}

TEST(Generator_Trace_JsonTracer, Trace_Step_SingeOutput) {
  constexpr std::string_view expected_trace{
      R"({"algorithmTrace":[)"
      R"({"step":1,"action":"firstStep",)"
      R"("input":[],)"
      R"("output":[)"
      R"({"stringValue":"string","comment":"a comment to string"})"
      "]"
      "}"
      "]}"};

  JsonTracer tracer{};
  auto step = make_step(tracer, "firstStep");
  const std::string value = "string";
  step.trace_output(
      make_commented_value("stringValue", value, "a comment to string"));
  tracer.trace(std::move(step));

  const std::string actual_trace = TraceFormatter::format(std::move(tracer));
  EXPECT_EQ(actual_trace, expected_trace);
}

TEST(Generator_Trace_JsonTracer, Trace_Step_MultipleOutputs) {
  constexpr short short_value = 2;
  const std::string string_value = "string";

  constexpr std::string_view expected_trace{
      R"({"algorithmTrace":[)"
      R"({"step":1,"action":"firstStep",)"
      R"("input":[],)"
      R"("output":[)"
      R"({"stringValue":"string","comment":"a comment to string"},)"
      R"({"shortValue":2})"
      "]"
      "}"
      "]}"};

  JsonTracer tracer{};
  auto step = make_step(tracer, "firstStep");
  step.trace_output(
      make_commented_value("stringValue", string_value, "a comment to string"));
  step.trace_output(make_value("shortValue", short_value));
  tracer.trace(std::move(step));

  const std::string actual_trace = TraceFormatter::format(std::move(tracer));
  EXPECT_EQ(actual_trace, expected_trace);
}

}  // namespace
}  // namespace simulator::generator::trace::test