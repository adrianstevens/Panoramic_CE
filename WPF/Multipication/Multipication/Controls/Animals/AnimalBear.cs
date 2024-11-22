using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Media.Imaging;

namespace Rescue.Controls
{
    public enum BearAnimations
    {
        Walk,
        WideEyes,
        Blink,
        LookRight,
        Scratch,
        Count,
    };

    public enum FramesBear
    {
        Normal,
        Partial,
        Closed,
        Left,
        Left2,
        Right,
        WideOpen,
        Walk1,
        Walk2,
        Walk3,
        Walk4,
        Walk5,
        Walk6,
        Walk7,
        Walk8,
    };
    
    public class AnimalBear: IAnimal
    {
        BearAnimations idleAnimation;

        Random rand = new Random();

        public void SetAnimal(ref Image myImage, ref double dbFrameWidth, ref double dbFrameHeight)
        {
            myImage.Source = new BitmapImage(new Uri(@"/Assets/Animals/bear.png", UriKind.Relative));

            dbFrameWidth = 80;
            dbFrameHeight = 80;
        }

        public bool CanAnimate()
        {
            return true;
        }

        public int GetNumFrames(Animations ani)//common animations 
        {
            switch (ani)
            {
                case Animations.walk:
                    return GetNumFrames(BearAnimations.Walk);
                case Animations.poke:
                    return GetNumFrames(BearAnimations.WideEyes);
                case Animations.idle:
                    return GetNumFrames(idleAnimation);
            }
            return 0;
        }
        public int GetCurFrame(Animations ani, int iFrame)
        {
            switch (ani)
            {
                case Animations.walk:
                    return (int)GetFrameBear(BearAnimations.Walk, iFrame);
                case Animations.poke:
                    return (int)GetFrameBear(BearAnimations.WideEyes, iFrame);
                case Animations.idle:
                    return (int)GetFrameBear(idleAnimation, iFrame);
            }
            return 0;
        }

        private int GetNumFrames(BearAnimations ani)
        {
            switch (ani)
            {
                case BearAnimations.WideEyes:
                    return 1;
                case BearAnimations.Blink:
                    return 3;
                case BearAnimations.Walk:
                    return 8;
                case BearAnimations.LookRight:
                    return 15;
                case BearAnimations.Scratch:
                    return 15;
                default:
                    return 0;
            }
        }

        private FramesBear GetFrameBear(BearAnimations ani, int iFrame)
        {
            switch (ani)
            {
                case BearAnimations.WideEyes:
                    if (iFrame == 0)
                        return FramesBear.WideOpen;
                    break;
                case BearAnimations.Blink:
                    if (iFrame == 0 || iFrame == 2)
                        return FramesBear.Partial;
                    if (iFrame == 1)
                        return FramesBear.Closed;
                    break;
                case BearAnimations.LookRight:
                    if(iFrame == 0)
                        return FramesBear.Partial;
                    if(iFrame < 5)
                        return FramesBear.Closed;
                    if(iFrame < 10)
                        return FramesBear.Right;
                    if(iFrame < 14)
                        return FramesBear.Closed;
                    if(iFrame == 14)
                        return FramesBear.Partial;
                    break;
                case BearAnimations.Scratch:
                    if(iFrame == 0)
                        return FramesBear.Partial;
                    if(iFrame < 5)
                        return FramesBear.Closed;
                    if(iFrame == 14)
                        return FramesBear.Partial;
                    if (iFrame % 2 == 0)
                        return FramesBear.Left;
                    if (iFrame % 2 == 1)
                        return FramesBear.Left2;
                    break;
                case BearAnimations.Walk:
                    if (iFrame < 8)
                        return (FramesBear)(iFrame + (int)FramesBear.Walk1);
                    break;
            }

            return FramesBear.Normal;
        }

        public void SetIdleAnimation()
        {
            idleAnimation = (BearAnimations)rand.Next((int)BearAnimations.Blink, (int)BearAnimations.Count);
        }
    }

    
}
