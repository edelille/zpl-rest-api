#include <string>
#include <iostream>

#include "api.hpp"
#include "../generatelib/generatelib.hpp"
#include "../httplib/httplib.h"
#include "../jsonlib/json.h"
#include "../utility/utility.hpp"

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

  void Api::handle_test_label_generate(const httplib::Request& req, httplib::Response& res, const httplib::ContentReader& content_reader)
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
      bool error_b = false;
      if (inp[param].empty()) 
      {
        std::sprintf(errorMsg, "'%s' field is required", param.c_str());
        error_b = true;
      }
      else if (type != inp[param].type()) 
      {
        std::sprintf( errorMsg, "%s was incorrect type. received: %d, expected: %d", param.c_str(), inp[param].type(), type);
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

    // Find the definition file, and check if it exists
    std::string rel_path = DEFAULT_DEF_DIR + inp["label_type"].asString() + DEF_EXT;
    if (!utility::file_exists(rel_path))
    {
      res.status = 400;
      char errorMsg[1<<8];
      std::sprintf(errorMsg, "This label, %s, is not yet defined. Please utilize the /definition endpoint to create a definiton first", inp["label_type"].asCString());
      res.set_content(build_error_ret(errorMsg), "application/json");
      return;
    }

  // Check the definition parse
    generatelib::Definition def;
    def.load_file(inp["label_type"].asString());
    if (def.is_error())
    {
      res.status = 400;
      std::string errorMsg = "There was an issue parsing the loaded definition: " + def.get_last_error();
      res.set_content(build_error_ret(errorMsg), "application/json");
      return;
    }

    // Assert the field requirements
    char errorMsg[1<<8];
    bool error=true;
    if (def.get_barcode_count() != inp["barcode"].size())
      std::sprintf(errorMsg, "This label, %s, requires %lu barcode value(s) but you have only supplied %u value(s) in the {'barcode': []} attribute. Please contribute %lu barcode values in your next request.", inp["label_type"].asCString(), def.get_barcode_count(), inp["barcode"].size(), def.get_barcode_count());
    else if (def.get_field_count() != inp["data"].size())
      std::sprintf(errorMsg, "This label, %s, requires %lu field(s) but you have only supplied %u field(s) the {'data': []} attribute. Please contribute %lu fields in your next request.", inp["label_type"].asCString(), def.get_field_count(), inp["data"].size(), def.get_field_count());
    else if (def.get_rfid_count() != inp["rfid"].size())
      std::sprintf(errorMsg, "This label, %s, requires %lu rfid value(s) but you have only supplied %u value(s) in the {'rfid': []} attribute. Please contribute %lu rfid values in your next request.", inp["label_type"].asCString(), def.get_rfid_count(), inp["rfid"].size(), def.get_rfid_count());
    else
      error=false;
    if (error)
    {
      res.status = 400;
      res.set_content(build_error_ret(errorMsg), "application/json");
      return;
    }


    // Grab the data and then push it to print
    std::vector<std::string> data;
    for (size_t i=0; i<inp["data"].size(); ++i) data.push_back(inp["data"][int(i)].asString());
    int status = generatelib::print_label(def, data);
    if (status != 0)
    {
      res.status = 400;
      std::string errorMsg = "There was an error in printing your label";
      res.set_content(build_error_ret(errorMsg), "application/json");
      return;
    }

    Json::Value ret;
    Json::StreamWriterBuilder builder;
    ret["success"] = true;
    ret["message"] = "The Label was printed correctly";
    ret["readback"] = inp;
    res.set_content(Json::writeString(builder, ret), "application/json");
  }

  void Api::handle_test_definition_generate(const httplib::Request& req, httplib::Response& res, const httplib::ContentReader& content_reader)
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
      bool error_b = false;
      if (inp[param].empty()) 
      {
        std::sprintf(errorMsg, "'%s' field is required", param.c_str());
        error_b = true;
      } 
      else if (type != inp[param].type()) 
      {
        std::sprintf( errorMsg, "%s was incorrect type. received: %d, expected: %d", param.c_str(), inp[param].type(), type);
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

    int status = generatelib::label_definition(inp["label_type"].asString(), inp["raw"].asString(), inp["overwrite"].asBool());
    if (status != 0)
    {
      res.status = 400;
      std::string errorMsg;
      if (status == ERR_FILE_EXISTS) 
        errorMsg = "The file already exists. If you would like to overwrite, pass {'overwrite': true } in the request";
      else
        errorMsg = "There was an error creating the label definition";

      res.set_content(build_error_ret(errorMsg), "application/json");
      return;
    }

    // As a precaution, we will do a readback from the new label file.
    //  >>  Load the file right now
    generatelib::Definition def;
    status = def.load_file(inp["label_type"].asString());
    if (status != 0)
    {
      res.status = 400;
      std::string errorMsg("There was an error loading back the definition");
      res.set_content(build_error_ret(errorMsg), "application/json");
      return;
    }

    Json::Value ret;
    Json::StreamWriterBuilder builder; 
    char buf[1<<8];
    ret["success"] = true;
    Json::Value insts(Json::arrayValue);
    insts.append("1. (Completed) You have already created the definition.");
    insts.append("2. Generate a post request back at /label.");
    std::sprintf(buf, "2a. Append a 'type' field containing the label name, in this case: %s", inp["label_type"].asCString());
    insts.append(buf);
    std::sprintf(buf, "2b. Append a 'data' field containing %lu amount of strings to populate into the label", def.get_field_count());
    insts.append(buf);
    ret["instructions"] = insts;
    res.set_content(Json::writeString(builder, ret), "application/json");
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
