#region Header
//
//   Project:           WriteableBitmapEx - Silverlight WriteableBitmap extensions
//   Description:       Collection of draw extension methods for the Silverlight WriteableBitmap class.
//
//   Changed by:        $Author$
//   Changed on:        $Date$
//   Changed in:        $Revision$
//   Project:           $URL$
//   Id:                $Id$
//
//
//   Copyright © 2009-2010 Rene Schulte and WriteableBitmapEx Contributors
//
//   This Software is weak copyleft open source. Please read the License.txt for details.
//
#endregion

using System.Drawing;
namespace System.Windows.Media.Imaging
{
   /// <summary>
   /// Collection of draw extension methods for the Silverlight WriteableBitmap class.
   /// </summary>
   public static partial class WriteableBitmapExtensions
   {
      #region Fields

      private const int SizeOfArgb              = 4;

      #endregion

      #region Methods

      #region General

       public static void Invalidate(this WriteableBitmap bmp)
       {
           bmp.Lock();

           bmp.AddDirtyRect(new Int32Rect(0, 0,
              bmp.PixelWidth,
              bmp.PixelHeight)); 

          bmp.Unlock();
       }

      /// <summary>
      /// Fills the whole WriteableBitmap with a color.
      /// </summary>
      /// <param name="bmp">The WriteableBitmap.</param>
      /// <param name="color">The color used for filling.</param>
      public static void Clear(this WriteableBitmap bmp, Color color)
      {
          bmp.Lock();

          var b = new Bitmap(bmp.PixelWidth, 
              bmp.PixelHeight, 
              bmp.BackBufferStride, 
              System.Drawing.Imaging.PixelFormat.Format24bppRgb, 
              bmp.BackBuffer); 

          using(var bitmapGraphics = System.Drawing.Graphics.FromImage(b)) 
          {
             // bitmapGraphics.Clear(color);
              System.Drawing.Color clr = new System.Drawing.Color();
              clr = System.Drawing.Color.FromArgb(color.A, color.R, color.G, color.B);

              
              bitmapGraphics.Clear(clr);
          }

          bmp.AddDirtyRect(new Int32Rect(0, 0, 
              bmp.PixelWidth,
              bmp.PixelHeight)); 
          
          bmp.Unlock(); 

      }

      /// <summary>
      /// Fills the whole WriteableBitmap with an empty color (0).
      /// </summary>
      /// <param name="bmp">The WriteableBitmap.</param>
      public static void Clear(this WriteableBitmap bmp)
      {
         Clear(bmp, Colors.Transparent);
      }


      #endregion


      #endregion
   }
}