#ifndef COMMANDS_H
#define COMMANDS_H

// Command Structure
// +--------------+----------------------+
// | Command Byte | Description          |
// +--------------+----------------------+
// |     0xA1     | Get Firmware Version |
// +--------------+----------------------+
// |     0xA2     | Get Serial Number    |
// +--------------+----------------------+
// |     0xB1     | Set Color            |
// +--------------+----------------------+
// |     0xB2     | Set Mode             |
// +--------------+----------------------+
#define COMMAND_BYTE_IDX 0
#define COMMAND_GET_FIRMWARE_VERSION 0xA1
#define COMMAND_GET_SERIAL_NUMBER 0xA2
#define COMMAND_SET_COLOR 0xB1
#define COMMAND_SET_MODE 0xB2

// Set color Command Structure
// +--------------+------+------+-------+------------+
// | Command Byte | Red  |Green | Blue  | Brightness |
// +--------------+------+------+-------+------------+
// |     0xB1     | 0xFF | 0xFF  | 0xFF | 0xFF       |
// +--------------+------+------+-------+------------+
#define COMMAND_SET_RED_BYTE_IDX 1
#define COMMAND_SET_GREEN_BYTE_IDX 2
#define COMMAND_SET_BLUE_BYTE_IDX 3
#define COMMAND_SET_BRIGHTNESS_BYTE_IDX 4

// Set Mode Command Structure
// +--------------+-------------+
// | Command Byte | Mode        |
// +--------------+-------------+
// |     0xB2     | 0x01 - 0x04 |
// +--------------+-------------+
#define COMMAND_SET_MODE_BYTE_IDX 1

// ------- Response Codes -------
#define RESPONSE_OK 0x01
#define RESPONSE_ERR_COMMAND 0xE1
#define RESPONSE_ERR_MODE 0xE3

#endif // COMMANDS_H
