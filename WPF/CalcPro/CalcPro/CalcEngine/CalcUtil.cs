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

namespace CalcPro
{
    public class CalcUtil
    {
        public CalcUtil()
        {
        }


        /********************************************************************

	        Function:	FillOpenBrackets

	        Arguments:	szEquation - the actual equation
				
				
				
	
	        purpose:	Function that will parse through the equation and closes any
				        brackets that are open
				        eg. sin(54+5  ------> sin(54+5)
        *********************************************************************/
        public static bool FillOpenBrackets(ref string szEquation)
        {
	        int iOpenBracketCnt	= GetOpenBracketCount(szEquation);	// Open bracet count   (
	        int iCloseBracketCnt	= GetCloseBracketCount(szEquation);	// Close bracet count  )
	
	        for(int i = 0; i < (iOpenBracketCnt - iCloseBracketCnt); i++)
	        {
		        szEquation += ")";
	        }
	        return true;
        }


        public static int GetOpenBracketCount(string szEquation)
        {
            int iLength = szEquation.Length;
	        int iBracketPos		= -1;	// Bracket Position
	        int iCount=0;
	
	        while(iCount < iLength)
	        {
		        iBracketPos = szEquation.IndexOf("(", iBracketPos + 1);
		        if(iBracketPos==-1)
			        break;		
		        iCount++;
	        }
	        return iCount;
        }

        public static int GetCloseBracketCount(string szEquation)
        {
            int iLength = szEquation.Length;
	        int iBracketPos		= -1;	// Bracket Position
	        int iCount=0;

	        while(iCount < iLength)
	        {
		        iBracketPos = szEquation.IndexOf(")", iBracketPos + 1);
		        if(iBracketPos==-1)
			        break;		
		        iCount++;
	        }
	        return iCount;
        }

        bool GetTextFromClipboard(ref string szDestination)
        {
	        return false;
        }


        bool SendTextToClipboard(string szSource)
        {
	       
	        return false;
        }

        bool FindAndReplaceString(string szEquation,
		   							string szToReplace,
									string szToInsert,
									ref string szResult)
        {
            szResult = szEquation;
            szResult.Replace(szToReplace, szToInsert);

            return false;
        }


    }
}
