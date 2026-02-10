#ifndef SIMULATOR_HTTP_IH_ENDPOINT_HPP_
#define SIMULATOR_HTTP_IH_ENDPOINT_HPP_

#include <string>
#include <string_view>

namespace simulator::http::endpoint {

constexpr std::string_view VenueStatusByVenueIdFmt{"/api/venuestatus/{}"};

const std::string GenStatus{"/api/genstatus"};
const std::string GenStatusByVenueId{"/api/genstatus/:venueId"};
const std::string GenStart{"/api/genstart"};
const std::string GenStartByVenueId{"/api/genstart/:venueId"};
const std::string GenStop{"/api/genstop"};
const std::string GenStopByVenueId{"/api/genstop/:venueId"};
const std::string Store{"/api/store"};
const std::string StoreByVenueId{"/api/store/:venueId"};
const std::string Recover{"/api/recover"};
const std::string RecoverByVenueId{"/api/recover/:venueId"};
const std::string Reset{"/api/reset"};
const std::string ResetByVenueId{"/api/reset/:venueId"};
const std::string Halt{"/api/halt"};
const std::string HaltByVenueId{"/api/halt/:venueId"};
const std::string Resume{"/api/resume"};
const std::string ResumeByVenueId{"/api/resume/:venueId"};
const std::string VenueStatus{"/api/venuestatus"};
const std::string VenueStatusByVenueId{"/api/venuestatus/:venueId"};
const std::string AllVenueStatus{"/api/allvenuestatus"};
const std::string Venues{"/api/venues"};
const std::string VenuesByVenueId{"/api/venues/:venueId"};
const std::string ListingsBySymbol{"/api/listings/:symbol"};
const std::string Listings{"/api/listings"};
const std::string PriceSeedsById{"/api/priceseeds/:id"};
const std::string PriceSeeds{"/api/priceseeds"};
const std::string SyncPriceSeeds{"/api/syncpriceseeds"};
const std::string DataSourcesById{"/api/datasources/:id"};
const std::string DataSources{"/api/datasources"};
const std::string Settings{"/api/settings"};

}  // namespace simulator::http::endpoint

#endif  // SIMULATOR_HTTP_IH_ENDPOINT_HPP_
