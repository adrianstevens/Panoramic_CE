using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Media.Imaging;
using System.Windows.Resources;
using System.Media;

namespace GuitarSuite 
{
    public partial class GuitarString : UserControl
    {

        #region Private vars

        Image image;
        Canvas canvasImage;
        Grid LayoutRoot;
        
        int currentFrame = 0;
        //int _iString;
        System.Windows.Threading.DispatcherTimer dt = null;

        private MediaPlayer mPlayer;// = new MediaPlayer();

        private bool bPlayOnLoad = false;

        #endregion

        #region Public vars



        public string szResource
        {
            get
            {
                return _szResource;
            }
            set
            {
                _szResource = value;
                Uri uriSound = new Uri(_szResource, UriKind.Relative);
                //mPlayer.Stop();

                mPlayer.Volume = 0;
                mPlayer.Open(uriSound);
                bPlayOnLoad = false;
            }
        }
        string _szResource;

        void mPlayer_MediaOpened(object sender, EventArgs e)
        {
            if (bPlayOnLoad == true)
            {
                bPlayOnLoad = false;
                Play();
            }
        }
   
        #endregion

        public GuitarString(int iString)
        {
            mPlayer = new MediaPlayer();
            mPlayer.MediaOpened += new EventHandler(mPlayer_MediaOpened);

            LayoutRoot = new Grid();
            canvasImage = new Canvas();

            //_iString = iString;

            RectangleGeometry geo = new RectangleGeometry();
            
            geo.Rect = new Rect(0, 0, 33, 550);
            canvasImage.Clip = geo;
            canvasImage.Width = 33;
            canvasImage.HorizontalAlignment = HorizontalAlignment.Left;
            canvasImage.VerticalAlignment = VerticalAlignment.Top;

            LayoutRoot.MouseEnter += image_MouseEnter;
            LayoutRoot.MouseDown += image_MouseEnter;
            //LayoutRoot.ManipulationDelta += new EventHandler<ManipulationDeltaEventArgs>(String_ManipulationDelta);

            image = new Image();
            
            switch (iString)
            {
                case 0:
                    //brush.ImageSource = new BitmapImage(new Uri(@"/Assets/String01.png", UriKind.Relative));
                    image.Source = new BitmapImage(new Uri(@"/Assets/String01.png", UriKind.Relative));
                    break;
                case 1:
                    image.Source = new BitmapImage(new Uri(@"/Assets/String02.png", UriKind.Relative));
                    break;
                case 2:
                    image.Source = new BitmapImage(new Uri(@"/Assets/String03.png", UriKind.Relative));
                    break;
                case 3:
                    image.Source = new BitmapImage(new Uri(@"/Assets/String04.png", UriKind.Relative));
                    break;
                case 4:
                    image.Source = new BitmapImage(new Uri(@"/Assets/String05.png", UriKind.Relative));
                    break;
                case 5:
                default:
                    image.Source = new BitmapImage(new Uri(@"/Assets/String06.png", UriKind.Relative));
                    break;
            }

            image.Stretch = Stretch.Uniform;

            image.Width = 165;
          //  image.Height = 800;

            canvasImage.Children.Add(image);

            image.SetValue(Canvas.LeftProperty, 0d);

            LayoutRoot.Children.Add(canvasImage);

            LayoutRoot.Width = 50;

            LayoutRoot.Background = new SolidColorBrush(System.Windows.Media.Color.FromArgb(1, 0, 0, 0));

            //LayoutRoot.Opacity = 0.15;

            LayoutRoot.HorizontalAlignment = HorizontalAlignment.Right;

            this.Content = LayoutRoot;
        }

        public void SetInUse(bool bInUse)
        {
            if (bInUse == true)
                image.Opacity = 1.0;
            else
                image.Opacity = 0.5;
        }

        void dt_Tick(object sender, EventArgs e)
        {
            currentFrame ++;

            // are we done the animation?
            if (currentFrame == 5)
            {
                currentFrame = 0;
                dt.Stop();
                dt = null;
            }

            if(image != null)
                image.SetValue(Canvas.LeftProperty, currentFrame * -33d);
        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            
        }

        private void image_MouseEnter(object sender, MouseEventArgs e)
        {
            if (Mouse.LeftButton != MouseButtonState.Pressed)
                return;

            if (szResource == null || szResource.Length < 1)
            {
                System.Diagnostics.Debug.WriteLine("Guitar String - no resource set");
                return;
            }
            
            if (dt != null)
            {
                dt.Stop();
                dt = null;
            }

            dt = new System.Windows.Threading.DispatcherTimer();
            dt.Interval = new TimeSpan(0, 0, 0, 0, 80); // Milliseconds
            dt.Tick += new EventHandler(dt_Tick);
            dt.Start();

            Play();
        }

        /*void String_ManipulationDelta(object sender, ManipulationDeltaEventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("String_ManipulationDelta String - " + _iString.ToString());
        }*/

        public void Play()
        {
            if (szResource == null || szResource.Length < 1)
                return;

           mPlayer.Stop();

           if (mPlayer.HasAudio == true)
           {
               mPlayer.Volume = 1f;
               bPlayOnLoad = false;
               mPlayer.Play();
            //   System.Diagnostics.Debug.WriteLine("Guitar String playing:" + szResource);
           }
           else
           {
               bPlayOnLoad = true;
           }
        }
    }
}
