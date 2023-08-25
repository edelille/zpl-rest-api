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
#define DEF_SCL_DELIM_C "^"
#define DEF_SCL_FIELD_C "FD"
#define DEF_SCL_BARCODE_C "BC"
#define DEF_SCL_CENTERED_C "FB"
#define DEF_SCL_CENTERED_ADD_C "\\&"
#define DEF_SCL_RFID_C "RF"
#define DEF_SCL_RFID_TRYONCE_C "^RS,,,1"
#define DEF_SYMB_L 3

#define ERR_FILE_EXISTS 2
#define DEFAULT_CUPS_PRINTER "ZebraPrinterRaw"

namespace generatelib
{
  class DefinitionClause
  {
    public:
      DefinitionClause();
      DefinitionClause(const std::string rawdata);
      DefinitionClause(const Json::Value json);
      int load(const Json::Value json);
      bool is_barcode() const;
      bool is_centered() const;
      bool is_data() const;
      bool is_error() const;
      bool is_field() const;
      bool is_rfid() const;
      std::string get_last_error() const;
      std::vector<std::string> get_subclauses() const;
      std::string out() const;
      std::string out(const std::string s) const;
      operator std::string() const;
    private:
      bool _barcode;
      bool _centered;
      bool _data;
      bool _error;
      bool _field;
      bool _rfid;
      std::string _last_error;
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
      int save(const std::string name, bool overwrite=false) const;
      int load(const Json::Value json);
      int load_file();
      int load_file(const std::string name);
      bool is_error() const;
      size_t get_barcode_count() const;
      size_t get_field_count() const;
      size_t get_rfid_count() const;
      std::string get_last_error() const;
      std::string out() const;
      std::string out(const std::vector<std::string> ss) const;
      std::string out(const std::vector<std::string> ss, const std::vector<std::string> bs) const;
      std::string out(const std::vector<std::string> ss, const std::vector<std::string> bs, const std::vector<std::string> rs) const;
      operator std::string() const;
    protected:
      bool _error;
      std::string _last_error;
      size_t _barcode_count;
      size_t _field_count;
      size_t _rfid_count;
      std::vector<DefinitionClause> _clauses;
  };
  inline std::ostream& operator<<(std::ostream& os, Definition const& v);

  int print_label(const Definition& definition, const std::vector<std::string> data, const std::vector<std::string> barcode, const std::vector<std::string> rfid);
  int label_definition(const std::string type, const std::string raw, bool overwrite=false);
}

#endif /* GENERATE_HPP */
