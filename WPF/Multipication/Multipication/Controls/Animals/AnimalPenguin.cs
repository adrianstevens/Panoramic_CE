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
    public enum PenguinAnimations
    {
        Walk,
        WideEyes,
        Blink,
        Look,
        ToeTap,//not implimented
        Count,
    };

    public enum FramesPenguin
    {
        Normal,
        UpRight,
        UpLeft,
        DownLeft,
        LeftOnly,
        LeftOnlyLeft,
        LeftOnlyRight,
        Closed,
        WideOpen,
        Walk1,
        Walk2,
        Walk3,
        Walk4,
        Walk5,
        Walk6,
        count,
    };

    public class AnimalPenguin: IAnimal
    {
        PenguinAnimations idleAnimation = PenguinAnimations.Blink;

        Random rand = new Random();

        public void SetAnimal(ref Image myImage, ref double dbFrameWidth, ref double dbFrameHeight)
        {
            myImage.Source = new BitmapImage(new Uri(@"/Assets/Animals/penguin.png", UriKind.Relative));
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
                    return GetNumFrames(PenguinAnimations.Walk);
                case Animations.poke:
                    return GetNumFrames(PenguinAnimations.WideEyes);
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
                    return (int)GetFramePenguin(PenguinAnimations.Walk, iFrame);
                case Animations.poke:
                    return (int)GetFramePenguin(PenguinAnimations.WideEyes, iFrame);
                case Animations.idle:
                    return (int)GetFramePenguin(idleAnimation, iFrame);
            }
            return 0;
        }

        private int GetNumFrames(PenguinAnimations ani)
        {
            switch (ani)
            {
                case PenguinAnimations.Blink:
                    return 3;
                case PenguinAnimations.Look:
                    return 6;
                case PenguinAnimations.Walk:
                    return 6;
                case PenguinAnimations.WideEyes:
                    return 1;
                default:
                    return 0;
            }
        }

        private FramesPenguin GetFramePenguin(PenguinAnimations ani, int iFrame)
        {
            switch (ani)
            {
                case PenguinAnimations.Blink:
                    //if statement is cleaner
                    if (iFrame == 0)
                        return FramesPenguin.Closed;
                    if (iFrame == 2)
                        return FramesPenguin.Closed;
                    if (iFrame == 3)
                        return FramesPenguin.Closed;
                    break;

                case PenguinAnimations.Look:
                    //if statement is cleaner
                    if (iFrame == 0)
                        return FramesPenguin.DownLeft;
                    if (iFrame == 1)
                        return FramesPenguin.UpLeft;
                    if (iFrame == 2)
                        return FramesPenguin.UpRight;
                    if (iFrame == 3)
                        return FramesPenguin.UpRight;
                    if (iFrame == 4)
                        return FramesPenguin.UpLeft;
                    if (iFrame == 5)
                        return FramesPenguin.DownLeft;
                    break;

                case PenguinAnimations.Walk:
                    if (iFrame < 6)
                        return (FramesPenguin)(iFrame + (int)FramesPenguin.Walk1);
                    break;

                case PenguinAnimations.WideEyes:
                    if (iFrame == 0)
                        return FramesPenguin.WideOpen;
                    break;
            }
            return FramesPenguin.Normal;
        }

        public void SetIdleAnimation()
        {
            idleAnimation = (PenguinAnimations)rand.Next((int)PenguinAnimations.Blink, (int)PenguinAnimations.ToeTap);
        }

        

    }
}
