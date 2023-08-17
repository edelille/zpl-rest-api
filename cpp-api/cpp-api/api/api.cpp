#include <string>
#include <iostream>

#include "api.hpp"
#include "../generatelib/generatelib.hpp"
#include "../httplib/httplib.h"
#include "../jsonlib/json.h"

namespace Api 
{
  // Static Members
  cupslib::Controller Api::controller;

  Api::Api() : endpoints(std::vector<std::string>()) {}
  Api::Api(std::string aUrl, int aPort) 
  {
    url = aUrl; 
    port = aPort; 
    endpoints = std::vector<std::string>();
    controller = cupslib::Controller();
    
    // Init the server
    server.Get("/", Api::handle_noargs);
    server.Get("/status", Api::handle_status);
    server.Get("/stop", Api::handle_stop);
    server.Post("/mirror", Api::handle_mirror);
    server.Post("/test/print", Api::handle_test_print);
    server.Post("/test/label", Api::handle_test_label_generate);
    server.Post("/test/definition", Api::handle_test_definition_generate);
  }

  // This is to be called to start the server which will also block the runtime
  int Api::start() 
  {
    std::cout << "The Api has been started" << std::endl;
    server.listen(url, port);
    return EXIT_SUCCESS;
  }

  void Api::handle_noargs(const httplib::Request& req, httplib::Response& res)
  {
    std::string msg("Welcome to the zpl cpp-api");
    res.set_content(msg, "text/plain");
    log_request(__func__, req.method, HTTP_STATUS_OK);
  }

  // This is function to call to check the information of the server
  void Api::handle_status(const httplib::Request& req, httplib::Response& res) 
  {
    controller.get_status();

    log_request(__func__, req.method, HTTP_STATUS_OK);
    return;
  }

  void Api::handle_stop(const httplib::Request& req, httplib::Response& res)
  {
    std::string msg("Server to shutdown...");
    res.set_content(msg, "text/plain");
    // With this we shut down the server, or maybe not yet if this is static
    // server.stop();
  }

  void Api::handle_mirror(const httplib::Request& req, httplib::Response& res, const httplib::ContentReader& content_reader)
  {
    std::string body;
    content_reader([&](const char* data, size_t data_length) {
      body.append(data, data_length);
      return true;
    });

    std::cout << "Mirror str: " << body << std::endl;
  }

  // This will ink with whatever is currently the test label
  void Api::handle_test_print(const httplib::Request& req, httplib::Response& res, const httplib::ContentReader& content_reader)
  {
    Json::Value inp;
    Json::Reader reader;
    content_reader([&](const char* data, size_t data_length) {
      bool parsingSuccess = reader.parse(data, inp);
      if ( !parsingSuccess ) {
          std::printf("[ Error ]: There was an error while parsing string: %s\n", data);
          // Return an error
          return false;
      }
      return true;
    });

    // Build the response
    Json::Value ret;
    Json::StreamWriterBuilder builder; 
    ret["success"] = true;
    ret["message"] = "The Label was printed correctly";
    ret["readback"] = inp;
    res.set_content(Json::writeString(builder, ret), "application/json");

    return; 
  }

  void handle_test_label_generate(const httplib::Request& req, httplib::Response& res, const httplib::ContentReader& content_reader)
  {
    Json::Value inp;
    Json::Reader reader;
    content_reader([&](const char* data, size_t data_length) {
      bool parsingSuccess = reader.parse(data, inp);
      if (!parsingSuccess) {
        std::printf("[ Error ]: There was an error while parsing string: %s\n", data);
      }
      return parsingSuccess;
    });

    std::map<std::string, Json::ValueType> params = {
        {"label_type", Json::stringValue},
    };

    for (const auto&[param, type] : params)
    {
      char errorMsg[100];
      bool error_b;
      if (inp[param].empty()) 
      {
        std::sprintf(errorMsg, "\"%s\" field is required", param.c_str());
        error_b = true;
      }
      
      if (type != inp[param].type()) 
      {
        std::sprintf( errorMsg, "%s was incorrect type. received: %d, expected: %d",
                      param.c_str(), inp[param].type(), type);
        error_b = true;
      }

      if (error_b)
      {
        res.status = 400;
        std::printf("[ Error ]: %s\n", errorMsg);
        res.set_content(build_error_ret(errorMsg), "application/json");
        return;
      }
    }

    generatelib::print_label(inp["label_type"].asString());
  }

  void handle_test_definition_generate(const httplib::Request& req, httplib::Response& res, const httplib::ContentReader& content_reader)
  {
    Json::Value inp;
    Json::Reader reader;
    content_reader([&](const char* data, size_t data_length) {
      bool parsingSuccess = reader.parse(data, inp);
      if (!parsingSuccess) {
        std::printf("[ Error ]: There was an error while parsing string: %s\n", data);
      }
      return parsingSuccess;
    });

    std::map<std::string, Json::ValueType> params = {
        {"label_type", Json::stringValue},
        {"raw", Json::stringValue},
    };

    for (const auto&[param, type] : params)
    {
      char errorMsg[100];
      bool error_b;
      if (inp[param].empty()) 
      {
        std::sprintf(errorMsg, "\"%s\" field is required", param.c_str());
        error_b = true;
      }
      
      if (type != inp[param].type()) 
      {
        std::sprintf( errorMsg, "%s was incorrect type. received: %d, expected: %d",
                      param.c_str(), inp[param].type(), type);
        error_b = true;
      }

      if (error_b)
      {
        res.status = 400;
        std::printf("[ Error ]: %s\n", errorMsg);
        res.set_content(build_error_ret(errorMsg), "application/json");
        return;
      }
    }

    generatelib::label_definition(inp["label_type"].asString(), inp["raw"].asString());
  }

  void log_request(std::string func, std::string req_type, int result)
  {
    // Potentially add a logging component
    std::cout << "[ " << req_type << " ]: "; 
    std::cout << func << ",\t<status>:\t" << result;
    std::cout << std::endl;
  }

  std::string build_error_ret(std::string errorStr) 
  {
    Json::Value ret;
    Json::StreamWriterBuilder builder; 
    ret["error"] = true;
    ret["error_message"] = errorStr;
    return Json::writeString(builder, ret);
  }
}
