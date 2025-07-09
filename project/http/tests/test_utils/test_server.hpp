#ifndef SIMULATOR_HTTP_TEST_UTILS_TEST_SERVER_HPP_
#define SIMULATOR_HTTP_TEST_UTILS_TEST_SERVER_HPP_

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

#include <utility>

namespace simulator::http::test::util {

class Responder {
 public:
  virtual ~Responder() = default;

  void set_response_data(Pistache::Http::Code response_code,
                         std::string response_body = "") noexcept {
    code_ = response_code;
    body_ = std::move(response_body);
  }

 protected:
  std::string body_;
  Pistache::Http::Code code_{Pistache::Http::Code::Ok};
};

class Router : public Pistache::Http::Handler, public Responder {
 public:
  HTTP_PROTOTYPE(Router)

  Router() { init(); }

  void onRequest(const Pistache::Http::Request& request,
                 Pistache::Http::ResponseWriter response) override {
    router_.route(request, std::move(response));
  }

 private:
  void init() {
    Pistache::Rest::Routes::Get(
        router_,
        "/test/get/request",
        Pistache::Rest::Routes::bind(&Router::respond, this));

    Pistache::Rest::Routes::Post(
        router_,
        "/test/post/request",
        Pistache::Rest::Routes::bind(&Router::respond, this));

    Pistache::Rest::Routes::Put(
        router_,
        "/test/put/request",
        Pistache::Rest::Routes::bind(&Router::respond, this));

    Pistache::Rest::Routes::Delete(
        router_,
        "/test/delete/request",
        Pistache::Rest::Routes::bind(&Router::respond, this));
  }

  auto respond([[maybe_unused]] const Pistache::Rest::Request& request,
               Pistache::Http::ResponseWriter response) -> void {
    response.send(code_, body_);
  }

  Pistache::Rest::Router router_;
};

class Server {
 public:
  Server()
      : endpoint_{Pistache::Address{Pistache::Ipv4::any(), Pistache::Port{0}}},
        router_{Pistache::Http::make_handler<Router>()} {
    auto opts = Pistache::Http::Endpoint::options().threads(1);
    endpoint_.init(opts);
    endpoint_.setHandler(router_);

    endpoint_.serveThreaded();
  }

  Server(Server const&) = delete;
  Server operator=(Server const&) = delete;

  Server(Server&&) noexcept = delete;
  Server operator=(Server&&) noexcept = delete;

  ~Server() { endpoint_.shutdown(); }

  auto port() const -> std::uint16_t { return endpoint_.getPort(); }

  auto router() const noexcept -> std::shared_ptr<Router> { return router_; }

  static auto create() -> std::shared_ptr<Server> {
    return std::make_shared<Server>();
  }

 private:
  Pistache::Http::Endpoint endpoint_;
  std::shared_ptr<Router> router_;
};

}  // namespace simulator::http::test::util

#endif  // SIMULATOR_HTTP_TEST_TEST_UTILS_TEST_SERVER_HPP_
