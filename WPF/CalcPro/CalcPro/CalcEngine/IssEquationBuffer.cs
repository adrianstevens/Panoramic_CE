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
using System.Collections.Generic;

namespace CalcPro
{
    public class StructEquation
    {
        public string szEquation { get; set; }
        public string szResult { get; set; }
    }

    public class IssEquationBuffer
    {
        public IssEquationBuffer()
        {
            ClearBuffer();
        }

        //static int  MAX_EQUATIONS	=20;


        List<StructEquation> arrItems = new List<StructEquation>();

	    int					iCurrentIndex;



        public void ClearBuffer()
        {
            if(arrItems != null)
                arrItems.Clear();
            iCurrentIndex = -1;

        }

        public void AddToEquation(string szEq, ref string szAnswer)
        {
            // first we check and see if the equation we want to add and the last one 
            // are not the same
            if (arrItems.Count > 0)
            {
                StructEquation sLast = arrItems[arrItems.Count - 1];
                if (szEq == sLast.szEquation)
                    return;
            }

            StructEquation newItem = new StructEquation();

            newItem.szEquation = szEq;
            newItem.szResult = szAnswer;

            arrItems.Add(newItem);
           

            // set the index to the last position
            iCurrentIndex = arrItems.Count - 1;

        }

        public bool GetLastEquation(ref string szEq, ref string szAns)
        {
            if (arrItems.Count == 0)
                return false;

            // find out if we have to loop around
            if (iCurrentIndex == 0)
                return false;

            iCurrentIndex--;

            szEq = arrItems[iCurrentIndex].szEquation;
            szAns = arrItems[iCurrentIndex].szResult;

            return true;
        }

        public bool GetNextEquation(string szEq, ref string szAns)
        {
            if (arrItems.Count == 0)
                return false;

            //make sure we don't go over...you know what I'm sayin?
            if (iCurrentIndex >= arrItems.Count)
                return false;

            iCurrentIndex++;

            // find out if we have to loop around
            if (iCurrentIndex == arrItems.Count)
            {
                szEq = null;
                szAns = null;
            }
            else
            {
                szEq = arrItems[iCurrentIndex].szEquation;
                szAns = arrItems[iCurrentIndex].szResult;
            }
            return true;
        }

        public bool IsUpAvailable()
        {
            if (iCurrentIndex > 0)
                return true;
            else
                return false;
        }

        public bool IsDownAvailable()
        {
            if (iCurrentIndex < arrItems.Count && arrItems.Count != 0)
                return true;
            else
                return false;
        }
    }


}
