#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <pistache/http.h>

#include <memory>
#include <string>
#include <vector>

#include "core/common/session_settings.hpp"
#include "ih/processors/head_processor.hpp"
#include "ih/router.hpp"
#include "mocks/config_provider.hpp"
#include "mocks/delete_processor.hpp"
#include "mocks/get_processor.hpp"
#include "mocks/post_processor.hpp"
#include "mocks/put_processor.hpp"
#include "mocks/redirection_processor.hpp"
#include "test_utils/processors.hpp"

namespace simulator::http::test {
namespace {

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Eq;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;

class HttpHeadProcessor : public ::testing::Test {
 protected:
  auto SetUp() -> void override {
    redirector = std::make_shared<mock::RedirectionProcessor>();
    config_provider = std::make_shared<NiceMock<http::mock::ConfigProvider>>();

    ON_CALL(*config_provider, venue_id).WillByDefault(ReturnRef(VenueName));
    ON_CALL(*config_provider, session_settings)
        .WillByDefault(ReturnRef(session_settings));

    head_processor =
        std::make_shared<HeadProcessorImpl>(redirector, config_provider);

    router =
        std::make_unique<Router>(std::make_shared<mock::GetProcessor>(),
                                 head_processor,
                                 std::make_shared<mock::PostProcessor>(),
                                 std::make_shared<mock::PutProcessor>(),
                                 std::make_shared<mock::DeleteProcessor>());
  }

  auto head_request(const std::string& path) -> void {
    const auto request = util::make_request(MethodName, path);
    auto response_writer = util::make_response_writer(*router);
    router->onRequest(request, std::move(response_writer.writer));
  }

  static constexpr std::string MethodName{"HEAD"};
  static constexpr std::string VenueName{"current_venue"};
  static constexpr std::string OtherVenueName{"other_venue"};
  static constexpr std::string SessionId{"session-1"};

  std::shared_ptr<mock::RedirectionProcessor> redirector;
  std::shared_ptr<NiceMock<http::mock::ConfigProvider>> config_provider;
  std::shared_ptr<HeadProcessorImpl> head_processor;
  std::unique_ptr<Router> router;

  std::vector<core::FixSessionSettings> session_settings{
      core::FixSessionSettings{
          .heading = "SESSION", .id = SessionId, .settings = {}}};
};

class HttpHeadProcessorGetDataDictionaries : public HttpHeadProcessor {
 protected:
  static auto data_dictionaries_path(const std::string& venue_id,
                                     const std::string& session_id)
      -> std::string {
    return "/api/venues/" + venue_id + "/sessions/" + session_id +
           "/dataDictionaries";
  }
};

TEST_F(HttpHeadProcessorGetDataDictionaries,
       DoesNotRedirectRequestIfVenueIsCurrent) {
  EXPECT_CALL(*redirector, redirect_to_venue).Times(0);

  head_request(data_dictionaries_path(VenueName, SessionId));
}

TEST_F(HttpHeadProcessorGetDataDictionaries,
       RedirectsRequestIfVenueIsNotCurrent) {
  EXPECT_CALL(*redirector,
              redirect_to_venue(
                  Eq(OtherVenueName), Eq(Pistache::Http::Method::Head), _, _))
      .Times(1)
      .WillOnce(Return(redirect::Result{Pistache::Http::Code::Ok}));

  head_request(data_dictionaries_path(OtherVenueName, SessionId));
}

TEST_F(HttpHeadProcessorGetDataDictionaries,
       DecodesPercentEncodedVenueBeforeResolving) {
  EXPECT_CALL(*redirector, redirect_to_venue(Eq(OtherVenueName), _, _, _))
      .Times(1)
      .WillOnce(Return(redirect::Result{Pistache::Http::Code::Ok}));

  // "%6Fther_venue" decodes to "other_venue"
  head_request(data_dictionaries_path("%6Fther_venue", SessionId));
}

}  // namespace
}  // namespace simulator::http::test
