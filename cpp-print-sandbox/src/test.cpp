#include <string>
#include <cups/cups.h>

#include "test.h"

void testHelloWorld()
{
    printf("Hello World from test.cpp!\n");
}

void printTestFile() 
{
    char printer_name[] = "ZebraPrinterRaw";

    //cups_dest_t *dest;
    int job_id;
    int num_options = 0;
    cups_dest_t *dest, *dests;
    cups_option_t *options;

    // Set the dest on the ZPL printer
    int num_dests = cupsGetDests(&dests);
    dest = cupsGetDest(printer_name, NULL, num_dests, dests);

    // Connect to the Dest
    char resource[] = "ZebraPrinterTesting";
    http_t 
    *http = cupsConnectDest(dest, CUPS_DEST_FLAGS_DEVICE, 30000, NULL, 
                            resource, sizeof(resource), NULL, NULL);

    // Copy Dest Info?
    cups_dinfo_t 
    *info = cupsCopyDestInfo(http, dest);

    // Set our print options
    num_options = cupsAddOption(CUPS_COPIES, "1", num_options, &options);

    // Create a print job
    char title[] = "SomeTitle";
    if (cupsCreateDestJob( http, dest, info, &job_id, title, num_options,
                           options) == IPP_STATUS_OK) 
        printf("[ 1 ]:\tDest Job was created successfully\n");
    else {
        printf("[ 1 ]:\tDest Job failed on creation: %s\n",
                cupsLastErrorString());
        return;
    }

    // Add documents to the job
    char docname[] = "label1.zpl";
    char format[] = "text/plain";
    int lastdoc = 1;
    if (cupsStartDestDocument(  http, dest, info, job_id, docname, format, 
                                num_options, options, lastdoc
                                ) == HTTP_STATUS_CONTINUE) 
        printf("[ 2 ]:\tStart Dest Job succeeded\n");
    else {
        printf("[ 2 ]:\tStart Dest Job failed: %s\n", 
                cupsLastErrorString());
        return;
    }

    // Load the file into the buffer
    char buffer[1<<16];
    int buffer_size = sizeof(buffer);
    size_t bytes;
    char filename[] = "./example/label2.zpl";
    FILE *fp = fopen(filename, "rb");

    // Write the request
    while ((bytes = fread(buffer, 1, buffer_size, fp)) > 0)
    {
        if (cupsWriteRequestData(http, buffer, buffer_size) == HTTP_STATUS_CONTINUE)
            break;
        else {
            printf("[ 3 ]:\tWrite Request failed: %s\n", 
                    cupsLastErrorString());
            return;
        }
    }
    printf("[ 3 ]:\tWrite Request succeeded\n");

    // Finish the document send
    if (cupsFinishDestDocument(http, dest, info) == IPP_STATUS_OK)
        printf("[ 4 ]:\tFinish Dest Document succeeded\n");
    else {
        printf("[ 4 ]:\tFinish Dest Document failed: %s\n", 
               cupsLastErrorString());
        return;
    }

    // Cleanup
    httpClose(http);

}

