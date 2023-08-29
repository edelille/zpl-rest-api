# Z.s Drive.ins and Servings

This is a project to detail a rest server to handle prints using Zebra printers with RFID capabilities. Capabilities include, generating general purpose label definition files and printing off of these newly registered printer labels.

# Current Capabilities

- Printing a label via a post request
    - The following will result in a label printed with encoding on the rfid if *testfromAPI_1* has been successfully taught to the API server by the `/label` endpoint
```json
{
    "label_type": "testfromAPI_1",
    "barcode": ["barcode1"],
    "rfid": ["rfid1"],
    "data": [
        "something1",
        "something2",
        "something3",
        "something4",
        "something5",
        "something6"
    ]
}
```
- Teaching a label via a post request (The server learns and remembers the label definition and field descriptors to fill out)
     - This request will teach a label_type to be used for `/print` endpoint
     - The `raw` attribute can be populated with any valid zpl label. This server will strip out the populated fields which can then be repopulated later.
```json
{
    "label_type": "testfromAPI_1",
    "overwrite": true,
    "raw": "^XA^FO50,50^A0N,50^FB700,1,0,C^FDThis is my first Attempt :)\\&^FS^RFW,H^FDHi_RFID_Reader!-Lam^FS^FO195,115^BCN,100,Y,N,N^FDfofofo is no mo^FS^FO50,290^A0N,30^FB700,1,0,C^FDSince this doesn't use ink\\&^FS^FO50,335^A0N,30^FB700,1,0,C^FDI can write as much as I want here\\&^FS^FO50,380^A0N,30^FB700,1,0,C^FDWowie\\&^FS^FO50,425^A0N,30^FB700,1,0,C^FDPlease enjoy game!\\&^FS^FO440,850^GE100,300,10,B^FS^FO430,850^GB70,300,50,W^FS^FO300,650^GE177,177,4,B^FS^FO303,653^GE171,171,100,W^FS^FO400,550^GE177,177,4,B^FS^FO403,553^GE171,171,100,W^FS^FO500,650^GE177,177,4,B^FS^FO503,653^GE171,171,100,W^FS^FO400,750^GE177,177,4,B^FS^FO403,753^GE171,171,100,W^FS^FO435,700^GE110,100,100,B^FS^FO220,950^A0N,30^FD^FS^XZ"
}
```
- Protection features to printer
    - LIMIT 1 attempt on any label
    Usually, Zebra would attempt 3 times upon failed RFID write.
    - Static and Explicit requirements for each label
    Each label will have a certain amount of fields, for the label above, we have 6/1/1 [standard/barcodes/rfid] fields. The `/print` endpoint will not print unless the explicit requirements are met.

# Planned features
- Logging engine (v0.1.x) 
- More robust error checking (v0.1.x)
- CUPS server management (v0.1.x)
- Connection tracking (v0.1.x)
