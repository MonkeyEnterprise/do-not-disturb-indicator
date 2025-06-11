# NeoPixel Status Tray Application

A Windows system tray application for controlling a NeoPixel device via serial port.  
Easily set your status (Do Not Disturb, Busy, Available, Off), adjust brightness, and customize colors.

## Features

- System tray icon with status indicator
- Selectable statuses: Do Not Disturb, Busy, Available, Off
- Adjustable brightness (Dimmer, Normal, Brighter)
- Custom color picker for each status
- COM port selection menu
- Settings saved to `settings.json`
- Logging to `main.log` and console

## Requirements

- Python 3.13+
- [pyserial](https://pypi.org/project/pyserial/)
- [pystray](https://pypi.org/project/pystray/)
- [Pillow](https://pypi.org/project/Pillow/)

Install dependencies with:

```sh
pip install -r requirements.txt
```

## Usage

1. Connect your NeoPixel device to a serial port.
2. Run the application:
3. Use the tray icon to select your COM port and control the device.

## Project Structure

- `main.py` — Application entry point
- `source/toolbar_menu.py` — Main tray menu logic
- `source/serial_controller.py` — Serial port communication
- `source/protocol.py` — Protocol for device commands
- `source/icon_creator.py` — Icon image generation
- `source/settings_manager.py` — Settings load/save logic

## Settings

Settings are stored in `settings.json` and include:

- Colors for each status
- Last used COM port
- Brightness factor

## Logging

Logs are written to `main.log` and the console for troubleshooting.

---

*Developed for NeoPixel status indication via system tray.*
