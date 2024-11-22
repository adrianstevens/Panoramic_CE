using System;
using System.Windows;
using System.Windows.Controls;

//this interface really isn't needed ....
namespace CalcPro
{
    public interface ICalcProBtn
    {
        CalcProBtn cBtn
        {
            get;
            set;
        }

        EventHandler<EventArgs> BtnSelected
        {
            get;
            set;
        }
        
        bool SetMainText(string szMain);

        bool SetExpText(string szExp);

        bool Set2ndText(string szMain);

        bool Set2ndExpText(string szExp);
    }
}
