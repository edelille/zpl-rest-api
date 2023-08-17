#ifndef GENERATE_HPP
#define GENERATE_HPP
#include <string>
#include <vector>

#define DEFAULT_DEF_DIR "definitions/"

namespace generatelib
{
  class DefinitionClause
  {
  };
  class Definition
  {
    vector<DefinitionClause> clauses;
    public:
      Definition();
      Definition(std::string rawdata);
      int parse(std::string rawdata);

  };

  int print_label(const std::string definition /*, some data */);
  int label_definition(const std::string type, const std::string raw);

  bool file_exists(const std::string filename);
}

#endif /* GENERATE_HPP */
