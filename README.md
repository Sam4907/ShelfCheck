# ShelfCheck: A Smart Library Shelf Scanner

## Overview
ShelfCheck is a handheld embedded system designed to detect misplaced books in library shelves using RFID (Radio Frequency Identification) technology. The system enables shelf-specific verification by scanning RFID-tagged books and comparing them against a predefined database mapped to designated shelf locations.

## Features
- Shelf-specific book validation using RFID tags
- Real-time detection of misplaced books
- Visual feedback using LEDs (Green, Red, Yellow)
- Audible alert using buzzer for incorrect placement
- OLED display for system status and book information
- Portable, low-cost embedded design based on Arduino Uno

## Hardware Used
- Arduino Uno R3
- MFRC522 RFID Reader Module
- RFID Keychain Tags (MIFARE-based)
- OLED Display (SSD1306)
- Push Button
- LEDs (Green, Red, Yellow)
- Buzzer
- Supporting passive components

## Working Principle
The system operates in two modes:
1. Shelf Selection Mode: A shelf RFID tag is scanned to set the active shelf context.
2. Scanning Mode: Books are scanned and validated against the stored database for that shelf. Misplaced books trigger visual and audio alerts.

## Future Improvements
- Integration with IoT-based cloud database
- Long-range RFID support
- Mobile application interface for librarians
- PCB-based compact hardware design


## Authors
1. Sameeha Yasmin
2. Reballi Shrika
3. Rishab Puri
4. Rampudi Greeshma
5. Pranav Bheemshetty
Under the guidance of Dr. Venkata Sushma, Assistant Professor, Department of Mechanical Engineering
