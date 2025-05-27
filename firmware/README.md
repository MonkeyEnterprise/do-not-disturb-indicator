# Communication Protocol

This document explains the serial communication protocol and Arduino logic to control the do not disturb indicator. The controller stores mode configurations and supports fade transitions.

---

## Serial Settings

| Setting       | Value    |
| ------------- | -------- |
| **Baudrate**  | `115200` |
| **Data Bits** | `8`      |
| **Parity**    | `None`   |
| **Stop Bits** | `1`      |

---

## Packet Format

Each serial command sent to the device must be **9 bytes** in length and follow this structure:

| command       | Value    |
| ------------- | -------- |
| **set mode**  | `0xA0`   |
| **set color** | `0xA1`   |

```text
+------------+-----+-----+-----+------------+--------------+----------------+-----------+
| Start Byte |  R  |  G  |  B  | Brightness | Fade Time MS |   Checksum     | Stop Byte |
+------------+-----+-----+-----+------------+--------------+----------------+-----------+
|  command   |0–255|0–255|0–255|   0–255    |   0–65535    | (R+G+B+Br)%256 |   0x01    |
+------------+-----+-----+-----+------------+--------------+----------------+-----------+
```

* **Start Byte**: Indicates the beginning of a packet including a command.
* **R, G, B**: RGB color values (0–255 each).
* **Brightness**: Global brightness level (0–255).
* **Fade Time**: 16-bit little-endian fade time in milliseconds.
* **Checksum**: Calculated as `(R + G + B + Brightness) % 256`.
* **Stop Byte (0x01)**: Marks the end of the packet.

---

## Behavior

* The RGB color and brightness will transition to the new value over the given fade time.
* The last received values are stored and used for resuming on restart.
* Invalid packets (wrong start/stop byte, wrong checksum) are ignored.

---

## Example Packet

To fade to blue (`R=0`, `G=0`, `B=255`) at 50% brightness (`128`) over 1 second (`1000 ms`):

```text
Start Byte:      0xA1
R:               0
G:               0
B:               255
Brightness:      128
Fade Time:       0xE8 0x03  (1000 in little endian)
Checksum:        (0 + 0 + 255 + 128) % 256 = 127
Stop Byte:       0x01
```

**Raw bytes**:

```text
0xFF  0x00  0x00  0xFF  0x80  0xE8  0x03  0x7F  0x01
```
