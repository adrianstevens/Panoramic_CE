using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;

namespace Rescue.Controls
{
    public interface IAnimal
    {
        void SetAnimal(ref Image myImage, ref double dbFrameWidth, ref double dbFrameHeight);

        bool CanAnimate();

        int GetNumFrames(Animations ani);//common animations 

        int GetCurFrame(Animations ani, int iFrame);

        void SetIdleAnimation();
    }
}
