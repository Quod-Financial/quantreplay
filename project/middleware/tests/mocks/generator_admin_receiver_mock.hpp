#ifndef SIMULATOR_MIDDLEWARE_TESTS_MOCKS_GENERATOR_ADMIN_RECEIVER_MOCK_HPP_
#define SIMULATOR_MIDDLEWARE_TESTS_MOCKS_GENERATOR_ADMIN_RECEIVER_MOCK_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/generator_admin_channel.hpp"

namespace simulator::middleware::test {

struct GeneratorAdminRequestReceiverMock
    : public GeneratorAdminRequestReceiver {
  MOCK_METHOD(void,
              process,
              (const protocol::GenerationStatusRequest&,
               protocol::GenerationStatusReply&),
              (override));

  MOCK_METHOD(void,
              process,
              (const protocol::StartGenerationRequest&,
               protocol::StartGenerationReply&),
              (override));

  MOCK_METHOD(void,
              process,
              (const protocol::StopGenerationRequest&,
               protocol::StopGenerationReply&),
              (override));
};

}  // namespace simulator::middleware::test

#endif  // SIMULATOR_MIDDLEWARE_TESTS_MOCKS_GENERATOR_ADMIN_RECEIVER_MOCK_HPP_
