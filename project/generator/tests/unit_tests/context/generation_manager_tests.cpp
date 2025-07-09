#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <regex>
#include <string>
#include <string_view>

#include "cfg/api/cfg.hpp"
#include "ih/context/generation_manager.hpp"

namespace simulator::generator::test {
namespace {

class NoopCallback {
 public:
  MOCK_METHOD(void, call, ());
};

class Generator_IdentifierGenerator : public testing::Test {
 public:
  static auto parse_identifier(std::string_view generated_id,
                               std::size_t& number_part,
                               std::string& id_prefix) -> void {
    constexpr char delimiter = '-';
    const auto prefix_end_pos = generated_id.find(delimiter);
    ASSERT_TRUE(prefix_end_pos != std::string_view::npos);

    id_prefix = generated_id.substr(0, prefix_end_pos + 1);
    const auto id_number = std::string{generated_id.substr(prefix_end_pos + 1)};

    ASSERT_NO_THROW(number_part = std::stoull(id_number));
  }
};

class Generator_GenerationManager : public testing::Test {
 public:
  static auto make_manager(const std::string& venue_id)
      -> std::shared_ptr<GenerationManager> {
    data_layer::Venue::Patch patch;
    patch.with_venue_id(venue_id);
    return GenerationManager::create(
        data_layer::Venue::create(std::move(patch)));
  }

  static auto make_manager(bool order_on_startup_enabled)
      -> std::shared_ptr<GenerationManager> {
    static const std::string default_venue_id{"XNSE"};

    data_layer::Venue::Patch patch;
    patch.with_venue_id(default_venue_id)
        .with_orders_on_startup_flag(order_on_startup_enabled);
    return GenerationManager::create(
        data_layer::Venue::create(std::move(patch)));
  }
};

TEST(Generator_GenerationState, Suspended) {
  const GenerationState state{false};

  EXPECT_FALSE(state.is_running());
  EXPECT_FALSE(state.is_terminated());
}

TEST(Generator_GenerationState, Active) {
  const GenerationState state{true};

  EXPECT_TRUE(state.is_running());
  EXPECT_FALSE(state.is_terminated());
}

TEST(Generator_GenerationState, Change_SuspendedToActive) {
  GenerationState state{false};

  ASSERT_FALSE(state.is_running());
  ASSERT_FALSE(state.is_terminated());

  state.set_running();

  EXPECT_TRUE(state.is_running());
  EXPECT_FALSE(state.is_terminated());
}

TEST(Generator_GenerationState, Change_ActiveToSuspended) {
  GenerationState state{true};

  ASSERT_TRUE(state.is_running());
  ASSERT_FALSE(state.is_terminated());

  state.set_stopped();

  EXPECT_FALSE(state.is_running());
  EXPECT_FALSE(state.is_terminated());
}

TEST(Generator_GenerationState, Terminate_Suspended) {
  GenerationState state{false};

  ASSERT_FALSE(state.is_running());
  ASSERT_FALSE(state.is_terminated());

  state.set_terminated();

  EXPECT_FALSE(state.is_running());
  EXPECT_TRUE(state.is_terminated());
}

TEST(Generator_GenerationState, Terminate_Active) {
  GenerationState state{true};

  ASSERT_TRUE(state.is_running());
  ASSERT_FALSE(state.is_terminated());

  state.set_terminated();

  EXPECT_FALSE(state.is_running());
  EXPECT_TRUE(state.is_terminated());
}

TEST(Generator_GeneratedMessagesCounter, Increment) {
  constexpr std::size_t max_count = 5;
  GeneratedMessagesCounter counter{};

  for (std::size_t count = 1; count <= max_count; ++count) {
    EXPECT_EQ(counter.increment_order_messages_count(), count);
  }
}

TEST(Generator_GeneratedMessagesCounter, ConcurentIncrement) {
  constexpr std::size_t min_increment = 1;
  constexpr std::size_t max_increments = 10;

  GeneratedMessagesCounter counter{};
  const auto incrementor = [&]() -> std::size_t {
    return counter.increment_order_messages_count();
  };

  std::set<std::size_t> created_ids{};
  const auto increment_check = [&](std::size_t increment) {
    EXPECT_TRUE(min_increment <= increment);
    EXPECT_TRUE(increment <= max_increments);
    EXPECT_TRUE(created_ids.emplace(increment).second);
  };

  auto async_increment1 = std::async(std::launch::async, incrementor);
  auto async_increment2 = std::async(std::launch::async, incrementor);
  auto async_increment3 = std::async(std::launch::async, incrementor);
  auto async_increment4 = std::async(std::launch::async, incrementor);
  auto async_increment5 = std::async(std::launch::async, incrementor);
  auto async_increment6 = std::async(std::launch::async, incrementor);
  auto async_increment7 = std::async(std::launch::async, incrementor);
  auto async_increment8 = std::async(std::launch::async, incrementor);
  auto async_increment9 = std::async(std::launch::async, incrementor);
  auto async_increment10 = std::async(std::launch::async, incrementor);

  increment_check(async_increment1.get());
  increment_check(async_increment2.get());
  increment_check(async_increment3.get());
  increment_check(async_increment4.get());
  increment_check(async_increment5.get());
  increment_check(async_increment6.get());
  increment_check(async_increment7.get());
  increment_check(async_increment8.get());
  increment_check(async_increment9.get());
  increment_check(async_increment10.get());
}

TEST_F(Generator_IdentifierGenerator, CreateIdentifier) {
  using std::chrono::system_clock;

  const std::string expected_prefix{"SIM-"};
  const auto min_number =
      static_cast<std::size_t>(system_clock::now().time_since_epoch().count());

  IdentifierGenerator id_generator{};

  const std::string generated_id = id_generator.generate_identifier();
  EXPECT_FALSE(generated_id.empty());

  std::string prefix{};
  std::size_t number_part = 0;
  parse_identifier(generated_id, number_part, prefix);

  EXPECT_EQ(prefix, expected_prefix);
  EXPECT_TRUE(number_part > min_number);
}

TEST_F(Generator_IdentifierGenerator, CreateIdentifierConcurently) {
  using std::chrono::system_clock;

  const std::string expected_prefix{"SIM-"};
  const auto min_number =
      static_cast<std::size_t>(system_clock::now().time_since_epoch().count());

  IdentifierGenerator generator{};
  const auto generate_id = [&]() -> std::string {
    return generator.generate_identifier();
  };

  auto async_gen_id1 = std::async(std::launch::async, generate_id);
  auto async_gen_id2 = std::async(std::launch::async, generate_id);
  auto async_gen_id3 = std::async(std::launch::async, generate_id);
  auto async_gen_id4 = std::async(std::launch::async, generate_id);
  auto async_gen_id5 = std::async(std::launch::async, generate_id);
  auto async_gen_id6 = std::async(std::launch::async, generate_id);
  auto async_gen_id7 = std::async(std::launch::async, generate_id);
  auto async_gen_id8 = std::async(std::launch::async, generate_id);
  auto async_gen_id9 = std::async(std::launch::async, generate_id);
  auto async_gen_id10 = std::async(std::launch::async, generate_id);

  const auto max_number =
      static_cast<std::size_t>(system_clock::now().time_since_epoch().count());

  std::set<std::string> created_ids{};
  const auto check_id = [&](const std::string& id) {
    std::string prefix_part{};
    std::size_t id_number = 0;

    parse_identifier(id, id_number, prefix_part);
    EXPECT_TRUE(id_number > min_number);
    EXPECT_TRUE(id_number < max_number);
    EXPECT_EQ(prefix_part, expected_prefix);
    EXPECT_TRUE(created_ids.emplace(id).second);
  };

  check_id(async_gen_id1.get());
  check_id(async_gen_id2.get());
  check_id(async_gen_id3.get());
  check_id(async_gen_id4.get());
  check_id(async_gen_id5.get());
  check_id(async_gen_id6.get());
  check_id(async_gen_id7.get());
  check_id(async_gen_id8.get());
  check_id(async_gen_id9.get());
  check_id(async_gen_id10.get());
}

TEST_F(Generator_GenerationManager, Create) {
  const std::string venue_id{"XBOM"};
  const auto manager = make_manager(venue_id);
  ASSERT_TRUE(manager);

  EXPECT_EQ(manager->get_venue().venue_id(), venue_id);
}

TEST_F(Generator_GenerationManager, Create_OrderOnStartup_Disabled) {
  const auto manager = make_manager(false);
  ASSERT_TRUE(manager);

  EXPECT_FALSE(manager->is_component_running());
}

TEST_F(Generator_GenerationManager, Create_OrderOnStartup_Enabled) {
  const auto manager = make_manager(true);
  ASSERT_TRUE(manager);

  EXPECT_TRUE(manager->is_component_running());
}

TEST_F(Generator_GenerationManager, Generate_Order_MessageNumber) {
  const auto manager = make_manager("XNSE");
  ASSERT_TRUE(manager);

  EXPECT_EQ(manager->next_generated_order_message_number(), 1);
  EXPECT_EQ(manager->next_generated_order_message_number(), 2);
  EXPECT_EQ(manager->next_generated_order_message_number(), 3);
}

TEST_F(Generator_GenerationManager, Generate_Identifier) {
  // standard sim generated prefix 'SIM-'
  // and a 19-digit number (based on UNIX nanosecond time)
  const std::regex id_regex{"SIM-[0-9]{19}", std::regex::ECMAScript};
  const auto manager = make_manager("XLSE");
  ASSERT_TRUE(manager);

  EXPECT_TRUE(std::regex_match(manager->generate_identifier(), id_regex));
}

TEST_F(Generator_GenerationManager, Suspend_Suspended) {
  const auto manager = make_manager(false);
  ASSERT_TRUE(manager);
  ASSERT_FALSE(manager->is_component_running());

  manager->suspend();

  EXPECT_FALSE(manager->is_component_running());
}

TEST_F(Generator_GenerationManager, Suspend_Active) {
  const auto manager = make_manager(true);
  ASSERT_TRUE(manager);
  ASSERT_TRUE(manager->is_component_running());

  manager->suspend();

  EXPECT_FALSE(manager->is_component_running());
}

TEST_F(Generator_GenerationManager, Launch_Suspended) {
  const auto manager = make_manager(false);
  ASSERT_TRUE(manager);
  ASSERT_FALSE(manager->is_component_running());

  manager->launch();

  EXPECT_TRUE(manager->is_component_running());
}

TEST_F(Generator_GenerationManager, Launch_Active) {
  const auto manager = make_manager(true);
  ASSERT_TRUE(manager);
  ASSERT_TRUE(manager->is_component_running());

  manager->launch();

  EXPECT_TRUE(manager->is_component_running());
}

TEST_F(Generator_GenerationManager, Terminate_Suspended) {
  const auto manager = make_manager(false);
  ASSERT_TRUE(manager);
  ASSERT_FALSE(manager->is_component_running());

  manager->terminate();

  EXPECT_FALSE(manager->is_component_running());

  manager->launch();

  EXPECT_FALSE(manager->is_component_running());
}

TEST_F(Generator_GenerationManager, Terminate_Active) {
  const auto manager = make_manager(true);
  ASSERT_TRUE(manager);
  ASSERT_TRUE(manager->is_component_running());

  manager->terminate();

  EXPECT_FALSE(manager->is_component_running());

  manager->launch();

  EXPECT_FALSE(manager->is_component_running());
}

TEST_F(Generator_GenerationManager, CallOnLaunch_Suspended) {
  const auto manager = make_manager(false);
  ASSERT_TRUE(manager);
  ASSERT_FALSE(manager->is_component_running());

  NoopCallback callback{};
  EXPECT_CALL(callback, call).Times(1);

  manager->call_on_launch([&callback]() { callback.call(); });
  manager->launch();

  manager->suspend();
  manager->launch();
}

TEST_F(Generator_GenerationManager, CallOnLaunch_Active) {
  const auto manager = make_manager(true);
  ASSERT_TRUE(manager);
  ASSERT_TRUE(manager->is_component_running());

  NoopCallback callback{};
  EXPECT_CALL(callback, call).Times(1);

  manager->call_on_launch([&callback]() { callback.call(); });
  manager->suspend();
  manager->launch();

  manager->suspend();
  manager->launch();
}

TEST_F(Generator_GenerationManager, CallOnLaunch_Terminated) {
  const auto manager = make_manager(true);
  ASSERT_TRUE(manager);
  ASSERT_TRUE(manager->is_component_running());

  NoopCallback callback{};
  EXPECT_CALL(callback, call).Times(0);

  manager->call_on_launch([&callback]() { callback.call(); });
  manager->terminate();

  manager->launch();
}

}  // namespace
}  // namespace simulator::generator::test