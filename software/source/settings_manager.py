import json
import os
import logging

class SettingsManager:
    """
    Manages loading and saving application settings from/to a JSON file.
    It handles default settings and merges them with loaded configurations.
    """

    SETTINGS_FILE = "settings.json"

    def __init__(self, default_settings: dict):
        self._default_settings = default_settings
        self._settings = self._load_settings()

    @property
    def settings(self) -> dict:
        """Returns the current loaded settings."""
        return self._settings

    def _load_settings(self) -> dict:
        """
        Loads settings from the SETTINGS_FILE. If the file doesn't exist,
        is invalid, or is missing keys, it uses default values.

        Returns:
            dict: The loaded or default settings.
        """
        if os.path.exists(self.SETTINGS_FILE):
            try:
                with open(self.SETTINGS_FILE, 'r') as f:
                    loaded_settings = json.load(f)
                logging.info(f"Settings loaded from {self.SETTINGS_FILE}")

                # Deep merge loaded settings with defaults to ensure all keys are present
                merged_settings = self._default_settings.copy()
                for key, value in loaded_settings.items():
                    if key in merged_settings and isinstance(merged_settings[key], dict) and \
                       isinstance(value, dict):
                        merged_settings[key].update(value)
                    else:
                        merged_settings[key] = value

                # Ensure 'config' and its sub-keys exist with defaults if missing
                config_defaults = self._default_settings.get("config", {})
                if "config" not in merged_settings:
                    merged_settings["config"] = config_defaults.copy()
                    logging.warning("Loaded settings is missing 'config' section. Using default config.")
                else:
                    for sub_key, default_sub_value in config_defaults.items():
                        if sub_key not in merged_settings["config"]:
                            merged_settings["config"][sub_key] = default_sub_value
                            logging.warning(f"Loaded settings 'config' is missing '{sub_key}'. "
                                            f"Using default '{sub_key}'.")

                return merged_settings

            except json.JSONDecodeError:
                logging.error(f"Invalid JSON format in {self.SETTINGS_FILE}. Using default settings.")
            except Exception as e:
                logging.error(f"Error loading settings from {self.SETTINGS_FILE}: {e}")
        else:
            logging.info(f"{self.SETTINGS_FILE} not found. Using default settings.")

        return self._default_settings.copy()

    def save_settings(self) -> None:
        """Saves the current settings to the SETTINGS_FILE."""
        try:
            with open(self.SETTINGS_FILE, 'w') as f:
                json.dump(self._settings, f, indent=4)
            logging.info(f"Settings saved to {self.SETTINGS_FILE}")
        except Exception as e:
            logging.error(f"Error saving settings to {self.SETTINGS_FILE}: {e}")

    def update_setting(self, category: str, key: str, value: any) -> None:
        """
        Updates a specific setting and saves the settings file.

        Args:
            category (str): The top-level category of the setting (e.g., 'dnd', 'config').
            key (str): The key within the category (e.g., 'primary', 'com_port').
            value (any): The new value for the setting.
        """
        if category not in self._settings:
            self._settings[category] = {}
        self._settings[category][key] = value
        self.save_settings()