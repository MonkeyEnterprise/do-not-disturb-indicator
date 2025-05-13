namespace DoNotDisturb
{
    partial class DoNotDisturb
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DoNotDisturb));
            this.busyButton = new System.Windows.Forms.Button();
            this.availableButton = new System.Windows.Forms.Button();
            this.toolStrip = new System.Windows.Forms.ToolStrip();
            this.serialPortDropDownButton = new System.Windows.Forms.ToolStripDropDownButton();
            this.brightnessTrackBar = new System.Windows.Forms.TrackBar();
            this.offButton = new System.Windows.Forms.Button();
            this.doNotDisturbButton = new System.Windows.Forms.Button();
            this.toolStrip.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.brightnessTrackBar)).BeginInit();
            this.SuspendLayout();
            // 
            // busyButton
            // 
            resources.ApplyResources(this.busyButton, "busyButton");
            this.busyButton.Name = "busyButton";
            this.busyButton.UseVisualStyleBackColor = true;
            this.busyButton.Click += new System.EventHandler(this.BusyButton_Click);
            // 
            // availableButton
            // 
            resources.ApplyResources(this.availableButton, "availableButton");
            this.availableButton.Name = "availableButton";
            this.availableButton.UseVisualStyleBackColor = true;
            this.availableButton.Click += new System.EventHandler(this.AvailableButton_Click);
            // 
            // toolStrip
            // 
            this.toolStrip.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.toolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.serialPortDropDownButton});
            resources.ApplyResources(this.toolStrip, "toolStrip");
            this.toolStrip.Name = "toolStrip";
            // 
            // serialPortDropDownButton
            // 
            this.serialPortDropDownButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            resources.ApplyResources(this.serialPortDropDownButton, "serialPortDropDownButton");
            this.serialPortDropDownButton.Name = "serialPortDropDownButton";
            // 
            // brightnessTrackBar
            // 
            resources.ApplyResources(this.brightnessTrackBar, "brightnessTrackBar");
            this.brightnessTrackBar.Maximum = 255;
            this.brightnessTrackBar.Name = "brightnessTrackBar";
            this.brightnessTrackBar.TabStop = false;
            this.brightnessTrackBar.Value = 255;
            this.brightnessTrackBar.Scroll += new System.EventHandler(this.brightnessTrackBar_Scroll);
            // 
            // offButton
            // 
            resources.ApplyResources(this.offButton, "offButton");
            this.offButton.Name = "offButton";
            this.offButton.UseVisualStyleBackColor = true;
            this.offButton.Click += new System.EventHandler(this.OffButton_Click);
            // 
            // doNotDisturbButton
            // 
            resources.ApplyResources(this.doNotDisturbButton, "doNotDisturbButton");
            this.doNotDisturbButton.Name = "doNotDisturbButton";
            this.doNotDisturbButton.UseVisualStyleBackColor = true;
            this.doNotDisturbButton.Click += new System.EventHandler(this.DoNotDisturbButton_Click);
            // 
            // DoNotDisturb
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.doNotDisturbButton);
            this.Controls.Add(this.offButton);
            this.Controls.Add(this.brightnessTrackBar);
            this.Controls.Add(this.toolStrip);
            this.Controls.Add(this.availableButton);
            this.Controls.Add(this.busyButton);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "DoNotDisturb";
            this.toolStrip.ResumeLayout(false);
            this.toolStrip.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.brightnessTrackBar)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button busyButton;
        private System.Windows.Forms.Button availableButton;
        private System.Windows.Forms.ToolStrip toolStrip;
        private System.Windows.Forms.ToolStripDropDownButton serialPortDropDownButton;
        private System.Windows.Forms.TrackBar brightnessTrackBar;
        private System.Windows.Forms.Button offButton;
        private System.Windows.Forms.Button doNotDisturbButton;
    }
}

