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

            Form1.Win32.SendMessage(mainWindowHandle, Form1.BEGIN_PROCESS_DIRECTORY, 0, 0);
            while (true)
			{
				if (terminateProcess)
					break;
                ProcessImageHandler();
                Form1.Win32.SendMessage(mainWindowHandle, Form1.SHOW_IMAGE, 0, 0);
                Thread.Sleep(1000);
              
			}
//			StopProcessingHandler();
            Form1.Win32.SendMessage(mainWindowHandle, Form1.STOP_PROCESS_DIRECTORY, 0, 0);
		}
	}
}
