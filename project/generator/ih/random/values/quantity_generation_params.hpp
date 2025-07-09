#ifndef SIMULATOR_GENERATOR_IH_RANDOM_VALUES_QUANTITY_GENERATION_PARAMS_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_VALUES_QUANTITY_GENERATION_PARAMS_HPP_

namespace simulator::generator::random {

class QuantityGenerationParams {
  struct Params {
    double qty_multiplier{0};
    double min_qty{0.};
    double max_qty{0.};
  };

 public:
  class Builder {
    friend class QuantityGenerationParams;

   public:
    Builder() = default;

    auto set_quantity_multiplier(double multiplier) noexcept -> void;

    auto set_minimal_quantity(double min_quantity) noexcept -> void;

    auto set_maximal_quantity(double max_quantity) noexcept -> void;

   private:
    Params specified_params_;
  };

  QuantityGenerationParams() = delete;

  explicit QuantityGenerationParams(Builder builder);

  [[nodiscard]]
  auto get_multiplier() const noexcept -> double;

  [[nodiscard]]
  auto get_minimal_quantity() const noexcept -> double;

  [[nodiscard]]
  auto get_maximal_quantity() const noexcept -> double;

 private:
  static auto validate(const Params& qty_generation_params) -> void;

  Params params_;
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_VALUES_QUANTITY_GENERATION_PARAMS_HPP_
