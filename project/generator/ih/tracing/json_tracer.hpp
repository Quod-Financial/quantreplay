#ifndef SIMULATOR_GENERATOR_IH_TRACING_JSON_TRACER_HPP_
#define SIMULATOR_GENERATOR_IH_TRACING_JSON_TRACER_HPP_

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "ih/tracing/trace_value.hpp"

namespace simulator::generator::trace {

class JsonTracer {
  template <typename T>
  friend class FormattingHelper;

  class TracedAlgorithm {
    friend class JsonTracer;

   public:
    TracedAlgorithm();

    auto take_allocator() -> decltype(auto) { return trace_.GetAllocator(); }

    [[nodiscard]] auto compose_algorithm_trace() -> rapidjson::Document&;

    auto add_step(rapidjson::Document& document) -> void;

   private:
    rapidjson::Document trace_;
    rapidjson::Document steps_;
  };

 public:
  class Step {
    friend class JsonTracer;

   public:
    Step() = delete;

    template <typename T>
    auto trace_input(const TraceValue<T>& entry) -> void;

    template <typename T>
    auto trace_output(const TraceValue<T>& entry) -> void;

   private:
    template <typename JsonAllocator>
    Step(std::string_view action, JsonAllocator& allocator);

    auto take_traced_input() noexcept -> rapidjson::Document&;

    auto take_traced_output() noexcept -> rapidjson::Document&;

    [[nodiscard]] auto get_action() const noexcept -> std::string_view;

    template <typename T>
    static auto trace_entry_into(rapidjson::Document& dest_doc,
                                 const TraceValue<T>& entry) -> void;

    rapidjson::Document input_trace_;
    rapidjson::Document output_trace_;

    std::string_view step_action_;
  };

  JsonTracer();

  auto make_step(std::string_view action) -> Step;

  auto trace(Step&& step) -> void;

 private:
  [[maybe_unused]] auto compose_trace() -> std::string;

  TracedAlgorithm traced_data_;
  std::size_t next_step_{0};
};

inline JsonTracer::TracedAlgorithm::TracedAlgorithm()
    : steps_{std::addressof(trace_.GetAllocator())} {
  trace_.SetObject();
  steps_.SetObject().SetArray();
}

inline auto JsonTracer::TracedAlgorithm::compose_algorithm_trace()
    -> rapidjson::Document& {
  assert(trace_.IsObject());
  trace_.AddMember("algorithmTrace", steps_, take_allocator());
  return trace_;
}

inline auto JsonTracer::TracedAlgorithm::add_step(rapidjson::Document& document)
    -> void {
  assert(steps_.IsArray());
  assert(document.IsObject());
  steps_.PushBack(document, take_allocator());
}

template <typename T>
auto JsonTracer::Step::trace_input(const TraceValue<T>& entry) -> void {
  trace_entry_into(input_trace_, entry);
}

template <typename T>
auto JsonTracer::Step::trace_output(const TraceValue<T>& entry) -> void {
  trace_entry_into(output_trace_, entry);
}

template <typename JsonAllocator>
JsonTracer::Step::Step(std::string_view action, JsonAllocator& allocator)
    : input_trace_{std::addressof(allocator)},
      output_trace_{std::addressof(allocator)},
      step_action_{action} {
  input_trace_.SetObject().SetArray();
  output_trace_.SetObject().SetArray();
}

inline auto JsonTracer::Step::take_traced_input() noexcept
    -> rapidjson::Document& {
  return input_trace_;
}

inline auto JsonTracer::Step::take_traced_output() noexcept
    -> rapidjson::Document& {
  return output_trace_;
}

inline auto JsonTracer::Step::get_action() const noexcept -> std::string_view {
  return step_action_;
}

template <typename T>
auto JsonTracer::Step::trace_entry_into(rapidjson::Document& dest_doc,
                                        const TraceValue<T>& entry) -> void {
  using Entry = TraceValue<T>;
  using rapidjson::SizeType;
  using rapidjson::Value;

  auto& allocator = dest_doc.GetAllocator();
  rapidjson::Document entry_json{std::addressof(allocator)};
  entry_json.SetObject();

  const std::string_view key = entry.get_key();
  assert(!key.empty());
  const auto key_size = static_cast<SizeType>(key.size());
  Value json_key{key.data(), key_size, allocator};

  Value json_value;
  if constexpr (Entry::holds_string() || Entry::holds_string_view()) {
    const auto& value = entry.get_value();
    const auto value_size = static_cast<SizeType>(value.size());
    if constexpr (Entry::holds_string()) {
      json_value = Value{value.c_str(), value_size, allocator};
    } else  // Entry holds string_view
    {
      json_value = Value{value.data(), value_size, allocator};
    }
  } else  // Entry holds arithmetic type
  {
    json_value = Value{entry.get_value()};
  }

  entry_json.AddMember(json_key, json_value, allocator);

  if (entry.holds_comment()) {
    const std::string_view comment = entry.get_comment();
    Value json_comment{
        comment.data(), static_cast<SizeType>(comment.size()), allocator};
    entry_json.AddMember("comment", json_comment, allocator);
  }

  assert(dest_doc.IsArray());
  dest_doc.PushBack(entry_json, allocator);
}

inline JsonTracer::JsonTracer() : traced_data_{TracedAlgorithm{}} {}

inline auto JsonTracer::make_step(std::string_view action) -> JsonTracer::Step {
  return JsonTracer::Step{action, traced_data_.take_allocator()};
}

inline auto JsonTracer::trace(JsonTracer::Step&& step) -> void {
  auto& allocator = traced_data_.take_allocator();

  rapidjson::Document step_doc{std::addressof(allocator)};
  step_doc.SetObject();

  const std::string_view action = step.get_action();
  rapidjson::Value action_json{action.data(),
                               static_cast<rapidjson::SizeType>(action.size()),
                               allocator};

  step_doc.AddMember("step", rapidjson::Value{++next_step_}, allocator);
  step_doc.AddMember("action", action_json, allocator);
  step_doc.AddMember("input", step.take_traced_input(), allocator);
  step_doc.AddMember("output", step.take_traced_output(), allocator);

  traced_data_.add_step(step_doc);
}

[[maybe_unused]] inline auto JsonTracer::compose_trace() -> std::string {
  auto& json_trace = traced_data_.compose_algorithm_trace();
  assert(json_trace.IsObject());

  rapidjson::StringBuffer buffer{};
  rapidjson::Writer<rapidjson::StringBuffer> writer{buffer};
  json_trace.Accept(writer);

  return buffer.GetString();
}

}  // namespace simulator::generator::trace

#endif  // SIMULATOR_GENERATOR_IH_TRACING_JSON_TRACER_HPP_
