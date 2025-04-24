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

        public DoNotDisturb()
        {
            InitializeComponent();
            LoadComPorts();

            doNotDisturbColorToolStripMenuItem.BackColor = LoadFromRegistry(registeryKeyDoNotDisturbColor, Color.Red);
            busyColorToolStripMenuItem.BackColor = LoadFromRegistry(registeryKeyBusyColor, Color.Orange);
            availableColorToolStripMenuItem.BackColor = LoadFromRegistry(registeryKeyAvailableColor, Color.Green);
            brightnessTrackBar.Value = LoadFromRegistry(registeryKeyBrightnessValue, 255);

            serialPort = new SerialPort();
        }

        private void LoadComPorts()
        {
            string[] ports = SerialPort.GetPortNames();
            comToolStripDropDownButton.DropDownItems.Clear();

            foreach (string port in ports)
            {
                ToolStripMenuItem portMenuItem = new ToolStripMenuItem(port);
                portMenuItem.Click += (sender, e) => OnComPortSelected(port, portMenuItem);
                comToolStripDropDownButton.DropDownItems.Add(portMenuItem);
            }
        }

        private void OnComPortSelected(string port, ToolStripMenuItem selectedItem)
        {
            selectedPort = port;

            foreach (ToolStripMenuItem item in comToolStripDropDownButton.DropDownItems)
                item.Checked = false;

            selectedItem.Checked = true;
            doNotDisburbButton.Enabled = true;
            busyButton.Enabled = true;
            offButton.Enabled = true;
            availableButton.Enabled = true;
            brightnessTrackBar.Enabled = true;
        }

        private async void MainClosing(object sender, FormClosingEventArgs e)
        {
            if (serialPort != null && serialPort.IsOpen)
                await Task.Run(() => serialPort.Close());
        }

        private async void SendCommand(byte[] data)
        {
            try
            {
                this.Cursor = Cursors.WaitCursor;

                if (!serialPort.IsOpen)
                    OpenSerialPort();

                DateTime startTime = DateTime.Now;
                serialPort.Write(data, 0, data.Length);

                byte[] response = new byte[1];
                bool responseReceived = false;

                while ((DateTime.Now - startTime).TotalMilliseconds < serialTimeout)
                {
                    if (serialPort.BytesToRead > 0)
                    {
                        serialPort.Read(response, 0, 1);
                        HandleResponse(response[0]);
                        responseReceived = true;
                        break;
                    }
                    await Task.Delay(serialTimeout);
                }

                if (!responseReceived)
                    MessageBox.Show("Error: No response received within the timeout period.");
            }
            catch (TimeoutException)
            {
                MessageBox.Show("Error: Timeout occurred while waiting for a response.");
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

        private void OpenSerialPort()
        {
            try
            {
                if (serialPort == null || !serialPort.IsOpen)
                {
                    serialPort.PortName = selectedPort;
                    serialPort.BaudRate = serialBaudrate;
                    serialPort.Open();
                }
            }
            catch (UnauthorizedAccessException)
            {
                MessageBox.Show("Error: Access to the COM port is denied.");
            }
            catch (IOException)
            {
                MessageBox.Show("Error: COM port is already in use.");
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error opening COM port {selectedPort}: {ex.Message}");
            }
        }

        private byte CalculateChecksum(byte red, byte green, byte blue, byte brightness)
        {
            int sum = red + green + blue + brightness;
            return (byte)(sum % 256);
        }

        private void ExecuteCommand(Color color)
        {
            byte brightness = Convert.ToByte(brightnessTrackBar.Value);
            byte checksum = CalculateChecksum(color.R, color.G, color.B, brightness);
            byte[] command = new byte[] { serialStartByte, color.R, color.G, color.B, brightness, checksum, serialStopByte };
            SendCommand(command);
        }

        private void HandleResponse(byte responseCode)
        {
            switch (responseCode)
            {
                case serialResponseOK:
                    // OK
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
                    MessageBox.Show($"Unknown response received: {responseCode:X2}");
                    break;
            }
        }

        private void doNotDisburbButton_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(selectedPort)) return;

            Color availableColor = LoadFromRegistry(registeryKeyDoNotDisturbColor, Color.Red);
            ExecuteCommand(availableColor);
        }

        private void busyButton_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(selectedPort)) return;

            Color busyColor = LoadFromRegistry(registeryKeyBusyColor, Color.Orange);
            ExecuteCommand(busyColor);
        }

        private void availableButton_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(selectedPort)) return;

            Color availableColor = LoadFromRegistry(registeryKeyAvailableColor, Color.Green);
            ExecuteCommand(availableColor);
        }

        private void offButton_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(selectedPort)) return;

            byte startByte = 0xFF;
            byte stopByte = 0xFF;
            byte[] command = new byte[] { startByte, 0x0, 0x0, 0x0, 0x0, 0x00, stopByte };
            SendCommand(command);
        }

        private void SaveToRegistry<T>(string name, T value)
        {
            RegistryKey key = Registry.CurrentUser.CreateSubKey(registeryEntry);

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
                key.Close();
            }
        }


        private T LoadFromRegistry<T>(string name, T defaultValue)
        {
            RegistryKey key = Registry.CurrentUser.OpenSubKey(registeryEntry);
            if (key != null)
            {
                if (key.GetValue(name) != null)
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


        private void busyColorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (ColorDialog colorDialog = new ColorDialog())
            {
                colorDialog.Color = LoadFromRegistry(registeryKeyBusyColor, Color.Orange);
                if (colorDialog.ShowDialog() == DialogResult.OK)
                {
                    SaveToRegistry(registeryKeyBusyColor, colorDialog.Color);
                    busyColorToolStripMenuItem.BackColor = colorDialog.Color;
                }
            }
        }

        private void availableColorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (ColorDialog colorDialog = new ColorDialog())
            {
                colorDialog.Color = LoadFromRegistry(registeryKeyAvailableColor, Color.Green);
                if (colorDialog.ShowDialog() == DialogResult.OK)
                {
                    SaveToRegistry(registeryKeyAvailableColor, colorDialog.Color);
                    availableColorToolStripMenuItem.BackColor = colorDialog.Color;
                }
            }
        }

        private void doNotDisturbColorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (ColorDialog colorDialog = new ColorDialog())
            {
                colorDialog.Color = LoadFromRegistry(registeryKeyAvailableColor, Color.Red);
                if (colorDialog.ShowDialog() == DialogResult.OK)
                {
                    SaveToRegistry(registeryKeyDoNotDisturbColor, colorDialog.Color);
                    doNotDisturbColorToolStripMenuItem.BackColor = colorDialog.Color;
                }
            }
        }

        private void brightnessTrackBar_ValueChanged(object sender, EventArgs e)
        {
            SaveToRegistry(registeryKeyBrightnessValue, Convert.ToInt32(brightnessTrackBar.Value));
        }

    }
}
