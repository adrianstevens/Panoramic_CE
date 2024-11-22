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
    public enum HippoAnimations
    {
        Walk,
        WideEyes,
        Blink,
        Look,
        Yawn,
        ToeTap,
        Count,
    };

    public enum FramesHippo
    {
        Normal,
        Partial,
        Closed,
        PartialYawn,
        Yawn,
        LookDownRight,
        LookUp,
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

    public class AnimalHippo: IAnimal
    {
        HippoAnimations idleAnimation;

        Random rand = new Random();

        public void SetAnimal(ref Image myImage, ref double dbFrameWidth, ref double dbFrameHeight)
        {
            myImage.Source = new BitmapImage(new Uri(@"/Assets/Animals/hippo.png", UriKind.Relative));

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
                    return GetNumFrames(HippoAnimations.Walk);
                case Animations.poke:
                    return GetNumFrames(HippoAnimations.WideEyes);
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
                    return (int)GetFrameHippo(HippoAnimations.Walk, iFrame);
                case Animations.poke:
                    return (int)GetFrameHippo(HippoAnimations.WideEyes, iFrame);
                case Animations.idle:
                    return (int)GetFrameHippo(idleAnimation, iFrame);
            }
            return 0;
        }

        private int GetNumFrames(HippoAnimations ani)
        {
            switch (ani)
            {
                case HippoAnimations.Walk:
                    return 8;
                case HippoAnimations.WideEyes:
                    return 1;
                case HippoAnimations.Blink:
                    return 3;
                case HippoAnimations.Look:
                    return 5;
                case HippoAnimations.ToeTap:
                    return 12;
                case HippoAnimations.Yawn:
                    return 10;
                default:
                    return 0;
            }
        }

        private FramesHippo GetFrameHippo(HippoAnimations ani, int iFrame)
        {
            switch (ani)
            {
                case HippoAnimations.Blink:
                    //if statement is cleaner
                    if (iFrame == 0)
                        return FramesHippo.Partial;
                    if (iFrame == 1)
                        return FramesHippo.Closed;
                    if (iFrame == 2)
                        return FramesHippo.Partial;
                    break;
                case HippoAnimations.WideEyes:
                    if (iFrame == 0)
                        return FramesHippo.LookUp;
                    break;
                case HippoAnimations.Look:
                    if (iFrame < 5)
                        return FramesHippo.LookDownRight;
                    break;
                case HippoAnimations.Walk:
                    if (iFrame < 7)
                        return (FramesHippo)(iFrame + (int)FramesHippo.Walk1);
                    break;
                case HippoAnimations.ToeTap:
                    if (iFrame % 3 == 0)
                        return FramesHippo.Walk5;
                    return FramesHippo.Walk4;
                case HippoAnimations.Yawn:
                    if (iFrame == 0)
                        return FramesHippo.Partial;
                    else if (iFrame == 1 || iFrame == 2)
                        return FramesHippo.Closed;
                    else if (iFrame == 3)
                        return FramesHippo.PartialYawn;
                    else if (iFrame > 3 && iFrame < 9)
                        return FramesHippo.Yawn;
                    else if (iFrame == 9)
                        return FramesHippo.PartialYawn;
                    break;
            }

            return FramesHippo.Normal;
        }

        public void SetIdleAnimation()
        {
            idleAnimation = (HippoAnimations)rand.Next((int)HippoAnimations.Blink, (int)HippoAnimations.Count);
        }
    }
}
