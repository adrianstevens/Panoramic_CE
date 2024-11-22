using System;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Sudoku.Engine;
using System.IO;

namespace Sudoku
{
    public struct TypeSudokuSkin
    {
        public Color crPencil1;
        public Color crPencil2;
        public Color crPencilKakuro;  //for kakuro    
        public Color crTopText;
        public Color crTotal1;        //for killer sudoku
        public Color crTotal2;        //for killer sudoku
        public Color crOutline1;      //for killer sudoku
        public Color crOutline2;      //for killer sudoku
        public Color crOutline3;      //for killer sudoku
        public Color crOutline4;      //for killer sudoku
        public Color crOutline5;      //for killer sudoku
        public int iNumOutlineColors;
        public Color crKakuroTotals;  //for Kakuro
        public Color crGrad1;
        public Color crGrad2;
        public Color crSelector;
        public int iSelAlpha;

        public BitmapImage imgFont1;
        public BitmapImage imgFont2;
        public BitmapImage imgCellArray;
        public BitmapImage imgCellArray2;
        public BitmapImage imgBackground;
        public WriteableBitmap wbCellArray;
        public WriteableBitmap wbCellArray2;
        /*public int uiFont1;
        public int uiFont2;
        public int uiCellArray;
        public int uiCellArray2;
        public int uiBackground;*/

     //    CIssGDIEx gdiSQs;
    //    CIssGDIEx gdiSQs2;    
    //    CIssGDIEx gdiFont1;
    //    CIssGDIEx gdiFont2;

        public EnumSudokuSkins eSkin;
        public EnumBackground eBackground;
    };

    public enum EnumSquares
    {
        Board1,
        Board2,
        Given,
        Guess1,
        Guess2,
        Locked,
        Highlighted,
        Selector,
        Kakuro,
        Killer1,//seperate image
        Killer2,
        KillerSel,
        Killer3,
        Killer4,
        KillerSel2,
        None = 999,
    };

    public enum EnumSudokuSkins
    {
        Glass,
        Wood,
        Grey,
        Custom0,
        Custom1,
        Custom2,
        Custom3,
        Custom4,
        Custom5,
        Notepad,
        Count,
    };

    enum EnumPicSelection
    {
        PIC_Camera,
        PIC_Picture,
        PIC_Given,
        PIC_None,
    };


    public class ObjGui
    {
       public TypeSudokuSkin sSkin = new TypeSudokuSkin();

       double _dbGridSize;

        Color RGB(byte R, byte G, byte B)
        {
            return Color.FromArgb(255, R, G, B);
        }

        Color RGB(uint uiColor)
        {
            //return Color.FromArgb(255, (byte)((uiColor & 0xff0000) >> 0x10), (byte)((uiColor & 0xff00) >> 8), (byte)(uiColor & 0xff));
            return Color.FromArgb(255, (byte)(uiColor & 0xff), (byte)((uiColor & 0xff00) >> 8), (byte)((uiColor & 0xff0000) >> 0x10));
        }

        //for skin correction based on game and skin
        public EnumSquares GetSquareEnum(ref EnumSquares eSquare, EnumSudokuType eType, bool bSameRowAsSelector)
        {
            switch(eType)
            {
            case EnumSudokuType.STYPE_Kakuro:
                switch(sSkin.eSkin)
                {
                case EnumSudokuSkins.Glass:
        	        break;
                case EnumSudokuSkins.Grey:
                    break;
                case EnumSudokuSkins.Notepad:
                    if(eSquare == EnumSquares.Selector)
                        return EnumSquares.Given;//3rd ...
                    else if(eSquare == EnumSquares.Kakuro)
                        return EnumSquares.Kakuro;//9th
                    else
                        return EnumSquares.Board2;//2nd

                case EnumSudokuSkins.Custom0:
                case EnumSudokuSkins.Custom1:
                case EnumSudokuSkins.Custom4:
                case EnumSudokuSkins.Custom5:

                    if(eSquare == EnumSquares.Locked)
                        return EnumSquares.Board1;
                    break;
                case EnumSudokuSkins.Custom2:
                    if(eSquare == EnumSquares.Locked)
                        return EnumSquares.Locked;
                    break;
                case EnumSudokuSkins.Custom3:
                    if(eSquare == EnumSquares.Locked)
                        return EnumSquares.Board2;
                    break;
                default://all custom skins
                    break;
                }
              /*  if (eSquare == EnumSquares.Given && Globals.Settings.bDrawBlanks == false)
                    return EnumSquares.None;*/

    	        break;
            case EnumSudokuType.STYPE_KenKen:
            case EnumSudokuType.STYPE_Killer:
                switch (sSkin.eSkin)
                {
                case EnumSudokuSkins.Glass:
                    if(eSquare == EnumSquares.Board1)
                        return EnumSquares.Killer3;
                    if(eSquare == EnumSquares.Board2)
                        return EnumSquares.Killer4;
                    if(eSquare == EnumSquares.Selector)
                        return EnumSquares.KillerSel2;
                    break;
                case EnumSudokuSkins.Grey:
                    if(eSquare == EnumSquares.Board1)
                        return EnumSquares.Killer1;
                    if(eSquare == EnumSquares.Board2)
                        return EnumSquares.Killer2;
                //    if(eSquare == EnumSquares.Highlighted)
                //        return EnumSquares.None;//blank //winmo changed the grouping color ... we're not doing that
                    break;
                case EnumSudokuSkins.Notepad:
                    if(eSquare == EnumSquares.Guess1)
                        return EnumSquares.Board1;
                    if(eSquare == EnumSquares.Guess2)
                        return EnumSquares.Board2;
                    if(eSquare == EnumSquares.Highlighted)
                        return EnumSquares.None;//blank
                    break;
                default:
                    break;
                }
                break;    
             case EnumSudokuType.STYPE_Sudoku:
                switch (sSkin.eSkin)
                {
                //only correction
                case EnumSudokuSkins.Notepad:
                    if(UseNotePadBG(Globals.Game.GetGameType()))//uugghhh
                    {
                        if(eSquare == EnumSquares.Board1 ||
                            eSquare == EnumSquares.Guess1 ||
                            eSquare == EnumSquares.Board2 ||
                            eSquare == EnumSquares.Guess2 ||
                            eSquare == EnumSquares.Given || 
                            eSquare == EnumSquares.Locked)
                            return EnumSquares.Guess1;//3rd ... EnumSquares.None; //blank over the notepad
                    }
                    if(eSquare == EnumSquares.Guess1 || eSquare == EnumSquares.Board1 || eSquare == EnumSquares.Given)
                        return EnumSquares.Board1;
                    else if(eSquare == EnumSquares.Guess2 || eSquare == EnumSquares.Board2)
                        return EnumSquares.Board2;
                    //else if(eSquare == EnumSquares.
                    if(eSquare == EnumSquares.Highlighted)
                    {
                        if(bSameRowAsSelector)
                            return EnumSquares.Locked;
                        else
                            return EnumSquares.Highlighted;//here we need to check and sometimes return locked
                    }
                    else if(eSquare == EnumSquares.Selector)
                        return EnumSquares.Selector;
                    return EnumSquares.None;//blank cell
                 
                default:
                   break;
                }
                break;
            case EnumSudokuType.STYPE_Greater:
                switch (sSkin.eSkin)
                {
                    case EnumSudokuSkins.Notepad:
                        {
                            if (eSquare == EnumSquares.Highlighted)
                            {
                                if (bSameRowAsSelector)
                                    return EnumSquares.Locked;
                                else
                                    return EnumSquares.Highlighted;//here we need to check and sometimes return locked
                            }
                            else if (eSquare == EnumSquares.Selector)
                                return EnumSquares.Selector;

                            if (eSquare == EnumSquares.Guess1)
                                return EnumSquares.Board1;
                            if (eSquare == EnumSquares.Guess2)
                                return EnumSquares.Board2;
                            if (eSquare == EnumSquares.Given)
                                return EnumSquares.Kakuro;
                        }
                        break;

                    default:
                        break;

                }
                break;
            default:
                break;
            }

            return eSquare;
        }

        public Color GetOutlineColor(int iSection, bool bHighlight)
        {
            int iColor = iSection%((int)sSkin.iNumOutlineColors);

            if( (sSkin.eSkin == EnumSudokuSkins.Glass || sSkin.eSkin == EnumSudokuSkins.Grey) &&
                bHighlight)
                return RGB(255,240,0); //yellow BUGBUG hack

            if(iColor == 0)
                return sSkin.crOutline1;
            if(iColor == 1)
                return sSkin.crOutline2;
            if(iColor == 2)
                return sSkin.crOutline3;
            if(iColor == 3)
                return sSkin.crOutline4;
            return sSkin.crOutline5;
        }

        public Color GetBackgroundColor(bool bColor1)
        {
            return GetBackgroundColor(bColor1, EnumBackground.White);
        }

        public Color GetBackgroundColor(bool bColor1, EnumBackground eBack)
        {
            switch(eBack)
            {
            case EnumBackground.White:
                return RGB(255,255,255);
            case EnumBackground.Black:
                return RGB(0,0,0);
            case EnumBackground.Grey:
                return RGB(102,102,102);
            case EnumBackground.Green:
                return RGB(146,189,103);
            case EnumBackground.LPink:
                return RGB(255,185,209);
            case EnumBackground.Orange:
                return RGB(255,145,70);
            case EnumBackground.Blue:
                return RGB(104,163,210);
            case EnumBackground.DBlue:
                return RGB(40,66,98);
            case EnumBackground.DGreen:
                return RGB(49,85,43);
            case EnumBackground.Pink:
                return RGB(255,91,149);
            case EnumBackground.Purple:
                return RGB(151,118,201);
            case EnumBackground.PurpleWhite:
                return bColor1?RGB(106,66,168):Colors.White;
            case EnumBackground.BlackWhite:
                return bColor1?RGB(0,0,0):Colors.White;
            case EnumBackground.GreenWhite:
                return bColor1?RGB(146,189,103):Colors.White;
            case EnumBackground.BlueWhite:
                return bColor1?RGB(104,163,210):Colors.White;
            case EnumBackground.DBlueWhite:
                return bColor1?RGB(40,66,98):Colors.White;
            case EnumBackground.PinkWhite:
                return bColor1?RGB(255,91,149):Colors.White;
            case EnumBackground.OrangeWhite:
                return bColor1?RGB(255,109,8):Colors.White;
            case EnumBackground.GreyBlack:
                return bColor1?RGB(153,153,153):Colors.Black;
            case EnumBackground.RedBlack:
                return bColor1?RGB(98,0,0):Colors.Black;
            case EnumBackground.PurpleBlack:
                return bColor1?RGB(98,0,130):Colors.Black;
            case EnumBackground.BlueBlack:
                return bColor1?RGB(40,117,210):Colors.Black;
            case EnumBackground.GreenBlack:
                return bColor1?RGB(88,112,56):Colors.Black;
            case EnumBackground.DBlueBlue:
                return bColor1?RGB(0,18,91):RGB(44,117,170);
            case EnumBackground.BlueGreen:
                return bColor1?RGB(44,117,170):RGB(120,186,75);
            case EnumBackground.DGReenGreen:
                return bColor1?RGB(0,55,0):RGB(120,186,75);
            }
            return Colors.White;
        }

        bool UseNotePadBG(EnumSudokuType eType)
        {
            if(eType != EnumSudokuType.STYPE_Sudoku)// && eType != EnumSudokuType.STYPE_Greater)
                return false;

            return true;
        }

        public bool LoadSkin(EnumSudokuSkins eSkin, EnumBackground eBackground, bool bForceReload, double dbGridSize)
        {
             if(eSkin == EnumSudokuSkins.Count)
                return false;//so we can init the gui before loading the skin

             _dbGridSize = dbGridSize;

            // start with some defaults
            sSkin.eSkin = eSkin;
            sSkin.eBackground = eBackground;

            sSkin.imgFont1 = null;
            sSkin.imgFont2 = null;
            sSkin.imgCellArray = null;
            sSkin.imgCellArray2 = null;
            sSkin.imgBackground = null;
            sSkin.wbCellArray = null;
            sSkin.wbCellArray2 = null;

            sSkin.imgCellArray2 = new BitmapImage(new Uri(@"/Assets/killer.png", UriKind.Relative));
            sSkin.iNumOutlineColors = 1;
            sSkin.crOutline1 = RGB(0);//black for the custom skins
            sSkin.crKakuroTotals = RGB(0);

            switch(eSkin)
            {
            case EnumSudokuSkins.Glass:
                sSkin.crPencil1      =  RGB(0xDDDDDD);
                sSkin.crPencil2      =  RGB(0x444444);
                sSkin.crPencilKakuro =  RGB(0xDDDDDD);  
                sSkin.crSelector     =  RGB(0x00FFFF);
                sSkin.crTopText      =  RGB(0xFFFFFF); //RGB(108,168,225);
                sSkin.crTotal1       =  RGB(0);//killer sudoku total values
                sSkin.crTotal2       =  RGB(0);//killer sudoku total values
                sSkin.crOutline1     =  RGB(255,153,0);
                sSkin.crOutline2     =  RGB(0,204,0);
                sSkin.crOutline3     =  RGB(245,0,255);
                sSkin.iNumOutlineColors = 3;
                sSkin.imgCellArray  = new BitmapImage(new Uri(@"/Assets/Sqs_glass.png", UriKind.Relative));
                sSkin.imgFont1 = new BitmapImage(new Uri(@"/Assets/Font_glass_white.png", UriKind.Relative));
                sSkin.imgFont2 = new BitmapImage(new Uri(@"/Assets/Font_glass_grey.png", UriKind.Relative));

               /* sSkin.imgFont1. = 270;
                sSkin.imgFont1.Height = 36;

                sSkin.imgFont2.Width = 270;
                sSkin.imgFont2.Height = 36;*/




                sSkin.crGrad1 = RGB(10, 49, 101);
                sSkin.crGrad2       =  RGB(0);
                break;
            case EnumSudokuSkins.Wood:
                sSkin.crPencil1     =  RGB(0xDDDDDD);
                sSkin.crPencil2     =  RGB(0x222222);
                sSkin.crPencilKakuro=  RGB(0xDDDDDD);  
                sSkin.crSelector    =  RGB(0x00FFFF);
                sSkin.crTopText     =  RGB(0xFFFFFF);
                sSkin.crTotal1      =  RGB(0x00FFFF);
                sSkin.crTotal2      =  RGB(0);
                sSkin.crOutline1     =  RGB(0x00FFFF);
                sSkin.crKakuroTotals = RGB(0xFFFFFF);

                sSkin.imgCellArray = new BitmapImage(new Uri(@"/Assets/Sqs_wood.png", UriKind.Relative));
                sSkin.imgFont1 = new BitmapImage(new Uri(@"/Assets/Font_Grey_white.png", UriKind.Relative));
                sSkin.imgFont2 = new BitmapImage(new Uri(@"/Assets/Font_Grey_black.png", UriKind.Relative));
                //sSkin.imgBackground = new BitmapImage(new Uri(@"../../Assets/wood.jpg", UriKind.Relative));

                sSkin.crGrad1       =  RGB(0);
                sSkin.crGrad2       =  RGB(0);
    	        break;
            case EnumSudokuSkins.Notepad:
                sSkin.crPencil1     =  RGB(0x666666);
                sSkin.crPencil2     =  RGB(0x666666);
                sSkin.crPencilKakuro=  RGB(0x666666);  
                sSkin.crSelector    =  RGB(0x00FFFF);
                sSkin.crTopText     =  RGB(0xFFFFFF); //0x666666;
                sSkin.crTotal1      =  RGB(0xFFFFFF);
                sSkin.crTotal2      =  RGB(0xFFFFFF);
                sSkin.crOutline1    =  RGB(0,0,255);
                sSkin.crOutline2    =  RGB(255,0,0);
                sSkin.crKakuroTotals = RGB(0x444444);
                sSkin.crOutline3    =  RGB(0x444444);
                sSkin.iNumOutlineColors = 3;

                sSkin.imgCellArray = new BitmapImage(new Uri(@"/Assets/Sqs_notepad.png", UriKind.Relative));
                sSkin.imgFont1 = new BitmapImage(new Uri(@"/Assets/Font_Notepad_blue.png", UriKind.Relative));
                sSkin.imgFont2 = new BitmapImage(new Uri(@"/Assets/Font_Notepad_red.png", UriKind.Relative));
                //sSkin.imgBackground = new BitmapImage(new Uri(@"/Assets/notepad.jpg", UriKind.Relative));

                sSkin.crGrad1       =  RGB(0xFFFFFF);
                sSkin.crGrad2       =  RGB(0xFFFFFF);

                //lets handle this here


                break;
            case EnumSudokuSkins.Grey:
                sSkin.crPencil1     =  RGB(0xDDDDDD);
                sSkin.crPencil2     =  RGB(0x444444);
                sSkin.crPencilKakuro=  RGB(0xDDDDDD);
                sSkin.crSelector    =  RGB(0x00FFFF);
                sSkin.crTopText     =  RGB(0xFFFFFF); //0xDDDDDD;
                sSkin.crTotal1      =  RGB(0xFFFFFF);
                sSkin.crTotal2      =  RGB(0);
                sSkin.crOutline1    =  RGB(60,153,247);
                sSkin.crOutline2    =  RGB(0,204,0);
                sSkin.crOutline3    =  RGB(245,0,255);
                sSkin.iNumOutlineColors = 3;

                sSkin.imgCellArray = new BitmapImage(new Uri(@"/Assets/Sqs_grey.png", UriKind.Relative));
                sSkin.imgFont1 = new BitmapImage(new Uri(@"/Assets/Font_Grey_white.png", UriKind.Relative));
                sSkin.imgFont2 = new BitmapImage(new Uri(@"/Assets/Font_Grey_black.png", UriKind.Relative));

                sSkin.crGrad1       =  RGB(46,55,65);
                sSkin.crGrad2       =  RGB(86,98,112);
                break;
            case EnumSudokuSkins.Custom0:
                sSkin.crPencil1     =  RGB(0xDDDDDD);
                sSkin.crPencil2     =  RGB(0x444444);
                sSkin.crPencilKakuro=  RGB(0x444444);
                sSkin.crSelector    =  RGB(0x00FFFF);
                sSkin.crTopText     =  RGB(0xFFFFFF); //0;
                sSkin.crTotal1      =  RGB(0x666666);
                sSkin.crTotal2      =  RGB(0x666666);
                //sSkin.crOutline1    =  0x666666;

                sSkin.imgCellArray = new BitmapImage(new Uri(@"/Assets/Sqs_custom.png", UriKind.Relative));
                sSkin.imgFont1 = new BitmapImage(new Uri(@"/Assets/Font_glass_white.png", UriKind.Relative));
                sSkin.imgFont2 = new BitmapImage(new Uri(@"/Assets/Font_glass_grey.png", UriKind.Relative));

                sSkin.crGrad1       =  RGB(0);
                sSkin.crGrad2       =  RGB(0);

                break;
            case EnumSudokuSkins.Custom1:
                sSkin.crPencil1     =  RGB(0x333333);
                sSkin.crPencil2     =  RGB(0x333333);
                sSkin.crPencilKakuro=  RGB(0x333333);
                sSkin.crSelector    =  RGB(0x00FFFF);
                sSkin.crTopText     =  RGB(0xFFFFFF); //0;
                sSkin.crTotal1      =  RGB(0x666666);
                sSkin.crTotal2      =  RGB(0x666666);
                //sSkin.crOutline1     =  0;
        
                sSkin.imgCellArray = new BitmapImage(new Uri(@"/Assets/Sqs_custom1.png", UriKind.Relative));
                sSkin.imgFont1 = new BitmapImage(new Uri(@"/Assets/Font_custom2_white.png", UriKind.Relative));
                sSkin.imgFont2 = new BitmapImage(new Uri(@"/Assets/Font_custom2_black.png", UriKind.Relative));

                sSkin.crGrad1       =  RGB(0xFFFFFF);
                sSkin.crGrad2       =  RGB(0xFFFFFF);

    	        break;
            case EnumSudokuSkins.Custom2:
                sSkin.crPencil1     =  RGB(0xDDDDDD);
                sSkin.crPencil2     =  RGB(0x444444);
                sSkin.crPencilKakuro=  RGB(0x444444);
                sSkin.crSelector    =  RGB(0x00FFFF);
                sSkin.crTopText     =  RGB(0xFFFFFF); //0;
                sSkin.crTotal1      =  RGB(0x666666);
                sSkin.crTotal2      =  RGB(0x666666);
                //sSkin.crOutline1     =  0;

                sSkin.imgCellArray = new BitmapImage(new Uri(@"/Assets/Sqs_custom2.png", UriKind.Relative));
                sSkin.imgFont1 = new BitmapImage(new Uri(@"/Assets/Font_custom2_white.png", UriKind.Relative));
                sSkin.imgFont2 = new BitmapImage(new Uri(@"/Assets/Font_custom2_black.png", UriKind.Relative));

                sSkin.crGrad1       =  RGB(0xFFFFFF);
                sSkin.crGrad2       =  RGB(203,254,215);

    	        break;
            case EnumSudokuSkins.Custom3:
                sSkin.crPencil1     =  RGB(0xDDDDDD);
                sSkin.crPencil2     =  RGB(0x444444);
                sSkin.crPencilKakuro=  RGB(0x444444);
                sSkin.crSelector    =  RGB(0x00FFFF);
                sSkin.crTopText     =  RGB(0xFFFFFF); //0;
                sSkin.crTotal1      =  RGB(0x666666);
                sSkin.crTotal2      =  RGB(0x666666);
                //sSkin.crOutline1     =  0x666666;
                sSkin.iSelAlpha     =  100; 

                sSkin.imgCellArray = new BitmapImage(new Uri(@"/Assets/Sqs_custom3.png", UriKind.Relative));
                sSkin.imgFont1 = new BitmapImage(new Uri(@"/Assets/Font_custom2_white.png", UriKind.Relative));
                sSkin.imgFont2 = new BitmapImage(new Uri(@"/Assets/Font_custom2_black.png", UriKind.Relative));

                sSkin.crGrad1       =  RGB(0xFFFFFF);
                sSkin.crGrad2       =  RGB(128,188,252);
                break;
            case EnumSudokuSkins.Custom4:
                sSkin.crPencil1     =  RGB(0xDDDDDD);
                sSkin.crPencil2     =  RGB(0x444444);
                sSkin.crPencilKakuro=  RGB(0x444444);
                sSkin.crSelector    =  RGB(0x00FFFF);
                sSkin.crTopText     =  RGB(0xFFFFFF); //0;
                sSkin.crTotal1      =  RGB(0x666666);
                sSkin.crTotal2      =  RGB(0x666666);
                //sSkin.crOutline1     =  0x666666;
                sSkin.iSelAlpha     =  100; 

                sSkin.imgCellArray = new BitmapImage(new Uri(@"/Assets/Sqs_custom4.png", UriKind.Relative));
                sSkin.imgFont1 = new BitmapImage(new Uri(@"/Assets/Font_custom2_black.png", UriKind.Relative));
                sSkin.imgFont2 = new BitmapImage(new Uri(@"/Assets/Font_custom2_black.png", UriKind.Relative));
                sSkin.crGrad1       =  RGB(0xFFFFFF);
                sSkin.crGrad2       =  RGB(248,199,216);
                break;
            case EnumSudokuSkins.Custom5:
                sSkin.crPencil1     =  RGB(0xDDDDDD);
                sSkin.crPencil2     =  RGB(0x444444);
                sSkin.crPencilKakuro=  RGB(0x444444);
                sSkin.crSelector    =  RGB(0x00FFFF);
                sSkin.crTopText     =  RGB(0xFFFFFF); //0;
                sSkin.crTotal1      =  RGB(0x666666);
                sSkin.crTotal2      =  RGB(0x666666);
                //sSkin.crOutline1     =  0x666666;
                sSkin.iSelAlpha     =  100; 
                sSkin.imgCellArray = new BitmapImage(new Uri(@"/Assets/Sqs_custom5.png", UriKind.Relative));
                sSkin.imgFont1 = new BitmapImage(new Uri(@"/Assets/Font_custom2_black.png", UriKind.Relative));
                sSkin.imgFont2 = new BitmapImage(new Uri(@"/Assets/Font_custom2_black.png", UriKind.Relative));

                sSkin.crGrad1       =  RGB(164,66,0);
                sSkin.crGrad2       =  RGB(0);
            
                break;
            default: 
                return false;
            }

      /*      if (sSkin.imgCellArray != null && sSkin.imgCellArray.PixelWidth > 0)
            {
                int iWidth = (int)(sSkin.imgCellArray.PixelWidth * _dbGridSize / sSkin.imgCellArray.PixelHeight);
                int iHeight = (int)_dbGridSize;

                sSkin.wbCellArray = new WriteableBitmap(iWidth, iHeight);

                sSkin.imgCellArray.CreateOptions = BitmapCreateOptions.None;
                sSkin.imgCellArray.ImageOpened += cellArray_ImageOpened;
            }

            if (sSkin.imgCellArray2 != null && sSkin.imgCellArray2.PixelWidth > 0)
            {
                int iWidth = (int)(sSkin.imgCellArray2.PixelWidth * _dbGridSize / sSkin.imgCellArray2.PixelHeight);
                int iHeight = (int)_dbGridSize;

                sSkin.wbCellArray2 = new WriteableBitmap(iWidth, iHeight);

                sSkin.imgCellArray2.CreateOptions = BitmapCreateOptions.None;
                sSkin.imgCellArray2.ImageOpened += cellArray2_ImageOpened;
            }*/

            /*if(WIDTH(rcCell) > 0)
            {
                sSkin.gdiSQs.Destroy();
                sSkin.gdiFont1.Destroy();
                sSkin.gdiFont2.Destroy();
                
                SetCellSize(rcCell, iGridSize, eGameType);
            }

            LoadBackground(eGameType);*/

            

            return true;
        }

        public void LoadBackground(ref Grid gridBackground, EnumSudokuType eType)
        {
            return;

            if (sSkin.eSkin == EnumSudokuSkins.Notepad)
            {
                if (UseNotePadBG(eType) && sSkin.imgBackground != null)
                {
                    gridBackground.Background = GetImageBrush(ref sSkin.imgBackground);
                }
                else
                {
                    int iR, iG, iB;

                    iR = (sSkin.crGrad1.R + sSkin.crGrad2.R) / 2;
                    iG = (sSkin.crGrad1.G + sSkin.crGrad2.G) / 2;
                    iB = (sSkin.crGrad1.B + sSkin.crGrad2.B) / 2;

                    gridBackground.Background = new SolidColorBrush(Color.FromArgb(255, (byte)iR, (byte)iG, (byte)iB));

                   // gridBackground.Background = new SolidColorBrush(sSkin.crGrad1);

                    //gridBackground.Background = GetGradientBrush(sSkin.crGrad1, sSkin.crGrad2);
                }
            }
            else if (sSkin.imgBackground != null)
            {
                

                gridBackground.Background = GetImageBrush(ref sSkin.imgBackground);
            }
            /*else if (sSkin.eBackground == EnumBackground.Default || sSkin.eSkin == EnumSudokuSkins.Glass || sSkin.eSkin == EnumSudokuSkins.Grey)
            {
                gridBackground.Background = GetGradientBrush(sSkin.crGrad1, sSkin.crGrad2);
            }
            else if (sSkin.eBackground == EnumBackground.Custom)
            {
                // our custom background
            }*/
            else
            {
                int iR, iG, iB;

                iR = (sSkin.crGrad1.R + sSkin.crGrad2.R) / 2;
                iG = (sSkin.crGrad1.G + sSkin.crGrad2.G) / 2;
                iB = (sSkin.crGrad1.B + sSkin.crGrad2.B) / 2;

                gridBackground.Background = new SolidColorBrush(Color.FromArgb(255, (byte)iR, (byte)iG, (byte)iB));

                //gridBackground.Background = new SolidColorBrush(sSkin.crGrad1);
                //gridBackground.Background = GetGradientBrush(sSkin.crGrad1, sSkin.crGrad2);
            }

        }

      /*  private void cellArray_ImageOpened(object sender, RoutedEventArgs e)
        {
           ResizeImage(ref sSkin.imgCellArray, ref sSkin.wbCellArray);
        }

        private void cellArray2_ImageOpened(object sender, RoutedEventArgs e)
        {
            ResizeImage(ref sSkin.imgCellArray2, ref sSkin.wbCellArray2);
        }

        private void ResizeImage(ref BitmapImage img, ref WriteableBitmap wb)
        {
            Image image = new Image();
            image.Source = img;

            wb.Render(image, new ScaleTransform() { ScaleX = (_dbGridSize) / img.PixelHeight, ScaleY = (_dbGridSize) / img.PixelHeight });
            wb.Invalidate();
            
        }*/

        //currently only used on the background and I don't want to scale it - Adrian
        public ImageBrush GetImageBrush(ref BitmapImage img)
        {
            ImageBrush i = new ImageBrush();
            i.Stretch = Stretch.None;
            i.ImageSource = sSkin.imgBackground;
            

            return i;
        }

        public LinearGradientBrush GetGradientBrush(Color cr1, Color cr2)
        {
            LinearGradientBrush br = null;

            br = new LinearGradientBrush();

            br.StartPoint = new Point(0.5, 0);
            br.EndPoint = new Point(0.5, 1);

            GradientStop stop = new GradientStop();
            stop.Offset = 0;
            stop.Color = cr1;
            br.GradientStops.Add(stop);

            stop = new GradientStop();
            stop.Offset = 1;
            stop.Color = cr2;
            br.GradientStops.Add(stop);

            return br;
        }

        public Color GetArrow()
        {
            switch (sSkin.eSkin)
            {
                case EnumSudokuSkins.Glass:
                case EnumSudokuSkins.Grey:
                    return Color.FromArgb(255, 255, 246, 0);
                default:
                    return sSkin.crOutline1;
            }
        }

        public Color GetPencilMark(EnumSquares eSquare, EnumSudokuType eType)
        {
            if (eType == EnumSudokuType.STYPE_Kakuro)
            {
                if (eSquare == EnumSquares.Selector)
                switch(sSkin.eSkin)
                {
                case EnumSudokuSkins.Grey:
                case EnumSudokuSkins.Glass:
                case EnumSudokuSkins.Wood:
                    return sSkin.crPencil2;//dark for the selector
                case EnumSudokuSkins.Custom0:
                default:
                    break;
                }
                return sSkin.crPencilKakuro;
            }
            else if (eType == EnumSudokuType.STYPE_Killer || eType == EnumSudokuType.STYPE_KenKen)
            {
                if (sSkin.eSkin == EnumSudokuSkins.Glass)
                    return sSkin.crPencilKakuro;
                if (sSkin.eSkin == EnumSudokuSkins.Grey)
                    return sSkin.crPencil1;
            }
    
            if(UseFont1(eSquare))
                return sSkin.crPencil1;
            return sSkin.crPencil2;
        }

        public bool UseFont1(EnumSquares eSquare)
        {
            switch(sSkin.eSkin)
            {
            case EnumSudokuSkins.Glass:
                switch(eSquare)
                {
                case EnumSquares.Board1://black
                case EnumSquares.Guess1:
                case EnumSquares.Highlighted:
                case EnumSquares.Selector:
                    return false;
                case EnumSquares.Board2://white
                case EnumSquares.Guess2:
                case EnumSquares.Locked:
                case EnumSquares.Given:
                default:
                    return true;
                }
             case EnumSudokuSkins.Wood:
                switch(eSquare)
                {
                case EnumSquares.Highlighted:
                case EnumSquares.Selector:
                    return false;
                 case EnumSquares.Board2:
                case EnumSquares.Guess2:
                case EnumSquares.Locked:
                case EnumSquares.Given:
                case EnumSquares.Board1:
                case EnumSquares.Guess1:
                default:
                    return true;
                }
            case EnumSudokuSkins.Notepad://blue is true ... red is false
                switch(eSquare)
                {
                case EnumSquares.Given:
                case EnumSquares.Kakuro:
                    return false;
                 case EnumSquares.Board2:
                case EnumSquares.Guess2:
                case EnumSquares.Locked:
                case EnumSquares.Board1:
                case EnumSquares.Guess1:
                case EnumSquares.Highlighted:
                case EnumSquares.Selector:
                default:
                    return true;
                }
            case EnumSudokuSkins.Grey:
                switch(eSquare)
                {
                case EnumSquares.Board1://black
                case EnumSquares.Guess1:
                case EnumSquares.Highlighted:
                case EnumSquares.Selector:
                case EnumSquares.Board2://white
                case EnumSquares.Guess2:
                case EnumSquares.Locked:        
                    return false;
                    case EnumSquares.Given:
                default:
                    return true;
                }
            case EnumSudokuSkins.Custom1:
                switch(eSquare)
                {
                case EnumSquares.Board1:
                case EnumSquares.Guess1:
                case EnumSquares.Board2:
                case EnumSquares.Guess2:
                case EnumSquares.Highlighted:
                case EnumSquares.Selector:
                case EnumSquares.Given:        
                    return false;
                case EnumSquares.Locked:
                default:
                    return true;
                }
            case EnumSudokuSkins.Custom3:
                switch(eSquare)
                {
                case EnumSquares.Board1:
                case EnumSquares.Guess1:
                case EnumSquares.Board2:
                case EnumSquares.Guess2:
                case EnumSquares.Highlighted:
                case EnumSquares.Selector:
                case EnumSquares.Locked:        
                    return false;
                case EnumSquares.Given:
                default:
                    return true;
                }
            default:
                switch(eSquare)
                {
                case EnumSquares.Board1://black
                case EnumSquares.Guess1:
                case EnumSquares.Board2://white
                case EnumSquares.Guess2:
                case EnumSquares.Highlighted:
                case EnumSquares.Selector:
                    return false;
                case EnumSquares.Locked:
                case EnumSquares.Given:
                default:
                    return true;
                }
            }
        }
    }
}
