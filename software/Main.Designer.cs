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
            this.toolStripButton = new System.Windows.Forms.ToolStripDropDownButton();
            this.comToolStripDropDownButton = new System.Windows.Forms.ToolStripMenuItem();
            this.doNotDisturbColorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.busyColorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.availableColorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.brightnessTrackBar = new System.Windows.Forms.TrackBar();
            this.offButton = new System.Windows.Forms.Button();
            this.doNotDisburbButton = new System.Windows.Forms.Button();
            this.toolStrip.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.brightnessTrackBar)).BeginInit();
            this.SuspendLayout();
            // 
            // busyButton
            // 
            resources.ApplyResources(this.busyButton, "busyButton");
            this.busyButton.Name = "busyButton";
            this.busyButton.UseVisualStyleBackColor = true;
            this.busyButton.Click += new System.EventHandler(this.busyButton_Click);
            // 
            // availableButton
            // 
            resources.ApplyResources(this.availableButton, "availableButton");
            this.availableButton.Name = "availableButton";
            this.availableButton.UseVisualStyleBackColor = true;
            this.availableButton.Click += new System.EventHandler(this.availableButton_Click);
            // 
            // toolStrip
            // 
            this.toolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton});
            resources.ApplyResources(this.toolStrip, "toolStrip");
            this.toolStrip.Name = "toolStrip";
            // 
            // toolStripButton
            // 
            this.toolStripButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripButton.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.comToolStripDropDownButton,
            this.doNotDisturbColorToolStripMenuItem,
            this.busyColorToolStripMenuItem,
            this.availableColorToolStripMenuItem});
            resources.ApplyResources(this.toolStripButton, "toolStripButton");
            this.toolStripButton.Name = "toolStripButton";
            // 
            // comToolStripDropDownButton
            // 
            this.comToolStripDropDownButton.Name = "comToolStripDropDownButton";
            resources.ApplyResources(this.comToolStripDropDownButton, "comToolStripDropDownButton");
            // 
            // doNotDisturbColorToolStripMenuItem
            // 
            this.doNotDisturbColorToolStripMenuItem.Name = "doNotDisturbColorToolStripMenuItem";
            resources.ApplyResources(this.doNotDisturbColorToolStripMenuItem, "doNotDisturbColorToolStripMenuItem");
            this.doNotDisturbColorToolStripMenuItem.Click += new System.EventHandler(this.doNotDisturbColorToolStripMenuItem_Click);
            // 
            // busyColorToolStripMenuItem
            // 
            this.busyColorToolStripMenuItem.Name = "busyColorToolStripMenuItem";
            resources.ApplyResources(this.busyColorToolStripMenuItem, "busyColorToolStripMenuItem");
            this.busyColorToolStripMenuItem.Click += new System.EventHandler(this.busyColorToolStripMenuItem_Click);
            // 
            // availableColorToolStripMenuItem
            // 
            this.availableColorToolStripMenuItem.Name = "availableColorToolStripMenuItem";
            resources.ApplyResources(this.availableColorToolStripMenuItem, "availableColorToolStripMenuItem");
            this.availableColorToolStripMenuItem.Click += new System.EventHandler(this.availableColorToolStripMenuItem_Click);
            // 
            // brightnessTrackBar
            // 
            resources.ApplyResources(this.brightnessTrackBar, "brightnessTrackBar");
            this.brightnessTrackBar.Maximum = 255;
            this.brightnessTrackBar.Name = "brightnessTrackBar";
            this.brightnessTrackBar.TabStop = false;
            this.brightnessTrackBar.Value = 255;
            this.brightnessTrackBar.ValueChanged += new System.EventHandler(this.brightnessTrackBar_ValueChanged);
            // 
            // offButton
            // 
            resources.ApplyResources(this.offButton, "offButton");
            this.offButton.Name = "offButton";
            this.offButton.UseVisualStyleBackColor = true;
            this.offButton.Click += new System.EventHandler(this.offButton_Click);
            // 
            // doNotDisburbButton
            // 
            resources.ApplyResources(this.doNotDisburbButton, "doNotDisburbButton");
            this.doNotDisburbButton.Name = "doNotDisburbButton";
            this.doNotDisburbButton.UseVisualStyleBackColor = true;
            this.doNotDisburbButton.Click += new System.EventHandler(this.doNotDisburbButton_Click);
            // 
            // DoNotDisturb
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.doNotDisburbButton);
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
        private System.Windows.Forms.ToolStripDropDownButton toolStripButton;
        private System.Windows.Forms.ToolStripMenuItem comToolStripDropDownButton;
        private System.Windows.Forms.ToolStripMenuItem busyColorToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem availableColorToolStripMenuItem;
        private System.Windows.Forms.TrackBar brightnessTrackBar;
        private System.Windows.Forms.Button offButton;
        private System.Windows.Forms.Button doNotDisburbButton;
        private System.Windows.Forms.ToolStripMenuItem doNotDisturbColorToolStripMenuItem;
    }
}

