using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Imaging;
using System.Windows.Media;

namespace GuitarSuite
{
    public class ObjGui
    {
        public ImageBrush GetBackground()
        {
            ImageBrush brush = new ImageBrush();

            switch (Globals.Settings.eSkin)
            {
                case EnumSkin.Rosewood:
                    brush.ImageSource = new BitmapImage(new Uri("pack://application:,,/Assets/Skin1.png"));
                    break;
                case EnumSkin.Alder:
                    brush.ImageSource = new BitmapImage(new Uri("pack://application:,,/Assets/Skin2.png"));
                    break;
                case EnumSkin.Rebel:
                    brush.ImageSource = new BitmapImage(new Uri("pack://application:,,/Assets/Skin3.png"));
                    break;
                case EnumSkin.Fire:
                    brush.ImageSource = new BitmapImage(new Uri("pack://application:,,/Assets/Skin4.png"));
                    break;
                case EnumSkin.SonicBlue:
                    brush.ImageSource = new BitmapImage(new Uri("pack://application:,,/Assets/Skin5.png"));
                    break;
                case EnumSkin.Carbon:
                    brush.ImageSource = new BitmapImage(new Uri("pack://application:,,/Assets/Skin6.png"));
                    break;
                case EnumSkin.PinkBubbles:
                    brush.ImageSource = new BitmapImage(new Uri("pack://application:,,/Assets/Skin8.png"));
                    break;
                case EnumSkin.Lava:
                    brush.ImageSource = new BitmapImage(new Uri("pack://application:,,/Assets/Skin7.png"));
                    break;
                case EnumSkin.Zebra:
                    brush.ImageSource = new BitmapImage(new Uri("pack://application:,,/Assets/Skin10.png"));
                    break;
                case EnumSkin.Leopard:
                    brush.ImageSource = new BitmapImage(new Uri("pack://application:,,/Assets/Skin9.png"));
                    break;
                case EnumSkin.Eleven:
                    brush.ImageSource = new BitmapImage(new Uri("pack://application:,,/Assets/Skin11.png"));
                    break;
            }

            return brush;
        }
    }

    


}
