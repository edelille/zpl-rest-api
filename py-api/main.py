import cups

if __name__ == '__main__':
    print("Testing CUPS printer")

    conn = cups.Connection ()

    printers = conn.getPrinters()

    for printer in printers:
        print(printer, printers[printer]['device-uri'])
