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

public enum EnumCalcType
{
    CALC_Standard,
    CALC_Scientific,
    CALC_BaseConversions,
    CALC_Graphing,
    CALC_UnitConv,
    CALC_Currency,
    CALC_DateTime,
    CALC_Constants,
    CALC_Financial,
    CALC_Statistics,
    CALC_SciRPN, //sci with RPN layout
    CALC_WorkSheet,//this is kind of a hack but oh well
    CALC_WSStats,
    CALC_NumberOfLayouts,
};

public enum EnumCalcProSkinStyle
{
    Classic,
    Shine,
    Type,
    Flat,
    Standard,
    Count
}

public enum EnumCalcProSkinColor
{
    Black,
    Blue,
    BlueSteel,
    Brown,
    //Earth,
    Green,
    Grey,
    Orange,
    Pink,
    Count
}

public enum EnumKeyboard
{
    KB_Original,
    KB_QWERTY,
    KB_AZERTY,
    KB_QWERTZ,
    KB_Count,
};

public enum EnumButtonLayout
{
    BL_Alt,
    BL_Alt2ndF,
    BL_AltHyp,
    BL_Alt2ndFHype,
    //BL_Main,
};

namespace CalcPro
{
    public class ButtonType	//information not stored in the button class
    {					//if needed we can add a pointer to the actual IssDynButton
        public int iButtonType;	//number, operator, etc
        public int iButtonValue;	//index within that type
        public int iSkinIndex;
        public string szLabel;		//we'll just point this to the string in the header
        public string szLabelExp;
        //we can add keyboard information here as well ... good stuff
    }

    public class LayoutCalcType
    {
        public int iMainBtnRows;
        public int iMainBtnColumns;
        public int iAltBtnRows;
        public int iAltBtnColumns;
        
        public List<ButtonType> sMainButtons = new List<ButtonType>();
        public List<ButtonType> sMainAltButtons = new List<ButtonType>();	//to get the values and the label
        public List<ButtonType> sAltButtons = new List<ButtonType>();
        public List<ButtonType> sKeyboardButtons = new List<ButtonType>();
        //only for Sci at the moment ... maybe used for others later
        public List<ButtonType> s2ndFButtons = new List<ButtonType>();
        public List<ButtonType> s2ndHypFButtons = new List<ButtonType>();
        public List<ButtonType> sHypButtons = new List<ButtonType>();
    }

    public class BtnManager
    {
        private Dictionary<string, FontFamily> _arrFont;
        public Brush brAltTextColor
        {
            get
            {
                if (_brAltTextColor == null)
                    _brAltTextColor = Globals.Brush.GetBrush(GetAltTextColor());
                return _brAltTextColor;
            }
        }
        public Brush brMainTextColor
        {
            get
            {
                if (_brMainTextColor == null)
                    _brMainTextColor = Globals.Brush.GetBrush(GetMainTextColor());
                return _brMainTextColor;
            }
        }

        LayoutCalcType[]    sLayouts = new LayoutCalcType[(int)EnumCalcType.CALC_NumberOfLayouts];
        IssIniEx IniButtons = new IssIniEx();
	    EnumCalcType		eCalcType;
        EnumCalcType        ePrevCalc;
        EnumCalcProSkinColor eSkinColor;
        EnumCalcProSkinStyle eSkinStyle;
        EnumKeyboard        eKeyboard;

        Brush _brAltTextColor = null;
        Brush _brMainTextColor = null;
        bool                bKBLetters;
        bool                bAdvancedMem;
        bool                bRPN;
        bool                bFullscreen;
        int                 i1Indent; //offset from the left for the 1 button ....

        //public delegate void UpdateSkin(Color crBackground, EnumCalcProSkins eNewSkin);
        public delegate void UpdateSkin(Color crBackground, EnumCalcProSkinColor eColor, EnumCalcProSkinStyle eStyle);

        public UpdateSkin SkinUpdate
        {
            get { return _SkinUpdate; }
            set { _SkinUpdate = value; }
        }
        private UpdateSkin _SkinUpdate;

        public BtnManager()
        {
            _arrFont = new Dictionary<string, FontFamily>();
            eCalcType = EnumCalcType.CALC_NumberOfLayouts; //yes this looks wrong BUT .. Load Registry will always set this
            ePrevCalc = EnumCalcType.CALC_NumberOfLayouts;

            eSkinColor = EnumCalcProSkinColor.Blue;
            eSkinStyle = EnumCalcProSkinStyle.Classic;

            bAdvancedMem = false;
            bRPN = false;
            eKeyboard = EnumKeyboard.KB_Original;
            bKBLetters = false;
            bFullscreen = false;
            i1Indent = (0);
        }
        public EnumCalcType GetCalcType(){return eCalcType;}
        public EnumCalcType GetPrevCalc() { return ePrevCalc; }
        //public EnumCalcProSkins GetSkinType() { return eCalcSkin; }
        public EnumCalcProSkinColor GetSkinColor() { return eSkinColor; }
        public EnumCalcProSkinStyle GetSkinStyle() { return eSkinStyle; }

        public bool GetUseAdvMem() { return bAdvancedMem; }
        public void SetUseAdvMem(bool bAdvMem) { bAdvancedMem = bAdvMem; }
        public bool GetUseRPN() { return bRPN; }
        public void SetUseRPN(bool bNewRPN) { bRPN = bNewRPN; }
        public int GetKBOffset() { return i1Indent; }
        public void SetKBOffset(int iOff) { i1Indent = iOff; }
        public EnumKeyboard GetKBType() { return eKeyboard; }
        public void SetKBType(EnumKeyboard eKB) { eKeyboard = eKB; }
        public bool GetKBLetters() { return bKBLetters; }
        public void SetKBLetters(bool bKB) { bKBLetters = bKB; }
        public bool GetFullscreen() { return bFullscreen; }
        public void SetFullscreen(bool bFUll) { bFullscreen = bFUll; }

        public void SetSkinStyle(EnumCalcProSkinStyle eStyle, bool bForceUpdate)
        {
            if (eStyle == eSkinStyle &&
                sLayouts[(int)eStyle] != null &&
                bForceUpdate == false)
                return;

            if ((int)eStyle < 0 || (int)eStyle >= (int)EnumCalcProSkinStyle.Count)
                return;

            eSkinStyle = eStyle;

            // reset the brushes here
            _brAltTextColor = null;
            _brMainTextColor = null;

            if (SkinUpdate != null)
                SkinUpdate(GetBackgroundColor(), eSkinColor, eSkinStyle);
        }

        public void SetSkinColor(EnumCalcProSkinColor eColor, bool bForceUpdate)
        {
            if (eColor == eSkinColor &&
                sLayouts[(int)eSkinColor] != null &&
                bForceUpdate == false)
                return;

            if ((int)eColor < 0 || (int)eColor >= (int)EnumCalcProSkinColor.Count)
                return;

            eSkinColor = eColor;

            // reset the brushes here
            _brAltTextColor = null;
            _brMainTextColor = null;

            if (SkinUpdate != null)
                SkinUpdate(GetBackgroundColor(), eSkinColor, eSkinStyle);
        }

        //call this to force a reload of the buttons (advanced mem, rpn, etc)
        public void ResetLayouts()
        {
            int iCount = sLayouts.Length;

            for (int i = 0; i < iCount; i++)
                sLayouts[i] = null; //good ol garbage collection will do the rest
        }

        public bool SetCalcType(EnumCalcType eType)
        {
            if((int)eType < 0)
                return false;
            if(eType == eCalcType)
                return true;

            EnumCalcType eTypeTemp = eType;

    //        if (eTypeTemp == EnumCalcType.CALC_Scientific && Globals.Settings.eCalcType == CalcOperationType.CALC_RPN)
    //            eTypeTemp = EnumCalcType.CALC_SciRPN;

            if (sLayouts[(int)eTypeTemp] == null)
	        {	//we have to initialize the layout
                LoadLayout(eTypeTemp);
	        }

	        //important of course but we'll only set it if we don't fail
            //save the previous calc
            if(eType < EnumCalcType.CALC_WorkSheet && eCalcType < EnumCalcType.CALC_WorkSheet)
            {
                ePrevCalc = eCalcType;
                if(ePrevCalc == EnumCalcType.CALC_NumberOfLayouts)
                    ePrevCalc = EnumCalcType.CALC_Standard;
                if(ePrevCalc == eType)
                    ePrevCalc = EnumCalcType.CALC_Scientific;
            }
    
	        eCalcType = eType;

            //double check
            if(eCalcType < 0 || eCalcType >= EnumCalcType.CALC_NumberOfLayouts)
                eCalcType = EnumCalcType.CALC_Standard;
            if(ePrevCalc < 0 || ePrevCalc >= EnumCalcType.CALC_NumberOfLayouts)
                ePrevCalc = EnumCalcType.CALC_Scientific;
	        return true;
        }
        public bool GetLayout(ref LayoutCalcType sCalcType, EnumCalcType eCalcType)
        {
            if ((int)eCalcType < 0 || (int)eCalcType >= (int)EnumCalcType.CALC_NumberOfLayouts)
            {
                sCalcType = null;
                return false;
            }

            //now .. has it been loaded?
            if (sLayouts[(int)eCalcType] == null)
            {
                if (LoadLayout(eCalcType) == false)
                    return false;
            }

            sCalcType = sLayouts[(int)eCalcType];
            return true;
        }
        public bool GetCurrentLayout(ref LayoutCalcType sCalcType)
        {
            return GetLayout(ref sCalcType, eCalcType);
        }
        public FontFamily GetFont(string szFonts)
        {
            FontFamily f = null;

            if (_arrFont.TryGetValue(szFonts, out f))
                return f;

            f = new FontFamily(szFonts);
            if (f == null)
                return null;

            _arrFont.Add(szFonts, f);

            return f;
        }
        public void SetFontItem(ref TextBlock txt)
        {
            switch (eSkinStyle)
            {
                case EnumCalcProSkinStyle.Standard:
                case EnumCalcProSkinStyle.Flat:
                    return; // no font setting for flat buttons
            }

            switch (eSkinColor)
            {
                case EnumCalcProSkinColor.BlueSteel:
                case EnumCalcProSkinColor.Green:
                case EnumCalcProSkinColor.Brown:
                    txt.FontFamily = GetFont("/CalcPro;component/Fonts/Fonts.zip#Panoramic Calc Pro Digital Font");
                    break;
                default:
                    break;
            }
        }
        public Color GetBackgroundColor()
        {
            Color crReturn = Color.FromArgb(255, 49, 81, 132);//default to black
            switch (eSkinColor)
            {
                case EnumCalcProSkinColor.Blue:
                    crReturn = Color.FromArgb(255, 49, 81, 132);
                    break;
                case EnumCalcProSkinColor.BlueSteel:
                    crReturn = Color.FromArgb(255, 90, 109, 132);
                    break;
                case EnumCalcProSkinColor.Black:
                    crReturn = Color.FromArgb(255, 40, 40, 40);
                    break;
                case EnumCalcProSkinColor.Brown:
                    crReturn = Color.FromArgb(255, 77, 69, 51);
                    break;
                case EnumCalcProSkinColor.Grey:
                    crReturn = Color.FromArgb(255, 90, 90, 90);
                    break;
                case EnumCalcProSkinColor.Pink:
                    crReturn = Color.FromArgb(255, 255, 154, 189);
                    break;
             /*   case EnumCalcProSkinColor.Earth:
                    crReturn = Color.FromArgb(255, 68, 78, 48);
                    break;*/
                case EnumCalcProSkinColor.Green:
                    crReturn = Color.FromArgb(255, 68, 78, 48);
                    break;
                case EnumCalcProSkinColor.Orange:
                    crReturn = Color.FromArgb(255, 50, 50, 50);
                    break;
            }
            return crReturn;
        }
        public Color GetBackgroundMenuColor()
        {
            Color crReturn = Color.FromArgb(255, 49, 81, 132);//default to black

            switch (eSkinColor)
            {
                case EnumCalcProSkinColor.Blue:
                    crReturn = Color.FromArgb(255, 30, 48, 79);
                    break;
                case EnumCalcProSkinColor.BlueSteel:
                    crReturn = Color.FromArgb(255, 48, 57, 69);
                    break;
                case EnumCalcProSkinColor.Black:
                    crReturn = Color.FromArgb(255, 40, 40, 40);
                    break;
                case EnumCalcProSkinColor.Brown:
                    crReturn = Color.FromArgb(255, 37, 33, 24);
                    break;
                case EnumCalcProSkinColor.Grey:
                    crReturn = Color.FromArgb(255, 53, 53, 53);
                    break;
                case EnumCalcProSkinColor.Pink:
                    crReturn = Color.FromArgb(255, 255, 74, 138);
                    break;
                case EnumCalcProSkinColor.Green:
                    crReturn = Color.FromArgb(255, 39, 45, 28);
                    break;
                case EnumCalcProSkinColor.Orange:
                    crReturn = Color.FromArgb(255, 30, 30, 30);
                    break;
            }
            return crReturn;
        }

        Color GetMetroTextColor(EnumCalcProSkinColor eColor)
        {
            switch (eSkinColor)
            {
                default:
                case EnumCalcProSkinColor.Black:
                    return CalcPro.BrushFactory.ToColor(0xFF1F8EE7);
                case EnumCalcProSkinColor.Blue:
                    return CalcPro.BrushFactory.ToColor(0xFF131F33);
                case EnumCalcProSkinColor.BlueSteel:
                    return CalcPro.BrushFactory.ToColor(0xFF212B37);
                case EnumCalcProSkinColor.Brown:
                    return CalcPro.BrushFactory.ToColor(0xFF1D1A11);
                case EnumCalcProSkinColor.Green:
                    return CalcPro.BrushFactory.ToColor(0xFF1A230F);
                case EnumCalcProSkinColor.Grey:
                    return CalcPro.BrushFactory.ToColor(0xFF000000);
                case EnumCalcProSkinColor.Orange:
                    return CalcPro.BrushFactory.ToColor(0xFFEB7702);
                case EnumCalcProSkinColor.Pink:
                    return CalcPro.BrushFactory.ToColor(0xFF1B0F11);
            }
        }

        public Color GetMainTextColor()
        {
            if (eSkinStyle == EnumCalcProSkinStyle.Flat || eSkinStyle == EnumCalcProSkinStyle.Standard)
                return GetMetroTextColor(eSkinColor);
            
            Color crReturn = Color.FromArgb(255, 49, 81, 132);//default to black

            switch (eSkinColor)
            {
                case EnumCalcProSkinColor.Black:
                    crReturn = Color.FromArgb(255, 255, 255, 255);
                    break;
                case EnumCalcProSkinColor.BlueSteel:
                    crReturn = Color.FromArgb(255, 20, 42, 53);
                    break;
                case EnumCalcProSkinColor.Green:
                case EnumCalcProSkinColor.Brown:
                    crReturn = Color.FromArgb(255, 32, 33, 31);
                    break;

                case EnumCalcProSkinColor.Orange:
                    crReturn = Color.FromArgb(255, 231, 117, 0);
                    break;

                case EnumCalcProSkinColor.Pink:
                    crReturn = Color.FromArgb(255, 88, 44, 58);
                    break;

                case EnumCalcProSkinColor.Blue:
                case EnumCalcProSkinColor.Grey:
                default:
                    crReturn = Color.FromArgb(255, 67, 67, 67);
                    break;
            }
            return crReturn;
        }

        public Color GetAltTextColor()
        {
            if (eSkinStyle == EnumCalcProSkinStyle.Flat || eSkinStyle == EnumCalcProSkinStyle.Standard)
                return GetMetroTextColor(eSkinColor);

            Color crReturn = Color.FromArgb(255, 0, 0, 0);//default to black\

            switch (eSkinColor)
            {
                case EnumCalcProSkinColor.Black:
                    crReturn = (Color)System.Windows.Application.Current.Resources["PhoneAccentColor"];
                    break;
                case EnumCalcProSkinColor.Blue:
                    crReturn = Color.FromArgb(255, 20, 42, 53);
                    break;
                case EnumCalcProSkinColor.Green:
                case EnumCalcProSkinColor.Brown:
                    crReturn = Color.FromArgb(255, 71, 75, 55);
                    break;
                case EnumCalcProSkinColor.Orange:
                    crReturn = Color.FromArgb(255, 231, 117, 0);
                    break;
                case EnumCalcProSkinColor.Pink:
                    crReturn = Color.FromArgb(255, 135, 63, 86);
                    break;
                case EnumCalcProSkinColor.BlueSteel:
                case EnumCalcProSkinColor.Grey:
                default:
                    crReturn = Color.FromArgb(255, 67, 67, 67);
                    break;
            }
            return crReturn;
        }
        public void CreateButton(ref ICalcProBtn btn, int iSkinIndex)
        {
            btn = new ButtonSkin(eSkinColor, eSkinStyle, iSkinIndex);
            return;
        }
 
        public void CreateMainButtons(ref Grid myGrid)
        {

            //lets try and make the grid control in code
            LayoutCalcType sLayout = null;
            GetCurrentLayout(ref sLayout);

            for (int x = 0; x < sLayout.iMainBtnColumns; x++)
            {
                for (int y = 0; y < sLayout.iMainBtnRows; y++)
                {
                    int iBtn = sLayout.iMainBtnColumns * y + x;

                    ICalcProBtn btn = null;

                    CreateButton(ref btn, sLayout.sMainButtons[iBtn].iSkinIndex);


                    string szTemp = null;

                    szTemp = "Main" + eCalcType.ToString() + x.ToString() + y.ToString();

                    //cast to a user control
                    UserControl uCon = (UserControl)btn;

                    btn.cBtn.iBtnIndex = sLayout.sMainButtons[iBtn].iButtonValue;
                    btn.cBtn.iBtnType = sLayout.sMainButtons[iBtn].iButtonType;

                    btn.SetMainText(sLayout.sMainButtons[iBtn].szLabel);
                    btn.SetExpText("");


                    btn.BtnSelected += delegate(object MySender, EventArgs eArg)
                    {
                        // put your calculator code in here
                        //MessageBox.Show(btn.Name, "Hi", MessageBoxButton.OKCancel);
                        Globals.Calc.CalcButtonPress(btn.cBtn.iBtnType, btn.cBtn.iBtnIndex);
                    };

                     myGrid.Children.Add(uCon);

                     Grid.SetColumn(uCon, x);
                     Grid.SetRow(uCon, y);
               }
            }
        }

        public EnumCalcDisplay GetDisplayType()
        {
            switch (Globals.Settings.eCalcType)
            {
                case CalcOperationType.CALC_RPN:
                    //    if (Globals.Settings.eDisplayType == DisplayType.DISPLAY_Fractions)
                    //        return CreateDisplay(ref myGrid, "ctrlDisplay", EnumCalcDisplay.Fractions);
                    return  EnumCalcDisplay.RPN;
                case CalcOperationType.CALC_String:
                    return EnumCalcDisplay.Expression;
                default:
                    if (Globals.Settings.eDisplayType == DisplayType.DISPLAY_Fractions)
                        return EnumCalcDisplay.Fractions;
                    return EnumCalcDisplay.Normal;
            }
        }

        public ICalcProDisplay CreateDisplay(ref Grid myGrid)
        {
            return CreateDisplay(ref myGrid, "ctrlDisplay", GetDisplayType());
         /*   switch (Globals.Settings.eCalcType)
            {
                case CalcOperationType.CALC_RPN:
                //    if (Globals.Settings.eDisplayType == DisplayType.DISPLAY_Fractions)
                //        return CreateDisplay(ref myGrid, "ctrlDisplay", EnumCalcDisplay.Fractions);
                    return CreateDisplay(ref myGrid, "ctrlDisplay", EnumCalcDisplay.RPN);
                case CalcOperationType.CALC_String:
                    return CreateDisplay(ref myGrid, "ctrlDisplay", EnumCalcDisplay.Expression);
                default:
                    if(Globals.Settings.eDisplayType == DisplayType.DISPLAY_Fractions)
                        return CreateDisplay(ref myGrid, "ctrlDisplay", EnumCalcDisplay.Fractions);
                    return CreateDisplay(ref myGrid, "ctrlDisplay", EnumCalcDisplay.Normal);
            }*/
        }

        public ICalcProDisplay CreateDisplay(ref Grid myGrid, EnumCalcDisplay eDisplay)
        {
            return CreateDisplay(ref myGrid, "ctrlDisplay", eDisplay);
        }
        
        public ICalcProDisplay CreateDisplay(ref Grid myGrid, string szDisplayName, EnumCalcDisplay eDisplay)
        {
            DisplaySkin newItem = null;
            
            // special case for worksheet
            if(eDisplay == EnumCalcDisplay.WorkSheet)
                newItem = new DisplaySkin(EnumCalcProSkinColor.Grey, EnumCalcProSkinStyle.Classic);
            else
                newItem = new DisplaySkin(eSkinColor, eSkinStyle);

            newItem.SetValue(FrameworkElement.NameProperty, szDisplayName);

            ICalcProDisplay disp = (ICalcProDisplay)newItem;

            disp.SetDisplayType(eDisplay, false);

            Thickness thick = new Thickness(Globals.MARGIN);
            newItem.Margin = thick;

            myGrid.Children.Add(newItem);

            return disp;
        }

        public void CreateAltButtons(ref Grid myGrid, EnumButtonLayout eLayout)
        {

            LayoutCalcType sLayout = null;
            GetCurrentLayout(ref sLayout);

            List<ButtonType> listBtns;

            switch (eLayout)
            {
                case EnumButtonLayout.BL_Alt2ndF:
                    listBtns = sLayout.s2ndFButtons;
                    break;
                case EnumButtonLayout.BL_AltHyp:
                    listBtns = sLayout.sHypButtons;
                    break;
                case EnumButtonLayout.BL_Alt2ndFHype:
                    listBtns = sLayout.s2ndHypFButtons;
                    break;
                default:
                    listBtns = sLayout.sAltButtons;
                    break;
            }



            for (int x = 0; x < sLayout.iAltBtnColumns; x++)
            {
                for (int y = 0; y < sLayout.iAltBtnRows; y++)
                {
                    int iBtn = sLayout.iAltBtnColumns * y + x;

                    ICalcProBtn btn = null; // = new Skin_Blue_01();


                    CreateButton(ref btn, listBtns[iBtn].iSkinIndex);


                    UserControl uCon = (UserControl)btn;

                    btn.cBtn.iBtnIndex = listBtns[iBtn].iButtonValue;
                    btn.cBtn.iBtnType = listBtns[iBtn].iButtonType;

                    btn.SetMainText(listBtns[iBtn].szLabel);
                    btn.SetExpText(listBtns[iBtn].szLabelExp);

                    string sz2nd = null;
                    string sz2ndExp = null;

                    //ok let's figure out if we need to set the alt text
                    switch (eLayout)
                    {
                        case EnumButtonLayout.BL_Alt2ndF:
                            sz2nd = sLayout.sAltButtons[iBtn].szLabel;
                            sz2ndExp = sLayout.sAltButtons[iBtn].szLabelExp;
                            break;
                        case EnumButtonLayout.BL_AltHyp:
                            sz2nd = sLayout.s2ndHypFButtons[iBtn].szLabel;
                            sz2ndExp = sLayout.s2ndHypFButtons[iBtn].szLabelExp;
                            break;
                        case EnumButtonLayout.BL_Alt2ndFHype:
                            sz2nd = sLayout.sHypButtons[iBtn].szLabel;
                            sz2ndExp = sLayout.sHypButtons[iBtn].szLabelExp;
                            break;
                        default:
                            if (sLayout.s2ndFButtons.Count > 0)
                            {
                                sz2nd = sLayout.s2ndFButtons[iBtn].szLabel;
                                sz2ndExp = sLayout.s2ndFButtons[iBtn].szLabelExp;
                            }
                            break;
                    }

                    //if its not null and its not the same as the other state
                    if (sz2nd != null && sz2nd != listBtns[iBtn].szLabel)
                    {
                        btn.Set2ndText(sz2nd);

                        if (sz2ndExp != null && sz2ndExp.Length > 0)
                            btn.Set2ndExpText(sz2ndExp);

                    }
                    btn.BtnSelected += delegate(object MySender, EventArgs eArg)
                    {

                        // put your calculator code in here

                        //MessageBox.Show(btn.Name, "Hi", MessageBoxButton.OKCancel);
                        Globals.Calc.CalcButtonPress(btn.cBtn.iBtnType, btn.cBtn.iBtnIndex);
                    };

                    Grid.SetColumn(uCon, x);
                    Grid.SetRow(uCon, y);

                    myGrid.Children.Add(uCon);

                }
            }

        }
        
        void DeleteAllButtons()
        {
            for (int i = 0; i < (int)EnumCalcType.CALC_NumberOfLayouts; i++)
            {
                LayoutCalcType layout = sLayouts[i];

                if (layout == null)
                    continue;

                if (layout.sMainButtons.Count > 0)
                    layout.sMainButtons.Clear();

                if (layout.sMainButtons.Count > 0)
                    layout.sMainButtons.Clear();

                if (layout.sMainAltButtons.Count > 0)
                    layout.sMainAltButtons.Clear();

                if (layout.sAltButtons.Count > 0)
                    layout.sAltButtons.Clear();

                if (layout.sKeyboardButtons.Count > 0)
                    layout.sKeyboardButtons.Clear();

                if (layout.s2ndFButtons.Count > 0)
                    layout.s2ndFButtons.Clear();

                if (layout.s2ndHypFButtons.Count > 0)
                    layout.s2ndHypFButtons.Clear();

                if (layout.sHypButtons.Count > 0)
                    layout.sHypButtons.Clear();
            }
        }
        void LoadSkin(ref IssIniEx iniButton)
        {
	        if(iniButton == null)
		        return;

            //just a placeholder for now ... where we could load in colors and such
 
        }        
        bool LoadLayout(EnumCalcType eCalcType)
        {
            bool bSwapRPN = true;//flag to enable/disable rpn swapping

            LayoutCalcType sLayout = sLayouts[(int)eCalcType]; //just less typing
            ButtonType sButton = null;
            IssIniEx IniLayout = new IssIniEx();
            

            if (sLayout != null)
                goto Error;
            
            //lets make one
            sLayout = new LayoutCalcType();

            bool bRet = true;
            
            //open the buttons 
            if(!IniButtons.IsInitialized())
                bRet = IniButtons.OpenFromResource("Files/btn-black.ini");

            if (bRet == false)
                goto Error;

            LoadSkin(ref IniButtons);

            string szTemp;

            switch(eCalcType)
	        {
	        default:
            case EnumCalcType.CALC_Statistics:
                szTemp = "Files/layout_stats.ini";
                bSwapRPN = true;
                break;
            case EnumCalcType.CALC_Financial:
                szTemp = "Files/layout_financial.ini";
                bSwapRPN = true;
                break;
	        case EnumCalcType.CALC_Standard:
		        szTemp = "Files/layout_standard.ini";
                bSwapRPN = true;
		        break;
	        case EnumCalcType.CALC_Scientific:
                szTemp = "Files/layout_sci.ini";
		        break;
	        case EnumCalcType.CALC_SciRPN:
		        szTemp = "Files/layout_sci_rpn.ini";
		        break;
	        case EnumCalcType.CALC_BaseConversions:
		        szTemp = "Files/layout_base.ini";
                bSwapRPN = true;
	            break;
	        case EnumCalcType.CALC_Graphing:
		        szTemp = "Files/layout_graph.ini";
		        break;
	        case EnumCalcType.CALC_UnitConv:
            case EnumCalcType.CALC_Currency://same layout for main buttons 
		        szTemp = "Files/layout_unit.ini";
                bSwapRPN = true;
		        break;
            case EnumCalcType.CALC_WorkSheet:
                szTemp = "Files/layout_worksheet.ini";
                break;
            case EnumCalcType.CALC_WSStats:
                szTemp = "Files/layout_wsstats.ini";
                break;
            case EnumCalcType.CALC_Constants:
                szTemp = "Files/layout_const.ini";
                bSwapRPN = true;
                break;
            case EnumCalcType.CALC_DateTime:
                szTemp = "Files/layout_timedate.ini";
                break;
            }

            bRet = IniLayout.OpenFromResource(szTemp);

            IniLayout.GetValue(ref sLayout.iAltBtnColumns, "Init", "AltBtnColumns");
            IniLayout.GetValue(ref sLayout.iAltBtnRows, "Init", "AltBtnRows");
            IniLayout.GetValue(ref sLayout.iMainBtnColumns, "Init", "MainBtnColumns");
            IniLayout.GetValue(ref sLayout.iMainBtnRows, "Init", "MainBtnRows");

            int iButtons = sLayout.iMainBtnColumns * sLayout.iMainBtnRows;
            int i = 0;
            int iTemp;

            string szButton = null;
            string szButtonName = null;

            for(i = 0; i < iButtons; i++)
	        {
                szButtonName = " ";
		        sButton = new ButtonType();
		        
                iTemp = i+1;
                szButton = "Btn" + iTemp.ToString();


		        if(IniLayout.GetValue(ref szButtonName, ("MainButtons"), szButton) == true)
		        {
			        IniButtons.GetValue(ref sButton.iButtonType, szButtonName, ("BtnType"));
			        IniButtons.GetValue(ref sButton.iButtonValue, szButtonName, ("BtnId"));
			        IniButtons.GetValue(ref sButton.iSkinIndex, szButtonName, ("BtnStyle"));

                    //bit of a hack but oh well
                    if(bAdvancedMem == true && sButton.iButtonType == 4)
                        UpdateMemoryButtons(ref sButton);
                    if(bRPN && bSwapRPN)
                        UpdateRPNButtons(ref sButton);

			        SetButtonLabels(ref sButton);
                    sLayout.sMainButtons.Add(sButton);
		        }
		        else
		        {
			        sButton = null;
		        }


        
	        }

            //LoadKeyboard(ref sLayout, IniButtons);



	        iButtons = sLayout.iAltBtnColumns*sLayout.iAltBtnRows;

            //gonna do a little hack to save some work
            bool bAlt = true;
            bool b2nd = true;
    
	        for(i = 0; i < iButtons; i++)
	        {
                iTemp = i+1;
                szButton = "Btn" + iTemp.ToString();
                szButtonName = "";
		        

		        if(bAlt == true && IniLayout.GetValue(ref szButtonName, ("AltButtons"), szButton) == true)
		        {
			        sButton = new ButtonType();

			        IniButtons.GetValue(ref sButton.iButtonType, szButtonName, ("BtnType"));
			        IniButtons.GetValue(ref sButton.iButtonValue, szButtonName, ("BtnId"));
			        IniButtons.GetValue(ref sButton.iSkinIndex, szButtonName, ("BtnStyle"));

                    //bit of a hack but oh well
                    if(bAdvancedMem == true && sButton.iButtonType == 4)
                        UpdateMemoryButtons(ref sButton);
                    if(bRPN && bSwapRPN)
                        UpdateRPNButtons(ref sButton);

			        SetButtonLabels(ref sButton);

			        sLayout.sAltButtons.Add(sButton);
		        }
                else
                    bAlt = false;

		        //if we have 2ndF
		        if(b2nd == true&& IniLayout.GetValue(ref szButtonName, ("2nd"), szButton) == true)
		        {
			        sButton = new ButtonType();
			        
			        IniButtons.GetValue(ref sButton.iButtonType, szButtonName, ("BtnType"));
			        IniButtons.GetValue(ref sButton.iButtonValue, szButtonName, ("BtnId"));
			        IniButtons.GetValue(ref sButton.iSkinIndex, szButtonName, ("BtnStyle"));

                    if(bRPN && bSwapRPN)
                        UpdateRPNButtons(ref sButton);

			        SetButtonLabels(ref sButton);

			        sLayout.s2ndFButtons.Add(sButton);
		        }
                else
                    b2nd = false;

		        //if we have Hyp
		        if(b2nd == true&& IniLayout.GetValue(ref szButtonName, ("Hyp"), szButton) == true)
		        {
			        sButton = new ButtonType();
			        
			        IniButtons.GetValue(ref sButton.iButtonType, szButtonName, ("BtnType"));
			        IniButtons.GetValue(ref sButton.iButtonValue, szButtonName, ("BtnId"));
			        IniButtons.GetValue(ref sButton.iSkinIndex, szButtonName, ("BtnStyle"));

                    if(bRPN && bSwapRPN)
                        UpdateRPNButtons(ref sButton);
                
			        SetButtonLabels(ref sButton);

			        sLayout.sHypButtons.Add(sButton);
		        }

		        //if we have 2ndf & hyp
		        if(b2nd == true && IniLayout.GetValue(ref szButtonName, ("Hyp2nd"), szButton) == true)
		        {
			        sButton = new ButtonType();
			        
			        IniButtons.GetValue(ref sButton.iButtonType, szButtonName, ("BtnType"));
			        IniButtons.GetValue(ref sButton.iButtonValue, szButtonName, ("BtnId"));
			        IniButtons.GetValue(ref sButton.iSkinIndex, szButtonName, ("BtnStyle"));

                    if(bRPN && bSwapRPN)
                        UpdateRPNButtons(ref sButton);

			        SetButtonLabels(ref sButton);

			        sLayout.s2ndHypFButtons.Add(sButton);
		        }
	        }

	        sLayouts[(int)eCalcType] = sLayout;
            sLayout = null;

            return true;

          Error:
            
            return false;
        }
        void SetButtonLabels(ref ButtonType sBtn)
        {
	        switch((InputType)sBtn.iButtonType)
	        {
	        case  InputType.INPUT_Number:
		        sBtn.szLabel		= ButtonLabels.szNumberLabels[sBtn.iButtonValue];
		        sBtn.szLabelExp	= null;
		        break;
	        case  InputType. INPUT_Operator:
		        sBtn.szLabel		= ButtonLabels.szOperatorLabels[sBtn.iButtonValue];
		        sBtn.szLabelExp	= ButtonLabels.szOperatorExp[sBtn.iButtonValue];
		        break;
	        case  InputType. INPUT_Function:
		        sBtn.szLabel		= ButtonLabels.szFunctionLabels[sBtn.iButtonValue];
		        sBtn.szLabelExp	= ButtonLabels.szFunctionExp[sBtn.iButtonValue];
		        break;
	        case  InputType. INPUT_Bracket:
		        sBtn.szLabel		= ButtonLabels.szBracketLabels[sBtn.iButtonValue];
		        sBtn.szLabelExp	= null;
		        break;
	        case  InputType. INPUT_Memory:
		        sBtn.szLabel		= ButtonLabels.szMemoryLabels[sBtn.iButtonValue];
		        sBtn.szLabelExp	= null;
		        break;
	        case  InputType. INPUT_Equals:
		        sBtn.szLabel		= ButtonLabels.szEqualsLabels[sBtn.iButtonValue];
		        sBtn.szLabelExp	= null;
		        break;
	        case  InputType. INPUT_Clear:

		        sBtn.szLabel		= ButtonLabels.szClearLabels[sBtn.iButtonValue];
		        sBtn.szLabelExp	= null;
		        break;
	        case  InputType. INPUT_BaseConversion:
                int iTemp = sBtn.iButtonValue - 2;
		        sBtn.szLabel		= ButtonLabels.szBaseLabels[iTemp];//bug bug ... should probably check this
		        sBtn.szLabelExp	= null;
		        break;
	        case  InputType. INPUT_Constants:
                if(sBtn.iButtonValue == (int)ConstantType.CONSTANT_Last_Ans)
                    sBtn.szLabel   = ButtonLabels.szConstantLabels[CalcGlobals.NUMBER_OF_Constants-1];
                else if (sBtn.iButtonValue == (int)ConstantType.CONSTANT_LastX)
                    sBtn.szLabel = ButtonLabels.szConstantLabels[CalcGlobals.NUMBER_OF_Constants - 2];
                else
		            sBtn.szLabel		= ButtonLabels.szConstantLabels[sBtn.iButtonValue];
		        sBtn.szLabelExp	= null;
		        break;
	        case  InputType. INPUT_CalcState:
		        sBtn.szLabel		= ButtonLabels.szCalcStateLabels[sBtn.iButtonValue];
		        sBtn.szLabelExp	= null;
		        break;
	        case  InputType. INPUT_Graph:
		        sBtn.szLabel		= ButtonLabels.szGraphLabels[sBtn.iButtonValue];
		        sBtn.szLabelExp	= null;
		        break;
	        case  InputType. INPUT_Clipboard:
                sBtn.szLabel		= ButtonLabels.szClipboardLabels[sBtn.iButtonValue];
                sBtn.szLabelExp	= null;
                break;
	        case  InputType. INPUT_RPN:
		        sBtn.szLabel		= ButtonLabels.szRPNLabels[sBtn.iButtonValue];
		        sBtn.szLabelExp	= null;
		        break;
	        case  InputType. INPUT_WS:
                sBtn.szLabel		= ButtonLabels.szWorksheetLabels[sBtn.iButtonValue];
                sBtn.szLabelExp	= null;
                break;
	        case  InputType. INPUT_Launch:	
                sBtn.szLabel		= ButtonLabels.szLaunchLabels[sBtn.iButtonValue];
                sBtn.szLabelExp	= null;
                break;
	        case  InputType.INPUT_NULL:		
	        case  InputType.INPUT_Custom:	
	        default:
		        sBtn.szLabel		= null;
		        sBtn.szLabelExp	= null;
		        break;
	        }
        }        
        void UpdateMemoryButtons(ref ButtonType sButton)
        {
            if(sButton == null)
                return;

            switch(sButton.iButtonValue)
            {
            case (int)MemoryType.MEMORY_StoreMem1:
                sButton.iButtonValue = (int)CalcStateType.CALCSTATE_MemoryStore;
                sButton.iButtonType = (int)InputType.INPUT_CalcState;
                break;
            case (int)MemoryType.MEMORY_RecallMem1:
                sButton.iButtonValue = (int)CalcStateType.CALCSTATE_MemoryRecall;
                sButton.iButtonType = (int)InputType.INPUT_CalcState;
    	        break;
            case (int)MemoryType.MEMORY_ClearMem1:
                sButton.iButtonValue = (int)MemoryType.MEMORY_1;
                break;
            case (int)MemoryType.MEMORY_Plus1:
                sButton.iButtonValue = (int)MemoryType.MEMORY_2;
                break;
            case (int)MemoryType.MEMORY_MINUS:
                sButton.iButtonValue = (int)MemoryType.MEMORY_3;
                break;
            }
        }
        void UpdateRPNButtons(ref ButtonType sButton)
        {
            if(sButton == null)
                return;

            //if we're here we need to check buttons ... this is going to be a bit manual
            if(sButton.iButtonValue == (int)EqualsType.EQUALS_Equals &&
                sButton.iButtonType == (int)InputType.INPUT_Equals)
            {
                sButton.iButtonValue = (int)EqualsType.EQUALS_Enter;
            }
            //plus minus
            else if (sButton.iButtonValue == (int)FunctionType.FUNCTION_PlusMinus &&
                sButton.iButtonType == (int)InputType.INPUT_Function)
            {
                sButton.iButtonValue = (int)FunctionType.FUNCTION_CHS;
            }
            //exp
            else if(sButton.iButtonValue == (int)OperatorType.OPP_EXP &&
                sButton.iButtonType == (int)InputType.INPUT_Operator)
            {
                sButton.iButtonValue = (int)OperatorType.OPP_EEX;
            }

            //clear
            else if(sButton.iButtonValue == (int)ClearType.CLEAR_Clear &&
                sButton.iButtonType ==  (int)InputType. INPUT_Clear)
            {
                sButton.iButtonValue = (int)ClearType.CLEAR_CLX;
            }

            //left bracket
            else if(sButton.iButtonValue == (int)BracketType.BRACKET_Open &&
                sButton.iButtonType ==  (int)InputType. INPUT_Bracket)
            {
                sButton.iButtonValue = (int)RPNManipulationType.RPN_XY;
                sButton.iButtonType =  (int)InputType. INPUT_RPN;
            }

            //right bracket
            else if(sButton.iButtonValue == (int)BracketType.BRACKET_Close &&
                sButton.iButtonType ==  (int)InputType. INPUT_Bracket)
            {
                sButton.iButtonValue = (int)RPNManipulationType.RPN_RotateDown;
                sButton.iButtonType =  (int)InputType. INPUT_RPN;
            }
    
            else if(sButton.iButtonValue == (int)ConstantType.CONSTANT_Last_Ans &&
                sButton.iButtonType ==  (int)InputType. INPUT_Constants)
            {
                sButton.iButtonValue = (int)ConstantType.CONSTANT_LastX;
            }
        }
        
    }
}
