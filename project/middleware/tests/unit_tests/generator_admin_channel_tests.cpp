#include <gmock/gmock.h>

#include <memory>

#include "middleware/channels/generator_admin_channel.hpp"
#include "middleware/routing/generator_admin_channel.hpp"
#include "mocks/generator_admin_receiver_mock.hpp"

namespace simulator::middleware::test {
namespace {

using namespace testing;  // NOLINT

struct GeneratorAdminChannel : Test {
  auto bind_channel() -> void {
    std::shared_ptr<GeneratorAdminRequestReceiver> receiver_pointer{
        std::addressof(receiver), [](auto* /*pointer*/) {}};
    bind_generator_admin_channel(receiver_pointer);
  }

  GeneratorAdminRequestReceiverMock receiver;

 private:
  auto TearDown() -> void override { release_generator_admin_channel(); }
};

TEST_F(GeneratorAdminChannel, SendsSyncGenerationStatusRequest) {
  bind_channel();
  const protocol::GenerationStatusRequest request;
  protocol::GenerationStatusReply reply;

  EXPECT_CALL(receiver,
              process(A<const protocol::GenerationStatusRequest&>(),
                      A<protocol::GenerationStatusReply&>()));

  ASSERT_NO_THROW(send_admin_request(request, reply));
}

TEST_F(GeneratorAdminChannel, SendsSyncStartGenerationRequest) {
  bind_channel();

  const protocol::StartGenerationRequest request;
  protocol::StartGenerationReply reply;

  EXPECT_CALL(receiver,
              process(A<const protocol::StartGenerationRequest&>(),
                      A<protocol::StartGenerationReply&>()));

  ASSERT_NO_THROW(send_admin_request(request, reply));
}

TEST_F(GeneratorAdminChannel, SendsSyncStopGenerationRequest) {
  bind_channel();

  const protocol::StopGenerationRequest request;
  protocol::StopGenerationReply reply;

  EXPECT_CALL(receiver,
              process(A<const protocol::StopGenerationRequest&>(),
                      A<protocol::StopGenerationReply&>()));

  ASSERT_NO_THROW(send_admin_request(request, reply));
}

TEST_F(GeneratorAdminChannel, ReportsChannelNotBoundWhenSendingSyncRequest) {
  const protocol::GenerationStatusRequest request;
  protocol::GenerationStatusReply reply;

  ASSERT_THROW(send_admin_request(request, reply), ChannelUnboundError);
}

}  // namespace
}  // namespace simulator::middleware::test