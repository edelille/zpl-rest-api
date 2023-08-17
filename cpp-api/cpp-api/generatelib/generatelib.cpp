#include "./generatelib.hpp"
#include <iostream>
#include <sys/stat.h>

namespace generatelib
{
  class DefinitionClause
  {

  };
  class Definition
  {

  };

  // Retrieves a definition file and then prints the label as is
  int print_label(const std::string deffn) 
  {
    // Find the definition file, and check
    std::string rel_path = DEFAULT_DEF_DIR + deffn;
    if (!file_exists(rel_path))
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
    if (file_exists(rel_path) && (std::remove(rel_path.c_str()) != 0))
    {
      // log an error on failing to delete old definition file
      std::cout << "[LOGGING REQUIRED] Error in deleting def file\n";
    }

    // Now we read the label, and try to create a definition file on it
    std::cout << "[Test Print " << type << " ]\n" << raw << "\n\n";
    
    // Let's create the object, in the init, we will generate it all
    definition def(raw);

    return 0;
  }

  bool file_exists(const std::string filename)
  {
    struct stat buffer;
    return (stat (filename.c_str(), &buffer) == 0);
  }
}
