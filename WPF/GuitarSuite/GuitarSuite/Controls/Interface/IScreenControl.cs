using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GuitarSuite
{
    interface IScreenControl
    {
        void Refresh();
        void UnloadScreen();
    }
}
