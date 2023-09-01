#include <cstdlib>
#include <iostream>

#include <api/api.hpp>
#include <cupslib/cupslib.hpp>
#include <httplib/httplib.h>
#include <testing/testing.hpp>
#include <jsonlib/json.h>

void preflight()
{
  
}

int main() 
{
  std::cout << "[main]:\tStarting ZPL Rest Server\n";

  // check os.stat
  preflight();

  // Attempt to create the API server to then start it
  std::string host("localhost");
  int port(8000);
  Api::Api api(host, port);
  api.start();

  return EXIT_SUCCESS; 
}
