#ifndef SIMULATOR_ACCEPTOR_IH_PROCESSORS_REQUEST_PROCESSOR_HPP_
#define SIMULATOR_ACCEPTOR_IH_PROCESSORS_REQUEST_PROCESSOR_HPP_

#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

namespace simulator::fix::acceptor {

class RequestProcessor {
 public:
  RequestProcessor() = default;
  RequestProcessor(const RequestProcessor&) = default;
  RequestProcessor(RequestProcessor&&) noexcept = default;
  virtual ~RequestProcessor() = default;

  auto operator=(const RequestProcessor&) -> RequestProcessor& = default;
  auto operator=(RequestProcessor&&) noexcept -> RequestProcessor& = default;

  virtual auto process_fix_request(const FIX::Message& fix_message,
                                   const FIX::SessionID& fix_session) const
      -> void = 0;
};

}  // namespace simulator::fix::acceptor

#endif  // SIMULATOR_ACCEPTOR_IH_PROCESSORS_REQUEST_PROCESSOR_HPP_