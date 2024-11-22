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
using System.Windows.Threading;
using System.Diagnostics;

namespace Rescue.Controls
{
    public enum Animals
    {
        penguin,
        sheep,
        cow,
        pig,
        bear,
        lion,
        hippo,
  //      bunny,
        count,
    };

    public enum Animations
    {
        walk,
        poke,
        idle,
        count,
    }

    public partial class Animal : UserControl
    {
        int iCurFrame = 0;

        //we'll set this when we load the image
        protected double dbFrameWidth = 0;
        protected double dbFrameHeight = 0;

        IAnimal curAnimal = new AnimalSheep();
        Animals eCurAnimal = Animals.sheep;

        DispatcherTimer idleTimer = null;
        DispatcherTimer aniTimer = new DispatcherTimer();

        Random rand = new Random();//self seeding yo

        public Animal()
        {
            InitializeComponent();

            StartIdleTimer();
        }

        public void SetAnimal(Animals eAnimal)
        {
            eCurAnimal = eAnimal;

            switch (eAnimal)
            {
                case Animals.sheep:
                    curAnimal = new AnimalSheep();
                    break;
                case Animals.pig:
                    curAnimal = new AnimalPig();
                    break;
                case Animals.penguin:
                    curAnimal = new AnimalPenguin();
                    break;
                case Animals.lion:
                    curAnimal = new AnimalLion();
                    break;
                case Animals.cow:
                    curAnimal = new AnimalCow();
                    break;
                case Animals.bear:
                    curAnimal = new AnimalBear();
                    break;
                case Animals.hippo:
                    curAnimal = new AnimalHippo();
                    break;
            }

            curAnimal.SetAnimal(ref myImage, ref dbFrameWidth, ref dbFrameHeight);
            SetFrame(0);
        }

        private void ResetIdletimer()
        {
            StartIdleTimer();
        }

        private void StartIdleTimer()
        {
            if (idleTimer == null)
                idleTimer = new DispatcherTimer();

            StopIdleTimer();
            idleTimer = new DispatcherTimer();//reset the delegate pls
            idleTimer.Interval = TimeSpan.FromMilliseconds(3000 + rand.Next(8000));
            idleTimer.Tick += delegate(object o, EventArgs args)
            {
                //StopIdleTimer();
                curAnimal.SetIdleAnimation();
                Idle();
            };
            idleTimer.Start();
        }

        private void StopIdleTimer()
        {
            if (idleTimer != null)
                idleTimer.Stop();
        }

        public bool Idle()
        {
            Debug.WriteLine("Idle");

            if (CanAnimate() == false)
                return false;

            SetFrame(0);
            iCurFrame = 0;

            aniTimer.Stop();
            aniTimer = new DispatcherTimer();//so we don't double up the deligates. .. *cough*
            aniTimer.Interval = TimeSpan.FromMilliseconds(150);
            aniTimer.Tick += delegate(object o, EventArgs args)
            {
                if (iCurFrame == curAnimal.GetNumFrames(Animations.idle))
                {
                    aniTimer.Stop();
                    SetFrame(0);
                    ResetIdletimer();//new interval
                }
                else
                {
                    SetFrame(curAnimal.GetCurFrame(Animations.idle, iCurFrame));
                    iCurFrame++;
                }
            };
            aniTimer.Start();

            return true;
        }

        public bool Walk()
        {
            if (CanAnimate() == false)
                return false;

            SetFrame(0);
            iCurFrame = 0;

            aniTimer.Stop();
            aniTimer = new DispatcherTimer();//so we don't double up the deligates. .. *cough*
            aniTimer.Interval = TimeSpan.FromMilliseconds(150);
            aniTimer.Tick += delegate(object o, EventArgs args)
            {
                if (iCurFrame == curAnimal.GetNumFrames(Animations.walk))
                {
                    aniTimer.Stop();
                    SetFrame(0);
                }
                else
                {
                    SetFrame(curAnimal.GetCurFrame(Animations.walk, iCurFrame));
                    iCurFrame++;
                }
            };
            aniTimer.Start();

            return true;
        }

        
        public bool Poke()
        {
            if (CanAnimate() == false)
                return false;

            SetFrame(curAnimal.GetCurFrame(Animations.poke, 0));
            
        /*    SoundEffect sfx = null;

            switch (eCurAnimal)
            {
                case Animals.sheep:
                    sfx = SoundEffect.FromStream(TitleContainer.OpenStream("Assets/wave/sheep1.wav"));
                    break;
                case Animals.cow:
                    sfx = SoundEffect.FromStream(TitleContainer.OpenStream("Assets/wave/cow1.wav"));
                    break;
                case Animals.pig:
                    sfx = SoundEffect.FromStream(TitleContainer.OpenStream("Assets/wave/pig1.wav"));
                    break;
            }

            if(sfx != null)
                sfx.Play();
         * 
         */ 

            aniTimer.Stop();
            aniTimer = new DispatcherTimer();//so we don't double up the deligates. .. *cough*
            aniTimer.Interval = TimeSpan.FromMilliseconds(800);
            aniTimer.Tick += delegate(object o, EventArgs args)
            {
                aniTimer.Stop();
                SetFrame(0);
            };
            aniTimer.Start();

            return true;
        }

        void SetFrame(int iFrame)
        {
            if (CanAnimate() == false)
                iFrame = 0;

            if(dbFrameHeight == 0 || dbFrameWidth == 0)
                return;

            //get the target width of the scaled image ... myCanvas is set to the actual size ... ratio is from the image itself (we'll get rid of that)
            double dbTemp = (double)iFrame * myCanvas.ActualHeight * dbFrameWidth / dbFrameHeight;

            //create a clipping region of the correct size on the canvas (again .. we'll get rid of this later)
            RectangleGeometry clip = new RectangleGeometry();
            clip.Rect = new Rect(0, 0, myCanvas.ActualHeight * dbFrameWidth / dbFrameHeight, myCanvas.ActualHeight);
            myCanvas.Clip = clip;
            
            //set the image to the correct "frame" 
            myImage.SetValue(Canvas.LeftProperty, -1d*dbTemp);
            
            //get the amount we're scaling the image up ... we "should" be able to get this back from 'myImage' but it wasn't working
            double scale = myCanvas.ActualHeight / dbFrameHeight;

            ScaleTransform scaleTransform = new ScaleTransform();
            scaleTransform.ScaleX = scale; //myCanvas.ActualWidth;
            scaleTransform.ScaleY = scale; //myCanvas.ActualHeight;   
            scaleTransform.CenterX = myImage.Width/2;
            scaleTransform.CenterY = myImage.Height / 2;
            myImage.RenderTransform = scaleTransform;
        }


        public virtual bool CanAnimate()
        {
            return curAnimal.CanAnimate();

        }
        
        private void UserControl_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            SetFrame(0);
        }
    }
   
}
