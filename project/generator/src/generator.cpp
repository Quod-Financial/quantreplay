#include "generator/generator.hpp"

#include <cstdlib>
#include <optional>
#include <utility>

#include "data_layer/api/data_access_layer.hpp"
#include "ih/generator.hpp"
#include "log/logging.hpp"

namespace simulator::generator {
namespace {

[[nodiscard]] auto create_generator_implementation(
    data_layer::database::Context db)
    -> std::unique_ptr<Generator::Implementation> {
  try {
    return std::make_unique<Generator::Implementation>(
        data_layer::select_simulated_venue(db), db);
  } catch (const std::exception& exception) {
    log::err("failed to create generator implementation, an error occurred: {}",
             exception.what());
  } catch (...) {
    log::err(
        "failed to create generator implementation, unknown error occurred");
  }
  throw std::runtime_error("failed to create generator implementation");
}

auto launch_generator(Generator::Implementation& generator) {
  try {
    generator.launch();
    return;
  } catch (const std::exception& exception) {
    log::err("failed to launch generator, an error occurred: {}",
             exception.what());
  } catch (...) {
    log::err("failed to launch generator, unknown error occurred");
  }
  throw std::runtime_error("failed to launch generator");
}

auto terminate_generator(Generator::Implementation& generator) {
  generator.terminate();
}

auto retrieve_generation_status(Generator::Implementation& generator)
    -> std::optional<protocol::GenerationStatusReply::Status> {
  try {
    return generator.is_running()
               ? protocol::GenerationStatusReply::Status::Running
               : protocol::GenerationStatusReply::Status::Stopped;
  } catch (const std::exception& exception) {
    log::warn("failed to resume generation, an error occurred: {}",
              exception.what());
  } catch (...) {
    log::warn("failed to resume generation, unknown error occurred");
  }
  return std::nullopt;
}

auto resume_generator(Generator::Implementation& generator)
    -> protocol::StartGenerationReply::Result {
  try {
    generator.resume();
    return protocol::StartGenerationReply::Result::Started;
  } catch (const std::exception& exception) {
    log::warn("failed to resume generation, an error occurred: {}",
              exception.what());
  } catch (...) {
    log::warn("failed to resume generation, unknown error occurred");
  }
  return protocol::StartGenerationReply::Result::ErrorOccurred;
}

auto suspend_generation(Generator::Implementation& generator)
    -> protocol::StopGenerationReply::Result {
  try {
    generator.suspend();
    return protocol::StopGenerationReply::Result::Stopped;
  } catch (const std::exception& exception) {
    log::warn("failed to stop generation, an error occurred: {}",
              exception.what());
  } catch (...) {
    log::warn("failed to stop generation, unknown error occurred");
  }
  return protocol::StopGenerationReply::Result::ErrorOccurred;
}

auto enrich_reply(const auto& reply, Generator::Implementation& generator)
    -> void {
  try {
    generator.enrich(reply);
  } catch (const std::exception& exception) {
    log::warn(
        "failed to handle the reply message, error occurred: {}, "
        "unable to process {}",
        exception.what(),
        reply);
  } catch (...) {
    log::err(
        "failed to handle the reply message, unknown error occurred, "
        "unable to process {}",
        reply);
  }
}

}  // namespace

Generator::Generator(std::unique_ptr<Implementation> impl) noexcept
    : impl_(std::move(impl)) {}

Generator::~Generator() noexcept = default;

auto Generator::implementation() noexcept -> Implementation& {
  if (impl_) [[likely]] {
    return *impl_;
  }

  log::err(
      "generator is not allocated/initialized, this may indicate "
      "a critical bug in the component, can not continue program execution, "
      "aborting...");

  std::abort();
}

auto create_generator(data_layer::database::Context db) -> Generator {
  log::debug("creating simulator generator instance");

  Generator generator{create_generator_implementation(std::move(db))};

  log::info("created simulator generator instance");

  return generator;
}

auto launch_generator(Generator& generator) -> void {
  log::debug("launching generator");

  launch_generator(generator.implementation());

  log::info("generator has been launched");
}

auto terminate_generator(Generator& generator) noexcept -> void {
  log::debug("terminating generator");

  terminate_generator(generator.implementation());

  log::info("generator has been terminated");
}

auto accept_reply(const protocol::ExecutionReport& reply, Generator& generator)
    -> void {
  log::debug("accepting ExecutionReport sent to the generator instance");
  enrich_reply(reply, generator.implementation());
}

auto accept_reply(const protocol::OrderPlacementConfirmation& reply,
                  Generator& generator) -> void {
  log::debug(
      "accepting OrderPlacementConfirmation sent to the generator instance");
  enrich_reply(reply, generator.implementation());
}

auto accept_reply(const protocol::OrderPlacementReject& reply,
                  Generator& generator) -> void {
  log::debug("accepting OrderPlacementReject sent to the generator instance");
  enrich_reply(reply, generator.implementation());
}

auto accept_reply(const protocol::OrderModificationConfirmation& reply,
                  Generator& generator) -> void {
  log::debug(
      "accepting OrderModificationConfirmation sent to the generator instance");

  enrich_reply(reply, generator.implementation());
}

auto accept_reply(const protocol::OrderModificationReject& reply,
                  [[maybe_unused]] Generator& generator) -> void {
  log::debug(
      "order generator does not handle OrderModificationReject messages, "
      "skipping {}",
      reply);
}

auto accept_reply(const protocol::OrderCancellationConfirmation& reply,
                  Generator& generator) -> void {
  log::debug(
      "accepting OrderCancellationConfirmation sent to the generator instance");
  enrich_reply(reply, generator.implementation());
}

auto accept_reply(const protocol::OrderCancellationReject& reply,
                  [[maybe_unused]] Generator& generator) -> void {
  log::debug(
      "order generator does not handle OrderCancellationReject messages, "
      "skipping {}",
      reply);
}

auto process_admin_request(
    Generator& generator,
    [[maybe_unused]] const protocol::GenerationStatusRequest& request,
    protocol::GenerationStatusReply& reply) -> void {
  reply.status = retrieve_generation_status(generator.implementation());
  log::info("processed request to retrieve generation status");
}

auto process_admin_request(
    Generator& generator,
    [[maybe_unused]] const protocol::StartGenerationRequest& request,
    protocol::StartGenerationReply& reply) -> void {
  reply.result = resume_generator(generator.implementation());
  log::info("processed request to start/resume generation");
}

auto process_admin_request(
    Generator& generator,
    [[maybe_unused]] const protocol::StopGenerationRequest& request,
    protocol::StopGenerationReply& reply) -> void {
  reply.result = suspend_generation(generator.implementation());
  log::info("processed request to suspend generation");
}

}  // namespace simulator::generator