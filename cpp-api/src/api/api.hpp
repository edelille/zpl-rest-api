#ifndef API_HPP
#define API_HPP
#include <string>
#include <vector>
#include "../cupslib/cupslib.hpp"
#include "../jsonlib/json.h"
#include "../httplib/httplib.h"

#define STATUS_DOWN 0
#define STATUS_UP 1
#define HTTP_STATUS_OK 200
#define HTTP_STATUS_BADREQUEST 400

namespace Api 
{
  class Api 
  {
    std::string url;
    int port;
    public:
      Api();
      Api(std::string url, int port);
      int status();
      int start();
    protected:
      httplib::Server server;
      static cupslib::Controller controller;
      std::vector<std::string> endpoints;
      static void handle_noargs(const httplib::Request&, httplib::Response&);
      static void handle_status(const httplib::Request&, httplib::Response&);
      static void handle_stop(const httplib::Request&, httplib::Response&);
      static void handle_print(const httplib::Request&, httplib::Response&, const httplib::ContentReader&);
      static void handle_definition_generate(const httplib::Request&, httplib::Response&, const httplib::ContentReader&);
    private:
  };
  
  void log_request(std::string func_name, std::string req_type, int result);
  std::string build_error_ret(std::string error); 
}
#endif /* API_HPP */

