# software/source/toolbar_menu.py
import logging
import sys
import threading
import tkinter as tk
from functools import partial
from tkinter import colorchooser
from typing import List, Tuple, Any

from pystray import Icon, Menu, MenuItem
from PIL import Image 

from .protocol import Protocol
from .settings_manager import SettingsManager
from .serial_controller import SerialController
from .icon_creator import IconCreator


class ToolbarMenu:
    """
    @brief Main application class for the system tray menu.

    Manages application state, UI interactions, settings, and communication
    with the NeoPixel device.
    """

    def __init__(self, title: str = "NeoPixel Status"):
        """
        @brief Initializes the ToolbarMenu application.

        @param title: The title to display for the system tray icon.
        @type title: str
        """
        self.title = title
        self._protocol = Protocol()
        self._icon_creator = IconCreator(limit_value_func=self._protocol.limit_value)

        self._default_app_settings = {
            "default": {
                "primary": "#3c3c3c",
                "accent": "#ffffff"
            },
            "config": {
                "dimmer_factor": 0.5,
                "baudrate": 115200,
                "com_port": None
            },
            "dnd": { "primary": "#ff0000" },
            "busy": { "primary": "#ffae00" },
            "available": { "primary": "#009600" },
            "off": { "primary": "#3c3c3c" } # Explicitly define 'off' color for consistency
        }

        self._settings_manager = SettingsManager(self._default_app_settings)
        self._app_settings = self._settings_manager.settings

        initial_baud_rate = int(self._app_settings["config"]["baudrate"])
        self._serial_controller = SerialController(self._protocol, initial_baud_rate)
        self._serial_controller.selected_port = self._app_settings["config"]["com_port"]

        self._current_status: str = "off"  # Start in 'off' mode by default
        self._current_brightness_factor: float = 1.0
        self._is_connected: bool = False  # Track serial connection status

        self._status_to_mode_map = {
            "dnd": self._protocol.MODE_DND,
            "busy": self._protocol.MODE_BUSY,
            "available": self._protocol.MODE_AVAILABLE,
            "off": self._protocol.MODE_OFF
        }
        self._mode_to_status_map = {v: k for k, v in self._status_to_mode_map.items()}

        self._tk_root = tk.Tk()
        self._tk_root.withdraw()
        self._tk_root.protocol("WM_DELETE_WINDOW", self.on_exit)

        self.icon = Icon(
            self.title,
            self._create_icon_image_from_status(self._current_status),
            menu=Menu(
                MenuItem('Status', Menu(
                    MenuItem('Do Not Disturb', partial(self._set_status_mode, 'dnd'),
                             checked=lambda item: self._current_status == 'dnd'),
                    MenuItem('Busy', partial(self._set_status_mode, 'busy'),
                             checked=lambda item: self._current_status == 'busy'),
                    MenuItem('Available', partial(self._set_status_mode, 'available'),
                             checked=lambda item: self._current_status == 'available'),
                    MenuItem('Off', self._clear_leds,
                             checked=lambda item: self._current_status == 'off')
                ), enabled=lambda item: self._is_connected), # Disable if not connected
                Menu.SEPARATOR,
                MenuItem('Brightness', Menu(
                    MenuItem('Dimmer', partial(self._adjust_brightness,
                                                self._app_settings["config"]["dimmer_factor"])),
                    MenuItem('Normal', partial(self._adjust_brightness, 1.0)),
                    MenuItem('Brighter', partial(self._adjust_brightness, 1.3))
                ), enabled=lambda item: self._is_connected), # Disable if not connected
                Menu.SEPARATOR,
                MenuItem('Set Colors', Menu(
                    MenuItem('Do Not Disturb', partial(self._pick_color_for_status, 'dnd')),
                    MenuItem('Busy', partial(self._pick_color_for_status, 'busy')),
                    MenuItem('Available', partial(self._pick_color_for_status, 'available'))
                ), enabled=lambda item: self._is_connected), # Disable if not connected
                Menu.SEPARATOR,
                MenuItem('COM Port', Menu(self._create_com_port_menu_items)), # Always enabled
                Menu.SEPARATOR,
                MenuItem('Exit', self.on_exit) # Always enabled
            )
        )
        # Removed the initial _set_status_mode("available") call here.
        # Status will be set to 'available' upon successful COM port connection.

    def _create_icon_image_from_status(self, status: str) -> Image.Image:
        """
        @brief Creates a PIL Image for the system tray icon based on the current status.

        The icon's colors are derived from the application settings for the given status,
        and adjusted for the current brightness factor.

        @param status: The current application status (e.g., "available", "dnd", "off").
        @type status: str
        @return: A PIL Image object representing the icon.
        @rtype: Image.Image
        """
        colors_data = self._app_settings.get(status, self._app_settings["default"])
        primary_hex = colors_data.get("primary", self._app_settings["default"]["primary"])

        derived_accent_rgb = self._icon_creator.derive_accent_color(
            self._icon_creator.hex_to_rgb(primary_hex)
        )
        derived_accent_hex = self._icon_creator.rgb_to_hex(derived_accent_rgb)

        return self._icon_creator.create_icon_image(
            primary_hex, derived_accent_hex, self._current_brightness_factor
        )

    def _send_command_to_device(self, command: int, command_val: int,
                                red: int, green: int, blue: int, brightness: int) -> None:
        """
        @brief Constructs and sends a command to the NeoPixel device via serial.
               Expects and attempts to parse a 9-byte response.

        @param command: The main command byte for the protocol.
        @type command: int
        @param command_val: The command-specific value.
        @type command_val: int
        @param red: Red component of the color (0-255).
        @type red: int
        @param green: Green component of the color (0-255).
        @type green: int
        @param blue: Blue component of the color (0-255).
        @type blue: int
        @param brightness: Brightness value (0-255).
        @type brightness: int
        @return: None
        """
        cmd_list = self._protocol.create_command(
            command=command,
            command_val=command_val,
            red=red,
            green=green,
            blue=blue,
            brightness=brightness
        )
        # Expecting a 9-byte response array
        response = self._serial_controller.send_command(cmd_list, expect_response_bytes=9)
        
        if response is not None and len(response) == 9:
            logging.info(f"Command sent: {bytes(cmd_list).hex().upper()}. Device response: {response.hex().upper()}")

            # Parse the response structure (SOF, Response Code, comc, R, G, B, Brightness, CRC, EOF)
            response_sof = response[0]
            response_code = response[1]
            response_comc = response[2]
            response_red = response[3]
            response_green = response[4]
            response_blue = response[5]
            response_brightness = response[6]
            response_crc = response[7]
            response_eof = response[8]

            # Basic validation of the response packet structure
            if response_sof != self._protocol.SOF:
                logging.error(f"Response SOF error: Expected {self._protocol.SOF:02x}, got {response_sof:02x}.")
                return
            if response_eof != self._protocol.EOF:
                logging.error(f"Response EOF error: Expected {self._protocol.EOF:02x}, got {response_eof:02x}.")
                return

            # CRC calculation for the response packet
            calculated_crc = (response_code + response_comc + response_red + 
                              response_green + response_blue + response_brightness) % 256
            if calculated_crc != response_crc:
                logging.error(f"Response CRC mismatch: Calculated {calculated_crc:02x}, got {response_crc:02x}.")
                # Even if CRC mismatches, we can still try to interpret the response code
            
            # Interpret the primary response code
            if response_code == self._protocol.RESP_SUCCESS:
                logging.info("Device acknowledged command: SUCCESS.")
                # If it's a GET_COLOR command, the response contains color/brightness
                if command == self._protocol.CMD_GET_COLOR:
                    # This is where you would process the received color/brightness
                    # E.g., print it, or update internal state if needed
                    logging.info(f"GET_COLOR response - Mode: {response_comc}, Color: ({response_red}, {response_green}, {response_blue}), Brightness: {response_brightness}")
            elif response_code == self._protocol.RESP_SOF_ERROR:
                logging.error("Device response: Start of Frame error (from device perspective).")
            elif response_code == self._protocol.RESP_COMMAND_ERROR:
                logging.error("Device response: Command error (invalid command code sent by PC).")
            elif response_code == self._protocol.RESP_COMMAND_VALUE_ERROR:
                logging.error("Device response: Command Value error (invalid value for command sent by PC).")
            elif response_code == self._protocol.RESP_CRC_ERROR:
                logging.error("Device response: CRC error (checksum mismatch in command sent by PC).")
            elif response_code == self._protocol.RESP_EOF_ERROR:
                logging.error("Device response: End of Frame error (in command sent by PC).")
            else:
                logging.warning(f"Device response: Unknown response code {response_code:02x}.")
        else:
            logging.warning(f"Failed to receive a valid response from the device after sending command: "
                           f"{bytes(cmd_list).hex().upper()}. Response was: {response.hex().upper() if response else 'None'}")

    def _create_com_port_menu_items(self) -> Tuple["MenuItem", ...]:
        """
        @brief Creates a list of MenuItem objects for the COM port selection menu.

        This function dynamically generates menu items based on available serial ports,
        allowing the user to select the active COM port.

        @return: A tuple of MenuItem objects for the COM port submenu.
        @rtype: Tuple["MenuItem", ...]
        """
        ports = self._serial_controller.get_available_ports()
        menu_items: List["MenuItem"] = []
        if not ports:
            menu_items.append(MenuItem("No COM ports found", None, enabled=False))
        else:
            for port_device, port_description in ports:
                display_name = f"{port_device} ({port_description})" \
                               if port_description else port_device
                
                action_callback = partial(self._select_com_port, port_device=port_device)
                checked_callback = \
                    lambda item_ref, p=port_device: self._serial_controller.selected_port == p

                menu_items.append(
                    MenuItem(
                        display_name, 
                        action_callback,
                        checked=checked_callback
                    )
                )
        return tuple(menu_items)

    def _select_com_port(self, icon: Any, item: Any, port_device: str) -> None:
        """
        @brief Handles the selection of a COM port from the menu.

        Attempts to connect to the selected port. On successful connection,
        updates application status to 'available' and enables control menus.
        On failure, maintains 'off' status and keeps control menus disabled.

        @param icon: The pystray Icon object.
        @type icon: "Icon"
        @param item: The pystray MenuItem object that was clicked.
        @type item: "MenuItem"
        @param port_device: The device name of the selected COM port.
        @type port_device: str
        @return: None
        """
        logging.info(f"Attempting to select COM port: {port_device}")

        # If a different port is selected or current port is not connected, try to close
        if self._serial_controller.selected_port != port_device or not self._serial_controller.is_connected:
            if self._serial_controller.is_connected:
                self._serial_controller.close_port()
                self._is_connected = False
                logging.info(f"Closed previous connection to {self._serial_controller.selected_port}.")
            
            self._serial_controller.selected_port = port_device
            self._app_settings["config"]["com_port"] = port_device # Save chosen port

            if self._serial_controller.connect(): # Assuming connect() attempts to open and returns success
                self._is_connected = True
                logging.info(f"Successfully connected to COM port: {self._serial_controller.selected_port}")
                # Transition to 'available' after successful connection
                self._set_status_mode("available") # This will update icon and send command
            else:
                self._is_connected = False
                logging.error(f"Failed to connect to COM port: {self._serial_controller.selected_port}")
                # Revert to 'off' status and icon
                self._current_status = "off"
                self.icon.icon = self._create_icon_image_from_status(self._current_status)
                self._serial_controller.selected_port = None # Clear selected port if connection failed
                self._app_settings["config"]["com_port"] = None # Also clear from settings
                logging.info("Status reverted to 'off' due to connection failure.")
        else:
            logging.info(f"COM port {port_device} is already selected and connected.")

        self._settings_manager.save_settings()
        icon.update_menu() # Update menu enabled/disabled states

    def _pick_color_for_status(self, status_key: str, icon: Any, item: Any) -> None:
        """
        @brief Initiates the color picker dialog for a given status.

        @param status_key: The key representing the status (e.g., 'dnd', 'busy', 'available').
        @type status_key: str
        @param icon: The pystray Icon object.
        @type icon: "Icon"
        @param item: The pystray MenuItem object that was clicked.
        @type item: "MenuItem"
        @return: None
        """
        logging.info(f"Opening color picker for '{status_key}' status...")
        self._tk_root.after(0, partial(self._run_color_picker_on_tk_thread, icon, status_key))

    def _run_color_picker_on_tk_thread(self, icon_ref: Any, status_key: str) -> None:
        """
        @brief Runs the tkinter color chooser dialog on the main Tkinter thread.

        This is necessary because the color chooser is a Tkinter widget and must be
        run on the same thread as the Tkinter root.

        @param icon_ref: The pystray Icon object to update after color selection.
        @type icon_ref: "Icon"
        @param status_key: The key for which the color is being picked.
        @type status_key: str
        @return: None
        """
        self._tk_root.deiconify()
        self._tk_root.attributes('-topmost', True)
        self._tk_root.lift()
        self._tk_root.focus_force()

        current_primary_hex = self._app_settings.get(status_key, {}).get(
            'primary', self._default_app_settings["default"]["primary"]
        )
        initial_color_rgb_tuple = self._icon_creator.hex_to_rgb(current_primary_hex)
        
        color_rgb_float_tuple, hex_color_str = colorchooser.askcolor(
            initialcolor=initial_color_rgb_tuple,
            parent=self._tk_root,
            title=f"Select Primary Color for {status_key.replace('_', ' ').title()}"
        )
        
        self._tk_root.attributes('-topmost', False)
        self._tk_root.withdraw()

        if hex_color_str:
            logging.info(f"Selected primary color for '{status_key}': {hex_color_str}")
            self._set_status_primary_color(icon_ref, status_key, hex_color_str)
        else:
            logging.info(f"Color selection cancelled for '{status_key}'.")

    def _set_status_primary_color(self, icon_ref: Any, status_key: str, primary_hex_color: str) -> None:
        """
        @brief Sets the primary color for a specific status and updates the device.

        This also derives an accent color, saves settings, and sends the new color
        to the NeoPixel device if a corresponding protocol mode exists.

        @param icon_ref: The pystray Icon object to update.
        @type icon_ref: "Icon"
        @param status_key: The key representing the status to update.
        @type status_key: str
        @param primary_hex_color: The new primary color in hexadecimal format.
        @type primary_hex_color: str
        @return: None
        """
        if status_key not in self._app_settings:
            logging.error(f"Attempted to set color for unknown status key: '{status_key}'")
            return

        self._app_settings[status_key]["primary"] = primary_hex_color
        
        derived_accent_rgb = self._icon_creator.derive_accent_color(
            self._icon_creator.hex_to_rgb(primary_hex_color)
        )
        self._app_settings[status_key]["accent"] = self._icon_creator.rgb_to_hex(derived_accent_rgb)
        
        logging.info(f"Set primary color for '{status_key}' to {primary_hex_color}")
        logging.info(f"Derived accent color for '{status_key}' to "
                     f"{self._app_settings[status_key]['accent']}")
        
        mode = self._status_to_mode_map.get(status_key)
        if mode is not None:
            base_rgb = self._icon_creator.hex_to_rgb(primary_hex_color)
            adjusted_red, adjusted_green, adjusted_blue = (
                self._icon_creator._adjust_rgb_brightness(base_rgb, self._current_brightness_factor)
            )
            brightness_for_protocol = self._protocol.limit_value(
                int(self._current_brightness_factor * 255), 0, 255
            )

            self._send_command_to_device(
                command=self._protocol.CMD_STORE_COLOR,
                command_val=mode,
                red=adjusted_red,
                green=adjusted_green,
                blue=adjusted_blue,
                brightness=brightness_for_protocol
            )
            logging.info(f"Sent STORE_COLOR command for mode {mode} "
                         f"with primary color update: {primary_hex_color}")
        else:
            logging.warning(f"No protocol mode found for status key: '{status_key}'. "
                            "Color not stored on device.")

        self._settings_manager.save_settings()
        
        if self._current_status == status_key:
            self.icon.icon = self._create_icon_image_from_status(self._current_status)
            self.icon.update_menu()
            logging.info(f"Updated pystray icon to reflect new color for '{status_key}'.")

    def _set_status_mode(self, status: str, icon: Any = None, item: Any = None) -> None:
        """
        @brief Sets the application's current status mode and updates the device.

        Updates the system tray icon and sends the corresponding mode command to the NeoPixel device.

        @param status: The new status to set (e.g., "available", "dnd", "busy", "off").
        @type status: str
        @param icon: The pystray Icon object (optional, used for menu updates).
        @type icon: "Icon"
        @param item: The pystray MenuItem object (optional).
        @type item: "MenuItem"
        @return: None
        """
        if status not in self._status_to_mode_map:
            logging.error(f"Attempted to set unknown status: '{status}'.")
            return

        self._current_status = status
        logging.info(f"Application status changed to: {status.replace('_', ' ').title()}")
        
        self.icon.icon = self._create_icon_image_from_status(self._current_status)
        
        mode = self._status_to_mode_map.get(status)
        if mode is not None:
            self._send_command_to_device(
                command=self._protocol.CMD_SET_MODE,
                command_val=mode,
                red=0, green=0, blue=0,
                brightness=0
            )
            logging.info(f"Sent SET_MODE command for mode {mode}.")
        else:
            logging.warning(f"No protocol mode mapped for status: '{status}'. Command not sent.")

        if icon:
            icon.update_menu()

    def _adjust_brightness(self, icon: Any, item: Any, factor: float) -> None:
        """
        @brief Adjusts the brightness of the current status display on the device.

        Updates the internal brightness factor, re-renders the system tray icon,
        and sends a SET_COLOR command to the NeoPixel device with adjusted brightness.

        @param icon: The pystray Icon object.
        @type icon: "Icon"
        @param item: The pystray MenuItem object that was clicked.
        @type item: "MenuItem"
        @param factor: The brightness factor (e.g., 0.5 for dim, 1.0 for normal).
        @type factor: float
        @return: None
        """
        self._current_brightness_factor = factor
        logging.info(f"Brightness adjusted by factor: {factor:.2f}")
        
        self.icon.icon = self._create_icon_image_from_status(self._current_status)
        
        colors = self._app_settings.get(self._current_status, self._app_settings["default"])
        base_primary_hex = colors.get("primary", self._app_settings["default"]["primary"])
        base_primary_rgb = self._icon_creator.hex_to_rgb(base_primary_hex)

        adjusted_red, adjusted_green, adjusted_blue = (
            self._icon_creator._adjust_rgb_brightness(base_primary_rgb, self._current_brightness_factor)
        )

        brightness_for_protocol = self._protocol.limit_value(
            int(self._current_brightness_factor * 255), 0, 255
        )

        self._send_command_to_device(
            command=self._protocol.CMD_SET_COLOR,
            command_val=self._status_to_mode_map.get(self._current_status, self._protocol.MODE_OFF),
            red=adjusted_red,
            green=adjusted_green,
            blue=adjusted_blue,
            brightness=brightness_for_protocol
        )
        logging.info(f"Sent SET_COLOR command to device for brightness adjustment.")

    def _clear_leds(self, icon: Any, item: Any) -> None:
        """
        @brief Turns off the NeoPixel LEDs and updates the application status to 'off'.

        @param icon: The pystray Icon object.
        @type icon: "Icon"
        @param item: The pystray MenuItem object that was clicked.
        @type item: "MenuItem"
        @return: None
        """
        self._current_status = 'off'
        logging.info("Sending command to clear LEDs (turn off).")

        self._send_command_to_device(
            command=self._protocol.CMD_CLEAR,
            command_val=self._protocol.MODE_OFF, # Or 0x00 based on Main.cpp
            red=0, green=0, blue=0,
            brightness=0
        )
        
        self.icon.icon = self._create_icon_image_from_status(self._current_status)
        icon.update_menu()

    def on_exit(self, icon: Any = None, item: Any = None) -> None:
        """
        @brief Handles the application exit sequence.

        Stops the system tray icon, quits the Tkinter event loop, and exits the application.
        Includes error handling for potential TypeErrors during pystray shutdown.

        @param icon: The pystray Icon object (optional).
        @type icon: "Icon"
        @param item: The pystray MenuItem object (optional).
        @type item: "MenuItem"
        @return: None
        """
        logging.info("Application is shutting down.")
        try:
            if self.icon:
                self.icon.stop()
            if self._tk_root:
                self._tk_root.quit()
            # Ensure serial port is closed on exit
            if self._serial_controller and self._serial_controller.is_connected:
                self._serial_controller.close_port()
        except TypeError as e:
            logging.warning(f"Ignored TypeError during pystray shutdown: {e}")
        except Exception as e:
            logging.error(f"An unexpected error occurred during shutdown: {e}", exc_info=True)
        finally:
            sys.exit(0)

    def run(self) -> None:
        """
        @brief Starts the application's main loop.

        Runs the pystray icon in a separate thread and starts the Tkinter main loop
        to handle UI events.

        @return: None
        """
        logging.info(f"Taskbar menu '{self.title}' is running. Initial status: "
                     f"{self._current_status.title()}")
        
        threading.Thread(target=self.icon.run, daemon=True).start()
        self._tk_root.mainloop()