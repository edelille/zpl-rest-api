#include <string>
#include <sys/stat.h>
#include <vector>

#include "./utility.hpp"

namespace utility
{
  bool file_exists(const std::string filename)
  {
    struct stat buffer;
    return (stat (filename.c_str(), &buffer) == 0);
  }

  // String replace all occurances of from with to in src
  void replace_all(std::string& src, const std::string& from, const std::string& to)
  {
    std::string newString; 
    newString.reserve(src.length());
    std::string::size_type lastPos = 0;
    std::string::size_type findPos;
    while (std::string::npos != (findPos = src.find(from, lastPos)))
    {
      newString.append(src, lastPos, findPos - lastPos);
      newString += to;
      lastPos = findPos + from.length();
    }
    newString.append(src, lastPos, src.length() - lastPos);
    src.swap(newString);
  }

  std::vector<std::string> split_on(const std::string s, const std::string delim)
  {
    size_t pos_start = 0, pos_end, delim_len = delim.length();
    std::string token;
    std::vector<std::string> res;
    while ((pos_end = s.find(delim, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }
    res.push_back (s.substr (pos_start));
    return res;
  }
}
