package main

import (
	"fmt"
	"log"
	"strings"
)

func logOnErr(e error) {
	if e != nil {
		logToFile("error", "%s", e)
	}
}

func logFatalOnErr(e error) {
	if e != nil {
		logToFile("error", "%s", e)
		log.Fatal(e)
	}
}

func panicOnErr(e error) {
	if e != nil {
		panic(e)
	}
}

func logToFile(f, m string, args ...any) {
	fn := strings.ToUpper(f)
	mf := fmt.Sprintf(m, args...)
	fmt.Printf("[%s]\t%s\n", fn, mf)
	log.Printf("[%s]\t%s\n", fn, mf)
}
