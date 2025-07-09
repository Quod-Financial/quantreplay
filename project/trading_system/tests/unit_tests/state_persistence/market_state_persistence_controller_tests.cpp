#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "core/common/return_code.hpp"
#include "ih/config/config.hpp"
#include "ih/state_persistence/market_state_persistence_controller.hpp"
#include "tests/mocks/executor_mock.hpp"
#include "tests/mocks/serializer_mock.hpp"

namespace simulator::trading_system::test {
namespace {

using namespace ::testing;  // NOLINT

struct TradingSystemMarketStatePersistenceController : public Test {
  Config config;
  NiceMock<ExecutorMock> executor;
  std::unique_ptr<NiceMock<SerializerMock>> serializer =
      std::make_unique<NiceMock<SerializerMock>>();
};

TEST_F(TradingSystemMarketStatePersistenceController,
       StoreReturnsPersistenceDisabledIfPersistenceIsDisabled) {
  config.set_persistence(false);

  MarketStatePersistenceController controller{
      config, executor, std::move(serializer), {}, {}};
  const auto reply = controller.store();
  ASSERT_EQ(reply, core::code::StoreMarketState::PersistenceDisabled);
}

TEST_F(TradingSystemMarketStatePersistenceController,
       StoreReturnsPersistenceFilePathIsEmptyIfPathIsEmpty) {
  config.set_persistence(true);
  config.set_persistence_file_path({});

  MarketStatePersistenceController controller{
      config, executor, std::move(serializer), {}, {}};
  const auto reply = controller.store();
  ASSERT_EQ(reply, core::code::StoreMarketState::PersistenceFilePathIsEmpty);
}

TEST_F(TradingSystemMarketStatePersistenceController,
       StoreReturnsPersistenceFilePathUnreachableIfPathDoesNotExist) {
  config.set_persistence(true);
  config.set_persistence_file_path("not_existed_directory/market_state.json");

  MarketStatePersistenceController controller{
      config, executor, std::move(serializer), {}, {}};
  const auto reply = controller.store();
  ASSERT_EQ(reply,
            core::code::StoreMarketState::PersistenceFilePathIsUnreachable);
}

TEST_F(TradingSystemMarketStatePersistenceController,
       StoreCallsExecutorStoreStateRequestWithInstruments) {
  config.set_persistence(true);
  config.set_persistence_file_path("file_name");

  Instrument instrument;
  instrument.symbol = Symbol{"Symbol"};
  std::vector<Instrument> instruments{instrument};

  market_state::InstrumentState instrument_state;
  instrument_state.instrument = instrument;

  EXPECT_CALL(executor, store_state_request(ElementsAre(instrument_state)));

  ON_CALL(*serializer, serialize(_, _)).WillByDefault(Return(true));

  MarketStatePersistenceController controller{
      config, executor, std::move(serializer), {}, std::move(instruments)};

  controller.store();
}

TEST_F(TradingSystemMarketStatePersistenceController,
       StoreSetsVenueIdInSnapshot) {
  config.set_persistence(true);
  config.set_persistence_file_path("file_name");

  auto strict_serializer = std::make_unique<StrictMock<SerializerMock>>();

  EXPECT_CALL(
      *strict_serializer,
      serialize(Field(&market_state::Snapshot::venue_id, Eq("Venue")), _))
      .WillRepeatedly(Return(true));

  MarketStatePersistenceController controller{
      config, executor, std::move(strict_serializer), "Venue", {}};

  controller.store();
}

TEST_F(TradingSystemMarketStatePersistenceController,
       StoreReturnsErrorCodeWhenSerializerReturnsFalse) {
  config.set_persistence(true);
  config.set_persistence_file_path("file_name");

  EXPECT_CALL(*serializer, serialize(_, _)).WillOnce(Return(false));

  MarketStatePersistenceController controller{
      config, executor, std::move(serializer), {}, {}};

  ASSERT_EQ(controller.store(),
            core::code::StoreMarketState::ErrorWhenWritingToPersistenceFile);
}

TEST_F(TradingSystemMarketStatePersistenceController,
       StoreReturnsStoredCodeWhenSerializerReturnsTrue) {
  config.set_persistence(true);
  config.set_persistence_file_path("file_name");

  EXPECT_CALL(*serializer, serialize(_, _)).WillOnce(Return(true));

  MarketStatePersistenceController controller{
      config, executor, std::move(serializer), {}, {}};

  ASSERT_EQ(controller.store(), core::code::StoreMarketState::Stored);
}

TEST_F(TradingSystemMarketStatePersistenceController,
       RecoverReturnsPersistenceDisabledIfPersistenceIsDisabled) {
  config.set_persistence(false);

  MarketStatePersistenceController controller{
      config, executor, std::move(serializer), {}, {}};
  const auto result = controller.recover();
  ASSERT_EQ(result.code, core::code::RecoverMarketState::PersistenceDisabled);
}

TEST_F(TradingSystemMarketStatePersistenceController,
       RecoverReturnsPersistenceFilePathIsEmptyIfPathIsEmpty) {
  config.set_persistence(true);
  config.set_persistence_file_path({});

  MarketStatePersistenceController controller{
      config, executor, std::move(serializer), {}, {}};
  const auto result = controller.recover();
  ASSERT_EQ(result.code,
            core::code::RecoverMarketState::PersistenceFilePathIsEmpty);
}

TEST_F(TradingSystemMarketStatePersistenceController,
       RecoverReturnsPersistenceFilePathUnreachableIfPathDoesNotExist) {
  config.set_persistence(true);
  config.set_persistence_file_path("not_existed_directory/market_state.json");

  MarketStatePersistenceController controller{
      config, executor, std::move(serializer), {}, {}};
  const auto result = controller.recover();
  ASSERT_EQ(result.code,
            core::code::RecoverMarketState::PersistenceFilePathIsUnreachable);
}

TEST_F(TradingSystemMarketStatePersistenceController,
       RecoverReturnsPersistenceFilePathUnreachableIfPathIsDirectory) {
  config.set_persistence(true);
  config.set_persistence_file_path(
      std::filesystem::temp_directory_path().string());

  MarketStatePersistenceController controller{
      config, executor, std::move(serializer), {}, {}};
  const auto result = controller.recover();
  ASSERT_EQ(result.code,
            core::code::RecoverMarketState::PersistenceFilePathIsUnreachable);
}

struct TradingSystemMarketStatePersistenceControllerRealFile
    : public TradingSystemMarketStatePersistenceController {
  auto SetUp() -> void override {
    config.set_persistence(true);
    config.set_persistence_file_path(output_file_path.string());

    std::ofstream ofs{output_file_path};
  }

  auto TearDown() -> void override {
    std::filesystem::remove(output_file_path);
  }

  const std::filesystem::path output_file_path{
      std::filesystem::temp_directory_path() /
      UnitTest::GetInstance()->current_test_info()->name()};
};

TEST_F(TradingSystemMarketStatePersistenceControllerRealFile,
       RecoverReturnsPersistenceFileIsMalformed) {
  EXPECT_CALL(*serializer, deserialize(_))
      .WillOnce(Return(tl::unexpected<std::string>{""}));

  MarketStatePersistenceController controller{
      config, executor, std::move(serializer), {}, {}};
  const auto result = controller.recover();
  ASSERT_EQ(result.code,
            core::code::RecoverMarketState::PersistenceFileIsMalformed);
}

TEST_F(TradingSystemMarketStatePersistenceControllerRealFile,
       RecoverSetsErrorMessageOnPersistenceFileIsMalformed) {
  EXPECT_CALL(*serializer, deserialize(_))
      .WillOnce(Return(tl::unexpected<std::string>{"Malformed file"}));

  MarketStatePersistenceController controller{
      config, executor, std::move(serializer), {}, {}};
  const auto result = controller.recover();
  ASSERT_EQ(result.error_message, "Malformed file");
}

TEST_F(TradingSystemMarketStatePersistenceControllerRealFile,
       RecoverCallsExecutorRecoverStateRequestWithInstruments) {
  market_state::InstrumentState state1;
  state1.instrument.symbol = Symbol{"Symbol1"};
  market_state::InstrumentState state2;
  state2.instrument.symbol = Symbol{"Symbol2"};
  market_state::Snapshot snapshot;
  snapshot.instruments = {state1, state2};

  ON_CALL(*serializer, deserialize(_)).WillByDefault(Return(snapshot));

  EXPECT_CALL(executor, recover_state_request(ElementsAre(state1, state2)));

  MarketStatePersistenceController controller{
      config, executor, std::move(serializer), {}, {}};
  controller.recover();
}

TEST_F(TradingSystemMarketStatePersistenceControllerRealFile,
       RecoverReturnsRecoveredCodeWhenDeserializationIsSuccessful) {
  EXPECT_CALL(*serializer, deserialize(_))
      .WillOnce(Return(market_state::Snapshot{}));

  MarketStatePersistenceController controller{
      config, executor, std::move(serializer), {}, {}};
  const auto result = controller.recover();

  ASSERT_EQ(result.code, core::code::RecoverMarketState::Recovered);
}

}  // namespace
}  // namespace simulator::trading_system::test
