from PIL import Image, ImageDraw, ImageColor
from colorsys import rgb_to_hls, hls_to_rgb
from typing import Tuple
import logging

# Assuming Protocol class is available or Protocol methods are inlined/passed
# For this example, we'll assume a dummy limit_value or it's part of the calling context
# If Protocol.limit_value is critical, it should be imported or passed in.
# For now, let's include a local dummy for demonstration or assume it's handled externally.

# Dummy limit_value for standalone testing or if not importing Protocol directly
def _limit_value_dummy(value: int, min_val: int, max_val: int) -> int:
    return max(min_val, min(value, max_val))

class IconCreator:
    """
    Handles the creation and manipulation of system tray icon images.
    It provides methods to generate icons based on primary and accent colors,
    and adjust their brightness.
    """

    def __init__(self, limit_value_func=None):
        """
        Initializes the IconCreator.

        Args:
            limit_value_func (callable, optional): A function to limit integer values.
                                                  Defaults to a simple internal dummy.
        """
        self._limit_value = limit_value_func if limit_value_func else _limit_value_dummy

    def hex_to_rgb(self, hex_color: str) -> Tuple[int, int, int]:
        """Converts a hexadecimal color string to an RGB tuple."""
        try:
            return ImageColor.getrgb(hex_color)
        except ValueError:
            logging.error(f"Invalid hex color format: {hex_color}. "
                          "Using fallback RGB (255, 255, 255).")
            return (255, 255, 255)

    def rgb_to_hex(self, rgb_tuple: Tuple[int, int, int]) -> str:
        """Converts an RGB tuple to a hexadecimal color string."""
        return '#%02x%02x%02x' % rgb_tuple

    def _adjust_rgb_brightness(self, rgb_color: Tuple[int, int, int], factor: float) -> Tuple[int, int, int]:
        """
        Adjusts the brightness of an RGB color by a given factor.

        Args:
            rgb_color (Tuple[int, int, int]): The original RGB color tuple.
            factor (float): The brightness adjustment factor (e.g., 1.0 for normal, 0.5 for dimmer).

        Returns:
            Tuple[int, int, int]: The adjusted RGB color tuple.
        """
        r, g, b = rgb_color
        r = self._limit_value(int(r * factor), 0, 255)
        g = self._limit_value(int(g * factor), 0, 255)
        b = self._limit_value(int(b * factor), 0, 255)
        return (r, g, b)

    def derive_accent_color(self, primary_rgb: Tuple[int, int, int]) -> Tuple[int, int, int]:
        """
        Derives an accent color from a primary RGB color.
        This heuristic makes the accent a lighter version of the primary color.
        """
        r, g, b = primary_rgb
        # Convert to HSL, increase Lightness, convert back
        h, l, s = rgb_to_hls(r / 255.0, g / 255.0, b / 255.0)

        # Increase lightness, blending towards white, but not aggressively.
        # This creates a subtle highlight effect.
        new_l = min(1.0, l + (1.0 - l) * 0.85)

        accent_rgb_float = hls_to_rgb(h, new_l, s)
        accent_rgb = tuple(self._limit_value(int(x * 255), 0, 255) for x in accent_rgb_float)

        return accent_rgb

    def create_icon_image(self, primary_color_hex: str, accent_color_hex: str,
                          brightness_factor: float = 1.0) -> Image.Image:
        """
        Creates a 16x16 pixel system tray icon image.

        Args:
            primary_color_hex (str): Hex string for the primary background color.
            accent_color_hex (str): Hex string for the accent (plus sign) color.
            brightness_factor (float): Overall brightness factor to apply to both colors.

        Returns:
            Image.Image: The PIL Image object for the icon.
        """
        base_primary_rgb = self.hex_to_rgb(primary_color_hex)
        base_accent_rgb = self.hex_to_rgb(accent_color_hex)

        adjusted_primary_rgb = self._adjust_rgb_brightness(base_primary_rgb, brightness_factor)
        adjusted_accent_rgb = self._adjust_rgb_brightness(base_accent_rgb, brightness_factor)

        image = Image.new('RGBA', (16, 16), (0, 0, 0, 0)) # Transparent background
        draw = ImageDraw.Draw(image)

        # Draw a rounded rectangle as the main body of the icon
        draw.rounded_rectangle((0, 0, 15, 15), radius=3, fill=adjusted_primary_rgb)

        # Draw a plus sign as the accent
        draw.line((4, 8, 11, 8), fill=adjusted_accent_rgb, width=2) # Horizontal line
        draw.line((8, 4, 8, 11), fill=adjusted_accent_rgb, width=2) # Vertical line

        return image