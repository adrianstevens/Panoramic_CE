using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CalcPro
{
    public enum EnumCalcDisplay
    {
        Normal,
        NormalExp,
        Expression,
        ExpressionExp,
        Fractions,
        RPN,
        Currency,
        CurrencyUpdate,
        Unit,
        UnitCategory,
        Constants,
        WorkSheet,
        Graphing,
        Count,
    }

    public interface ICalcProDisplay
    {
        void SetDisplayType(EnumCalcDisplay eDisplay, bool bDataBind);
        void SetDataContext();
        void ReleaseDataContext();
    }

    
}
