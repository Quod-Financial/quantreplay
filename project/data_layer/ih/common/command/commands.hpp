#ifndef SIMULATOR_DATA_LAYER_IH_COMMON_COMMAND_COMMANDS_HPP_
#define SIMULATOR_DATA_LAYER_IH_COMMON_COMMAND_COMMANDS_HPP_

#include <optional>
#include <stdexcept>
#include <utility>

#include "api/predicate/expression.hpp"

namespace simulator::data_layer::command {

template <typename Model>
class Insert {
 public:
  using ModelType = Model;
  using PatchType = typename ModelType::Patch;

  Insert() = delete;

  static auto create(PatchType initial_patch) -> Insert {
    return Insert{std::move(initial_patch)};
  }

  [[nodiscard]]
  auto initial_patch() const noexcept -> const PatchType& {
    return initial_patch_;
  }

  [[nodiscard]]
  auto result() -> ModelType& {
    if (!result_.has_value()) {
      throw std::logic_error{
          "no result has been set into the `Insert' database "
          "command"};
    }
    return *result_;
  }

  auto set_result(ModelType result) -> void { result_ = std::move(result); }

 private:
  explicit Insert(PatchType initial_patch) noexcept
      : initial_patch_(std::move(initial_patch)) {}

  std::optional<ModelType> result_;
  PatchType initial_patch_;
};

template <typename Model>
class SelectOne {
 public:
  using ModelType = Model;
  using PredicateType = predicate::Expression<ModelType>;

  SelectOne() = delete;

  static auto create(PredicateType predicate) -> SelectOne {
    return SelectOne{std::move(predicate)};
  }

  [[nodiscard]]
  auto predicate() noexcept -> const PredicateType& {
    return predicate_;
  }

  [[nodiscard]]
  auto result() -> ModelType& {
    if (!result_.has_value()) {
      throw std::logic_error{
          "BUG: no result has been set into the `SelectOne` database "
          "command"};
    }
    return *result_;
  }

  auto set_result(ModelType result) -> void { result_ = std::move(result); }

 private:
  explicit SelectOne(PredicateType predicate) noexcept
      : predicate_(std::move(predicate)) {}

  PredicateType predicate_;
  std::optional<ModelType> result_;
};

template <typename Model>
class SelectAll {
 public:
  using ModelType = Model;
  using PredicateType = predicate::Expression<ModelType>;

  SelectAll() = delete;

  static auto create(std::optional<PredicateType> predicate) -> SelectAll {
    return SelectAll{std::move(predicate)};
  }

  [[nodiscard]]
  auto predicate() noexcept -> const std::optional<PredicateType>& {
    return predicate_;
  }

  [[nodiscard]]
  auto result() -> std::vector<ModelType>& {
    return result_;
  }

  auto set_result(std::vector<ModelType> result) -> void {
    result_ = std::move(result);
  }

 private:
  explicit SelectAll(std::optional<PredicateType> predicate) noexcept
      : predicate_(std::move(predicate)) {}

  std::optional<PredicateType> predicate_;
  std::vector<ModelType> result_;
};

template <typename Model>
class UpdateOne {
 public:
  using ModelType = Model;
  using PatchType = typename ModelType::Patch;
  using PredicateType = predicate::Expression<ModelType>;

  UpdateOne() = delete;

  [[nodiscard]]
  static auto create(PatchType patch, PredicateType predicate) -> UpdateOne {
    return UpdateOne{std::move(patch), std::move(predicate)};
  }

  auto patch() const noexcept -> const PatchType& { return patch_; }

  [[nodiscard]]
  auto predicate() noexcept -> const PredicateType& {
    return predicate_;
  }

  [[nodiscard]]
  auto result() -> ModelType& {
    if (!result_.has_value()) {
      throw std::logic_error{
          "no result has been set into the `UpdateOne` database "
          "command"};
    }
    return *result_;
  }

  auto set_result(ModelType result) -> void { result_ = std::move(result); }

 private:
  explicit UpdateOne(PatchType patch, PredicateType predicate) noexcept
      : patch_(std::move(patch)), predicate_(std::move(predicate)) {}

  PatchType patch_;
  PredicateType predicate_;
  std::optional<ModelType> result_;
};

template <typename Model>
class UpdateAll {
 public:
  using ModelType = Model;
  using PatchType = typename ModelType::Patch;
  using PredicateType = predicate::Expression<ModelType>;

  UpdateAll() = delete;

  [[nodiscard]]
  static auto create(PatchType patch, std::optional<PredicateType> predicate)
      -> UpdateAll {
    return UpdateAll{std::move(patch), std::move(predicate)};
  }

  auto patch() const noexcept -> const PatchType& { return patch_; }

  [[nodiscard]]
  auto predicate() noexcept -> const std::optional<PredicateType>& {
    return predicate_;
  }

  [[nodiscard]]
  auto result() -> std::vector<ModelType>& {
    return result_;
  }

  auto set_result(std::vector<ModelType> result) -> void {
    result_ = std::move(result);
  }

 private:
  explicit UpdateAll(PatchType patch,
                     std::optional<PredicateType> predicate) noexcept
      : patch_(std::move(patch)), predicate_(std::move(predicate)) {}

  PatchType patch_;
  std::optional<PredicateType> predicate_;
  std::vector<ModelType> result_;
};

template <typename Model>
class DeleteOne {
 public:
  using ModelType = Model;
  using PredicateType = predicate::Expression<ModelType>;

  DeleteOne() = delete;

  static auto create(PredicateType predicate) -> DeleteOne {
    return DeleteOne{std::move(predicate)};
  }

  [[nodiscard]]
  auto predicate() noexcept -> const PredicateType& {
    return predicate_;
  }

 private:
  explicit DeleteOne(PredicateType predicate) noexcept
      : predicate_(std::move(predicate)) {}

  PredicateType predicate_;
};

template <typename Model>
class DeleteAll {
 public:
  using ModelType = Model;
  using PredicateType = predicate::Expression<ModelType>;

  DeleteAll() = delete;

  static auto create(std::optional<PredicateType> predicate) -> DeleteAll {
    return DeleteAll{std::move(predicate)};
  }

  [[nodiscard]]
  auto predicate() noexcept -> const std::optional<PredicateType>& {
    return predicate_;
  }

 private:
  explicit DeleteAll(std::optional<PredicateType> predicate) noexcept
      : predicate_(std::move(predicate)) {}

  std::optional<PredicateType> predicate_;
};

}  // namespace simulator::data_layer::command

#endif  // SIMULATOR_DATA_LAYER_IH_COMMON_COMMAND_COMMANDS_HPP_
