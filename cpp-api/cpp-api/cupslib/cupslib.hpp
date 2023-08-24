#ifndef CUPSLIB_HPP
#define CUPSLIB_HPP
#include <string>
#include <cups/cups.h>

#define DEFAULT_TIMEOUT 30000
#define IPP_RESOURCE_S 1<<8

#define MAX_ZPL_BUFFER_S 1<<16
#define DEFAULT_CACHED_ZPL1_FN "../temp/label1.zpl"

namespace cupslib
{
  class Controller
  {
    std::string url;
    public:
      Controller();
      Controller(std::string url);
      static int get_status();
      int create_zebra_label_1();
      int create_zebra_label_1(std::string filename);
    protected:
      static http_t *_http;
      static char _ipp_resource[IPP_RESOURCE_S];
      static cups_dest_t *_dest; 
      static cups_dinfo_t *_info; 
      static cups_option_t *_options;
      static int _num_options;
  };

  struct ControllerStatus
  {
    std::string http;
    std::string printer;
  };
}


#endif /* CUPSLIB_HPP */
