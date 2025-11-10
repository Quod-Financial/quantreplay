#include <gmock/gmock.h>

#include <optional>

#include "http.hpp"
#include "ih/controllers/app_controller.hpp"
#include "tests/mocks/venue_accessor.hpp"

namespace simulator::http::test {
namespace {

using namespace testing;  // NOLINT

struct HttpAppControllerTest : Test {
  static auto make_venue(const std::string& venue_id,
                         std::optional<std::uint16_t> rest_port)
      -> data_layer::Venue {
    if (rest_port.has_value()) {
      return data_layer::Venue::create(
          data_layer::Venue::Patch{}.with_venue_id(venue_id).with_rest_port(
              *rest_port));
    }
    return data_layer::Venue::create(
        data_layer::Venue::Patch{}.with_venue_id(venue_id));
  }

  mock::VenueAccessor venue_accessor;
  cfg::VenueConfiguration venue_cfg{.name = "TEST_VENUE", .start_time = {}};
};

TEST_F(HttpAppControllerTest, ConstructorThrowsExceptionIfVenueNotFound) {
  const mock::VenueAccessor::VenueResult error_reply{
      tl::unexpected{data_bridge::Failure::ResponseCardinalityError}};

  EXPECT_CALL(venue_accessor, select_single(Eq(venue_cfg.name)))
      .Times(1)
      .WillOnce(Return(error_reply));

  ASSERT_THROW(
      (AppController{venue_accessor, venue_cfg, ControlCallbacks{[] {}}}),
      std::runtime_error);
}

TEST_F(HttpAppControllerTest, ConstructorThrowsExceptionIfVenueRestPortIsNull) {
  const mock::VenueAccessor::VenueResult successful_reply{
      make_venue(venue_cfg.name, std::nullopt)};

  EXPECT_CALL(venue_accessor, select_single(Eq(venue_cfg.name)))
      .Times(1)
      .WillOnce(Return(successful_reply));

  ASSERT_THROW(
      (AppController{venue_accessor, venue_cfg, ControlCallbacks{[] {}}}),
      std::runtime_error);
}

TEST_F(HttpAppControllerTest,
       ConstructorThrowsExceptionIfCallbackResetAppIsNotSet) {
  const mock::VenueAccessor::VenueResult successful_reply{
      make_venue(venue_cfg.name, 1234)};

  EXPECT_CALL(venue_accessor, select_single(Eq(venue_cfg.name)))
      .Times(1)
      .WillOnce(Return(successful_reply));

  ASSERT_THROW((AppController{venue_accessor, venue_cfg, ControlCallbacks{}}),
               std::runtime_error);
}

TEST_F(HttpAppControllerTest, ConstructorNoThrowsException) {
  const mock::VenueAccessor::VenueResult accessor_reply{
      make_venue(venue_cfg.name, 1234)};

  EXPECT_CALL(venue_accessor, select_single(Eq(venue_cfg.name)))
      .Times(1)
      .WillOnce(Return(accessor_reply));

  ASSERT_NO_THROW(
      (AppController{venue_accessor, venue_cfg, ControlCallbacks{[] {}}}));
}

struct HttpAppControllerReadyToReset : HttpAppControllerTest {};

TEST_F(HttpAppControllerReadyToReset, ReturnsConflictIfVenueIDIsNotFoundInDB) {
  const mock::VenueAccessor::VenueResult constructor_successful_reply{
      make_venue("LSE", 1234)};

  const mock::VenueAccessor::VenueResult reset_app_error_reply{
      tl::unexpected{data_bridge::Failure::ResponseCardinalityError}};

  EXPECT_CALL(venue_accessor, select_single(Eq("LSE")))
      .Times(2)
      .WillOnce(Return(constructor_successful_reply))
      .WillOnce(Return(reset_app_error_reply));

  venue_cfg.name = "LSE";
  const AppController controller{
      venue_accessor, venue_cfg, ControlCallbacks{[] {}}};

  auto [code, body] = controller.ready_to_reset();
  ASSERT_EQ(code, Pistache::Http::Code::Conflict);
  ASSERT_EQ(body,
            "{\"result\":\"Unable to reset the venue state. The venue id `LSE' "
            "is not found. It may have been modified or the venue deleted. "
            "Please check the database.\"}");
}

TEST_F(HttpAppControllerReadyToReset, ReturnsConflictIfVenueRestPortIsNull) {
  const mock::VenueAccessor::VenueResult constructor_successful_reply{
      make_venue("LSE", 1234)};

  const mock::VenueAccessor::VenueResult reset_app_successful_reply{
      make_venue("LSE", std::nullopt)};

  EXPECT_CALL(venue_accessor, select_single(Eq("LSE")))
      .Times(2)
      .WillOnce(Return(constructor_successful_reply))
      .WillOnce(Return(reset_app_successful_reply));

  venue_cfg.name = "LSE";
  const AppController controller{
      venue_accessor, venue_cfg, ControlCallbacks{[] {}}};

  auto [code, body] = controller.ready_to_reset();
  ASSERT_EQ(code, Pistache::Http::Code::Conflict);
  ASSERT_EQ(body,
            "{\"result\":\"Unable to reset the venue state. The venue `LSE' "
            "restPort is not set in the database. It may have been deleted. "
            "Please check the database.\"}");
}

TEST_F(HttpAppControllerReadyToReset,
       ReturnsConflictIfVenueRestPortWasChanged) {
  const mock::VenueAccessor::VenueResult constructor_successful_reply{
      make_venue("LSE", 1234)};

  const mock::VenueAccessor::VenueResult reset_app_successful_reply{
      make_venue("LSE", 2345)};

  EXPECT_CALL(venue_accessor, select_single(Eq("LSE")))
      .Times(2)
      .WillOnce(Return(constructor_successful_reply))
      .WillOnce(Return(reset_app_successful_reply));

  venue_cfg.name = "LSE";
  const AppController controller{
      venue_accessor, venue_cfg, ControlCallbacks{[] {}}};

  auto [code, body] = controller.ready_to_reset();
  ASSERT_EQ(code, Pistache::Http::Code::Conflict);
  ASSERT_EQ(body,
            "{\"result\":\"Unable to reset the venue state. The venue `LSE' "
            "restPort has been modified in the database. It was `1234' at the "
            "simulator launch. Please check the database.\"}");
}

TEST_F(HttpAppControllerReadyToReset,
       ReturnsOKIfVenueIDAndRestPortWasNotChanged) {
  const mock::VenueAccessor::VenueResult constructor_successful_reply{
      make_venue("LSE", 1234)};

  const mock::VenueAccessor::VenueResult reset_app_successful_reply =
      constructor_successful_reply;

  EXPECT_CALL(venue_accessor, select_single(Eq("LSE")))
      .Times(2)
      .WillOnce(Return(constructor_successful_reply))
      .WillOnce(Return(reset_app_successful_reply));

  venue_cfg.name = "LSE";
  const AppController controller{
      venue_accessor, venue_cfg, ControlCallbacks{[] {}}};

  auto [code, body] = controller.ready_to_reset();
  ASSERT_EQ(code, Pistache::Http::Code::Ok);
  ASSERT_EQ(body, "{\"result\":\"Resetting the venue state starts.\"}");
}

struct HttpAppControllerResetAppState : HttpAppControllerTest {
  MockFunction<void()> mock_reset_app;
};

TEST_F(HttpAppControllerResetAppState,
       CallsCallbackResetAppIfVenueIDAndRestPortWasNotChanged) {
  const mock::VenueAccessor::VenueResult constructor_successful_reply{
      make_venue("LSE", 1234)};

  EXPECT_CALL(venue_accessor, select_single(Eq("LSE")))
      .Times(1)
      .WillOnce(Return(constructor_successful_reply));

  EXPECT_CALL(mock_reset_app, Call).Times(1);

  venue_cfg.name = "LSE";
  const AppController controller{
      venue_accessor,
      venue_cfg,
      ControlCallbacks{mock_reset_app.AsStdFunction()}};

  controller.reset_app_state();
}

}  // namespace
}  // namespace simulator::http::test
