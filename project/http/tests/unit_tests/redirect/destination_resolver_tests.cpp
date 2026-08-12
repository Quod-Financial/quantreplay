#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <tl/expected.hpp>

#include "data_layer/api/models/venue.hpp"
#include "ih/redirect/destination_resolver.hpp"
#include "ih/redirect/resolver.hpp"
#include "mocks/venue_accessor.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::http::redirect::test {
namespace {

using namespace ::testing;

class HttpRedirectDestinationResolver : public testing::Test {
 public:
  auto resolve(const std::string& venue_id)
      -> tl::expected<Destination, Resolver::Error> {
    return make_resolver().resolve_by_venue_id(venue_id);
  }

  auto resolve_hostname_as_venue_id(bool value) -> void {
    resolve_hostname_as_venue_id_ = value;
  }

  static auto make_venue(const std::string& venue_id) -> data_layer::Venue {
    data_layer::Venue::Patch patch;
    patch.with_venue_id(venue_id);
    return data_layer::Venue::create(std::move(patch));
  }

  static auto make_venue(const std::string& venue_id, std::uint16_t port)
      -> data_layer::Venue {
    data_layer::Venue::Patch patch;
    patch.with_venue_id(venue_id).with_rest_port(port);
    return data_layer::Venue::create(std::move(patch));
  }

  std::shared_ptr<mock::VenueAccessor> venue_accessor =
      std::make_shared<mock::VenueAccessor>();

 protected:
  static constexpr std::string Localhost = "localhost";
  static constexpr std::string CurrentVenueId = "XLSE";
  static constexpr std::string DifferentVenueId = "LSE";
  static constexpr std::uint16_t CurrentVenuePort = 9000;
  static constexpr std::uint16_t DifferentVenuePort = 9001;

 private:
  auto make_resolver() -> redirect::DestinationResolver {
    return redirect::DestinationResolver(venue_accessor,
                                         CurrentVenueId,
                                         CurrentVenuePort,
                                         resolve_hostname_as_venue_id_);
  }

  bool resolve_hostname_as_venue_id_ = false;
};

TEST_F(HttpRedirectDestinationResolver,
       DoesNotResolveByVenueIDOfNonexistentVenue) {
  const mock::VenueAccessor::VenueResult accessor_reply{
      tl::unexpected(data_bridge::Failure::ResponseCardinalityError)};

  EXPECT_CALL(*venue_accessor, select_single(Eq(DifferentVenueId)))
      .Times(1)
      .WillOnce(Return(accessor_reply));

  auto result = resolve(DifferentVenueId);

  ASSERT_THAT(result, IsUnexpected(Resolver::Error::NonexistentInstance));
}

TEST_F(HttpRedirectDestinationResolver,
       DoesNotResolveByVenueIDIfRestPortIsAbsent) {
  const auto venue = make_venue(CurrentVenueId);

  const mock::VenueAccessor::VenueResult accessor_reply{venue};
  EXPECT_CALL(*venue_accessor, select_single(Eq(CurrentVenueId)))
      .Times(1)
      .WillOnce(Return(accessor_reply));

  auto result = resolve(CurrentVenueId);

  ASSERT_THAT(result, IsUnexpected(Resolver::Error::ResolvingFailed));
}

TEST_F(HttpRedirectDestinationResolver, ResolvesByVenueIDIfRestPortIsPresent) {
  resolve_hostname_as_venue_id(false);
  const auto venue = make_venue(DifferentVenueId, DifferentVenuePort);

  const mock::VenueAccessor::VenueResult accessor_reply{venue};
  EXPECT_CALL(*venue_accessor, select_single(Eq(DifferentVenueId)))
      .Times(1)
      .WillOnce(Return(accessor_reply));

  auto result = resolve(DifferentVenueId);

  ASSERT_THAT(result, IsExpected(Localhost, DifferentVenuePort));
}

TEST_F(HttpRedirectDestinationResolver, ResolvesByHostnameIfItIsUsedAsVenueId) {
  resolve_hostname_as_venue_id(true);
  const auto venue = make_venue(DifferentVenueId, DifferentVenuePort);

  const mock::VenueAccessor::VenueResult accessor_reply{venue};
  EXPECT_CALL(*venue_accessor, select_single(Eq(DifferentVenueId)))
      .Times(1)
      .WillOnce(Return(accessor_reply));

  auto result = resolve(DifferentVenueId);

  ASSERT_THAT(result, IsExpected(DifferentVenueId, DifferentVenuePort));
}

TEST_F(HttpRedirectDestinationResolver,
       DoesNotResolveByVenueIDIfRedirectPointsToLocalhostAndCurrentPort) {
  resolve_hostname_as_venue_id(false);
  const auto venue = make_venue(CurrentVenueId, CurrentVenuePort);

  const mock::VenueAccessor::VenueResult accessor_reply{venue};
  EXPECT_CALL(*venue_accessor, select_single(Eq(CurrentVenueId)))
      .Times(1)
      .WillOnce(Return(accessor_reply));

  auto result = resolve(CurrentVenueId);

  ASSERT_THAT(result, IsUnexpected(Resolver::Error::SelfRedirect));
}

TEST_F(HttpRedirectDestinationResolver,
       DoesNotResolveByVenueIDIfRedirectPointsToCurrentVenueAndCurrentPort) {
  resolve_hostname_as_venue_id(true);
  const auto venue = make_venue(CurrentVenueId, CurrentVenuePort);

  const mock::VenueAccessor::VenueResult accessor_reply{venue};
  EXPECT_CALL(*venue_accessor, select_single(Eq(std::string{CurrentVenueId})))
      .Times(1)
      .WillOnce(Return(accessor_reply));

  auto result = resolve(std::string{CurrentVenueId});

  ASSERT_THAT(result, IsUnexpected(Resolver::Error::SelfRedirect));
}

}  // namespace
}  // namespace simulator::http::redirect::test
