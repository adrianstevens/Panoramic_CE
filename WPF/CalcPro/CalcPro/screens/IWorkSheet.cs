using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Windows;

namespace CalcPro
{
    interface IWorkSheet
    {
        bool OnCalc(int iIndex, FrameworkElement fwElem);
        
        void OnReset();
        
        void OnGraph();
        
        void OnLoad();
        
        void OnSave();

        void OnUp(int iCurrentIndex);

        void OnDown(int iCurrentIndex);//for stats

        void OnClose();

        bool Init(ref ObservableCollection<MyListboxItems> list, ref System.Windows.Controls.TextBlock tBlock);

        bool SetScreenValue(int iIndex, ref string szValue);

        bool GetScreenValue(int iIndex, ref string szValue);

        bool SetDataIndex(ref ObservableCollection<MyListboxItems> list, int iIndex);//really only for stats

        
    }
}
