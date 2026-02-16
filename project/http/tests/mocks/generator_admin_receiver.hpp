#ifndef SIMULATOR_HTTP_TESTS_MOCKS_GENERATOR_ADMIN_RECEIVER_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_GENERATOR_ADMIN_RECEIVER_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/generator_admin_channel.hpp"

namespace simulator::http::test::mock {

struct GeneratorAdminRequestReceiver
    : public middleware::GeneratorAdminRequestReceiver {
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

}  // namespace simulator::http::test::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_GENERATOR_ADMIN_RECEIVER_HPP_
