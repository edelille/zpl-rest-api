#include <iostream>
#include <cups/cups.h>

#include "cupslib.hpp"

namespace cupslib
{
  // Static Members
  http_t *Controller::http; 
  char *Controller::ipp_resource;
  cups_dest_t *Controller::dest;
  cups_dinfo_t *Controller::info;
  cups_option_t *Controller::options;
  int Controller::num_options;

  Controller::Controller() : Controller::Controller("localhost") {}
  Controller::Controller(std::string url) : url(url) {
    // Find a pointer to the inker
    cups_dest_t *dests;
    int num_dests = cupsGetDests(&dests);
    dest = cupsGetDest(url.c_str(), NULL, num_dests, dests);

    // Set up a HTTP connection to the inker
    ipp_resource = new char[IPP_RESOURCE_S]();
    http = cupsConnectDest( dest, 
                            CUPS_DEST_FLAGS_DEVICE, 
                            DEFAULT_TIMEOUT, 
                            NULL,
                            ipp_resource, 
                            sizeof(ipp_resource), 
                            NULL, 
                            NULL);

    info = cupsCopyDestInfo(http, dest);
  }

  // This is WIP, not quite yet
  int Controller::get_status()
  {
    // Get the connection state
    http_status_t http_status = httpGetStatus(http);
    std::cout << "http_status:\t" << httpStatus(http_status) << '\n';

    puts("did I get here?");

    // Use default dest connection
    ipp_attribute_t 
    *ready_media = cupsFindDestReady( CUPS_HTTP_DEFAULT,
                                      dest,
                                      info, 
                                      CUPS_MEDIA_SOURCE); // Find the correct option?
    if (ready_media != NULL) 
    {
      puts("media is ready");
      int i, count = ippGetCount(ready_media);
      for (i = 0; i < count; ++i)
        std::cout << "media:\t" << ippGetInteger(ready_media, i) << "\n";
    } else {
      puts("media is not ready");
    }

    return 0;
  }

  // This is a macro to ink a specific label
  int Controller::create_zebra_label_1() { return Controller::create_zebra_label_1(DEFAULT_CACHED_ZPL1_FN); }
  int Controller::create_zebra_label_1(std::string filename)
  {
    // Initialize the job
    int job_id;
    ipp_status_t 
    i_status = cupsCreateDestJob( http,
                                  dest,
                                  info,
                                  &job_id,
                                  "ZebraLabelTitlePending",
                                  num_options,
                                  options);
    if (i_status != IPP_STATUS_OK) 
    {
      printf("[ 1 ]:\tJob creation failed: %d/%s\n", 
              i_status, cupsLastErrorString());
      return i_status;
    }

    // We then add documents to our job
    http_status_t 
    h_status = cupsStartDestDocument( http,
                                      dest,
                                      info,
                                      job_id,
                                      "doc0", // For the sister fn, this will be enumerated
                                      "",     // Format?
                                      num_options,
                                      dest->options,
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
      h_status = cupsWriteRequestData(http, buffer, buffer_size);
      if (h_status != HTTP_STATUS_CONTINUE)
      {
        std::printf("[ 3 ]:\tWrite Request failed: %s\n", cupsLastErrorString());
        break;
      }
    }

    i_status = cupsFinishDestDocument(http, dest, info);
    if (i_status != IPP_STATUS_OK)
    {
      std::printf("[ 4 ]:\tFinish Dest Document failed: %s\n", cupsLastErrorString());
    }

    return 0;
  }
}
