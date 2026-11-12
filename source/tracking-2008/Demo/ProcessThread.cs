using System;
using System.IO;
using System.Threading;

namespace Tracking
{
	/// <summary>
	/// Summary description for ProcessThread.
	/// </summary>
	/// 
	public class ProcessThread
	{
		private bool terminateProcess;
        private IntPtr mainWindowHandle;
        public delegate void ProcessImage();
        public event ProcessImage ProcessImageHandler;

        public ProcessThread(IntPtr h)
		{
            mainWindowHandle = h;
			
			terminateProcess = false;
		}
		public void TerminateProcess()
		{
			terminateProcess = true; 
 		}
		public void Run()
		{

            MainForm.Win32.SendMessage(mainWindowHandle, MainForm.BEGIN_PROCESS_DIRECTORY, 0, 0);
            while (true)
			{
				if (terminateProcess)
					break;
                ProcessImageHandler();
                MainForm.Win32.SendMessage(mainWindowHandle, MainForm.SHOW_IMAGE, 0, 0);
//                Thread.Sleep(1000);
              
			}
//			StopProcessingHandler();
            MainForm.Win32.SendMessage(mainWindowHandle, MainForm.STOP_PROCESS_DIRECTORY, 0, 0);
		}
	}
}
