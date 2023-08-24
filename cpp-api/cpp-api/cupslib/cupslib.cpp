#include <iostream>
#include <cups/cups.h>

#include "cupslib.hpp"

namespace cupslib
{
  // Static Members
  http_t *Controller::_http; 
  char Controller::_ipp_resource[IPP_RESOURCE_S];
  cups_dest_t *Controller::_dest;
  cups_dinfo_t *Controller::_info;
  cups_option_t *Controller::_options;
  int Controller::_num_options;

  Controller::Controller() : Controller::Controller("localhost") {}
  Controller::Controller(std::string url) : url(url) 
  {
    // Find a pointer to the inker
    cups_dest_t *dests;
    int num_dests = cupsGetDests(&dests);
    _dest = cupsGetDest(url.c_str(), NULL, num_dests, dests);
    if (_dest == nullptr)
    {
      std::cout << "dest is a nullptr" << std::endl;
    }

    // Set up a HTTP connection to the inker
    if (_http == nullptr)
      _http = cupsConnectDest(_dest, 
                              CUPS_DEST_FLAGS_DEVICE, 
                              DEFAULT_TIMEOUT, 
                              NULL,
                              _ipp_resource, 
                              sizeof(_ipp_resource), 
                              NULL, 
                              NULL);
    if (_http == nullptr)
    {
      std::cout << "http is a nullptr" << std::endl;
    }

    _info = cupsCopyDestInfo(_http, _dest);
  }

  // This is WIP, not quite yet
  int Controller::get_status()
  {
    // Get the connection state
    http_status_t http_status = httpGetStatus(_http);
    std::cout << "http_status:\t" << httpStatus(http_status) << '\n';

    // update the info
    _info = cupsCopyDestInfo(_http, _dest);

    // Use default dest connection
    ipp_attribute_t 
    *ready_media = cupsFindDestReady( CUPS_HTTP_DEFAULT,
                                      _dest,
                                      _info, 
                                      CUPS_MEDIA_SOURCE); // Find the correct option?

    return 0;
  }

  // This is a macro to ink a specific label
  int Controller::create_zebra_label_1() { return Controller::create_zebra_label_1(DEFAULT_CACHED_ZPL1_FN); }
  int Controller::create_zebra_label_1(std::string filename)
  {
    // Initialize the job
    int job_id;
    ipp_status_t 
    i_status = cupsCreateDestJob( _http,
                                  _dest,
                                  _info,
                                  &job_id,
                                  "ZebraLabelTitlePending",
                                  _num_options,
                                  _options);
    if (i_status != IPP_STATUS_OK) 
    {
      printf("[ 1 ]:\tJob creation failed: %d/%s\n", i_status, cupsLastErrorString());
      return i_status;
    }

    // We then add documents to our job
    http_status_t 
    h_status = cupsStartDestDocument( _http,
                                      _dest,
                                      _info,
                                      job_id,
                                      "doc0.zpl", // For the sister fn, this will be enumerated
                                      "text/plain",     // Format?
                                      _num_options,
                                      _dest->options,
                                      1);     // int last_document == 1 on last doc in job
    if (h_status != HTTP_STATUS_CONTINUE)
    {
      std::printf("[ 2 ]:\tAdding Docs to Job Failed: %s\n", cupsLastErrorString());
    }

    // Load the file into a buffer
    char buffer[MAX_ZPL_BUFFER_S];
    int buffer_size = sizeof(buffer);
    size_t bytes;
    FILE *fp = fopen(filename.c_str(), "rb");
    if (fp == nullptr) 
    {
      std::cout << "File does not Exist!" << std::endl;
      return 1;
    }

    // Write the request
    while ((bytes = fread(buffer, 1, buffer_size, fp)) > 0)
    {
      http_status_t
      h_status = cupsWriteRequestData(_http, buffer, buffer_size);
      if (h_status != HTTP_STATUS_CONTINUE)
      {
        std::printf("[ 3 ]:\tWrite Request failed: %s\n", cupsLastErrorString());
        break;
      }
    }

    i_status = cupsFinishDestDocument(_http, _dest, _info);
    if (i_status != IPP_STATUS_OK)
    {
      std::printf("[ 4 ]:\tFinish Dest Document failed: %s\n", ippErrorString(i_status));
    }

    return 0;
  }
}
