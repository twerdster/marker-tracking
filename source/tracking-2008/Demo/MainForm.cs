using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.IO;
using Touchless.Vision.Camera;
using System.Threading;
//using WebCam_Capture;

namespace Tracking
{
    unsafe public partial class MainForm : Form
    {
        [DllImport("TrackingDLL.dll")]
        private static extern int Add(int x, int y);

        [DllImport("TrackingDLL.dll")]
        private static extern void TransformImage(IntPtr pSource, int sWidth, int sHeight, int sStride,
                                            double xPivot, double yPivot, IntPtr pDest, int dWidth, int dHeight, int dStride, double[] p);
        [DllImport("TrackingDLL.dll")]
        private static extern double* Inverse(double[] p);

        [DllImport("TrackingDLL.dll")]
        private static extern int GetCorners(IntPtr pSource, int sWidth, int sHeight, int sStride, int threshold,
                                         IntPtr buff, int[] x_points, int[] y_points, int max_ret_points);

        [DllImport("TrackingDLL.dll")]
        private static extern int FindStable(IntPtr pSource, int sWidth, int sHeight, int sStride, double distance,
        int max_ret_pnts, int[] x_points, int[] y_points);

        [DllImport("TrackingDLL.dll")]
        private static extern void CreatePatches(IntPtr pSource, int sWidth, int sHeight, int sStride, int num_transforms,
                               int num_pnts, int[] x_points, int[] y_points,
                               IntPtr pDest, int dWidth, int dHeight, int dStride);

        [DllImport("TrackingDLL.dll")]
        private static extern void Track(IntPtr pSource, int sWidth, int sHeight, int sStride,
            int[] x_corners, int[] y_corners);

        [DllImport("TrackingDLL.dll")]
        private static extern void CreateComplexPicture(IntPtr pSource, int sWidth, int sHeight, int sStride,
                               IntPtr pDest, int dWidth, int dHeight, int dStride);

        [DllImport("TrackingDLL.dll")]
        private static extern void TrackVisualisation(IntPtr pSource, int sWidth, int sHeight, int sStride,
                         IntPtr pMarker, int mWidth, int mHeight, int mStride,
                         IntPtr pVis, int vWidth, int vHeight, int vStride);

        [DllImport("TrackingDLL.dll")]
        private static extern void SetThreshold(double thresh_percent);

        [DllImport("TrackingDLL.dll")]
        private static extern void SetTrainDirectory(char[] path);

        [DllImport("TrackingDLL.dll")]
        private static extern void SetRansacIterations(int iters_num);

        [DllImport("TrackingDLL.dll")]
        private static extern void DrawRect(IntPtr pSource, int sWidth, int sHeight, int sStride);

        [DllImport("TrackingDLL.dll")]
        private static extern bool LoadTrainData();

        [DllImport("TrackingDLL.dll")]
        private static extern bool SetRansacThresh(double thresh);


        public const int WM_USER = 0x0400;
        public const int BEGIN_PROCESS_DIRECTORY = WM_USER + 1;
        public const int SHOW_IMAGE = WM_USER + 2;
        public const int STOP_PROCESS_DIRECTORY = WM_USER + 3;
        public class Win32
        {
            [DllImport("user32.dll")]
            public static extern int SendMessage(
                  IntPtr hWnd,      // handle to destination window
                  int Msg,       // message
                  long wParam,  // first message parameter
                  long lParam   // second message parameter
                  );
        }


        Bitmap src, dst;
        Bitmap marker;
        string marker_name;
        string train_directory_name = "C:\\Temp\\";

        int[] x_points;
        int[] y_points;
        int[] x_corners;
        int[] y_corners;
        string selectedDirectory = "C:\\Temp\\";
        int num_pnts;
        const int max_num_pnts = 25000;
        int num_pic;
        int framenum;
   //     WebCam webcam;
        private ProcessThread processThread;
        private System.Object lockObject;
        bool bProcessThreadRunning;


        public MainForm()
        {
            InitializeComponent();
            x_points = new int[max_num_pnts];
            y_points = new int[max_num_pnts];
            // x_corners = new int[4];
            // y_corners = new int[4];
            x_corners = new int[max_num_pnts];
            y_corners = new int[max_num_pnts];
            //   path_for_dll = new char[256];

            num_pnts = 0;
            Width = System.Windows.Forms.Screen.AllScreens[0].WorkingArea.Width;
            Height = System.Windows.Forms.Screen.AllScreens[0].WorkingArea.Height;
            sourcePicture.Location = new Point(0, menuStrip1.Bottom);
            sourcePicture.Size = new Size(Width / 2, ClientSize.Height - menuStrip1.Height);
            destinationPicture.Location = new Point(Width / 2, menuStrip1.Bottom);
            destinationPicture.Size = sourcePicture.Size;
            num_pic = 0;
            marker = (Bitmap)(Bitmap.FromFile(selectedDirectory + "marker1.jpg"));

  //          webcam = new WebCam();
            bProcessThreadRunning = false;
            lockObject = new Object();
            framenum = 0;
        }
        private void toolStripMenuItem1_Click(object sender, EventArgs e)
        {
            // openFileDialog1.Filter = "All files (*.*)|*.*|Image files (*.bmp)|*.bmp |(*.jpg)|*.jpg";
            // if (openFileDialog1.ShowDialog(this) == DialogResult.OK)
            {
                // read image from disk
                Cursor.Current = Cursors.WaitCursor;
                src = new Bitmap(marker_name);
                double[] transf = {Math.Cos(Math.PI/6),Math.Sin(Math.PI/6), 0,
                                 -Math.Sin(Math.PI/6), Math.Cos(Math.PI/6), 0,
                                 0 , 0 ,1f};

                double[] inv_transf = {Math.Cos(Math.PI/6),-Math.Sin(Math.PI/6), 0,
                                 Math.Sin(Math.PI/6), Math.Cos(Math.PI/6), 0,
                                 0 , 0 ,1f};
                //    double* inverse = Inverse(transf);


                BitmapData sbd = src.LockBits(new Rectangle(0, 0, src.Width, src.Height), ImageLockMode.ReadWrite, src.PixelFormat);

                //CPoint CProjTransform::GetTransformedCoords(double* p,CPoint point)
                //GetTransformedCoords(inv_transf, CPoint(0,0));
                //GetTransformedCoords();
                int xPivot = (int)transf[2]; //TODO
                int yPivot = (int)transf[5];


                Point[] corners = new Point[4];
                corners[0] = new Point(0, 0);
                corners[1] = new Point(src.Width, 0);
                corners[2] = new Point(src.Width, src.Height);
                corners[3] = new Point(0, src.Height);
                double x_min = double.MaxValue, y_min = double.MaxValue;
                double x_max = double.MinValue, y_max = double.MinValue;
                for (int i = 0; i < 4; i++)
                {
                    double x, y, z;
                    x = transf[0] * corners[i].X + transf[1] * corners[i].Y + transf[2];
                    y = transf[3] * corners[i].X + transf[4] * corners[i].Y + transf[5];
                    z = transf[6] * corners[i].X + transf[7] * corners[i].Y + transf[8];
                    x_min = Math.Min(x / z, x_min);
                    y_min = Math.Min(y / z, y_min);
                    x_max = Math.Max(x / z, x_max);
                    y_max = Math.Max(y / z, y_max);
                }




                //dst = new Bitmap((int)(x_max-x_min),(int)(y_max-y_min),src.PixelFormat);
                //  dst = new Bitmap(sbd.Width, sbd.Height,PixelFormat.Format8bppIndexed);
                /* ColorPalette pal = dst.Palette;
                 for (int i = 0; i < pal.Entries.Length; i++)
                 {
                     // create greyscale color table
                     pal.Entries[i] = Color.FromArgb(i, i, i);
                 }
                 dst.Palette = pal;
                 */



                //  TransformImage(sbd.Scan0, sbd.Width, sbd.Height, sbd.Stride,
                //                     x_min,y_min, destbits.Scan0, destbits.Width, destbits.Height, destbits.Stride, transf);
                //  num_pnts = GetCorners(sbd.Scan0, sbd.Width, sbd.Height, sbd.Stride, 30,
                //                         destbits.Scan0, x_points, y_points, max_num_pnts);

                double distance = 25;
                int max_ret_pnts = 150;
                num_pnts = FindStable(sbd.Scan0, sbd.Width, sbd.Height, sbd.Stride, distance,
                                max_ret_pnts, x_points, y_points);
                //src.UnlockBits(sbd);
                //   sbd = src.LockBits(new Rectangle(0, 0, src.Width, src.Height), ImageLockMode.ReadWrite, src.PixelFormat);
                //  src.Save("C:\\temp\\4.bmp");

                int wind = 64;
                int num_transforms = 200;
                dst = new Bitmap(wind * num_pnts, wind * num_transforms, PixelFormat.Format8bppIndexed);
                ColorPalette pal = dst.Palette;
                for (int i = 0; i < pal.Entries.Length; i++)
                {
                    // create greyscale color table
                    pal.Entries[i] = Color.FromArgb(i, i, i);
                }
                dst.Palette = pal;
                BitmapData destbits = dst.LockBits(new Rectangle(0, 0, dst.Width, dst.Height), ImageLockMode.ReadWrite,
                   dst.PixelFormat);

                CreatePatches(sbd.Scan0, sbd.Width, sbd.Height, sbd.Stride, num_transforms,
                                num_pnts, x_points, y_points,
                                destbits.Scan0, destbits.Width, destbits.Height, destbits.Stride);

                src.UnlockBits(sbd);
                dst.UnlockBits(destbits);
                dst.Save("C:\\temp\\2.bmp");
                src.Save("C:\\temp\\1.bmp");
                sourcePicture.Image = src;
                destinationPicture.Image = dst;
                destinationPicture.Size = dst.Size;
            }
        }
        private void readTreeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            openFileDialog1.Filter = "All files (*.*)|*.*|Image files (*.bmp)|*.bmp |(*.jpg)|*.jpg";
            if (openFileDialog1.ShowDialog(this) == DialogResult.OK)
            {

                Cursor.Current = Cursors.WaitCursor;
                src = new Bitmap(openFileDialog1.FileName);
                ProcessImage(src);

            }
        }
        public void ProcessImageFromThread()
        {
            framenum++;
            if (src == null)
                return;
            lock (lockObject)
            {
                dst = (Bitmap)src.Clone();// new Bitmap(src.Width, src.Height, src.PixelFormat);
            }
            //dst.Save(string.Format(@"C:\Temp\FromCamera\{0}.bmp",framenum));
            BitmapData dbd = dst.LockBits(new Rectangle(0, 0, dst.Width, dst.Height), ImageLockMode.ReadWrite, dst.PixelFormat);
 
            Track(dbd.Scan0, dbd.Width, dbd.Height, dbd.Stride, x_corners, y_corners);
            
            dst.UnlockBits(dbd);
        }

        public void ProcessImage(Bitmap src)
        {

            BitmapData sbd = src.LockBits(new Rectangle(0, 0, src.Width, src.Height), ImageLockMode.ReadWrite, src.PixelFormat);
            Track(sbd.Scan0, sbd.Width, sbd.Height, sbd.Stride, x_corners, y_corners);
            destinationPicture.Image = src;
            destinationPicture.Size = src.Size;

            Bitmap vis = new Bitmap(src.Width + marker.Width, Math.Max(src.Height, marker.Height), src.PixelFormat);
            BitmapData mbd = marker.LockBits(new Rectangle(0, 0, marker.Width, marker.Height), ImageLockMode.ReadWrite, marker.PixelFormat);
            BitmapData vbd = vis.LockBits(new Rectangle(0, 0, vis.Width, vis.Height), ImageLockMode.ReadWrite, vis.PixelFormat);

            TrackVisualisation(sbd.Scan0, sbd.Width, sbd.Height, sbd.Stride,
                mbd.Scan0, mbd.Width, mbd.Height, mbd.Stride, vbd.Scan0, vbd.Width, vbd.Height, vbd.Stride);

            src.UnlockBits(sbd);
            vis.UnlockBits(vbd);
            marker.UnlockBits(mbd);
           // vis.Save("C:\\Temp\\matchings.bmp");

            string name = openFileDialog1.FileName;
            int pos = name.LastIndexOf('\\');
            name = name.Substring(pos + 1);
            int pos1 = name.LastIndexOf('.');
            src.Save("C:\\temp\\3.bmp");

        }


        private void createPictureToolStripMenuItem_Click(object sender, EventArgs e)
        {
            openFileDialog1.Filter = "All files (*.*)|*.*|Image files (*.bmp)|*.bmp |(*.jpg)|*.jpg";
            if (openFileDialog1.ShowDialog(this) == DialogResult.OK)
            {
                // read image from disk
                Cursor.Current = Cursors.WaitCursor;
                src = new Bitmap(openFileDialog1.FileName);
                if (openFileDialog1.ShowDialog(this) == DialogResult.OK)
                {
                    dst = new Bitmap(openFileDialog1.FileName);
                    sourcePicture.Image = src;
                    destinationPicture.Image = dst;
                    BitmapData sbd = src.LockBits(new Rectangle(0, 0, src.Width, src.Height), ImageLockMode.ReadWrite, src.PixelFormat);
                    BitmapData destbits = dst.LockBits(new Rectangle(0, 0, dst.Width, dst.Height), ImageLockMode.ReadWrite,
                                 dst.PixelFormat);

                    CreateComplexPicture(sbd.Scan0, sbd.Width, sbd.Height, sbd.Stride,
                                    destbits.Scan0, destbits.Width, destbits.Height, destbits.Stride);

                    src.UnlockBits(sbd);
                    dst.UnlockBits(destbits);
                    string name = openFileDialog1.FileName;
                    int pos = name.LastIndexOf('\\');
                    name = name.Substring(pos + 1);
                    int pos1 = name.LastIndexOf('.');
                    name = name.Substring(0, pos1);
                    dst.Save(string.Format("C:\\temp\\{0}{1}.bmp", name, ++num_pic));
                    destinationPicture.Refresh();
                }
            }
        }

        private void toolStripMenuItem3_Click(object sender, EventArgs e)
        {
            //    SetThreshold(double.Parse(((MenuItem)sender).Text));
            SetThreshold(0.1);
        }

        private void toolStripMenuItem4_Click(object sender, EventArgs e)
        {
            //     SetThreshold(double.Parse(((MenuItem)sender).Text));
            SetThreshold(0.05);
        }

        private void toolStripMenuItem5_Click(object sender, EventArgs e)
        {
            //      SetThreshold(double.Parse(((MenuItem)sender).Text));
            SetThreshold(0.04);
        }

        private void toolStripMenuItem6_Click(object sender, EventArgs e)
        {
            //SetThreshold(double.Parse(((MenuItem)sender).Text));
            SetThreshold(0.02);
        }

        private void toolStripMenuItem7_Click(object sender, EventArgs e)
        {
            SetThreshold(0.03);
        }

        private void toolStripMenuItem8_Click(object sender, EventArgs e)
        {
            //SetThreshold(double.Parse(((MenuItem)sender).Text));
            SetThreshold(0.01);
        }

        private void toolStripMenuItem9_Click(object sender, EventArgs e)
        {
            //SetThreshold(double.Parse(((MenuItem)sender).Text));
            SetThreshold(0.005);
        }

        private void toolStripMenuItem10_Click(object sender, EventArgs e)
        {
            SetThreshold(0.004);
            // SetThreshold(double.Parse(((MenuItem)sender).Text));
        }

        private void setTrainDirectoryToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (folderBrowserDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                char[] path = folderBrowserDialog1.SelectedPath.ToArray();
                selectedDirectory = folderBrowserDialog1.SelectedPath;
                char[] path_for_dll = new char[path.Length + 1];
                Array.Copy(path, path_for_dll, path.Length);
                path_for_dll[path.Length] = '\0';
                SetTrainDirectory(path_for_dll);
                //  marker = (Bitmap)(Bitmap.FromFile(selectedDirectory + "\\marker1.jpg"));
            }

        }
        private void setMarkerToolStripMenuItem_Click(object sender, EventArgs e)
        {
                   }

        private void toolStripMenuItem11_Click(object sender, EventArgs e)
        {
            SetThreshold(0.06);
        }

        private void toolStripMenuItem12_Click(object sender, EventArgs e)
        {
            SetThreshold(0.07);
        }

        private void setRansacIterationsToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void toolStripMenuItem13_Click(object sender, EventArgs e)
        {
            SetRansacIterations(2000);
        }

        private void toolStripMenuItem14_Click(object sender, EventArgs e)
        {
            SetRansacIterations(1000);
        }

        private void toolStripMenuItem15_Click(object sender, EventArgs e)
        {
            SetRansacIterations(500);
        }

        private void toolStripMenuItem16_Click(object sender, EventArgs e)
        {
            SetRansacIterations(300);
        }

        private void toolStripMenuItem17_Click(object sender, EventArgs e)
        {
            SetRansacIterations(200);
        }

        private void toolStripMenuItem18_Click(object sender, EventArgs e)
        {
            SetRansacIterations(100);
        }

        private void startToolStripMenuItem_Click(object sender, EventArgs e)
        {           
            if (bProcessThreadRunning)
            {
                processThread.TerminateProcess();
                //webcam.Stop();
                thrashOldCamera();
                bProcessThreadRunning = false;
            }
            else
            {
 //               webcam.Start();
                if (_frameSource != null && _frameSource.Camera == comboBoxCameras.SelectedItem)
                    return;

                thrashOldCamera();
                startCapturing();
               
                Cursor.Current = Cursors.WaitCursor;
                processThread = new ProcessThread(this.Handle);
                if (!Directory.Exists("C:\\Temp\\FromCamera"))
                    Directory.CreateDirectory("C:\\Temp\\FromCamera");
                processThread.ProcessImageHandler += new ProcessThread.ProcessImage(ProcessImageFromThread);
                bProcessThreadRunning = true;
                new Thread(new ThreadStart(processThread.Run)).Start();
               // webcam.Continue();
            } 
        }

        private void stopToolStripMenuItem_Click(object sender, EventArgs e)
        {
            processThread.TerminateProcess();
            bProcessThreadRunning = false;
 //           webcam.Stop();
            thrashOldCamera();
        }

        private void chooseToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        // ------------------------  WndProc  -------------------------------
        //
        [System.Security.Permissions.PermissionSet(System.Security.Permissions.SecurityAction.Demand, Name = "FullTrust")]
        protected override void WndProc(ref Message m)
        {
            // Listen for operating system messages
            switch (m.Msg)
            {
                case BEGIN_PROCESS_DIRECTORY:
                    //                   BeginProcessing(m.WParam.ToInt32());
                    break;
                case STOP_PROCESS_DIRECTORY:
                    //                   StopProcessing();
                    break;
                case SHOW_IMAGE:
                    ShowImage();
                    break;
            }
            base.WndProc(ref m);
        }
        void ShowImage()
        {
            if (src == null)
                return;
 /*           BitmapData sbd = src.LockBits(new Rectangle(0, 0, src.Width, src.Height), ImageLockMode.ReadWrite, src.PixelFormat);
 
            
            DrawRect(sbd.Scan0, sbd.Width, sbd.Height, sbd.Stride);


            src.UnlockBits(sbd);*/
 //           lock (lockObject)
            {
                sourcePicture.Image = dst;
            }
        }

        private void fromCameraToolStripMenuItem_Click(object sender, EventArgs e)
        {
           if (!bProcessThreadRunning)
            {
//                webcam.Start();
                if (_frameSource != null && _frameSource.Camera == comboBoxCameras.SelectedItem)
                    return;

                thrashOldCamera();
                startCapturing(); 
               button1.Visible = true;
            } 
         }

        private void fromDirectoryToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog(this) == DialogResult.OK)
            {
                marker = (Bitmap)(Bitmap.FromFile(openFileDialog1.FileName));
                //marker.Save("C:\\Temp\\10.bmp");
                marker_name = openFileDialog1.FileName;


                int pos = openFileDialog1.FileName.LastIndexOf('\\');

                if (pos > 0)
                {
                    train_directory_name = openFileDialog1.FileName.Substring(0, pos);
                    char[] path = train_directory_name.ToArray();
                    selectedDirectory = folderBrowserDialog1.SelectedPath;
                    char[] path_for_dll = new char[path.Length + 1];
                    Array.Copy(path, path_for_dll, path.Length);
                    path_for_dll[path.Length] = '\0';
                    SetTrainDirectory(path_for_dll);
                }
                if (File.Exists(train_directory_name + "\\tree0.dat"))
                {
                    Cursor = Cursors.WaitCursor;
                    LoadTrainData();
                    Cursor = Cursors.Arrow;
                }


            }

        }

        private void button1_Click(object sender, EventArgs e)
        {
            src = (Bitmap)destinationPicture.Image;
 //           webcam.Stop();
            thrashOldCamera(); 
            Thread.Sleep(1000);

            dst = (Bitmap)src.Clone();
            dst.Save(train_directory_name + "marker1.bmp");//.jpg", ImageFormat.Jpeg);
            button1.Visible = false;
        }
        private void MainForm_Load(object sender, EventArgs e)
        {
            if (!DesignMode)
            {
                Width = System.Windows.Forms.Screen.AllScreens[0].WorkingArea.Width;
                Height = System.Windows.Forms.Screen.AllScreens[0].WorkingArea.Height;
                // Refresh the list of available cameras
                comboBoxCameras.Items.Clear();
                foreach (Camera cam in CameraService.AvailableCameras)
                    comboBoxCameras.Items.Add(cam);

                if (comboBoxCameras.Items.Count > 0)
                    comboBoxCameras.SelectedIndex = 0;
            }
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            thrashOldCamera();
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            thrashOldCamera();
        }

        private CameraFrameSource _frameSource;
        private static Bitmap _latestFrame;

        private void btnStart_Click(object sender, EventArgs e)
        {
            // Early return if we've selected the current camera
            if (_frameSource != null && _frameSource.Camera == comboBoxCameras.SelectedItem)
                return;

            thrashOldCamera();
            startCapturing();
        }

        private void startCapturing()
        {
            try
            {
                Camera c = (Camera)comboBoxCameras.SelectedItem;
                setFrameSource(new CameraFrameSource(c));
                _frameSource.Camera.CaptureWidth = 640;// 1280;// 320;
                _frameSource.Camera.CaptureHeight = 480;// 1024;// 240;
                _frameSource.Camera.Fps = 20;// 20;
                _frameSource.NewFrame += OnImageCaptured;

                destinationPicture.Paint += new PaintEventHandler(drawLatestImage);
                _frameSource.StartFrameCapture();
            }
            catch (Exception ex)
            {
                comboBoxCameras.Text = "Select A Camera";
                MessageBox.Show(ex.Message);
            }
        }

        private void drawLatestImage(object sender, PaintEventArgs e)
        {
            if (_latestFrame != null)
            {
                // Draw the latest image from the active camera
                e.Graphics.DrawImage(_latestFrame, 0, 0, _latestFrame.Width, _latestFrame.Height);
            }
        }

        public void OnImageCaptured(Touchless.Vision.Contracts.IFrameSource frameSource, Touchless.Vision.Contracts.Frame frame, double fps)
        {
            _latestFrame = frame.Image;
            lock (lockObject)
            {
                src = (Bitmap)_latestFrame.Clone();
            }
            destinationPicture.Invalidate();
        }

        private void setFrameSource(CameraFrameSource cameraFrameSource)
        {
            if (_frameSource == cameraFrameSource)
                return;

            _frameSource = cameraFrameSource;
        }

        //

        private void thrashOldCamera()
        {
            // Trash the old camera
            if (_frameSource != null)
            {
                _frameSource.NewFrame -= OnImageCaptured;
                _frameSource.Camera.Dispose();
                setFrameSource(null);
                destinationPicture.Paint -= new PaintEventHandler(drawLatestImage);
            }
        }

        //

        private void btnSave_Click(object sender, EventArgs e)
        {
            if (_frameSource == null)
                return;

            Bitmap current = (Bitmap)_latestFrame.Clone();
            using (SaveFileDialog sfd = new SaveFileDialog())
            {
                sfd.Filter = "*.bmp|*.bmp";
                if (sfd.ShowDialog() == DialogResult.OK)
                {
                    current.Save(sfd.FileName);
                }
            }

            current.Dispose();
        }

        private void calibrationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // snap camera
            if (_frameSource != null)
                _frameSource.Camera.ShowPropertiesDialog();
        }

        private void toolStripMenuItem19_Click(object sender, EventArgs e)
        {
            SetRansacThresh(0.01);

        }

        private void toolStripMenuItem20_Click(object sender, EventArgs e)
        {
            SetRansacThresh(0.1);
        }

        private void toolStripMenuItem21_Click(object sender, EventArgs e)
        {
            SetRansacThresh(1);
        }

        private void toolStripMenuItem22_Click(object sender, EventArgs e)
        {
            SetRansacThresh(10);
        }

        private void toolStripMenuItem23_Click(object sender, EventArgs e)
        {
            SetRansacThresh(50);
        }

        private void toolStripMenuItem24_Click(object sender, EventArgs e)
        {
            SetRansacThresh(100);
        }
    }
}
