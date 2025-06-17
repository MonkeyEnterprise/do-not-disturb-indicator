# Serial Protocol Documentation

This document describes the serial protocol used to communicate with the NeoPixel controller firmware (`Main.cpp`). The protocol is designed for robust communication, including error checking via CRC, and supports commands for setting colors, changing modes, and querying the current state.

## Protocol Structure

Each command sent to the device must be **9 bytes** long and follow this structure:

| Byte Index | Name         | Description                                  | Value Range      |
|------------|--------------|----------------------------------------------|------------------|
| 0          | SOF          | Start of Frame                               | `0x01`           |
| 1          | Command      | Command code (comc)                          | `0x01` - `0x05`  |
| 2          | Command Val  | Command value (comv), e.g. mode              | `0x00` - `0x02`  |
| 3          | Red          | Red component                                | `0x00` - `0xFF`  |
| 4          | Green        | Green component                              | `0x00` - `0xFF`  |
| 5          | Blue         | Blue component                               | `0x00` - `0xFF`  |
| 6          | Brightness   | Brightness                                   | `0x00` - `0xFF`  |
| 7          | CRC          | Checksum: sum of bytes 1-6, modulo 256       |                  |
| 8          | EOF          | End of Frame                                 | `0x01`           |

**CRC Calculation:**  
`CRC = (Command + Command Val + Red + Green + Blue + Brightness) % 256`

## Command Codes

| Code | Name         | Description                                 |
|------|-------------|---------------------------------------------|
| 0x01 | Set Color    | Set the current color and brightness        |
| 0x02 | Clear        | Clear the LED strip                         |
| 0x03 | Set Mode     | Change the active mode (0, 1, or 2)         |
| 0x04 | Store Color  | Store color and brightness for current mode |
| 0x05 | Get Color    | Query the current color and brightness      |

## Example Commands

### 1. Set Color (Red, mode 0, brightness 255)

- **Command:** Set color to red, mode 0, brightness 255  
- **Fields:** SOF=01, Command=01, Command Val=00, R=FF, G=00, B=00, Brightness=FF  
- **CRC:** (01 + 00 + FF + 00 + 00 + FF) % 256 = 0x01 + 0x00 + 0xFF + 0x00 + 0x00 + 0xFF = 0x1FF, 0x1FF % 256 = 0xFF  
- **Bytes:** `01 01 00 FF 00 00 FF FF 01`

### 2. Store Color (Green, mode 1, brightness 128)

- **Command:** Store color green for mode 1, brightness 128  
- **Fields:** SOF=01, Command=04, Command Val=01, R=00, G=FF, B=00, Brightness=80  
- **CRC:** (04 + 01 + 00 + FF + 00 + 80) % 256 = 0x184, 0x184 % 256 = 0x84  
- **Bytes:** `01 04 01 00 FF 00 80 84 01`

### 3. Set Mode (mode 2)

- **Command:** Set mode to 2  
- **Fields:** SOF=01, Command=03, Command Val=02, R=00, G=00, B=00, Brightness=00  
- **CRC:** (03 + 02 + 00 + 00 + 00 + 00) % 256 = 0x05  
- **Bytes:** `01 03 02 00 00 00 00 05 01`

### 4. Get Color (for current mode)

- **Command:** Get color for current mode  
- **Fields:** SOF=01, Command=05, Command Val=00, R=00, G=00, B=00, Brightness=00  
- **CRC:** (05 + 00 + 00 + 00 + 00 + 00) % 256 = 0x05  
- **Bytes:** `01 05 00 00 00 00 00 05 01`

### 5. Clear LEDs

- **Command:** Clear the LED strip  
- **Fields:** SOF=01, Command=02, Command Val=00, R=00, G=00, B=00, Brightness=00  
- **CRC:** (02 + 00 + 00 + 00 + 00 + 00) % 256 = 0x02  
- **Bytes:** `01 02 00 00 00 00 00 02 01`

## Error Codes

The device responds with a response code indicating the result:

| Code | Meaning                |
|------|------------------------|
| 0x0  | Success                |
| 0x1  | Start of Frame error   |
| 0x2  | Command error          |
| 0x3  | Command value error    |
| 0x4  | CRC error              |
| 0x5  | End of Frame error     |

---

For more details, see the implementation in `Main.cpp`.
