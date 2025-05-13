using System;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Threading.Tasks;
using System.Windows.Forms;
using Microsoft.Win32;

namespace DoNotDisturb
{
    public partial class DoNotDisturb : Form
    {
        private SerialPort serialPort;
        private string selectedPort;

        // Registery items
        private readonly string registeryEntry = @"Software\DoNotDisturb";
        private readonly string registeryKeyBusyColor = "BusyColor";
        private readonly string registeryKeyAvailableColor = "AvailableColor";
        private readonly string registeryKeyDoNotDisturbColor = "DoNotDisturbColor";
        private readonly string registeryKeyBrightnessValue = "BrightnessValue";

        // Serial communication settings
        const int serialBaudrate = 115200;
        const int serialTimeout = 5000;
        const byte serialStartByte = 0xff;
        const byte serialStopByte = 0xff;
        const byte serialResponseOK = 0xaa;
        const byte serialResponseErrStartByte = 0xbb;
        const byte serialResponseErrStopByte = 0xcc;
        const byte serialResponseErrChecksum = 0xdd;

        // Application functionality
        public DoNotDisturb()
        {
            this.InitializeComponent();
            this.LoadSerialComPorts();

            doNotDisturbButton.ForeColor = LoadFromRegistry(registeryKeyDoNotDisturbColor, Color.Red);
            busyButton.ForeColor = LoadFromRegistry(registeryKeyBusyColor, Color.Orange);
            availableButton.ForeColor = LoadFromRegistry(registeryKeyAvailableColor, Color.Green);
            brightnessTrackBar.Value = LoadFromRegistry(registeryKeyBrightnessValue, 255);

            serialPort = new SerialPort();
        }

        private void MainClosing(object sender, FormClosingEventArgs e)
        {
            if (serialPort.IsOpen) serialPort.Close();
        }

        private Color PickAColor()
        {
            using (ColorDialog colorDialog = new ColorDialog())
            {
                if (colorDialog.ShowDialog() == DialogResult.OK)
                {
                    return colorDialog.Color;
                }
                return Color.Black;
            }
        }

        // --------------------------- Registry functions ---------------------------
        private void SaveToRegistry<T>(string name, T value)
        {
            using (RegistryKey key = Registry.CurrentUser.CreateSubKey(registeryEntry))
            {
                if (key != null)
                {
                    if (value is Color)
                    {
                        key.SetValue(name, ((Color)(object)value).ToArgb());
                    }
                    else if (value is int)
                    {
                        key.SetValue(name, (int)(object)value);
                    }
                    else if (value is bool)
                    {
                        key.SetValue(name, (bool)(object)value ? 1 : 0);
                    }
                    else if (value is string)
                    {
                        key.SetValue(name, (string)(object)value);
                    }
                    else
                    {
                        throw new NotSupportedException($"Unsupported type: {typeof(T)}");
                    }
                }
            }
        }

        private T LoadFromRegistry<T>(string name, T defaultValue)
        {
            using (RegistryKey key = Registry.CurrentUser.OpenSubKey(registeryEntry))
            {
                if (key != null && key.GetValue(name) != null)
                {
                    object value = key.GetValue(name);
                    if (value is int && typeof(T) == typeof(Color))
                    {
                        return (T)(object)Color.FromArgb((int)value);
                    }
                    else if (value is int && typeof(T) == typeof(int))
                    {
                        return (T)value;
                    }
                    else if (value is byte[] && typeof(T) == typeof(bool))
                    {
                        return (T)(object)((int)value == 1);
                    }
                    else if (value is string && typeof(T) == typeof(string))
                    {
                        return (T)value;
                    }
                    else
                    {
                        throw new InvalidCastException($"Cannot convert registry value to type {typeof(T)}");
                    }
                }
            }
            return defaultValue;
        }

        // --------------------------- Buttons and Menu Items ---------------------------
        private void LoadSerialComPorts()
        {
            try
            {
                // Get available COM ports
                string[] ports = SerialPort.GetPortNames();

                // Clear any existing items in the dropdown
                serialPortDropDownButton.DropDownItems.Clear();

                // Check if any ports are found
                if (ports.Length == 0)
                {
                    ToolStripMenuItem noPortsItem = new ToolStripMenuItem("No COM ports available");
                    noPortsItem.Enabled = false; // Disable the "No COM ports available" item
                    serialPortDropDownButton.DropDownItems.Add(noPortsItem);
                    return;
                }

                // Add each available COM port as a menu item
                foreach (string port in ports)
                {
                    ToolStripMenuItem portMenuItem = new ToolStripMenuItem(port);
                    portMenuItem.Click += (itemSender, itemEventArgs) => this.OnSerialPortSelect(port, portMenuItem);
                    serialPortDropDownButton.DropDownItems.Add(portMenuItem);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error fetching COM ports: {ex.Message}");
            }
        }

        private void DoNotDisturbButton_Click(object sender, EventArgs e)
        {
            // Check if the Shift key is being held down
            if (Control.ModifierKeys == Keys.Shift)
            {
                Color color = this.PickAColor();
                this.SaveToRegistry(registeryKeyDoNotDisturbColor, color);
                doNotDisturbButton.ForeColor = color;
            }
            else
            {
                // If Shift is not pressed, use the default color
                Color color = this.LoadFromRegistry(registeryKeyDoNotDisturbColor, Color.Red);
                this.SendCommand(color, Convert.ToByte(brightnessTrackBar.Value));
            }
        }

        private void BusyButton_Click(object sender, EventArgs e)
        {
            // Check if the Shift key is being held down
            if (Control.ModifierKeys == Keys.Shift)
            {
                Color color = this.PickAColor();
                this.SaveToRegistry(registeryKeyBusyColor, color);
                busyButton.ForeColor = color;
            }
            else
            {
                // If Shift is not pressed, use the default color
                Color color = this.LoadFromRegistry(registeryKeyBusyColor, Color.Orange);
                this.SendCommand(color, Convert.ToByte(brightnessTrackBar.Value));
            }
        }

        private void AvailableButton_Click(object sender, EventArgs e)
        {
            // Check if the Shift key is being held down
            if (Control.ModifierKeys == Keys.Shift)
            {
                Color color = this.PickAColor();
                this.SaveToRegistry(registeryKeyAvailableColor, color);
                availableButton.ForeColor = color;
            }
            else
            {
                // If Shift is not pressed, use the default color
                Color color = this.LoadFromRegistry(registeryKeyAvailableColor, Color.Green);
                this.SendCommand(color, Convert.ToByte(brightnessTrackBar.Value));
            }
        }

        private void OffButton_Click(object sender, EventArgs e)
        {
            Color color = Color.Black;
            this.SendCommand(color, Convert.ToByte(brightnessTrackBar.Value));
        }
        private void brightnessTrackBar_Scroll(object sender, EventArgs e)
        {
            int brightnessValue = Convert.ToInt32(brightnessTrackBar.Value);
            this.SaveToRegistry(registeryKeyBrightnessValue, brightnessValue);
        }

        // ----------------------- Serial Port handlers ------------------------
        private void OnSerialPortSelect(string port, ToolStripMenuItem selectedItem)
        {
            selectedPort = port;

            foreach (ToolStripMenuItem item in serialPortDropDownButton.DropDownItems)
                item.Checked = false;

            selectedItem.Checked = true;

            if (serialPort.IsOpen) serialPort.Close();

            this.OpenSerialPort();
            if (serialPort.IsOpen)
            {
                doNotDisturbButton.Enabled = true;
                busyButton.Enabled = true;
                offButton.Enabled = true;
                availableButton.Enabled = true;
                brightnessTrackBar.Enabled = true;
            }
            else
            {
                doNotDisturbButton.Enabled = false;
                busyButton.Enabled = false;
                offButton.Enabled = false;
                availableButton.Enabled = false;
                brightnessTrackBar.Enabled = false;
            }
        }

        private bool OpenSerialPort()
        {
            try
            {
                // Set the serial port settings
                serialPort.PortName = selectedPort;
                serialPort.BaudRate = serialBaudrate;
                serialPort.ReadTimeout = serialTimeout;

                // Open the port first without resetting the Arduino
                serialPort.DtrEnable = false;
                serialPort.RtsEnable = false;
                serialPort.Open();

                // Prevent Arduino reset
                serialPort.RtsEnable = true;
                System.Threading.Thread.Sleep(100); // wait a moment
                serialPort.RtsEnable = false;

                return true;
            }
            catch (UnauthorizedAccessException)
            {
                MessageBox.Show("Error: Access to the COM port is denied.");
            }
            catch (IOException)
            {
                MessageBox.Show("Error: COM port is already in use.");
            }
            catch (TimeoutException)
            {
                MessageBox.Show("Error: Timeout occurred while opening the COM port.");
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error opening COM port {selectedPort}: {ex.Message}");
            }
            return false;
        }

        private async void SendCommand(Color color, byte brightness = 0xFF)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;

                DateTime startTime = DateTime.Now;

                byte[] command = new byte[] {
                    serialStartByte,
                    color.R,
                    color.G,
                    color.B,
                    brightness,
                    (byte)((color.R + color.G + color.B + brightness) % 256), // Checksum
                    serialStopByte
                 };

                // Send the command
                serialPort.Write(command, 0, command.Length);

                // Wait for a response
                byte[] response = new byte[1];
                bool responseValid = false;

                while ((DateTime.Now - startTime).TotalMilliseconds < serialTimeout && !responseValid)
                {
                    // Check if there are bytes to read
                    if (serialPort.BytesToRead > 0)
                    {
                        // Read the response byte
                        serialPort.Read(response, 0, 1);

                        // Check the response byte
                        switch (response[0])
                        {
                            case serialResponseOK:
                                responseValid = true;
                                break;
                            case serialResponseErrStartByte:
                                MessageBox.Show("Response ERR_START: Invalid start byte.");
                                break;
                            case serialResponseErrStopByte:
                                MessageBox.Show("Response ERR_STOP: Invalid stop byte.");
                                break;
                            case serialResponseErrChecksum:
                                MessageBox.Show("Response ERR_CHECKSUM: Invalid checksum.");
                                break;
                            default:
                                MessageBox.Show($"Unknown response received: {response[0]:X2}");
                                break;
                        }
                    }
                    await Task.Delay(10);  // Short delay to avoid blocking UI, waiting for the next byte
                }

                // If response is not valid, indicate timeout
                if (!responseValid)
                {
                    MessageBox.Show("Error: Timeout occurred while waiting for a valid response.");
                }
            }
            catch (TimeoutException)
            {
                MessageBox.Show("Error: Timeout occurred while waiting for a response.");
            }
            catch (IOException ex)
            {
                MessageBox.Show($"IO Error: {ex.Message}");
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error while sending the command: {ex.Message}");
            }
            finally
            {
                this.Cursor = Cursors.Default;
            }
        }
    }
}
