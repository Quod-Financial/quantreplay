#include <fmt/format.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "core/common/session_settings.hpp"
#include "ih/endpoint.hpp"
#include "ih/processors/get_processor.hpp"
#include "ih/router.hpp"
#include "middleware/channels/generator_admin_channel.hpp"
#include "middleware/routing/generator_admin_channel.hpp"
#include "mocks/config_provider.hpp"
#include "mocks/delete_processor.hpp"
#include "mocks/fix_session_controller.hpp"
#include "mocks/generator_admin_receiver.hpp"
#include "mocks/head_processor.hpp"
#include "mocks/post_processor.hpp"
#include "mocks/put_processor.hpp"
#include "mocks/redirection_processor.hpp"
#include "mocks/venue_accessor.hpp"
#include "test_utils/processors.hpp"

namespace simulator::http::test {
namespace {

using ::testing::_;
using ::testing::A;
using ::testing::Eq;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

class HttpGetProcessor : public ::testing::Test {
 protected:
  auto SetUp() -> void override {
    auto put_processor = std::make_shared<mock::PutProcessor>();
    auto post_processor = std::make_shared<mock::PostProcessor>();
    auto delete_processor = std::make_shared<mock::DeleteProcessor>();

    venue_accessor = std::make_shared<NiceMock<http::mock::VenueAccessor>>();
    redirector = std::make_shared<mock::RedirectionProcessor>();
    config_provider = std::make_shared<NiceMock<http::mock::ConfigProvider>>();
    fix_session_controller =
        std::make_shared<NiceMock<http::mock::FixSessionController>>();

    ON_CALL(*config_provider, venue_id).WillByDefault(ReturnRef(VenueName));
    ON_CALL(*config_provider, venue_start_time)
        .WillByDefault(ReturnRef(venue_start_time));
    ON_CALL(*config_provider, version).WillByDefault(ReturnRef(version));
    ON_CALL(*config_provider, session_settings)
        .WillByDefault(ReturnRef(session_settings));

    get_processor = std::make_shared<GetProcessorImpl>(venue_accessor,
                                                       redirector,
                                                       nullptr,
                                                       nullptr,
                                                       nullptr,
                                                       nullptr,
                                                       nullptr,
                                                       config_provider,
                                                       fix_session_controller);

    router = std::make_unique<Router>(get_processor,
                                      std::make_shared<mock::HeadProcessor>(),
                                      std::move(post_processor),
                                      std::move(put_processor),
                                      std::move(delete_processor));
  }

  auto create_venue(std::string venue_id) -> data_layer::Venue {
    return data_layer::Venue::create(
        data_layer::Venue::Patch{}.with_venue_id(venue_id));
  }

  static constexpr std::string MethodName{"GET"};
  static constexpr std::string VenueName{"current_venue"};
  static constexpr std::string OtherVenueName{"other_venue"};

  std::shared_ptr<NiceMock<http::mock::VenueAccessor>> venue_accessor;
  std::shared_ptr<mock::RedirectionProcessor> redirector;
  std::shared_ptr<NiceMock<http::mock::ConfigProvider>> config_provider;
  std::shared_ptr<NiceMock<http::mock::FixSessionController>>
      fix_session_controller;
  std::shared_ptr<GetProcessorImpl> get_processor;
  std::unique_ptr<Router> router;

  core::tz_us venue_start_time{std::chrono::microseconds(1773840208583000)};
  std::string version{"test-version"};
  std::vector<core::FixSessionSettings> session_settings{};
};

class HttpGetProcessorGetVenueStatus : public HttpGetProcessor {};

TEST_F(HttpGetProcessorGetVenueStatus, ChecksVenueFromRequest) {
  auto venue = create_venue(VenueName);

  EXPECT_CALL(*venue_accessor, select_single(Eq(VenueName)))
      .Times(1)
      .WillOnce(Return(venue));

  const auto request =
      util::make_request(MethodName, endpoint::VenueStatus + "/" + VenueName);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpGetProcessorGetVenueStatus, DoesNotRedirectRequestIfVenueIsCurrent) {
  auto venue = create_venue(VenueName);

  ON_CALL(*venue_accessor, select_single(Eq(VenueName)))
      .WillByDefault(Return(venue));
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);

  const auto request =
      util::make_request(MethodName, endpoint::VenueStatus + "/" + VenueName);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpGetProcessorGetVenueStatus, RedirectsRequestIfVenueIsNotCurrent) {
  auto venue = create_venue(OtherVenueName);

  ON_CALL(*venue_accessor, select_single(Eq(OtherVenueName)))
      .WillByDefault(Return(venue));
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _, _))
      .Times(1)
      .WillOnce(Return(redirect::Result(Pistache::Http::Code::Ok)));

  const auto request = util::make_request(
      MethodName, endpoint::VenueStatus + "/" + OtherVenueName);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpGetProcessorGetVenueStatus, DoesNotRedirectRequestIfVenueNotFound) {
  const std::string venue_id{"some_other_venue"};
  auto select_single_error =
      tl::unexpected{data_bridge::Failure::ResponseCardinalityError};

  ON_CALL(*venue_accessor, select_single(Eq(venue_id)))
      .WillByDefault(Return(select_single_error));
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);

  const auto request =
      util::make_request(MethodName, endpoint::VenueStatus + "/" + venue_id);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpGetProcessorGetVenueStatus,
       ChecksCurrentVenueIfRequestWithoutVenue) {
  auto venue = create_venue(VenueName);

  EXPECT_CALL(*venue_accessor, select_single(Eq(VenueName)))
      .Times(1)
      .WillOnce(Return(venue));
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);

  const auto request = util::make_request(MethodName, endpoint::VenueStatus);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

class HttpGetProcessorGetAllVenueStatus : public HttpGetProcessor {};

TEST_F(HttpGetProcessorGetAllVenueStatus,
       DoesNotRedirectRequestToCurrentVenue) {
  std::vector<data_layer::Venue> venues{create_venue(VenueName)};

  ON_CALL(*venue_accessor, select_all).WillByDefault(Return(venues));
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);

  const auto request = util::make_request(MethodName, endpoint::AllVenueStatus);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpGetProcessorGetAllVenueStatus, RedirectsRequestToOtherVenues) {
  std::vector<data_layer::Venue> venues = {create_venue(VenueName),
                                           create_venue("test_venue_name_2"),
                                           create_venue("test_venue_name_3")};

  ON_CALL(*venue_accessor, select_all).WillByDefault(Return(venues));
  EXPECT_CALL(*redirector, redirect_to_venue)
      .Times(2)
      .WillRepeatedly(Return(redirect::Result(Pistache::Http::Code::Ok)));

  const auto request = util::make_request(MethodName, endpoint::AllVenueStatus);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

class HttpGetProcessorGetOrderGenStatus : public HttpGetProcessor {
 protected:
  auto SetUp() -> void override {
    HttpGetProcessor::SetUp();

    std::shared_ptr<middleware::GeneratorAdminRequestReceiver> receiver_pointer{
        std::addressof(receiver), [](auto* /*pointer*/) {}};
    middleware::bind_generator_admin_channel(receiver_pointer);
  }

  auto TearDown() -> void override {
    middleware::release_generator_admin_channel();
  }

  mock::GeneratorAdminRequestReceiver receiver;
};

TEST_F(HttpGetProcessorGetOrderGenStatus, HandlesRequestForCurrentVenue) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::GenerationStatusRequest&>(),
                      A<protocol::GenerationStatusReply&>()))
      .Times(1);

  const auto request =
      util::make_request(MethodName, endpoint::GenStatus + "/" + VenueName);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpGetProcessorGetOrderGenStatus,
       HandlesRequestForCurrentVenueIfRequestWithoutVenue) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);
  EXPECT_CALL(receiver,
              process(A<const protocol::GenerationStatusRequest&>(),
                      A<protocol::GenerationStatusReply&>()))
      .Times(1);

  const auto request = util::make_request(MethodName, endpoint::GenStatus);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpGetProcessorGetOrderGenStatus, RedirectsWhenVenueIsDifferent) {
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _, _))
      .Times(1)
      .WillOnce(Return(redirect::Result(Pistache::Http::Code::Ok)));

  const auto request = util::make_request(
      MethodName, endpoint::GenStatus + "/" + OtherVenueName);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

class HttpGetProcessorGetDataDictionaries : public HttpGetProcessor {
 protected:
  static auto make_data_dictionaries_path(std::string_view venue,
                                          std::string_view session)
      -> std::string {
    return fmt::format(
        "{}/{}/sessions/{}/dataDictionaries", endpoint::Venues, venue, session);
  }

  static constexpr std::string SessionId{"session_1"};

  std::vector<core::FixSessionSettings> sessions_with_match{
      core::FixSessionSettings{
          .heading = "SESSION",
          .id = SessionId,
          .settings = {{"DATADICTIONARY", "nonexistent_dictionary.xml"}}}};
};

TEST_F(HttpGetProcessorGetDataDictionaries,
       RedirectsRequestIfVenueIsNotCurrent) {
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _, _))
      .Times(1)
      .WillOnce(Return(redirect::Result(Pistache::Http::Code::Ok)));

  const auto request = util::make_request(
      MethodName, make_data_dictionaries_path(OtherVenueName, SessionId));
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpGetProcessorGetDataDictionaries,
       DoesNotRedirectRequestIfVenueIsCurrent) {
  EXPECT_CALL(*config_provider, session_settings)
      .WillRepeatedly(ReturnRef(session_settings));
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);

  const auto request = util::make_request(
      MethodName, make_data_dictionaries_path(VenueName, SessionId));
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpGetProcessorGetDataDictionaries,
       DecodesUrlEncodedVenueIdBeforeRedirect) {
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _, _))
      .Times(1)
      .WillOnce(Return(redirect::Result(Pistache::Http::Code::Ok)));

  const auto request = util::make_request(
      MethodName, make_data_dictionaries_path("other%5Fvenue", SessionId));
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

TEST_F(HttpGetProcessorGetDataDictionaries,
       HandlesResolvableSessionForCurrentVenueWithoutPropagatingErrors) {
  EXPECT_CALL(*config_provider, session_settings)
      .WillRepeatedly(ReturnRef(sessions_with_match));
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);

  const auto request = util::make_request(
      MethodName, make_data_dictionaries_path(VenueName, SessionId));
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

}  // namespace
}  // namespace simulator::http::test
