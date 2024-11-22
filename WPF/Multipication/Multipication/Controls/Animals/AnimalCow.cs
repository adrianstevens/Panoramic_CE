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
    public enum CowAnimations
    {
        Walk,
        WideEyes,
        Blink,
        Look,
        ToeTap,//not implimented
        Count,
    };

    public enum FramesCow
    {
        Normal,
        Up,
        Left,
        Partial,
        Right,
        Closed,
        WideOpen,
        Walk1,
        Walk2,
        Walk3,
        Walk4,
        Walk5,
        Walk6,
        Walk7,
        Walk8,
        count,
    };

    public class AnimalCow : IAnimal
    {
        CowAnimations idleAnimation = CowAnimations.Blink;

        Random rand = new Random();

        public void SetAnimal(ref Image myImage, ref double dbFrameWidth, ref double dbFrameHeight)
        {
            myImage.Source = new BitmapImage(new Uri(@"/Assets/Animals/cow.png", UriKind.Relative));

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
                    return GetNumFrames(CowAnimations.Walk);
                case Animations.poke:
                    return GetNumFrames(CowAnimations.WideEyes);
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
                    return (int)GetFrameCow(CowAnimations.Walk, iFrame);
                case Animations.poke:
                    return (int)GetFrameCow(CowAnimations.WideEyes, iFrame);
                case Animations.idle:
                    return (int)GetFrameCow(idleAnimation, iFrame);
            }
            return 0;
        }

        private int GetNumFrames(CowAnimations ani)
        {
            switch (ani)
            {
                case CowAnimations.Blink:
                    return 5;
                case CowAnimations.Look:
                    return 11;
                case CowAnimations.Walk:
                    return 8;
                case CowAnimations.WideEyes:
                    return 1;
                case CowAnimations.ToeTap:
                    return 12;
                default:
                    return 0;
            }
        }

        private FramesCow GetFrameCow(CowAnimations ani, int iFrame)
        {
            switch (ani)
            {
                case CowAnimations.Blink:
                    //if statement is cleaner
                    if (iFrame == 0)
                        return FramesCow.Partial;
                    if (iFrame == 1)
                        return FramesCow.Closed;
                    if (iFrame == 2)
                        return FramesCow.Closed;
                    if (iFrame == 3)
                        return FramesCow.Closed;
                    if (iFrame == 4)
                        return FramesCow.Partial;
                    break;

                case CowAnimations.Look:
                    //if statement is cleaner
                    if (iFrame == 0)
                        return FramesCow.Partial;
                    if (iFrame == 1)
                        return FramesCow.Left;
                    if (iFrame == 2)
                        return FramesCow.Left;
                    if (iFrame == 3)
                        return FramesCow.Left;
                    if (iFrame == 4)
                        return FramesCow.Partial;
                    if (iFrame == 5)
                        return FramesCow.Partial;
                    if (iFrame == 6)
                        return FramesCow.Right;
                    if (iFrame == 7)
                        return FramesCow.Right;
                    if (iFrame == 8)
                        return FramesCow.Right;
                    if (iFrame == 9)
                        return FramesCow.Partial;
                    break;

                case CowAnimations.Walk:
                    if (iFrame < 8)
                        return (FramesCow)(iFrame + (int)FramesCow.Walk1);
                    break;

                case CowAnimations.ToeTap:
                    if (iFrame % 3 == 0)
                        return FramesCow.Walk4;
                    return FramesCow.Walk5;
                    
                case CowAnimations.WideEyes:
                    if (iFrame == 0)
                        return FramesCow.WideOpen;
                    break;
            }
            return FramesCow.Normal;
        }

        public void SetIdleAnimation()
        {
            idleAnimation = (CowAnimations)rand.Next((int)PenguinAnimations.Blink, (int)CowAnimations.Count);
        }
    }
      
}
