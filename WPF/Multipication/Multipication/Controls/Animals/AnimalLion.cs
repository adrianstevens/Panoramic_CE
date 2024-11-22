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
    public enum LionAnimations
    {
        Walk,
        WideEyes,
        Angry,
        Blink,
        Sleep,
        LookLeft,
        ToeTap,
        Count,
    };

    public enum FramesLion
    {
        Normal,
        Angry,
        Partial,
        Closed,
        ClosedTongue,
        WideOpen,
        LookLeft,
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

    public class AnimalLion: IAnimal
    {
        LionAnimations idleAnimation;

        Random rand = new Random();

        public void SetAnimal(ref Image myImage, ref double dbFrameWidth, ref double dbFrameHeight)
        {
            myImage.Source = new BitmapImage(new Uri(@"/Assets/Animals/lion.png", UriKind.Relative));

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
                    return GetNumFrames(LionAnimations.Walk);
                case Animations.poke:
                    return GetNumFrames(LionAnimations.WideEyes);
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
                    return (int)GetFrameLion(LionAnimations.Walk, iFrame);
                case Animations.poke:
                    return (int)GetFrameLion(LionAnimations.WideEyes, iFrame);
                case Animations.idle:
                    return (int)GetFrameLion(idleAnimation, iFrame);
            }
            return 0;
        }

        private int GetNumFrames(LionAnimations ani)
        {
            switch (ani)
            {
                case LionAnimations.Walk:
                    return 8;
                case LionAnimations.WideEyes:
                    return 1;
                case LionAnimations.Angry:
                    return 2;
                case LionAnimations.LookLeft:
                    return 5;
                case LionAnimations.Blink:
                    return 3;
                case LionAnimations.Sleep:
                    return 16;
                case LionAnimations.ToeTap:
                    return 10;
                default:
                    return 0;
            }
        }

        private FramesLion GetFrameLion(LionAnimations ani, int iFrame)
        {
            switch (ani)
            {
                case LionAnimations.WideEyes:
                    return FramesLion.WideOpen;
                case LionAnimations.Angry:
                    return FramesLion.Angry;
                case LionAnimations.Blink:
                    if(iFrame == 0 || iFrame == 2)
                        return FramesLion.Partial;
                    if(iFrame == 1)
                        return FramesLion.Closed;
                    break;
                case LionAnimations.LookLeft:
                    return FramesLion.LookLeft;
                case LionAnimations.ToeTap:
                     if (iFrame % 3 == 0)
                        return FramesLion.Walk5;
                    return FramesLion.Walk4;
                case LionAnimations.Sleep:
                    if(iFrame < 2)
                        return FramesLion.Partial;
                    if(iFrame < 4)
                        return FramesLion.Closed;
                    if(iFrame < 14)
                        return FramesLion.ClosedTongue;
                    if(iFrame < 15)
                        return FramesLion.Closed;
                    if(iFrame < 16)
                        return FramesLion.Partial;
                    break;
                case LionAnimations.Walk:
                    if (iFrame < 8)
                        return (FramesLion)(iFrame + (int)FramesLion.Walk1);
                    break;
            }

            return FramesLion.Normal;
        }

        public void SetIdleAnimation()
        {
            idleAnimation = (LionAnimations)rand.Next((int)LionAnimations.Blink, (int)LionAnimations.Count);
        }

    }
}
