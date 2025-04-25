# Communication protocol

This document explains the serial communication protocol used to control an RGB LED strip via an Arduino. The code is designed to interface with an Adafruit NeoPixel strip and accepts color commands via a specific serial packet structure.

---

## Serial Settings

- **Baud Rate:** `115200`
- **Data Bits:** `8`
- **Parity:** `None`
- **Stop Bits:** `1`

---

## Packet Format

Each command sent to the Arduino must be exactly **7 bytes** long:

```text
+------------+-----+-----+-----+------------+----------+-----------+
| Start Byte |  R  |  G  |  B  | Brightness | Checksum | Stop Byte |
+------------+-----+-----+-----+------------+----------+-----------+
|   0xFF     |0-255|0-255|0-255|   0-255    | 0-255    |   0xFF    |
+------------+-----+-----+-----+------------+----------+-----------+
```

### Fields Explained:

- **Start Byte:** Always `0xFF` to mark the beginning of a packet.
- **R, G, B:** Color intensity values (Red, Green, Blue), range `0–255`.
- **Brightness:** LED brightness, `0–255`.
- **Checksum:** `(R + G + B + Brightness) % 256` to verify integrity.
- **Stop Byte:** Always `0xFF` to mark the end of a packet.

---

## Diagram: Serial Packet Format

```text
      Byte Index:   0     1     2     3        4         5      6
                    ↓     ↓     ↓     ↓        ↓         ↓      ↓
Command Packet:   [0xFF][  R ][  G ][  B ][Brightness][Checksum][0xFF]
                             ↑                      ↑
                       RGB color values     Integrity check
```

---

## Responses from Arduino

After receiving and validating a packet, the Arduino will send back a **1-byte response code**:

| Code (Hex) | Meaning                  |
|------------|--------------------------|
| `0xAA`     | OK – Command accepted    |
| `0xBB`     | Error – Invalid start    |
| `0xCC`     | Error – Invalid stop     |
| `0xDD`     | Error – Invalid checksum |

---

## Error Handling

If a packet is malformed (invalid start/stop bytes or checksum), the Arduino will flush the remaining data in the serial buffer and return an appropriate error code.

---

## Example Command

To set the LEDs to **blue** at 50% brightness:

- R = `0`, G = `0`, B = `255`, Brightness = `128`
- Checksum = `(0 + 0 + 255 + 128) % 256 = 127`

**Command Packet:**

```
[0xFF][0x00][0x00][0xFF][0x80][0x7F][0xFF]
```