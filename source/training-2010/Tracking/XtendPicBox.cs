using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Tracking
{
    public partial class XtendPicBox : Panel
    {

        #region Local Member Variables

        PictureBox innerPicture = new PictureBox();
        private string mPictureFile = string.Empty;
        private bool mAutoScroll = true;

        #endregion


        #region Constructor


        /// <summary>
        /// Constructor - set up the inner
        /// picture box so that is plants itself
        /// in the top left corner of the panel 
        /// and so that its size mode is always
        /// set to normal.  At normal, the picture
        /// will appear at full size.
        /// </summary>

        public XtendPicBox()
        {
            AutoScroll = true;
            // add the inner picture
            innerPicture.Top = 0;
            innerPicture.Left = 0;
            innerPicture.SizeMode = PictureBoxSizeMode.Normal;

            Controls.Add(innerPicture);
        }

        public PictureBox InnerPicture
        {
            get { return innerPicture; }
        }
        #endregion
    }
}
