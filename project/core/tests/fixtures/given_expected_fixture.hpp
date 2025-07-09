#ifndef SIMULATOR_CORE_TESTS_FIXTURES_GIVEN_EXPECTED_FIXTURE_HPP_
#define SIMULATOR_CORE_TESTS_FIXTURES_GIVEN_EXPECTED_FIXTURE_HPP_

#include <gtest/gtest.h>

#include <utility>

namespace simulator::core::test {

template <typename GivenType, typename ExpectedType>
struct GivenExpectedFixture
    : public testing::TestWithParam<std::pair<GivenType, ExpectedType>> {
  auto given() const -> GivenType { return this->GetParam().first; }

  auto expected() const -> ExpectedType { return this->GetParam().second; }
};

}  // namespace simulator::core::test

#endif  // SIMULATOR_CORE_TESTS_FIXTURES_GIVEN_EXPECTED_FIXTURE_HPP_
