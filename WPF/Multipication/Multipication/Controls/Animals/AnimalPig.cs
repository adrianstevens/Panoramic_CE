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
    public enum PigAnimations
    {
        Walk,
        WideEyes,
        Blink,
        LookLeft,
        LookRight,
        ToeTap,
        Sniff,
        Count,
    };

    public enum PigFrames
    {
        Normal,
        Sniff,
        Partial,
        Closed,
        Left,
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
        count,
    };

    public class AnimalPig: IAnimal
    {
        PigAnimations idleAnimation = PigAnimations.Blink;

        Random rand = new Random();

        public void SetAnimal(ref Image myImage, ref double dbFrameWidth, ref double dbFrameHeight)
        {
            myImage.Source = new BitmapImage(new Uri(@"/Assets/Animals/pig.png", UriKind.Relative));

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
                    return GetNumFrames(PigAnimations.Walk);
                case Animations.poke:
                    return GetNumFrames(PigAnimations.WideEyes);
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
                    return (int)GetFramePig(PigAnimations.Walk, iFrame);
                case Animations.poke:
                    return (int)GetFramePig(PigAnimations.WideEyes, iFrame);
                case Animations.idle:
                    return (int)GetFramePig(idleAnimation, iFrame);
            }
            return 0;
        }

        private int GetNumFrames(PigAnimations ani)
        {
            switch (ani)
            {
                case PigAnimations.Blink:
                    return 3;
                case PigAnimations.LookLeft:
                    return 17;
                case PigAnimations.LookRight:
                    return 3;
                case PigAnimations.Walk:
                    return 8;
                case PigAnimations.WideEyes:
                    return 1;
                case PigAnimations.Sniff:
                    return 12;
                case PigAnimations.ToeTap:
                    return 9;
                default:
                    return 0;
            }
        }

        private PigFrames GetFramePig(PigAnimations ani, int iFrame)
        {
            switch (ani)
            {
                case PigAnimations.Blink:
                    //if statement is cleaner
                    if (iFrame == 0)
                        return PigFrames.Partial;
                    if (iFrame == 1)
                        return PigFrames.Closed;
                    if (iFrame == 2)
                        return PigFrames.Partial;
                    break;

                case PigAnimations.LookLeft:
                    //if statement is cleaner
                    if (iFrame < 3)
                        return PigFrames.Partial;
                    if (iFrame < 6)
                        return PigFrames.Closed;
                    if (iFrame < 12)
                        return PigFrames.Left;
                    if (iFrame < 15)
                        return PigFrames.Closed;
                    if (iFrame == 15)
                        return PigFrames.Partial;
                    break;
                case PigAnimations.Sniff:
                    if(iFrame%2 == 0)
                        return PigFrames.Sniff;
                    break;
                case PigAnimations.ToeTap:
                    if (iFrame % 3 == 0)
                        return PigFrames.Walk4;
                    return PigFrames.Walk5;

                case PigAnimations.Walk:
                    if (iFrame < 8)
                        return (PigFrames)(iFrame + (int)PigFrames.Walk1);
                    break;
                case PigAnimations.LookRight:
                    return PigFrames.Right;

                case PigAnimations.WideEyes:
                    if (iFrame == 0)
                        return PigFrames.WideOpen;
                    break;
            }

            return PigFrames.Normal;
        }

        public void SetIdleAnimation()
        {
            idleAnimation = (PigAnimations)rand.Next((int)SheepAnimations.Blink, (int)PigAnimations.Count);
        }
    }
}
