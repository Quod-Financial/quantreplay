#ifndef SIMULATOR_FIX_COMMON_META_HPP_
#define SIMULATOR_FIX_COMMON_META_HPP_

#include <concepts>

namespace simulator::fix {

template <typename Mapper, typename SourceMessage, typename TargetMessage>
concept RequestMessageMapper = requires(
    const Mapper& mapper, const SourceMessage& source, TargetMessage& target) {
  { mapper.map(source, target) } -> std::same_as<void>;
};

template <typename Mapper,
          typename SourceMessage,
          typename TargetMessage,
          typename Setting>
concept ReplyMessageMapper = requires(const Mapper& mapper,
                                      const SourceMessage& source,
                                      TargetMessage& target,
                                      const Setting& settings) {
  { mapper.map(source, target, settings) } -> std::same_as<void>;
};

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_COMMON_META_HPP_