# software/source/serial_controller.py
import logging
import serial
import serial.tools.list_ports
from typing import List, Tuple, Optional

# Assuming Protocol is defined in ./protocol.py
from .protocol import Protocol 

class SerialController:
    """
    @brief Manages serial communication with a device.

    Handles port discovery, connection, disconnection, and sending commands.
    """

    def __init__(self, protocol: Protocol, baudrate: int):
        """
        @brief Initializes the SerialController.

        @param protocol: An instance of the Protocol class for command creation.
        @type protocol: Protocol
        @param baudrate: The baud rate for serial communication.
        @type baudrate: int
        """
        self._protocol = protocol
        self._baudrate = baudrate
        self._serial_port: Optional[serial.Serial] = None  # Holds the pyserial Serial object
        self._selected_port_device: Optional[str] = None # Stores the device name, e.g., 'COM3'

    @property
    def selected_port(self) -> Optional[str]:
        """
        @brief Get the currently selected COM port device name.

        @return: The selected COM port device name, or None if not set.
        @rtype: str | None
        """
        return self._selected_port_device

    @selected_port.setter
    def selected_port(self, port_device: Optional[str]) -> None:
        """
        @brief Set the COM port device name.

        Note: Setting this property does not automatically connect. Call connect() explicitly.

        @param port_device: The COM port device name (e.g., 'COM3', '/dev/ttyUSB0').
        @type port_device: str | None
        @return: None
        """
        self._selected_port_device = port_device

    @property
    def is_connected(self) -> bool:
        """
        @brief Checks if the serial port is currently open and connected.

        @return: True if connected, False otherwise.
        @rtype: bool
        """
        return self._serial_port is not None and self._serial_port.is_open

    def connect(self) -> bool:
        """
        @brief Attempts to open the selected serial port.

        If a port is already open, it will be closed first.
        Sets self._serial_port to the pyserial Serial object on success.

        @return: True if connection was successful, False otherwise.
        @rtype: bool
        """
        if self._serial_port and self._serial_port.is_open:
            self.close_port() # Ensure any existing open port is closed

        if not self._selected_port_device:
            logging.warning("No COM port selected to connect.")
            return False

        try:
            logging.info(f"Attempting to open serial port: {self._selected_port_device} at {self._baudrate} baud.")
            # Timeout for reading: set to a small value, e.g., 1 second
            self._serial_port = serial.Serial(self._selected_port_device, self._baudrate, timeout=1)
            # You might want to send a handshake command here and read a response
            # to verify the device is responsive.
            return True
        except serial.SerialException as e:
            logging.error(f"Could not open serial port {self._selected_port_device}: {e}")
            self._serial_port = None  # Ensure it's None on failure
            return False
        except Exception as e:
            logging.error(f"An unexpected error occurred during serial connection: {e}", exc_info=True)
            self._serial_port = None
            return False

    def close_port(self) -> None:
        """
        @brief Closes the currently open serial port, if any.

        @return: None
        """
        if self._serial_port and self._serial_port.is_open:
            logging.info(f"Closing serial port: {self._selected_port_device}")
            try:
                self._serial_port.close()
            except serial.SerialException as e:
                logging.error(f"Error closing serial port {self._selected_port_device}: {e}")
            finally:
                self._serial_port = None
        elif self._serial_port: # Port object exists but isn't open (e.g., failed connect)
            self._serial_port = None


    def send_command(self, command_bytes: List[int], expect_response_bytes: int = 0) -> Optional[bytes]:
        """
        @brief Sends a list of integer bytes as a command over the serial port.
               Optionally reads a response after sending.

        Logs a warning if the port is not connected.

        @param command_bytes: A list of integers representing bytes to send.
        @type command_bytes: List[int]
        @param expect_response_bytes: Number of bytes to read as a response. If 0, no response is read.
        @type expect_response_bytes: int
        @return: The received bytes response, or None if no response expected/received/error.
        @rtype: bytes | None
        """
        if self._serial_port and self._serial_port.is_open:
            try:
                # Convert list of ints to bytes object for pyserial
                self._serial_port.write(bytes(command_bytes))
                logging.debug(f"Sent: {command_bytes}") # Useful for debugging serial communication
                
                if expect_response_bytes > 0:
                    # Read the specified number of bytes. This will block up to `timeout` if no data.
                    response = self._serial_port.read(expect_response_bytes)
                    logging.debug(f"Received: {response}")
                    # Validate the received response length
                    if len(response) != expect_response_bytes:
                        logging.warning(f"Expected {expect_response_bytes} bytes, but received {len(response)}.")
                        return None # Indicate incomplete response
                    return response
                return None
            except serial.SerialException as e:
                logging.error(f"Error sending command to {self._selected_port_device} or reading response: {e}. Disconnecting.")
                self.close_port() # Assume connection dropped
                return None
            except Exception as e:
                logging.error(f"An unexpected error occurred while sending command or reading response: {e}", exc_info=True)
                self.close_port()
                return None
        else:
            logging.warning("Serial port not connected. Command not sent.")
            return None

    def get_available_ports(self) -> List[Tuple[str, str]]:
        """
        @brief Discovers and returns a list of available serial ports.

        @return: A list of tuples, where each tuple contains (device_name, description).
        @rtype: List[Tuple[str, str]]
        """
        available_ports = []
        for port in serial.tools.list_ports.comports():
            available_ports.append((port.device, port.description))
        return available_ports