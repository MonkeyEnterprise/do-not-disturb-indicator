import logging

from source.toolbar_menu import ToolbarMenu

# Configure logging for the entire application
logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s - %(levelname)s - %(message)s',
                    handlers=[
                        logging.FileHandler("main.log"), # Log to a file
                        logging.StreamHandler()         # Log to console
                    ])

def main():
    """
    The main entry point of the application.
    Initializes and runs the ToolbarMenu.
    """
    logging.info("Starting NeoPixel Status application...")
    app = ToolbarMenu(title="NeoPixel Status")
    app.run()
    logging.info("NeoPixel Status application exited.")

if __name__ == "__main__":
    main()