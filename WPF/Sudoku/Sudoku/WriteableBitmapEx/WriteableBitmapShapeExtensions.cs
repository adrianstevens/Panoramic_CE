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
using System.Drawing.Drawing2D;
namespace System.Windows.Media.Imaging
{
   /// <summary>
   /// Collection of draw extension methods for the Silverlight WriteableBitmap class.
   /// </summary>
   public static partial class WriteableBitmapExtensions
   {
      #region Methods

      #region DrawLine

      /// <summary>
      /// Draws a colored line by connecting two points using an optimized DDA.
      /// </summary>
      /// <param name="bmp">The WriteableBitmap.</param>
      /// <param name="x1">The x-coordinate of the start point.</param>
      /// <param name="y1">The y-coordinate of the start point.</param>
      /// <param name="x2">The x-coordinate of the end point.</param>
      /// <param name="y2">The y-coordinate of the end point.</param>
      /// <param name="color">The color for the line.</param>
    /*  public static void DrawLine(this WriteableBitmap bmp, int x1, int y1, int x2, int y2, Color color)
      {
         // Add one to use mul and cheap bit shift for multiplication
         var a = color.A + 1;
         var col = (color.A << 24)
                  | ((byte)((color.R * a) >> 8) << 16)
                  | ((byte)((color.G * a) >> 8) << 8)
                  | ((byte)((color.B * a) >> 8)); 
         bmp.DrawLine(x1, y1, x2, y2, col);
      }*/

      /// <summary>
      /// Draws a colored line by connecting two points using an optimized DDA.
      /// </summary>
      /// <param name="bmp">The WriteableBitmap.</param>
      /// <param name="x1">The x-coordinate of the start point.</param>
      /// <param name="y1">The y-coordinate of the start point.</param>
      /// <param name="x2">The x-coordinate of the end point.</param>
      /// <param name="y2">The y-coordinate of the end point.</param>
      /// <param name="color">The color for the line.</param>
      public static void DrawLine(this WriteableBitmap bmp, int x1, int y1, int x2, int y2, Color color)
      {
          bmp.Lock();
          
          var b = new Bitmap(bmp.PixelWidth, 
              bmp.PixelHeight, 
              bmp.BackBufferStride, 
              System.Drawing.Imaging.PixelFormat.Format32bppArgb, 
              bmp.BackBuffer);

          using (var bitmapGraphics = System.Drawing.Graphics.FromImage(b))
          {
              bitmapGraphics.SmoothingMode = SmoothingMode.HighSpeed; 
              bitmapGraphics.InterpolationMode = InterpolationMode.NearestNeighbor; 
              bitmapGraphics.CompositingMode = CompositingMode.SourceCopy; 
              bitmapGraphics.CompositingQuality = CompositingQuality.HighSpeed;
              bitmapGraphics.DrawLine(new System.Drawing.Pen(System.Drawing.Color.FromArgb(color.A, color.R, color.G, color.B)),
                  new System.Drawing.Point(x1, x2), new System.Drawing.Point(x2, y2));
              bitmapGraphics.Dispose(); 
          }

          bmp.AddDirtyRect(new Int32Rect(0, 0,
              bmp.PixelWidth,
              bmp.PixelHeight)); 

          bmp.Unlock();
          
       // DrawLine(bmp.Pixels, bmp.PixelWidth, bmp.PixelHeight, x1, y1, x2, y2, color);
      }

      /// <summary>
      /// Draws a colored line by connecting two points using an optimized DDA. 
      /// Uses the pixels array and the width directly for best performance.
      /// </summary>
      /// <param name="pixels">An array containing the pixels as int RGBA value.</param>
      /// <param name="pixelWidth">The width of one scanline in the pixels array.</param>
      /// <param name="pixelHeight">The height of the bitmap.</param>
      /// <param name="x1">The x-coordinate of the start point.</param>
      /// <param name="y1">The y-coordinate of the start point.</param>
      /// <param name="x2">The x-coordinate of the end point.</param>
      /// <param name="y2">The y-coordinate of the end point.</param>
      /// <param name="color">The color for the line.</param>
      public static void DrawLine(int[] pixels, int pixelWidth, int pixelHeight, int x1, int y1, int x2, int y2, int color)
      {
         // Distance start and end point
         int dx = x2 - x1;
         int dy = y2 - y1;
         int len = pixels.Length;

         const int PRECISION_SHIFT = 8;
         const int PRECISION_VALUE = 1 << PRECISION_SHIFT;

         // Determine slope (absolute value)
         int lenX, lenY;
         int incy1;
         if (dy >= 0)
         {
            incy1 = PRECISION_VALUE;
            lenY = dy;
         }
         else
         {
            incy1 = -PRECISION_VALUE;
            lenY = -dy;
         }

         int incx1;
         if (dx >= 0)
         {
            incx1 = 1;
            lenX = dx;
         }
         else
         {
            incx1 = -1;
            lenX = -dx;
         }

         if (lenX > lenY)
         { // x increases by +/- 1
            // Init steps and start
            int incy = (dy << PRECISION_SHIFT) / lenX;
            int y = y1 << PRECISION_SHIFT;

            // Walk the line!
            for (int i = 0; i < lenX; i++)
            {
               // Check boundaries
               y1 = y >> PRECISION_SHIFT;
               if (x1 >= 0 && x1 < pixelWidth && y1 >= 0 && y1 < pixelHeight)
               {
                  var i2 = y1 * pixelWidth + x1;
                  pixels[i2] = color;
               }
               x1 += incx1;
               y += incy;
            }
         }
         else
         {
            // Prevent division by zero
            if (lenY == 0)
            {
               return;
            }

            // Init steps and start
            // since y increases by +/-1, we can safely add (*h) before the for() loop, since there is no fractional value for y
            int incx = (dx << PRECISION_SHIFT) / lenY;
            int x = x1 << PRECISION_SHIFT;
            int y = y1 << PRECISION_SHIFT;
            int index = (x1 + y1 * pixelWidth) << PRECISION_SHIFT;

            // Walk the line!
            var inc = incy1 * pixelWidth + incx;
            for (int i = 0; i < lenY; i++)
            {
               x1 = x >> PRECISION_SHIFT;
               y1 = y >> PRECISION_SHIFT;
               if (x1 >= 0 && x1 < pixelWidth && y1 >= 0 && y1 < pixelHeight)
               {
                  pixels[index >> PRECISION_SHIFT] = color;
               }
               x += incx;
               y += incy1;
               index += inc;
            }
         }
      } 

      #endregion

      
      #endregion
   }
}