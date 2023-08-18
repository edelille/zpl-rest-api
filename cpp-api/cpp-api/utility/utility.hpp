#ifndef UTILITY_HPP
#define UTILITY_HPP
#include <string>
#include <vector>

namespace utility
{
  bool file_exists(const std::string filepath);
  void replace_all(std::string& src, const std::string& from, const std::string& to);
  std::vector<std::string> split_on(const std::string s, const std::string delim);
}
#endif /* UTILITY_HPP */
