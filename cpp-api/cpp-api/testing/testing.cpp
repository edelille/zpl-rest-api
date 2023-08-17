#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include "./testing.hpp"
#include "../generatelib/generatelib.hpp"

namespace testing
{
  int sandbox()
  {
    // We currently want to test the label definition generation
    std::string example_label_fn = "../example/test_label1.zpl";
    if (!generatelib::file_exists(example_label_fn))
    {
      puts("[sandbox]\tfile does not exist");
    }
    std::ifstream f(example_label_fn);
    std::stringstream buf;
    buf << f.rdbuf();
    generatelib::label_definition("test1", buf.str());

    return 0;
  }
}
