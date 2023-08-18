#ifndef GENERATE_HPP
#define GENERATE_HPP
#include <string>
#include <vector>
#include "../jsonlib/json.h"

#define DEFAULT_DEF_DIR "../definitions/"
#define DEFAULT_DEF_NAME "default"
#define DEF_EXT ".json"
#define DEF_INIT_C "^XA"
#define DEF_CLAUSE_DELIM_C "^FS"
#define DEF_TERM_C "^XZ"
#define DEF_SUBCLAUSE_DELIM_C "^"
#define DEF_SUBCLAUSE_FIELD_C "FD"
#define DEF_SYMB_L 3

namespace generatelib
{
  class DefinitionClause
  {
    public:
      DefinitionClause();
      DefinitionClause(const std::string rawdata);
      DefinitionClause(const Json::Value json);
      std::string out() const;
      std::string out(const std::string s) const;
      int load(const Json::Value json);
      bool is_error() const;
      bool is_field() const;
      std::vector<std::string> get_subclauses() const;
      operator std::string() const;
    private:
      bool _error;
      bool _field;
      std::vector<std::string> _base_subclauses;
  };
  inline std::ostream& operator<<(std::ostream& os, DefinitionClause const& v);

  
  class Definition
  {
    public:
      Definition();
      Definition(std::string rawdata);
      Definition(Json::Value json);
      int save() const;
      int save(const std::string name) const;
      int load(const Json::Value json);
      int load_file();
      int load_file(const std::string name);
      bool is_error() const;
      size_t get_field_count() const; // TODO get field count
      std::string out() const;
      std::string out(const std::vector<std::string> ss) const;
      operator std::string() const;
    private:
      bool _error;
      size_t _field_count;
      std::vector<DefinitionClause> _clauses;
  };
  inline std::ostream& operator<<(std::ostream& os, Definition const& v);

  int print_label(const std::string definition /*, some data */);
  int label_definition(const std::string type, const std::string raw);
}

#endif /* GENERATE_HPP */
