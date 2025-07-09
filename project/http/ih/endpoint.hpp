#ifndef SIMULATOR_HTTP_IH_ENDPOINT_HPP_
#define SIMULATOR_HTTP_IH_ENDPOINT_HPP_

#include <string>
#include <string_view>

namespace simulator::http::endpoint {

constexpr std::string_view VenueStatusByVenueIdFmt{"/api/venuestatus/{}"};

const std::string GenStatus{"/api/genstatus/:venueId"};
const std::string GenStart{"/api/genstart/:venueId"};
const std::string GenStop{"/api/genstop/:venueId"};
const std::string Store{"/api/store"};
const std::string StoreById{"/api/store/:venueId"};
const std::string Recover{"/api/recover"};
const std::string RecoverById{"/api/recover/:venueId"};
const std::string Halt{"/api/halt/:venueId"};
const std::string Resume{"/api/resume/:venueId"};
const std::string Status{"/api/status"};
const std::string VenueStatusByVenueId{"/api/venuestatus/:id"};
const std::string VenueStatus{"/api/venuestatus"};
const std::string VenuesById{"/api/venues/:id"};
const std::string Venues{"/api/venues"};
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
