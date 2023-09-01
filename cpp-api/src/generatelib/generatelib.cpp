#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <ctime>

#include <filesystem>

#include "./generatelib.hpp"
#include "../cupslib/cupslib.hpp"
#include "../jsonlib/json.h"
#include "../utility/utility.hpp"

//#define PRINT_ENABLED

namespace generatelib
{
  DefinitionClause::DefinitionClause() {};
  DefinitionClause::DefinitionClause(const std::string raw) 
  {
    _barcode = false;
    _centered = false;
    _error = false;
    _field = false;
    _rfid = false;
    std::vector<std::string>
    preclauses = utility::split_on(raw, DEF_SCL_DELIM_C); 
    preclauses.erase(preclauses.begin()); 
    for (const auto& x : preclauses)
    {
      // If the prefix is a FD = field-descriptor, we mark it and keep it out
      // of the base subclasses
      if (x.starts_with(DEF_SCL_FIELD_C)) { _field = true; continue; }
      if (x.starts_with(DEF_SCL_BARCODE_C)) { _barcode = true; }
      if (x.starts_with(DEF_SCL_CENTERED_C)) { _centered = true; }
      if (x.starts_with(DEF_SCL_RFID_C)) { _rfid = true; }
      _base_subclauses.push_back(x);
    }
    _data = _barcode | _field | _rfid;
    _field = _field & ~(_barcode | _rfid);
  };
  DefinitionClause::DefinitionClause(const Json::Value json) { DefinitionClause::load(json); }
  int DefinitionClause::load(Json::Value json) 
  { 
    _barcode = json["barcode"].asBool();
    _centered = json["centered"].asBool();
    _data = json["data"].asBool();
    _error = json["error"].asBool();
    _field = json["field"].asBool();
    _rfid = json["rfid"].asBool();
    _base_subclauses.clear();
    for (size_t i = 0; i < json["subclauses"].size(); ++i)
    {
      _base_subclauses.push_back(json["subclauses"][int(i)].asString());
    }
    return 0;
  }
  bool DefinitionClause::is_barcode() const { return _barcode; }
  bool DefinitionClause::is_centered() const { return _centered; }
  bool DefinitionClause::is_data() const { return _data; }
  bool DefinitionClause::is_error() const { return _error; }
  bool DefinitionClause::is_field() const { return _field; }
  bool DefinitionClause::is_rfid() const { return _rfid; }
  std::string DefinitionClause::get_last_error() const { return _last_error; }
  std::vector<std::string> DefinitionClause::get_subclauses() const { return _base_subclauses; }
  std::string DefinitionClause::out() const { return DefinitionClause::out(""); }
  std::string DefinitionClause::out(const std::string s) const
  {
    std::string res;
    // TODO TRYONCE MAY NOT BE THE WAY WE WOU
    if (_rfid) { res += DEF_SCL_RFID_TRYONCE_C; }
    for (const std::string& x : _base_subclauses) { res += DEF_SCL_DELIM_C + x; }
    if (_data) { res += DEF_SCL_DELIM_C DEF_SCL_FIELD_C + s; }
    if (_data && _centered) { res += DEF_SCL_CENTERED_ADD_C; }
    res += DEF_CLAUSE_DELIM_C;
    return res;
  }

  DefinitionClause::operator std::string() const { return DefinitionClause::out(); }
  std::ostream& operator<<(std::ostream& os, DefinitionClause const& v) { os << v.out(); return os; }

  Definition::Definition() {};
  Definition::Definition(const std::string raw)
  {
    // Split the raw into individual clauses and store into its vector clauses.
    std::string temp = raw;
    utility::replace_all(temp, "\n", "");
    
    // Check for both the init characters and the term characters
    if (!temp.starts_with(DEF_INIT_C)) { _last_error="Issue on missing INIT char"; _error=true; return; }
    temp.erase(0, DEF_SYMB_L);
    
    std::vector<std::string> preclauses = utility::split_on(temp, DEF_CLAUSE_DELIM_C);

    if (preclauses.back() != DEF_TERM_C) { _last_error="Issue on missing TERM char"; _error=true; return; }
    preclauses.pop_back();

    _barcode_count = 0;
    _field_count = 0;
    _rfid_count = 0;
    for (const std::string& x : preclauses)
    {
      DefinitionClause tmp(x);
      if (tmp.is_error()) { _last_error=tmp.get_last_error(); _error = true; return; }
      if (tmp.is_barcode()) _barcode_count++;
      if (tmp.is_field()) _field_count++;
      if (tmp.is_rfid()) _rfid_count++;
      _clauses.push_back(tmp);
    }
    _error = false;
    _last_error = false;
  };
  int Definition::save() const { return Definition::save(DEFAULT_DEF_NAME, true); }
  int Definition::save(const std::string name, bool overwrite) const 
  { 
    // Check the file exists, if overwrite, then we will ignore the existing file
    std::string fullpath = DEFAULT_DEF_DIR + name + DEF_EXT;
    if (overwrite && utility::file_exists(fullpath)) { return ERR_FILE_EXISTS; }

    Json::Value out;
    Json::StreamWriterBuilder builder; 
    auto now = std::chrono::system_clock::now();
    out["date_generated"] = now.time_since_epoch().count();
    out["error"] = _error;
    out["barcode_count"] = _barcode_count;
    out["field_count"] = _field_count;
    out["rfid_count"] = _rfid_count;

    // Build the clauses object
    Json::Value clauses_t(Json::arrayValue);
    for (const DefinitionClause& x : _clauses)
    {
      // Set the clause based on individual values
      Json::Value clause;
      clause["barcode"] = x.is_barcode();
      clause["centered"] = x.is_centered();
      clause["data"] = x.is_data();
      clause["error"] = x.is_error();
      clause["field"] = x.is_field();
      clause["rfid"] = x.is_rfid();
      Json::Value subclauses_t(Json::arrayValue);
      for (const std::string& y : x.get_subclauses())
        subclauses_t.append(y);
      clause["subclauses"] = subclauses_t;
      clauses_t.append(clause);
    }
    out["clauses"] = clauses_t;

    std::ofstream ofs(fullpath);
    ofs << Json::writeString(builder, out);
    ofs.close();

    std::cout << "Successfully wrote definition to " << fullpath << "\n";
    return 0;
  }
  size_t Definition::get_barcode_count() const { return _barcode_count; }
  size_t Definition::get_field_count() const { return _field_count; }
  size_t Definition::get_rfid_count() const { return _rfid_count; }
  std::string Definition::get_last_error() const { return _last_error; }
  int Definition::load(const Json::Value json)
  {
    // Set member properties based on file
    _error = json["error"].asBool();
    _barcode_count = json["barcode_count"].asUInt();
    _field_count = json["field_count"].asUInt();
    _rfid_count = json["rfid_count"].asUInt();
    for (size_t i = 0; i < json["clauses"].size(); ++i)
      _clauses.push_back(DefinitionClause(json["clauses"][int(i)]));

    return 0;
  }
  int Definition::load_file() { return Definition::load_file(DEFAULT_DEF_NAME); }
  int Definition::load_file(const std::string name)
  {
    // load the entire file into a json object
    std::string fullpath(DEFAULT_DEF_DIR + name + DEF_EXT); 
    std::ifstream ifs(fullpath);
    std::stringstream buf;
    Json::Value in;
    Json::Reader reader;
    buf << ifs.rdbuf();
    bool parsing_success = reader.parse(buf.str(), in);
    if ( !parsing_success ) {
      std::cout << "[LOGGING REQUIRED] Error in reading def file\n";
      return 1;
    }

    return Definition::load(in);
  }
  bool Definition::is_error() const { return _error; }

  std::string Definition::out() const { return Definition::out(std::vector<std::string>(), std::vector<std::string>(), std::vector<std::string>()); }
  std::string Definition::out(const std::vector<std::string> ss) const { return Definition::out(ss, std::vector<std::string>(), std::vector<std::string>()); }
  std::string Definition::out(const std::vector<std::string> ss, const std::vector<std::string> bs) const { return Definition::out(bs, ss, std::vector<std::string>()); }
  std::string Definition::out(const std::vector<std::string> ss, const std::vector<std::string> bs, const std::vector<std::string> rs) const
  {
    std::string res;
    size_t barcode_i=0;
    size_t fields_i=0;
    size_t rfid_i=0;
    res += DEF_INIT_C;
    for (const DefinitionClause& x : _clauses)
    {
      if (x.is_barcode() && barcode_i < _barcode_count && barcode_i < bs.size())
      {
        res += x.out(bs[barcode_i++]);
      }
      else if (x.is_field() && fields_i < _field_count && fields_i < ss.size())
        res += x.out(ss[fields_i++]);
      else if (x.is_rfid() && rfid_i < _rfid_count && rfid_i < rs.size())
        res += x.out(rs[rfid_i++]);
      else
        res += x.out();
    }
    res += DEF_TERM_C;
    return res;
  }
  std::ostream& operator<<(std::ostream& os, Definition const& v) { os << v.out(); return os; }

  // Retrieves a definition file and then prints the label as is
  int print_label(const Definition& def, const std::vector<std::string> data, const std::vector<std::string> barcode, const std::vector<std::string> rfid) 
  {
    std::ofstream ofs(DEFAULT_CACHED_ZPL1_FN);
    ofs << def.out(data, barcode, rfid);
    ofs.close();
    std::cout << "Successfully wrote definition to " << DEFAULT_CACHED_ZPL1_FN << "\n";

    // Create a controller
    cupslib::Controller cups(DEFAULT_CUPS_PRINTER);
    int status = cups.get_status();
    if (status != 0)
    {
      std::cout << "There was an error with the current cups status" << std::endl;
      return 1;
    }

    status = cups.create_zebra_label_1();
    if (status != 0)
    {
      std::cout << "There was an error printing the zebra label" << std::endl;
      return 1;
    }

    return 0;
  }

  int label_definition(const std::string type, const std::string raw, bool overwrite)
  {
    // check if the label definition already exists
    std::string rel_path = DEFAULT_DEF_DIR + type + DEF_EXT;
    // If the file exists, delete it 
    if (overwrite && utility::file_exists(rel_path) && (std::remove(rel_path.c_str()) != 0))
    {
      std::cout << "[LOGGING REQUIRED] Error in deleting def file\n";
      return 1;
    }
    else if (!overwrite && utility::file_exists(rel_path))
    {
      std::cout << "[LOGGING REQUIRED] File already exists\n";
      return ERR_FILE_EXISTS;
    }
      

    // Now we read the label, and try to create a definition file on it
    std::cout << "[label raw contents of" << type << "]\n" << raw << "\n";
    
    // Let's create the object, in the init, we will generate it all
    Definition def(raw);
    if (def.is_error())
    {
      // Raise error to next level
      std::cout << "[LOGGING REQUIRED] Error in creating def file\n";
      std::cout << "[LOGGING REQUIRED] " << def.get_last_error() << '\n';
      return 1;
    }

    // If the definition is created successfully, we can then save it to a definition file
    int error = def.save(type);
    if (error == ERR_FILE_EXISTS) 
    {
      std::cout << "[LOGGING REQUIRED] Error in saving def file on overwrite\n";
      return ERR_FILE_EXISTS;
    }
    else if (error)
    {
      // Raise error to next level
      std::cout << "[LOGGING REQUIRED] Error in saving def file\n";
      return 1;
    }

    return 0;
  }
}
