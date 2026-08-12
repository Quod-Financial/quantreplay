#include <gmock/gmock.h>

#include <sstream>

#include "ih/headers/content_disposition_attachment.hpp"

namespace simulator::http::test {
namespace {

using namespace ::testing;  // NOLINT

TEST(ContentDispositionAttachment, WritesAttachmentWithFilename) {
  const ContentDispositionAttachment header{"custom_archive.zip"};

  std::ostringstream stream;
  header.write(stream);

  ASSERT_THAT(stream.str(), Eq(R"(attachment; filename="custom_archive.zip")"));
}

}  // namespace
}  // namespace simulator::http::test
