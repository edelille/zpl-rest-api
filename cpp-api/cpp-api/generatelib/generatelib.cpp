#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <ctime>

#include <filesystem>

#include "./generatelib.hpp"
#include "../utility/utility.hpp"
#include "../jsonlib/json.h"

namespace generatelib
{
  DefinitionClause::DefinitionClause() {};
  DefinitionClause::DefinitionClause(const std::string raw) 
  {
    _field = false;
    std::cout << "[clause]:\t" << raw << '\n';
    std::vector<std::string>
    preclauses = utility::split_on(raw, DEF_SUBCLAUSE_DELIM_C); 
    preclauses.erase(preclauses.begin()); 
    for (const auto& x : preclauses)
    {
      // If the prefix is a FD = field-descriptor, we mark it and keep it out
      // of the base subclasses
      if (x.starts_with(DEF_SUBCLAUSE_FIELD_C)) { _field = true; continue; }
      _base_subclauses.push_back(x);
    }  
  };
  DefinitionClause::DefinitionClause(const Json::Value json) { DefinitionClause::load(json); }

  std::string DefinitionClause::out() const { return DefinitionClause::out(""); }
  std::string DefinitionClause::out(const std::string s) const
  {
    std::string res;
    for (const std::string& x : _base_subclauses) { res += DEF_SUBCLAUSE_DELIM_C + x; }
    if (_field) { res += DEF_SUBCLAUSE_DELIM_C DEF_SUBCLAUSE_FIELD_C + s; }
    res += DEF_CLAUSE_DELIM_C;
    return res;
  }
  int DefinitionClause::load(Json::Value json) 
  { 
    _error = json["error"].asBool();
    _field = json["field"].asBool();
    _base_subclauses.clear();
    for (size_t i = 0; i < json["subclauses"].size(); ++i)
    {
      _base_subclauses.push_back(json["subclauses"][int(i)].asString());
    }
    return 0;
  }

  std::vector<std::string> DefinitionClause::get_subclauses() const { return _base_subclauses; }
  DefinitionClause::operator std::string() const { return DefinitionClause::out(); }
  std::ostream& operator<<(std::ostream& os, DefinitionClause const& v) { os << v.out(); return os; }

  Definition::Definition() {};
  Definition::Definition(const std::string raw)
  {
    // Split the raw into individual clauses and store into its vector clauses.
    std::string temp = raw;
    utility::replace_all(temp, "\n", "");
    
    // Check for both the init characters and the term characters
    if (!temp.starts_with(DEF_INIT_C)) { _error = true; return; }
    temp.erase(0, DEF_SYMB_L);
    
    std::vector<std::string> preclauses = utility::split_on(temp, DEF_CLAUSE_DELIM_C);

    if (preclauses.back() != DEF_TERM_C) { _error = true; return; }
    preclauses.pop_back();

    _field_count = 0;
    for (const auto& x : preclauses)
    {
      DefinitionClause tmp(x);
      if (tmp.is_error()) { _error = true; return; }
      if (tmp.is_field()) _field_count++;
      _clauses.push_back(tmp);
    }
    _error = false;
  };

  std::string Definition::out() const { return Definition::out(std::vector<std::string>()); }
  std::string Definition::out(const std::vector<std::string> ss) const
  {
    std::string res;
    size_t fields_filled = 0;
    res += DEF_INIT_C;
    for (const DefinitionClause& x : _clauses)
    {
      if (x.is_field() && fields_filled < _field_count && fields_filled < ss.size())
      {
        res += x.out(ss[fields_filled++]);
        continue;
      }
      res += x.out();
    }
    res += DEF_TERM_C;
    return res;
  }
  std::ostream& operator<<(std::ostream& os, Definition const& v) { os << v.out(); return os; }

  int Definition::save() const { return Definition::save(DEFAULT_DEF_NAME); }
  int Definition::save(const std::string name) const 
  { 
    Json::Value out;
    Json::StreamWriterBuilder builder; 
    auto now = std::chrono::system_clock::now();
    out["date_generated"] = now.time_since_epoch().count();
    out["error"] = _error;
    out["field_count"] = _field_count;

    // Build the clauses object
    Json::Value clauses_t(Json::arrayValue);
    for (const DefinitionClause& x : _clauses)
    {
      // Set the clause based on individual values
      Json::Value clause;
      clause["field"] = x.is_field();
      Json::Value subclauses_t(Json::arrayValue);
      for (const std::string& y : x.get_subclauses())
        subclauses_t.append(y);
      clause["subclauses"] = subclauses_t;
      clauses_t.append(clause);
    }
    out["clauses"] = clauses_t;

    std::string fullpath = DEFAULT_DEF_DIR + name + DEF_EXT;
    std::ofstream ofs(fullpath);
    ofs << Json::writeString(builder, out);
    ofs.close();

    std::cout << "Successfully wrote definition to " << fullpath << "\n";
    return 0;
  }

  int Definition::load(const Json::Value json)
  {
    // Set member properties based on file
    _error = json["error"].asBool();
    _field_count = json["field_count"].asUInt();
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


  // Retrieves a definition file and then prints the label as is
  int print_label(const std::string deffn) 
  {
    // Find the definition file, and check
    std::string rel_path = DEFAULT_DEF_DIR + deffn;
    if (!utility::file_exists(rel_path))
    {
      // File did not exist
      std::cout << "File " << deffn << " does not exist\n";
      return 1;
    }
    return 0;
  }

  int label_definition(const std::string type, const std::string raw)
  {
    // check if the label definition already exists
    std::string rel_path = DEFAULT_DEF_DIR + type;
    // If the file exists, delete it 
    if (utility::file_exists(rel_path) && (std::remove(rel_path.c_str()) != 0))
    {
      // log an error on failing to delete old definition file
      std::cout << "[LOGGING REQUIRED] Error in deleting def file\n";
    }

    // Now we read the label, and try to create a definition file on it
    std::cout << "[label raw contents of" << type << "]\n" << raw << "\n";
    
    // Let's create the object, in the init, we will generate it all
    Definition def(raw);
    if (def.is_error())
    {
      // Raise error to next level
      std::cout << "[LOGGING REQUIRED] Error in creating def file\n";
      return 1;
    }

    // If the definition is created successfully, we can then save it to a definition file
    int error = def.save(type);
    if (error)
    {
      // Raise error to next level
      std::cout << "[LOGGING REQUIRED] Error in saving def file\n";
      return 1;
    }

    return 0;
  }
}
