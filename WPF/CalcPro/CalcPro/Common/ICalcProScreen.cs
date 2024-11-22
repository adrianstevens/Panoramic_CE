using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;

namespace CalcPro
{
    interface ICalcProScreen
    {
        bool IsScreenLoaded();
        void LoadScreen(bool bOnScreen);
        void CloseSaveState();
        void OnBackButton(object sender, CancelEventArgs e);
    }
}
