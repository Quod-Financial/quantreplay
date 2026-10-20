#include <gmock/gmock.h>
#include <quickfix/Fields.h>
#include <quickfix/Message.h>

#include "common/mapping/checks.hpp"

namespace simulator::fix::test {
namespace {

using namespace testing;  // NOLINT

struct FixMappingChecks : public Test {
  FIX::Message fix_message;

  template <typename FixFieldType>
  auto set_field(const FixFieldType& fix_field) -> void {
    fix_message.setField(fix_field);
  }
};

struct FixContainsCheck : public FixMappingChecks {};

TEST_F(FixContainsCheck, DetectsSingleFieldInMessage) {
  set_field(FIX::SenderCompID{"sender_comp_id"});

  const bool result = contains<FIX::FIELD::SenderCompID>(fix_message);

  ASSERT_THAT(result, IsTrue());
}

TEST_F(FixContainsCheck, DetectsMultipleFieldsInMessage) {
  set_field(FIX::ClOrdID{"cl-ord-id"});
  set_field(FIX::OrigClOrdID{"orig-cl-ord-id"});
  set_field(FIX::OrderID{"order-id"});

  const bool result = contains<FIX::FIELD::ClOrdID,
                               FIX::FIELD::OrigClOrdID,
                               FIX::FIELD::OrderID>(fix_message);

  ASSERT_THAT(result, IsTrue());
}

TEST_F(FixContainsCheck, ReportsFieldNotPresentInMessage) {
  const bool result = contains<FIX::FIELD::ClOrdID>(fix_message);

  ASSERT_THAT(result, IsFalse());
}

TEST_F(FixContainsCheck, ReportsOneFieldOfManyNotPresentInMessage) {
  set_field(FIX::ClOrdID{"cl-ord-id"});
  set_field(FIX::OrigClOrdID{"orig-cl-ord-id"});

  const bool result = contains<FIX::FIELD::ClOrdID,
                               FIX::FIELD::OrigClOrdID,
                               FIX::FIELD::OrderID>(fix_message);

  ASSERT_THAT(result, IsFalse());
}

TEST_F(FixContainsCheck, ReportsAllFieldsNotPresentInMessage) {
  const bool result = contains<FIX::FIELD::ClOrdID,
                               FIX::FIELD::OrigClOrdID,
                               FIX::FIELD::OrderID>(fix_message);

  ASSERT_THAT(result, IsFalse());
}

}  // namespace
}  // namespace simulator::fix::test