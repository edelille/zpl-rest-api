#include <cstdlib>
#include <iostream>

#include "api/api.hpp"
#include "cupslib/cupslib.hpp"
#include "httplib/httplib.h"
#include "testing/testing.hpp"

#define TESTING

int main() 
{
#ifdef TESTING
  std::cout << "[main]:\tTesting flag detected, entering test routine\n";

  testing::sandbox();
#else
  std::cout << "[main]:\tStarting ZPL Rest Server\n";

  // Attempt to create the API server to then start it
  std::string host("localhost");
  int port(8000);
  Api::Api api(host, port);
  api.start();
#endif /* TESTING */

  return EXIT_SUCCESS; 
}
