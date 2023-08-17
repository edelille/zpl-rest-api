package main

import (
	"bytes"
	"fmt"
	"io"
	"net/http"

	ipp "github.com/phin1x/go-ipp"
)

// try to print
func test() {
	req := ipp.NewRequest(ipp.OperationPrintJob, 1)
	req.OperationAttributes[ipp.AttributeCharset] = "utf-8"
	req.OperationAttributes[ipp.AttributeNaturalLanguage] = "en"
	req.OperationAttributes[ipp.AttributePrinterURI] = "localhost:631"
	req.OperationAttributes[ipp.AttributeRequestingUserName] = "Lambo"
	req.OperationAttributes[ipp.AttributeDocumentFormat] = "application/octet-stream"

	payload, err := req.Encode()
	panicOnErr(err)

	// append our test file
	payload = append(payload, TEST_ZPL_FILE...)

	httpReq, err := http.NewRequest(
		"POST",
		"http://localhost:631/printers/ZebraPrinterTesting",
		bytes.NewBuffer(payload),
	)
	panicOnErr(err)

	httpClient := &http.Client{}
	httpResp, err := httpClient.Do(httpReq)
	panicOnErr(err)
	defer httpResp.Body.Close()

	if httpResp.StatusCode != http.StatusOK {
		errStr := fmt.Sprintf("[error]: %d error code", httpResp.StatusCode)
		panic(errStr)
	}

	buf, err := io.ReadAll(httpResp.Body)
	panicOnErr(err)

	resp, err := ipp.NewResponseDecoder(bytes.NewReader(buf)).Decode(nil)
	panicOnErr(err)

	err = resp.CheckForErrors()
	panicOnErr(err)

	for _, job := range resp.JobAttributes {
		fmt.Println("job: ", job)
	}

	fmt.Println("Successfully submitted print job")
}
