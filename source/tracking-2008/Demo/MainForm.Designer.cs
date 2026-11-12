namespace Tracking
{
    partial class MainForm
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
            this.btnSave = new System.Windows.Forms.Button();
            this.comboBoxCameras = new System.Windows.Forms.ComboBox();
            this.btnConfig = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.sourcePicture = new System.Windows.Forms.PictureBox();
            this.destinationPicture = new System.Windows.Forms.PictureBox();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.readTreeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.createPictureToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.changeThresholdToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem5 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem6 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem7 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem8 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem9 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem10 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem11 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem12 = new System.Windows.Forms.ToolStripMenuItem();
            this.setTrainDirectoryToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.setMarkerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.fromCameraToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.fromDirectoryToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.setRansacIterationsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem13 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem14 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem15 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem16 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem17 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem18 = new System.Windows.Forms.ToolStripMenuItem();
            this.cameraToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.chooseToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.startToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.stopToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.calibrationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            this.button1 = new System.Windows.Forms.Button();
            this.btnStop = new System.Windows.Forms.Button();
            this.btnStart = new System.Windows.Forms.Button();
            this.setRansacThresholdToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem19 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem20 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem21 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem22 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem23 = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem24 = new System.Windows.Forms.ToolStripMenuItem();
            ((System.ComponentModel.ISupportInitialize)(this.sourcePicture)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.destinationPicture)).BeginInit();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnSave
            // 
            this.btnSave.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnSave.Location = new System.Drawing.Point(815, 447);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(87, 23);
            this.btnSave.TabIndex = 14;
            this.btnSave.Text = "Save current";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // comboBoxCameras
            // 
            this.comboBoxCameras.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.comboBoxCameras.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxCameras.FormattingEnabled = true;
            this.comboBoxCameras.Location = new System.Drawing.Point(185, 448);
            this.comboBoxCameras.Name = "comboBoxCameras";
            this.comboBoxCameras.Size = new System.Drawing.Size(153, 21);
            this.comboBoxCameras.TabIndex = 12;
            // 
            // btnConfig
            // 
            this.btnConfig.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnConfig.Location = new System.Drawing.Point(908, 447);
            this.btnConfig.Name = "btnConfig";
            this.btnConfig.Size = new System.Drawing.Size(87, 23);
            this.btnConfig.TabIndex = 10;
            this.btnConfig.Text = "Configuration";
            this.btnConfig.UseVisualStyleBackColor = true;
            this.btnConfig.Click += new System.EventHandler(this.calibrationToolStripMenuItem_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // sourcePicture
            // 
            this.sourcePicture.Location = new System.Drawing.Point(12, 37);
            this.sourcePicture.Name = "sourcePicture";
            this.sourcePicture.Size = new System.Drawing.Size(486, 386);
            this.sourcePicture.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.sourcePicture.TabIndex = 7;
            this.sourcePicture.TabStop = false;
            // 
            // destinationPicture
            // 
            this.destinationPicture.Location = new System.Drawing.Point(504, 37);
            this.destinationPicture.Name = "destinationPicture";
            this.destinationPicture.Size = new System.Drawing.Size(640, 480);
            this.destinationPicture.TabIndex = 8;
            this.destinationPicture.TabStop = false;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem1,
            this.readTreeToolStripMenuItem,
            this.createPictureToolStripMenuItem,
            this.changeThresholdToolStripMenuItem,
            this.setTrainDirectoryToolStripMenuItem,
            this.setMarkerToolStripMenuItem,
            this.setRansacIterationsToolStripMenuItem,
            this.cameraToolStripMenuItem,
            this.setRansacThresholdToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1007, 24);
            this.menuStrip1.TabIndex = 9;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(46, 20);
            this.toolStripMenuItem1.Text = "Train";
            this.toolStripMenuItem1.Click += new System.EventHandler(this.toolStripMenuItem1_Click);
            // 
            // readTreeToolStripMenuItem
            // 
            this.readTreeToolStripMenuItem.Name = "readTreeToolStripMenuItem";
            this.readTreeToolStripMenuItem.Size = new System.Drawing.Size(48, 20);
            this.readTreeToolStripMenuItem.Text = "Track";
            this.readTreeToolStripMenuItem.Click += new System.EventHandler(this.readTreeToolStripMenuItem_Click);
            // 
            // createPictureToolStripMenuItem
            // 
            this.createPictureToolStripMenuItem.Name = "createPictureToolStripMenuItem";
            this.createPictureToolStripMenuItem.Size = new System.Drawing.Size(90, 20);
            this.createPictureToolStripMenuItem.Text = "CreatePicture";
            this.createPictureToolStripMenuItem.Click += new System.EventHandler(this.createPictureToolStripMenuItem_Click);
            // 
            // changeThresholdToolStripMenuItem
            // 
            this.changeThresholdToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem3,
            this.toolStripMenuItem4,
            this.toolStripMenuItem5,
            this.toolStripMenuItem6,
            this.toolStripMenuItem7,
            this.toolStripMenuItem8,
            this.toolStripMenuItem9,
            this.toolStripMenuItem10,
            this.toolStripMenuItem11,
            this.toolStripMenuItem12});
            this.changeThresholdToolStripMenuItem.Name = "changeThresholdToolStripMenuItem";
            this.changeThresholdToolStripMenuItem.Size = new System.Drawing.Size(91, 20);
            this.changeThresholdToolStripMenuItem.Text = "Set Threshold";
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(101, 22);
            this.toolStripMenuItem3.Text = "0.1";
            this.toolStripMenuItem3.Click += new System.EventHandler(this.toolStripMenuItem3_Click);
            // 
            // toolStripMenuItem4
            // 
            this.toolStripMenuItem4.Name = "toolStripMenuItem4";
            this.toolStripMenuItem4.Size = new System.Drawing.Size(101, 22);
            this.toolStripMenuItem4.Text = "0.05";
            this.toolStripMenuItem4.Click += new System.EventHandler(this.toolStripMenuItem4_Click);
            // 
            // toolStripMenuItem5
            // 
            this.toolStripMenuItem5.Name = "toolStripMenuItem5";
            this.toolStripMenuItem5.Size = new System.Drawing.Size(101, 22);
            this.toolStripMenuItem5.Text = "0.04";
            this.toolStripMenuItem5.Click += new System.EventHandler(this.toolStripMenuItem5_Click);
            // 
            // toolStripMenuItem6
            // 
            this.toolStripMenuItem6.Name = "toolStripMenuItem6";
            this.toolStripMenuItem6.Size = new System.Drawing.Size(101, 22);
            this.toolStripMenuItem6.Text = "0.03";
            this.toolStripMenuItem6.Click += new System.EventHandler(this.toolStripMenuItem6_Click);
            // 
            // toolStripMenuItem7
            // 
            this.toolStripMenuItem7.Name = "toolStripMenuItem7";
            this.toolStripMenuItem7.Size = new System.Drawing.Size(101, 22);
            this.toolStripMenuItem7.Text = "0.02";
            this.toolStripMenuItem7.Click += new System.EventHandler(this.toolStripMenuItem7_Click);
            // 
            // toolStripMenuItem8
            // 
            this.toolStripMenuItem8.Name = "toolStripMenuItem8";
            this.toolStripMenuItem8.Size = new System.Drawing.Size(101, 22);
            this.toolStripMenuItem8.Text = "0.01";
            this.toolStripMenuItem8.Click += new System.EventHandler(this.toolStripMenuItem8_Click);
            // 
            // toolStripMenuItem9
            // 
            this.toolStripMenuItem9.Name = "toolStripMenuItem9";
            this.toolStripMenuItem9.Size = new System.Drawing.Size(101, 22);
            this.toolStripMenuItem9.Text = "0.005";
            this.toolStripMenuItem9.Click += new System.EventHandler(this.toolStripMenuItem9_Click);
            // 
            // toolStripMenuItem10
            // 
            this.toolStripMenuItem10.Name = "toolStripMenuItem10";
            this.toolStripMenuItem10.Size = new System.Drawing.Size(101, 22);
            this.toolStripMenuItem10.Text = "0.004";
            this.toolStripMenuItem10.Click += new System.EventHandler(this.toolStripMenuItem10_Click);
            // 
            // toolStripMenuItem11
            // 
            this.toolStripMenuItem11.Name = "toolStripMenuItem11";
            this.toolStripMenuItem11.Size = new System.Drawing.Size(101, 22);
            this.toolStripMenuItem11.Text = "0.06";
            this.toolStripMenuItem11.Click += new System.EventHandler(this.toolStripMenuItem11_Click);
            // 
            // toolStripMenuItem12
            // 
            this.toolStripMenuItem12.Name = "toolStripMenuItem12";
            this.toolStripMenuItem12.Size = new System.Drawing.Size(101, 22);
            this.toolStripMenuItem12.Text = "0.07";
            this.toolStripMenuItem12.Click += new System.EventHandler(this.toolStripMenuItem12_Click);
            // 
            // setTrainDirectoryToolStripMenuItem
            // 
            this.setTrainDirectoryToolStripMenuItem.Name = "setTrainDirectoryToolStripMenuItem";
            this.setTrainDirectoryToolStripMenuItem.ShowShortcutKeys = false;
            this.setTrainDirectoryToolStripMenuItem.Size = new System.Drawing.Size(116, 20);
            this.setTrainDirectoryToolStripMenuItem.Text = "Set Train Directory";
            this.setTrainDirectoryToolStripMenuItem.Click += new System.EventHandler(this.setTrainDirectoryToolStripMenuItem_Click);
            // 
            // setMarkerToolStripMenuItem
            // 
            this.setMarkerToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fromCameraToolStripMenuItem,
            this.fromDirectoryToolStripMenuItem});
            this.setMarkerToolStripMenuItem.Name = "setMarkerToolStripMenuItem";
            this.setMarkerToolStripMenuItem.Size = new System.Drawing.Size(75, 20);
            this.setMarkerToolStripMenuItem.Text = "Set Marker";
            this.setMarkerToolStripMenuItem.Click += new System.EventHandler(this.setMarkerToolStripMenuItem_Click);
            // 
            // fromCameraToolStripMenuItem
            // 
            this.fromCameraToolStripMenuItem.Name = "fromCameraToolStripMenuItem";
            this.fromCameraToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.fromCameraToolStripMenuItem.Text = "From Camera ";
            this.fromCameraToolStripMenuItem.Click += new System.EventHandler(this.fromCameraToolStripMenuItem_Click);
            // 
            // fromDirectoryToolStripMenuItem
            // 
            this.fromDirectoryToolStripMenuItem.Name = "fromDirectoryToolStripMenuItem";
            this.fromDirectoryToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.fromDirectoryToolStripMenuItem.Text = "From Directory";
            this.fromDirectoryToolStripMenuItem.Click += new System.EventHandler(this.fromDirectoryToolStripMenuItem_Click);
            // 
            // setRansacIterationsToolStripMenuItem
            // 
            this.setRansacIterationsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem13,
            this.toolStripMenuItem14,
            this.toolStripMenuItem15,
            this.toolStripMenuItem16,
            this.toolStripMenuItem17,
            this.toolStripMenuItem18});
            this.setRansacIterationsToolStripMenuItem.Name = "setRansacIterationsToolStripMenuItem";
            this.setRansacIterationsToolStripMenuItem.Size = new System.Drawing.Size(121, 20);
            this.setRansacIterationsToolStripMenuItem.Text = "SetRansacIterations";
            this.setRansacIterationsToolStripMenuItem.Click += new System.EventHandler(this.setRansacIterationsToolStripMenuItem_Click);
            // 
            // toolStripMenuItem13
            // 
            this.toolStripMenuItem13.Name = "toolStripMenuItem13";
            this.toolStripMenuItem13.Size = new System.Drawing.Size(98, 22);
            this.toolStripMenuItem13.Text = "2000";
            this.toolStripMenuItem13.Click += new System.EventHandler(this.toolStripMenuItem13_Click);
            // 
            // toolStripMenuItem14
            // 
            this.toolStripMenuItem14.Name = "toolStripMenuItem14";
            this.toolStripMenuItem14.Size = new System.Drawing.Size(98, 22);
            this.toolStripMenuItem14.Text = "1000";
            this.toolStripMenuItem14.Click += new System.EventHandler(this.toolStripMenuItem14_Click);
            // 
            // toolStripMenuItem15
            // 
            this.toolStripMenuItem15.Name = "toolStripMenuItem15";
            this.toolStripMenuItem15.Size = new System.Drawing.Size(98, 22);
            this.toolStripMenuItem15.Text = "500";
            this.toolStripMenuItem15.Click += new System.EventHandler(this.toolStripMenuItem15_Click);
            // 
            // toolStripMenuItem16
            // 
            this.toolStripMenuItem16.Name = "toolStripMenuItem16";
            this.toolStripMenuItem16.Size = new System.Drawing.Size(98, 22);
            this.toolStripMenuItem16.Text = "300";
            this.toolStripMenuItem16.Click += new System.EventHandler(this.toolStripMenuItem16_Click);
            // 
            // toolStripMenuItem17
            // 
            this.toolStripMenuItem17.Name = "toolStripMenuItem17";
            this.toolStripMenuItem17.Size = new System.Drawing.Size(98, 22);
            this.toolStripMenuItem17.Text = "200";
            this.toolStripMenuItem17.Click += new System.EventHandler(this.toolStripMenuItem17_Click);
            // 
            // toolStripMenuItem18
            // 
            this.toolStripMenuItem18.Name = "toolStripMenuItem18";
            this.toolStripMenuItem18.Size = new System.Drawing.Size(98, 22);
            this.toolStripMenuItem18.Text = "100";
            this.toolStripMenuItem18.Click += new System.EventHandler(this.toolStripMenuItem18_Click);
            // 
            // cameraToolStripMenuItem
            // 
            this.cameraToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.chooseToolStripMenuItem,
            this.startToolStripMenuItem,
            this.stopToolStripMenuItem,
            this.calibrationToolStripMenuItem});
            this.cameraToolStripMenuItem.Name = "cameraToolStripMenuItem";
            this.cameraToolStripMenuItem.Size = new System.Drawing.Size(60, 20);
            this.cameraToolStripMenuItem.Text = "Camera";
            // 
            // chooseToolStripMenuItem
            // 
            this.chooseToolStripMenuItem.Name = "chooseToolStripMenuItem";
            this.chooseToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.chooseToolStripMenuItem.Text = "Choose";
            this.chooseToolStripMenuItem.Click += new System.EventHandler(this.chooseToolStripMenuItem_Click);
            // 
            // startToolStripMenuItem
            // 
            this.startToolStripMenuItem.Name = "startToolStripMenuItem";
            this.startToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.startToolStripMenuItem.Text = "Start";
            this.startToolStripMenuItem.Click += new System.EventHandler(this.startToolStripMenuItem_Click);
            // 
            // stopToolStripMenuItem
            // 
            this.stopToolStripMenuItem.Name = "stopToolStripMenuItem";
            this.stopToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.stopToolStripMenuItem.Text = "Stop";
            this.stopToolStripMenuItem.Click += new System.EventHandler(this.stopToolStripMenuItem_Click);
            // 
            // calibrationToolStripMenuItem
            // 
            this.calibrationToolStripMenuItem.Name = "calibrationToolStripMenuItem";
            this.calibrationToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.calibrationToolStripMenuItem.Text = "Calibration";
            this.calibrationToolStripMenuItem.Click += new System.EventHandler(this.calibrationToolStripMenuItem_Click);
            // 
            // button1
            // 
            this.button1.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.button1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(177)));
            this.button1.ForeColor = System.Drawing.SystemColors.ButtonHighlight;
            this.button1.Location = new System.Drawing.Point(690, 336);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(146, 77);
            this.button1.TabIndex = 10;
            this.button1.Text = "Set Marker";
            this.button1.UseVisualStyleBackColor = false;
            this.button1.Visible = false;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // btnStop
            // 
            this.btnStop.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnStop.Location = new System.Drawing.Point(93, 447);
            this.btnStop.Name = "btnStop";
            this.btnStop.Size = new System.Drawing.Size(75, 23);
            this.btnStop.TabIndex = 9;
            this.btnStop.Text = "Stop";
            this.btnStop.UseVisualStyleBackColor = true;
            this.btnStop.Click += new System.EventHandler(this.btnStop_Click);
            // 
            // btnStart
            // 
            this.btnStart.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnStart.Location = new System.Drawing.Point(12, 447);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(75, 23);
            this.btnStart.TabIndex = 8;
            this.btnStart.Text = "Start";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // setRansacThresholdToolStripMenuItem
            // 
            this.setRansacThresholdToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem19,
            this.toolStripMenuItem20,
            this.toolStripMenuItem21,
            this.toolStripMenuItem22,
            this.toolStripMenuItem23,
            this.toolStripMenuItem24});
            this.setRansacThresholdToolStripMenuItem.Name = "setRansacThresholdToolStripMenuItem";
            this.setRansacThresholdToolStripMenuItem.Size = new System.Drawing.Size(125, 20);
            this.setRansacThresholdToolStripMenuItem.Text = "SetRansacThreshold";
            // 
            // toolStripMenuItem19
            // 
            this.toolStripMenuItem19.Name = "toolStripMenuItem19";
            this.toolStripMenuItem19.Size = new System.Drawing.Size(152, 22);
            this.toolStripMenuItem19.Text = "0.01";
            this.toolStripMenuItem19.Click += new System.EventHandler(this.toolStripMenuItem19_Click);
            // 
            // toolStripMenuItem20
            // 
            this.toolStripMenuItem20.Name = "toolStripMenuItem20";
            this.toolStripMenuItem20.Size = new System.Drawing.Size(152, 22);
            this.toolStripMenuItem20.Text = "0.1";
            this.toolStripMenuItem20.Click += new System.EventHandler(this.toolStripMenuItem20_Click);
            // 
            // toolStripMenuItem21
            // 
            this.toolStripMenuItem21.Name = "toolStripMenuItem21";
            this.toolStripMenuItem21.Size = new System.Drawing.Size(152, 22);
            this.toolStripMenuItem21.Text = "1";
            this.toolStripMenuItem21.Click += new System.EventHandler(this.toolStripMenuItem21_Click);
            // 
            // toolStripMenuItem22
            // 
            this.toolStripMenuItem22.Name = "toolStripMenuItem22";
            this.toolStripMenuItem22.Size = new System.Drawing.Size(152, 22);
            this.toolStripMenuItem22.Text = "10";
            this.toolStripMenuItem22.Click += new System.EventHandler(this.toolStripMenuItem22_Click);
            // 
            // toolStripMenuItem23
            // 
            this.toolStripMenuItem23.Name = "toolStripMenuItem23";
            this.toolStripMenuItem23.Size = new System.Drawing.Size(152, 22);
            this.toolStripMenuItem23.Text = "50";
            this.toolStripMenuItem23.Click += new System.EventHandler(this.toolStripMenuItem23_Click);
            // 
            // toolStripMenuItem24
            // 
            this.toolStripMenuItem24.Name = "toolStripMenuItem24";
            this.toolStripMenuItem24.Size = new System.Drawing.Size(152, 22);
            this.toolStripMenuItem24.Text = "100";
            this.toolStripMenuItem24.Click += new System.EventHandler(this.toolStripMenuItem24_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1007, 482);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.destinationPicture);
            this.Controls.Add(this.sourcePicture);
            this.Controls.Add(this.menuStrip1);
            this.Controls.Add(this.btnSave);
            this.Controls.Add(this.comboBoxCameras);
            this.Controls.Add(this.btnConfig);
            this.Controls.Add(this.btnStop);
            this.Controls.Add(this.btnStart);
            this.MainMenuStrip = this.menuStrip1;
            this.MinimumSize = new System.Drawing.Size(640, 520);
            this.Name = "MainForm";
            this.Text = "WebCam Demo";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.sourcePicture)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.destinationPicture)).EndInit();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.PictureBox sourcePicture;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem readTreeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem createPictureToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem changeThresholdToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem3;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem4;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem5;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem6;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem7;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem8;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem9;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem10;
        private System.Windows.Forms.ToolStripMenuItem setTrainDirectoryToolStripMenuItem;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
        private System.Windows.Forms.ToolStripMenuItem setMarkerToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem11;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem12;
        private System.Windows.Forms.ToolStripMenuItem setRansacIterationsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem13;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem14;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem15;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem16;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem17;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem18;
        private System.Windows.Forms.ToolStripMenuItem cameraToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem chooseToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem startToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem stopToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem fromCameraToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem fromDirectoryToolStripMenuItem;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.PictureBox destinationPicture;
        private System.Windows.Forms.ToolStripMenuItem calibrationToolStripMenuItem;

        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.ComboBox comboBoxCameras;
        private System.Windows.Forms.Button btnConfig;
        private System.Windows.Forms.Button btnStop;
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.ToolStripMenuItem setRansacThresholdToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem19;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem20;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem21;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem22;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem23;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem24;
    }
}

