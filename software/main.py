import tkinter as tk
from protocol import McuProtocol
from tkinter import colorchooser
import serial.tools.list_ports
import logging


class App:
    def __init__(self, baudrate: int = 9600, timeout: int = 1) -> None:
        """Initialize the application."""
        self.baudrate: int = baudrate
        self.timeout: int = timeout
        
        # Serial port settings
        self.ser_ports: list[str] = self._get_ports()
        self.tunnel: serial.Serial = None

        # Initialize the tkinter root window
        self.root = tk.Tk()
        self.root.title("Do not disturb light - Serial Color Controller")
        self.root.config(bg="#f0f0f0")
        self.root.option_add("*Font", "Helvetica 10")
        
        # Set window size and position it in the center of the screen
        window_width = 500
        window_height = 700
        screen_width = self.root.winfo_screenwidth()
        screen_height = self.root.winfo_screenheight()

        # Calculate the position to center the window
        position_top = int(screen_height / 2 - window_height / 2)
        position_right = int(screen_width / 2 - window_width / 2)

        self.root.geometry(f'{window_width}x{window_height}+{position_right}+{position_top}')
        
        # Button to choose a color from the color picker
        self.color_button = tk.Button(self.root, text="Choose Color", command=self._choose_color)
        self.color_button.pack(pady=20)
        
        self.rgb_label = tk.Label(self.root, text="RGB: (255, 255, 255)", bg="lightgray")
        self.rgb_label.pack(side="bottom", fill="x", padx=10, pady=5)

    def __del__(self) -> None:
        """Destructor to ensure the serial port is closed on exit."""
        self._disconnect()

    def run(self) -> None:
        """Run the main loop of the application."""
        self.root.mainloop()

    def _get_ports(self) -> list[str]:
        """
        Dynamically load available serial ports.
        Returns a list of available ports or an empty list if none are found.
        """
        ports = [port.device for port in serial.tools.list_ports.comports()]
        logging.debug(f"Available ports: {ports}")
        return ports if ports else []

    def _disconnect(self) -> None:
        if self.tunnel and self.tunnel.is_open:
            self.tunnel.close()
            logging.info(f"Serial port {self.tunnel.name} closed.")

    def _hex_to_rgb(self, hex_code: str) -> tuple[int, int, int]:
        """Convert a hex color code to an RGB tuple."""
        hex_code = hex_code.lstrip('#')
        return tuple(int(hex_code[i:i+2], 16) for i in (0, 2, 4))

    def _rgb_to_hex(self, rgb: tuple[int, int, int]) -> str:
        """Convert an RGB tuple to a hex color code."""
        return f"#{''.join(f'{c:02x}' for c in rgb)}"

    def _choose_color(self) -> None:
        """Open a color picker and update the label with the selected color."""
        try:
            # Open the color chooser dialog
            color_code = colorchooser.askcolor(title="Select a Color", parent=self.root)

            if color_code and color_code[1]:  # color_code[1] contains the hex code
                selected_color = color_code[1]
                rgb_values = self._hex_to_rgb(selected_color)
                self.rgb_label.config(text=f"RGB: {rgb_values}")

                logging.info(f"Selected color: {selected_color} | RGB: {rgb_values}")
            else:
                logging.warning("No color selected.")
        except Exception as e:
            logging.error(f"Error selecting color: {e}")

    def open(self, port: str) -> bool:
        """Connect to a given serial port."""
        try:
            self.tunnel = serial.Serial(port=port, baudrate=self.baudrate, timeout=self.timeout)
            logging.info(f"Serial port {port} opened successfully.")
            return True
        except serial.SerialException as e:
            logging.error(f"Error opening serial port {port}: {e}")
            self.tunnel = None
            return False

    def close(self) -> None:
        """Explicitly close the serial port."""
        self._disconnect()


logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

if __name__ == "__main__":
    app = App(baudrate=9600, timeout=1)
    app.run()
