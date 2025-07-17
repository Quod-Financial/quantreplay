#ifndef SIMULATOR_GENERATOR_TESTS_TEST_UTILS_GENERATED_MESSAGE_UTILS_HPP_
#define SIMULATOR_GENERATOR_TESTS_TEST_UTILS_GENERATED_MESSAGE_UTILS_HPP_

#include <gmock/gmock.h>

#include "core/domain/party.hpp"

namespace simulator::generator {

using namespace ::testing;

MATCHER_P(GeneratedParty, party_id, "") {
  return ExplainMatchResult(
             Eq(party_id), arg.party_id(), result_listener) &&
         ExplainMatchResult(Eq(PartyRole::Option::ExecutingFirm),
                            arg.role().value(),
                            result_listener) &&
         ExplainMatchResult(Eq(PartyIdSource::Option::Proprietary),
                            arg.source().value(),
                            result_listener);
}

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_TESTS_TEST_UTILS_GENERATED_MESSAGE_UTILS_HPP_
