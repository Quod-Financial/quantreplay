#include "ih/controllers/app_controller.hpp"

#include "ih/utils/response_formatters.hpp"

namespace simulator::http {

AppControllerImpl::AppControllerImpl(
    std::shared_ptr<data_bridge::VenueAccessor> data_accessor,
    std::string venue_name,
    ControlCallbacks callbacks)
    : data_accessor_{std::move(data_accessor)},
      venue_id_{std::move(venue_name)},
      venue_rest_port_{0},
      callbacks_{std::move(callbacks)} {
  if (auto result = data_accessor_->select_single(venue_id_)) {
    const auto& venue = *result;
    if (venue.rest_port().has_value()) {
      venue_rest_port_ = *venue.rest_port();
    } else {
      throw std::runtime_error(
          "failed to initialize AppController, venue rest_port is not set");
    }
  } else {
    throw std::runtime_error(
        "failed to initialize AppController, venue record is not found");
  }

  if (!callbacks_.reset_app_state) {
    throw std::runtime_error(
        "failed to initialize AppController, restart application callback is "
        "not set");
  }
}

auto AppControllerImpl::ready_to_reset() const -> Result {
  const auto result = data_accessor_->select_single(venue_id_);
  if (!result) {
    return {Pistache::Http::Code::Conflict,
            format_result_response(fmt::format(
                "Unable to reset the venue state. The venue id `{}' is "
                "not found. It may have been modified or the venue "
                "deleted. Please check the database.",
                venue_id_))};
  }

  const auto& rest_port = result->rest_port();
  if (!rest_port.has_value()) {
    return {Pistache::Http::Code::Conflict,
            format_result_response(fmt::format(
                "Unable to reset the venue state. The venue `{}' restPort is "
                "not set in the database. It may have been deleted. Please "
                "check the database.",
                venue_id_))};
  }

  if (*rest_port != venue_rest_port_) {
    return {Pistache::Http::Code::Conflict,
            format_result_response(fmt::format(
                "Unable to reset the venue state. The venue `{}' "
                "restPort has been modified in the database. It was `{}' "
                "at the simulator launch. Please check the database.",
                venue_id_,
                venue_rest_port_))};
  }

  return {Pistache::Http::Code::Ok,
          format_result_response("Resetting the venue state starts.")};
}

auto AppControllerImpl::reset_app_state() const -> void {
  callbacks_.reset_app_state();
}

}  // namespace simulator::http
