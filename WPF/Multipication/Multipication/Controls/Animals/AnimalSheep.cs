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
    public enum SheepAnimations
    {
        Walk,
        WideEyes,
        Blink,
        Look,
        ToeTap,//not implimented
        Count,
    };

    public enum SheepFrames
    {
        Normal,
        Partial,
        Half,
        Left,
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

    public class AnimalSheep : IAnimal
    {
        SheepAnimations idleAnimation = SheepAnimations.Blink;

        Random rand = new Random();

        public void SetAnimal(ref Image myImage, ref double dbFrameWidth, ref double dbFrameHeight)
        {
            myImage.Source = new BitmapImage(new Uri(@"/Assets/Animals/sheep.png", UriKind.Relative));

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
                    return GetNumFrames(SheepAnimations.Walk);
                case Animations.poke:
                    return GetNumFrames(SheepAnimations.WideEyes);
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
                    return (int)GetFrameSheep(SheepAnimations.Walk, iFrame);
                case Animations.poke:
                    return (int)GetFrameSheep(SheepAnimations.WideEyes, iFrame);
                case Animations.idle:
                    return (int)GetFrameSheep(idleAnimation, iFrame);
            }
            return 0;
        }

        private int GetNumFrames(SheepAnimations ani)
        {
            switch (ani)
            {
                case SheepAnimations.Blink:
                    return 5;
                case SheepAnimations.Look:
                    return 11;
                case SheepAnimations.Walk:
                    return 8;
                case SheepAnimations.WideEyes:
                    return 1;
                case SheepAnimations.ToeTap:
                    return 12;
                default:
                    return 0;
            }
        }

        private SheepFrames GetFrameSheep(SheepAnimations ani, int iFrame)
        {
            switch (ani)
            {
                case SheepAnimations.Blink:
                    //if statement is cleaner
                    if (iFrame == 0)
                        return SheepFrames.Partial;
                    if (iFrame == 1)
                        return SheepFrames.Half;
                    if (iFrame == 2)
                        return SheepFrames.Closed;
                    if (iFrame == 3)
                        return SheepFrames.Half;
                    if (iFrame == 4)
                        return SheepFrames.Partial;
                    break;

                case SheepAnimations.Look:
                    //if statement is cleaner
                    if (iFrame == 0)
                        return SheepFrames.Partial;
                    if (iFrame == 1)
                        return SheepFrames.Half;
                    if (iFrame == 2)
                        return SheepFrames.Left;
                    if (iFrame == 3)
                        return SheepFrames.Left;
                    if (iFrame == 4)
                        return SheepFrames.Left;
                    if (iFrame == 5)
                        return SheepFrames.Half;
                    if (iFrame == 6)
                        return SheepFrames.Right;
                    if (iFrame == 7)
                        return SheepFrames.Right;
                    if (iFrame == 8)
                        return SheepFrames.Right;
                    if (iFrame == 9)
                        return SheepFrames.Half;
                    if (iFrame == 10)
                        return SheepFrames.Partial;
                    break;

                case SheepAnimations.Walk:
                    if (iFrame < 8)
                        return (SheepFrames)(iFrame + (int)SheepFrames.Walk1);
                    break;

                case SheepAnimations.ToeTap:
                    if (iFrame % 3 == 0)
                        return SheepFrames.Walk5;
                    return SheepFrames.Walk4;

                case SheepAnimations.WideEyes:
                    if (iFrame == 0)
                        return SheepFrames.WideOpen;
                    break;
            }

            return SheepFrames.Normal;
        }

        public void SetIdleAnimation()
        {
            idleAnimation = (SheepAnimations)rand.Next((int)SheepAnimations.Blink, (int)SheepAnimations.Count);
        }
    }
}
