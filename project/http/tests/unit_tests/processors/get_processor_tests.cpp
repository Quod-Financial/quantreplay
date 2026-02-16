#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/endpoint.hpp"
#include "ih/processors/get_processor.hpp"
#include "ih/router.hpp"
#include "middleware/channels/generator_admin_channel.hpp"
#include "middleware/routing/generator_admin_channel.hpp"
#include "mocks/delete_processor.hpp"
#include "mocks/generator_admin_receiver.hpp"
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

class HttpGetProcessor : public ::testing::Test {
 protected:
  auto SetUp() -> void override {
    auto put_processor = std::make_shared<mock::PutProcessor>();
    auto post_processor = std::make_shared<mock::PostProcessor>();
    auto delete_processor = std::make_shared<mock::DeleteProcessor>();

    venue_accessor = std::make_shared<NiceMock<http::mock::VenueAccessor>>();
    redirector = std::make_shared<mock::RedirectionProcessor>();

    get_processor = std::make_shared<GetProcessorImpl>(venue_accessor,
                                                       redirector,
                                                       nullptr,
                                                       nullptr,
                                                       nullptr,
                                                       nullptr,
                                                       nullptr,
                                                       VenueName);

    router = std::make_unique<Router>(get_processor,
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
  std::shared_ptr<GetProcessorImpl> get_processor;
  std::unique_ptr<Router> router;
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
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _))
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
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _))
      .Times(1)
      .WillOnce(Return(redirect::Result(Pistache::Http::Code::Ok)));

  const auto request = util::make_request(
      MethodName, endpoint::GenStatus + "/" + OtherVenueName);
  auto response_writer = util::make_response_writer(*router);
  router->onRequest(request, std::move(response_writer.writer));
}

}  // namespace
}  // namespace simulator::http::test
