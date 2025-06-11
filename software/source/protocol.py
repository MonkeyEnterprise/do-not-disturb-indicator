# software/source/protocol.py
from typing import List

class Protocol:
    """
    @brief Defines the serial communication protocol with the NeoPixel controller.

    This class encapsulates the structure of commands and provides methods
    to create command bytes for various operations.
    """

    # --- Protocol Constants ---
    SOF = 0x01  # Start Of Frame byte
    EOF = 0x01  # End Of Frame byte

    # --- Command Codes (comc) ---
    CMD_SET_COLOR = 0x01    # Set the current color and brightness
    CMD_CLEAR = 0x02        # Clear the LED strip (turn off)
    CMD_SET_MODE = 0x03     # Change the active mode (0, 1, or 2)
    CMD_STORE_COLOR = 0x04  # Store color and brightness for a specific mode
    CMD_GET_COLOR = 0x05    # Query the current color and brightness (expected response)

    # --- Command Values (comv) for Modes ---
    MODE_DND = 0x00      # Do Not Disturb mode
    MODE_BUSY = 0x01     # Busy mode
    MODE_AVAILABLE = 0x02  # Available mode
    MODE_OFF = 0xFF      # A special mode value for "off" (used with CMD_CLEAR)
                         # Note: While CMD_CLEAR uses 0x00 for comv in example,
                         # your _clear_leds in ToolbarMenu uses MODE_OFF (0xFF).
                         # Ensure consistency with Main.cpp firmware logic.
                         # If 0x00 is correct for CMD_CLEAR's comv, you might need
                         # a separate constant or adjust _clear_leds.

    # --- Response Codes ---
    RESP_SUCCESS = 0x00             # Command processed successfully
    RESP_SOF_ERROR = 0x01           # Start of Frame byte was incorrect
    RESP_COMMAND_ERROR = 0x02       # Invalid Command code
    RESP_COMMAND_VALUE_ERROR = 0x03 # Invalid Command Value for the given Command
    RESP_CRC_ERROR = 0x04           # Checksum (CRC) mismatch
    RESP_EOF_ERROR = 0x05           # End of Frame byte was incorrect

    def create_command(self, command: int, command_val: int,
                       red: int, green: int, blue: int, brightness: int) -> List[int]:
        """
        @brief Creates a 9-byte serial command packet based on the protocol.

        @param command: The command code (e.g., CMD_SET_COLOR).
        @type command: int
        @param command_val: The command-specific value (e.g., mode, or 0x00).
        @type command_val: int
        @param red: Red component of the color (0-255).
        @type red: int
        @param green: Green component of the color (0-255).
        @type green: int
        @param blue: Blue component of the color (0-255).
        @type blue: int
        @param brightness: Brightness value (0-255).
        @type brightness: int
        @return: A list of 9 integers representing the command bytes.
        @rtype: List[int]
        """
        # Ensure values are within the valid 0-255 range (0x00-0xFF)
        red = self.limit_value(red, 0, 255)
        green = self.limit_value(green, 0, 255)
        blue = self.limit_value(blue, 0, 255)
        brightness = self.limit_value(brightness, 0, 255)

        # Ensure command and command_val are within their specified ranges
        command = self.limit_value(command, 0x01, 0x05) # Clamp to known command codes
        # Command value range depends on the command, but 0x00-0xFF is a safe byte range
        command_val = self.limit_value(command_val, 0x00, 0xFF)

        # CRC Calculation: Sum of bytes 1-6 (Command to Brightness), modulo 256
        crc = (command + command_val + red + green + blue + brightness) % 256

        command_packet = [
            self.SOF,
            command,
            command_val,
            red,
            green,
            blue,
            brightness,
            crc,
            self.EOF
        ]
        return command_packet

    def limit_value(self, value: int, min_val: int, max_val: int) -> int:
        """
        @brief Limits an integer value to a specified range.

        @param value: The integer value to limit.
        @type value: int
        @param min_val: The minimum allowed value.
        @type min_val: int
        @param max_val: The maximum allowed value.
        @type max_val: int
        @return: The limited integer value.
        @rtype: int
        """
        return max(min_val, min(value, max_val))