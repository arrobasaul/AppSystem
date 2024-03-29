#pragma once
#include "Base.h"
#include "../include/MNetwork.h"
#include "imgui.h"
#include <cstdlib>  /* Aquí contiene la función system("pause"); */
#include <iostream> /* Importamos esta librería para hacer uso de cout y así ver el resultado en pantalla. */
#include <memory>
#include <string>
/*#include <boost/beast/core.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/type_traits.hpp>
#include <boost/beast/version.hpp>
#include <boost/json.hpp>*/
#include <functional>
#include <thread>
#include "CoreRuntime.hpp"

#include "ioc.h"
#include "ApplicationContext.h"
#include "../include/MNetwork.h"
/*
namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
namespace json = boost::json;
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
using boost::json::value;
//------------------------------------------------------------------------------
*/
/*
// Report a failure
void fail(beast::error_code ec, char const *what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

// Performs an HTTP GET and prints the response
class session : public std::enable_shared_from_this<session>
{
    tcp::resolver resolver_;
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_; // (Must persist between reads)
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;
    char text[2048 * 16];

public:
    // Objects are constructed with a strand to
    // ensure that handlers do not execute concurrently.

    explicit session(net::io_context &ioc)
        : resolver_(net::make_strand(ioc)), stream_(net::make_strand(ioc))
    {
    }

    // Start the asynchronous operation
    void
    run(
        char const *host,
        char const *port,
        char const *target,
        int version)
    {
        // Set up an HTTP GET request message
        req_.version(version);
        req_.method(http::verb::post);
        req_.target(target);
        req_.set(http::field::host, host);
        req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req_.set(http::field::content_type, "application/json");

        boost::beast::http::empty_body::value_type body;
        value data = {
            {"email", "arrobasaul2@gmail.es"},
            {"password", "Olimpia3"}};
        std::string message_to_client = std::string("{ \n\"email\":
\"arrobasaul2@gmail.es\", \n\"password\": \"Olimpia3\" \n}"); //
serialize(json::object{{"email", "arrobasaul2@gmail.es"}, {"password",
"Olimpia3"}});
        // body = message_to_client;
        // req_.body() = message_to_client;
        req_.body() = serialize(data);
        //  Look up the domain name
        // std::cout << req_ << std::endl;
        req_.prepare_payload();
        resolver_.async_resolve(
            host,
            port,
            beast::bind_front_handler(
                &session::on_resolve,
                shared_from_this()));
    }

    void
    on_resolve(
        beast::error_code ec,
        tcp::resolver::results_type results)
    {
        if (ec)
            return fail(ec, "resolve");

        // Set a timeout on the operation
        stream_.expires_after(std::chrono::seconds(30));

        // Make the connection on the IP address we get from a lookup
        stream_.async_connect(
            results,
            beast::bind_front_handler(
                &session::on_connect,
                shared_from_this()));
    }

    void
    on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
    {
        if (ec)
            return fail(ec, "connect");

        // Set a timeout on the operation
        stream_.expires_after(std::chrono::seconds(30));

        // Send the HTTP request to the remote host
        http::async_write(stream_, req_,
                          beast::bind_front_handler(
                              &session::on_write,
                              shared_from_this()));
    }

    void
    on_write(
        beast::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "write");

        // Receive the HTTP response
        http::async_read(stream_, buffer_, res_,
                         beast::bind_front_handler(
                             &session::on_read,
                             shared_from_this()));
    }

    void
    on_read(
        beast::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "read");

        // Write the message to standard out
        std::cout << res_.base() << std::endl;
        std::cout << res_.body() << std::endl;
        // Gracefully close the socket
        stream_.socket()
            .shutdown(tcp::socket::shutdown_both, ec);

        // not_connected happens sometimes so don't bother reporting it.
        if (ec && ec != beast::errc::not_connected)
            return fail(ec, "shutdown");

        // If we get here then the connection is closed gracefully
    }
};
*/
//------------------------------------------------------------------------------

#ifndef _WIN32
#define EXPORT
#else
#define EXPORT __declspec(dllexport)
#endif
extern "C" EXPORT std::string Name() {
  char *nombre;
  std::string str_obj("MNetwork");
  nombre = &str_obj[0];
  return str_obj;
}

APP_MOD_INFO{/* Name:            */ "MNetwork",
             /* Description:     */ "My fancy new module",
             /* Author:          */ "author1;author2,author3,etc...",
             /* Version:         */ 0,
             1,
             0,
             /* Max instances    */ -1};
class Person {  
    public:
    // ... Person members ...  
    virtual ~Person() {}  
};  

class Employee : public IMNetwork  {  
    int sayHello() const {
      printf("Say hello fron Employee\n");
    return 10;
  }
};
class MNetwork : public IMNetwork ,  public AppSystem::Service {
public:
  std::string paraPrueba = " paraPrueba MNetwork";
  MNetwork() = default;
  MNetwork(std::string name) {
    this->name = name;
    /*Person person;  
    Employee employee;  
    Person *ptr = &employee;  
    int t = 3;  

    std::cout << typeid(t).name() << std::endl;  
    std::cout << typeid(person).name() << std::endl;   // Person (statically known at compile-time)  
    std::cout << typeid(employee).name() << std::endl; // Employee (statically known at compile-time)  
    std::cout << typeid(ptr).name() << std::endl;      // Person * (statically known at compile-time)  
    std::cout << typeid(*ptr).name() << std::endl;  */
    // gui::menu.registerEntry(name, menuHandler, this, NULL);
  }

  ~MNetwork() {
    // gui::menu.removeEntry(name);
  }
        int sayHello() const {
        printf("Say hello fron MNetwork \n");
        return 10;
        }
  void OnAttach(std::shared_ptr<AppSystem::ApplicationContext> applicationContext) {}
  void OnDetach() {}
  /*void postInit() {}
  
  void enable() { enabled = true; }

  void disable() { enabled = false; }
  bool isEnabled() { return enabled; }
  */
  

private:
  static void menuHandler(void *ctx) {
    MNetwork *_this = (MNetwork *)ctx;
    // ImGui::Text("Hello SDR++, my name is %s", _this->name.c_str());
  }
  
  std::string name = "MNetwork";
  bool enabled = true;
};
MOD_EXPORT void _INIT_(AppSystem::ioc::container* container) {
    // Nothing here
    //container->RegisterInstance<IMNetwork,MNetwork>();
    container->register_type<IMNetwork, Employee>();
    

    std::coroutine_handle<ReturnObject3::promise_type> h = counter3();
    ReturnObject3::promise_type &promise = h.promise();
    for (int i = 0; i < 3; ++i) {
      std::cout << "counter3: " << promise.value_ << std::endl;
      h();
    }
    h.destroy();

    printf("You are in inside Mcore2 on win\n");
}

MOD_EXPORT AppSystem::Service *_CREATE_INSTANCE_() {
  return new MNetwork();
}

MOD_EXPORT void _DELETE_INSTANCE_(void *instance) {
  delete (MNetwork *)instance;
}

MOD_EXPORT void _END_() {
  // Nothing here
}
MOD_EXPORT AppSystem::Service *_CREATE_LAYER_(std::string name) {
  printf("_CREATE_LAYER_\n");
  return new MNetwork(name);
}