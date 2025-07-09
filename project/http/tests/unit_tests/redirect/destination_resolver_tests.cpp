#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <tl/expected.hpp>

#include "data_layer/api/models/venue.hpp"
#include "ih/redirect/destination_resolver.hpp"
#include "ih/redirect/resolver.hpp"
#include "mocks/venue_accessor.hpp"

namespace simulator::http::redirect::test {
namespace {

using namespace ::testing;

class HttpRedirectDestinationResolver : public testing::Test {
 public:
  auto resolve(const std::string& venue_id)
      -> DestinationResolver::ResolvingResult {
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

  mock::VenueAccessor venue_accessor;

 private:
  auto make_resolver() -> redirect::DestinationResolver {
    return redirect::DestinationResolver(venue_accessor,
                                         resolve_hostname_as_venue_id_);
  }

  bool resolve_hostname_as_venue_id_ = false;
};

TEST_F(HttpRedirectDestinationResolver,
       DoesNotResolveByVenueIDOfNonexistentVenue) {
  const std::string venue_id = "XLSE";
  const mock::VenueAccessor::VenueResult accessor_reply{
      tl::unexpected(data_bridge::Failure::ResponseCardinalityError)};

  EXPECT_CALL(venue_accessor, select_single(Eq(venue_id)))
      .Times(1)
      .WillOnce(Return(accessor_reply));

  auto [destination, status] = resolve(venue_id);

  ASSERT_EQ(status, Resolver::Status::NonexistentInstance);
  ASSERT_FALSE(destination.has_value());
}

TEST_F(HttpRedirectDestinationResolver,
       DoesNotResolveByVenueIDIfRestPortIsAbsent) {
  const std::string venue_id = "XLSE";
  const auto venue = make_venue(venue_id);

  const mock::VenueAccessor::VenueResult accessor_reply{venue};
  EXPECT_CALL(venue_accessor, select_single(Eq(venue_id)))
      .Times(1)
      .WillOnce(Return(accessor_reply));

  auto [destination, status] = resolve(venue_id);

  ASSERT_EQ(status, Resolver::Status::ResolvingFailed);
  ASSERT_FALSE(destination.has_value());
}

TEST_F(HttpRedirectDestinationResolver, ResolvesByVenueIDIfRestPortIsPresent) {
  const std::string venue_id = "XLSE";
  const std::string venue_host = "localhost";
  constexpr std::uint16_t venue_port = 9001;
  const auto venue = make_venue(venue_id, venue_port);

  const mock::VenueAccessor::VenueResult accessor_reply{venue};
  EXPECT_CALL(venue_accessor, select_single(Eq(venue_id)))
      .Times(1)
      .WillOnce(Return(accessor_reply));

  auto [destination, status] = resolve(venue_id);

  ASSERT_EQ(status, Resolver::Status::Success);

  // NOLINTBEGIN bugprone-unchecked-optional-access
  ASSERT_TRUE(destination.has_value());
  ASSERT_EQ(destination->host(), venue_host);
  ASSERT_EQ(destination->port(), venue_port);
  // NOLINTEND
}

TEST_F(HttpRedirectDestinationResolver, ResolvesByHostnameIfItIsUsedAsVenueId) {
  resolve_hostname_as_venue_id(true);
  const auto venue = make_venue("XLSE", 9000);

  const mock::VenueAccessor::VenueResult accessor_reply{venue};
  EXPECT_CALL(venue_accessor, select_single(Eq("XLSE")))
      .Times(1)
      .WillOnce(Return(accessor_reply));

  auto [destination, status] = resolve("XLSE");

  ASSERT_EQ(status, Resolver::Status::Success);

  ASSERT_TRUE(destination.has_value());
  ASSERT_EQ(destination->host(), "XLSE");
  ASSERT_EQ(destination->port(), 9000);
}

}  // namespace
}  // namespace simulator::http::redirect::test
