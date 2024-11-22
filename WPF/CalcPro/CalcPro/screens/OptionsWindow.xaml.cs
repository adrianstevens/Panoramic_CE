using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace CalcPro.screens
{
    public enum EnumOption
    {
        OPTION_UseRegionalSettings,
        OPTION_PlaySounds,
        OPTION_RecordTape,
        OPTION_AdvancedMemory,
        OPTION_CalculatorMode,
        OPTION_Notation,
        OPTION_ConversionBuffer,
        OPTION_RPNStackSize,
        OPTION_SkinColor,
        OPTION_SkinStyle,
        OPTION_FixedDigits,
        OPTION_GraphAccuracy,
        OPTION_GraphStyle,
        OPTION_EndPeriodPayments, //TVM
        OPTION_PaymentSchedule, // Mortgage
        OPTION_Size,			//size on PC
        OPTION_Keyboard,
        OPTION_KeyboardOffset, //gotta figure out how to label this bad boy
        OPTION_KeyboardLetters, //do we draw the letters on the keyboard?
        OPTION_Language,
        OPTION_FullScreen,
    };


    /// <summary>
    /// Interaction logic for OptionsWindow.xaml
    /// </summary>
    public partial class OptionsWindow : Window
    {
        public OptionsWindow()
        {
            InitializeComponent();

            string szAbout = Globals.rm.GetString("IDS_MSG_About");
            szAbout = szAbout.Replace("\\r\\n", "\r\n");
            //txtAboutCopyright.Text = szAbout;

            // skins
            optSkinStyle.txtTitle = Globals.rm.GetString("IDS_MENU_ButtonStyle");
            optSkinStyle.iRefID = (int)EnumOption.OPTION_SkinStyle;

            optSkinStyle.AddItem("Classic");
            optSkinStyle.AddItem("Shine");
            optSkinStyle.AddItem("Type");
            optSkinStyle.AddItem("Flat");
            optSkinStyle.AddItem("Standard");

            optSkinStyle.SetCurrentIndex((int)Globals.btnMan.GetSkinStyle());
            optSkinStyle.IndexUpdate = IndexUpdate;


            //color
            optSkinColor.txtTitle = Globals.rm.GetString("IDS_MENU_ButtonColor");
            optSkinColor.iRefID = (int)EnumOption.OPTION_SkinColor;

            optSkinColor.AddItem("Black");
            optSkinColor.AddItem("Blue");
            optSkinColor.AddItem("Blue Steel");
            optSkinColor.AddItem("Brown");
            optSkinColor.AddItem("Green");
            optSkinColor.AddItem("Grey");
            optSkinColor.AddItem("Orange");
            optSkinColor.AddItem("Pink");

            optSkinColor.SetCurrentIndex((int)Globals.btnMan.GetSkinColor());
            optSkinColor.IndexUpdate = IndexUpdate;

            /*
            optSkin.txtTitle = rm.GetString("IDS_OPT_Skin");
            optSkin.iRefID = (int)EnumOption.OPTION_Skin;
            EnumCalcProSkins eSkin;
            for (int i = 0; i < (int)EnumCalcProSkins.SKIN_Count; i++)
            {
                eSkin = (EnumCalcProSkins)i;
                optSkin.AddItem(eSkin.ToString());
            }
            optSkin.SetCurrentIndex((int)Globals.btnMan.GetSkinType());
            optSkin.IndexUpdate += IndexUpdate;*/

            /*   optSkin.txtTitle = rm.GetString("IDS_OPT_Skin");
               optSkin.AddItem(rm.GetString("IDS_OPT_MetallicBlue"));
               optSkin.AddItem(rm.GetString("IDS_OPT_MidnightBlack"));
               optSkin.AddItem(rm.GetString("IDS_OPT_Brown"));
               optSkin.AddItem(rm.GetString("IDS_OPT_WirelessGreen"));
               optSkin.AddItem(rm.GetString("IDS_OPT_GlamPink"));
               optSkin.AddItem(rm.GetString("IDS_OPT_BlueSteel"));
               optSkin.SetCurrentIndex(2);*/

            // PlaySounds
            optPlaySounds.txtTitle = Globals.rm.GetString("IDS_OPT_PlaySounds");
            optPlaySounds.iRefID = (int)EnumOption.OPTION_PlaySounds;
            optPlaySounds.AddItem(Globals.rm.GetString("IDS_MENU_Off"));
            optPlaySounds.AddItem(Globals.rm.GetString("IDS_MENU_On"));
            optPlaySounds.AddItem(Globals.rm.GetString("IDS_MENU_Quiet"));
            optPlaySounds.SetCurrentIndex(Globals.Settings.iPlaySounds);
            optPlaySounds.IndexUpdate += IndexUpdate;

            // Language
            /*optLanguage.txtTitle = "Language";
            optLanguage.iRefID = (int)EnumOption.OPTION_Language;
            optLanguage.AddItem("English");
            optLanguage.AddItem("French");
            optLanguage.SetCurrentIndex(Globals.Settings.iLanguageID);
            optLanguage.IndexUpdate += IndexUpdate;*/

            // use regional settings
            /*optUseRegionalSettings.txtTitle = rm.GetString("IDS_OPT_RegionalSettings");
            optUseRegionalSettings.iRefID = (int)EnumOption.OPTION_UseRegionalSettings;
            optUseRegionalSettings.SetValue(false);*/


            // Calculator mode
            optCalculatorMode.txtTitle = Globals.rm.GetString("IDS_OPT_CalculatorMode");
            optCalculatorMode.iRefID = (int)EnumOption.OPTION_CalculatorMode;
            optCalculatorMode.AddItem(Globals.rm.GetString("IDS_OPT_Expression"));
            optCalculatorMode.AddItem(Globals.rm.GetString("IDS_OPT_Algebraic"));
            optCalculatorMode.AddItem(Globals.rm.GetString("IDS_OPT_DirectAlgebraic"));
            optCalculatorMode.AddItem(Globals.rm.GetString("IDS_OPT_Normal"));
            optCalculatorMode.AddItem(Globals.rm.GetString("IDS_OPT_RPN"));
            optCalculatorMode.SetCurrentIndex((int)Globals.Settings.eCalcType);
            optCalculatorMode.IndexUpdate += IndexUpdate;

            // Notation
            optNotation.txtTitle = Globals.rm.GetString("IDS_OPT_Notation");
            optNotation.iRefID = (int)EnumOption.OPTION_Notation;
            optNotation.AddItem(Globals.rm.GetString("IDS_OPT_Normal"));
            optNotation.AddItem(Globals.rm.GetString("IDS_OPT_Fixed"));
            optNotation.AddItem(Globals.rm.GetString("IDS_OPT_Scientific"));
            optNotation.AddItem(Globals.rm.GetString("IDS_OPT_Engineering"));
            optNotation.AddItem(Globals.rm.GetString("IDS_OPT_Fractions"));
            optNotation.SetCurrentIndex((int)Globals.Settings.eDisplayType);
            optNotation.IndexUpdate += IndexUpdate;

            // Record tape
     /*       optRecordTape.txtTitle = Globals.rm.GetString("IDS_OPT_RecordTape");
            optRecordTape.iRefID = (int)EnumOption.OPTION_RecordTape;
            optRecordTape.AddItem(Globals.rm.GetString("IDS_MENU_Off"));
            optRecordTape.AddItem(Globals.rm.GetString("IDS_MENU_On"));
            optRecordTape.SetCurrentIndex(Globals.Settings.bUseTape ? 1 : 0);
            optRecordTape.IndexUpdate += IndexUpdate;  */

            // Advanced Memory
            optAdvancedMemory.txtTitle = Globals.rm.GetString("IDS_OPT_AdvancedMemory");
            optAdvancedMemory.iRefID = (int)EnumOption.OPTION_AdvancedMemory;
            optAdvancedMemory.AddItem(Globals.rm.GetString("IDS_MENU_Off"));
            optAdvancedMemory.AddItem(Globals.rm.GetString("IDS_MENU_On"));
            optAdvancedMemory.SetCurrentIndex(Globals.Settings.bAdvMem ? 1 : 0);
            //optAdvancedMemory.SetValue(Globals.Settings.bAdvMem);
            optAdvancedMemory.IndexUpdate += IndexUpdate;

            // Fixed Digits
            optFixedDigits.txtTitle = Globals.rm.GetString("IDS_OPT_FixedDigits");
            optFixedDigits.iRefID = (int)EnumOption.OPTION_FixedDigits;
            for (int i = 0; i < 9; i++)
                optFixedDigits.AddItem(i.ToString());
            optFixedDigits.SetCurrentIndex(Globals.Settings.iFixedDigits);
            optFixedDigits.IndexUpdate += IndexUpdate;

            // conversion buffer
            optConversionBuffer.txtTitle = Globals.rm.GetString("IDS_OPT_ConversionBuffer");
            optConversionBuffer.iRefID = (int)EnumOption.OPTION_ConversionBuffer;
            optConversionBuffer.AddItem("Byte (8 bit)");
            optConversionBuffer.AddItem("Word (16 bit)");
            optConversionBuffer.AddItem("DWord (32 bit)");
            optConversionBuffer.AddItem("Large (48 bit)");
            optConversionBuffer.SetCurrentIndex(2);//bugbug
            optConversionBuffer.IndexUpdate += IndexUpdate;

            // rpn stack size
            optRPNStackSize.txtTitle = Globals.rm.GetString("IDS_OPT_RPNStackSize");
            optRPNStackSize.iRefID = (int)EnumOption.OPTION_RPNStackSize;
            optRPNStackSize.AddItem("2(X,Y)");
            optRPNStackSize.AddItem("4(X,Y,Z,T)");
            optRPNStackSize.AddItem("5");
            optRPNStackSize.AddItem("10");
            optRPNStackSize.AddItem("50");
            optRPNStackSize.SetCurrentIndex(1);//bugbug
            optRPNStackSize.IndexUpdate += IndexUpdate;

            // end period payments
            optTVM.txtTitle = Globals.rm.GetString("IDS_OPT_EndPeriodPayments");
            optTVM.iRefID = (int)EnumOption.OPTION_EndPeriodPayments;
            //optTVM.SetValue(Globals.Settings.bEndPeriodPayments);
            optTVM.AddItem(Globals.rm.GetString("IDS_MENU_Off"));
            optTVM.AddItem(Globals.rm.GetString("IDS_MENU_On"));
            optTVM.SetCurrentIndex((Globals.Settings.bEndPeriodPayments ? 1 : 0));
            optTVM.IndexUpdate += IndexUpdate;

            // payment sched.
            optPaymentSched.txtTitle = Globals.rm.GetString("IDS_OPT_PaymentSched");
            optPaymentSched.iRefID = (int)EnumOption.OPTION_PaymentSchedule;
            optPaymentSched.AddItem(Globals.rm.GetString("IDS_OPT_Monthly"));
            optPaymentSched.AddItem(Globals.rm.GetString("IDS_OPT_MonthlyAccel"));
            optPaymentSched.AddItem(Globals.rm.GetString("IDS_OPT_BiWeekly"));
            optPaymentSched.AddItem(Globals.rm.GetString("IDS_OPT_BiWeeklyAccel"));
            optPaymentSched.AddItem(Globals.rm.GetString("IDS_OPT_Weekly"));
            optPaymentSched.AddItem(Globals.rm.GetString("IDS_OPT_WeeklyAccel"));
            optPaymentSched.AddItem(Globals.rm.GetString("IDS_OPT_SemiMonthly"));
            optPaymentSched.AddItem(Globals.rm.GetString("IDS_OPT_SemiMonthlyAccel"));
            optPaymentSched.AddItem(Globals.rm.GetString("IDS_OPT_BiMonthly"));
            optPaymentSched.SetCurrentIndex(0);
            optPaymentSched.IndexUpdate += IndexUpdate;

            //graphing style
            optGraphStyle.txtTitle = "Graph Style";//Globals.rm.GetString("IDS_OPT_ShowPointsOnly");
            optGraphStyle.iRefID = (int)EnumOption.OPTION_GraphStyle;
            optGraphStyle.AddItem(Globals.rm.GetString("IDS_OPT_Normal"));
            optGraphStyle.AddItem(Globals.rm.GetString("IDS_OPT_Large"));
            optGraphStyle.AddItem(Globals.rm.GetString("IDS_OPT_ShowPointsOnly"));
            optGraphStyle.SetCurrentIndex(0);
            optGraphStyle.IndexUpdate += IndexUpdate;
        }

        public void IndexUpdate(int iID, int iSelIndex)
        {
            switch (iID)//not all of these are used but 
            {
                case (int)EnumOption.OPTION_UseRegionalSettings:
                    break;
                case (int)EnumOption.OPTION_PlaySounds:
                    Globals.Settings.iPlaySounds = iSelIndex;
                    break;
                case (int)EnumOption.OPTION_RecordTape:
                   // Globals.Settings.bUseTape = Convert.ToBoolean(iSelIndex);
                    break;
                case (int)EnumOption.OPTION_AdvancedMemory:
                    Globals.Settings.bAdvMem = Convert.ToBoolean(iSelIndex);
                    Globals.btnMan.ResetLayouts();
                    Globals.btnMan.SetSkinColor(Globals.Settings.eSkinColor, true);//we need to reload the buttons
                    break;
                case (int)EnumOption.OPTION_CalculatorMode:
                    Globals.Settings.eCalcType = (CalcOperationType)iSelIndex;
                    Globals.btnMan.ResetLayouts();
                    Globals.btnMan.SetSkinColor(Globals.Settings.eSkinColor, true);//we need to reload displays & buttons, etc for expression/RPN
                    if ((Globals.Settings.eCalcType == CalcOperationType.CALC_RPN || Globals.Settings.eCalcType == CalcOperationType.CALC_String) &&
                        (DisplayType)iSelIndex == DisplayType.DISPLAY_Fractions)
                        Globals.Settings.eDisplayType = DisplayType.DISPLAY_Float;
                    break;
                case (int)EnumOption.OPTION_Notation:
                    if ((Globals.Settings.eCalcType == CalcOperationType.CALC_RPN || Globals.Settings.eCalcType == CalcOperationType.CALC_String) &&
                        (DisplayType)iSelIndex == DisplayType.DISPLAY_Fractions)
                        return;
                    Globals.Settings.eDisplayType = (DisplayType)iSelIndex;
                    break;
                case (int)EnumOption.OPTION_ConversionBuffer:
                    switch (iSelIndex)
                    {
                        case 0:
                            Globals.Settings.iConversionBuffer = (8);
                            break;
                        case 1:
                            Globals.Settings.iConversionBuffer = (16);
                            break;
                        case 2:
                        default:
                            Globals.Settings.iConversionBuffer = (32);
                            break;
                        case 3:
                            Globals.Settings.iConversionBuffer = (48);
                            break;

                    }
                    break;
                case (int)EnumOption.OPTION_RPNStackSize:
                    switch (iSelIndex)
                    {
                        case 0:
                            Globals.Settings.iRPNStackSize = 2;
                            break;
                        case 1:
                            Globals.Settings.iRPNStackSize = 4;
                            break;
                        case 2:
                            Globals.Settings.iRPNStackSize = 5;
                            break;
                        case 3:
                            Globals.Settings.iRPNStackSize = 10;
                            break;
                        case 4:
                        default:
                            Globals.Settings.iRPNStackSize = 50;
                            break;
                    }
                    break;
                case (int)EnumOption.OPTION_SkinStyle:
                    Globals.Settings.eSkinStyle = (EnumCalcProSkinStyle)iSelIndex;
                    break;
                case (int)EnumOption.OPTION_SkinColor:
                    Globals.Settings.eSkinColor = (EnumCalcProSkinColor)iSelIndex;
                    break;
                case (int)EnumOption.OPTION_FixedDigits:
                    Globals.Settings.iFixedDigits = iSelIndex;
                    break;
                case (int)EnumOption.OPTION_GraphAccuracy:
                    break;
                case (int)EnumOption.OPTION_GraphStyle:
                    Globals.Settings.eGraphStyle = (EnumGraphStyle)iSelIndex;
                    break;
                case (int)EnumOption.OPTION_EndPeriodPayments: //TVM
                    Globals.Settings.bEndPeriodPayments = (iSelIndex > 0) ? true : false;
                    break;
                case (int)EnumOption.OPTION_PaymentSchedule: // Mortgage
                    Globals.Settings.ePaymentSched = (PAYMENT_SCHEDULE)iSelIndex;
                    break;
                case (int)EnumOption.OPTION_Size:			//size on PC
                    break;
                case (int)EnumOption.OPTION_Keyboard:
                    break;
                case (int)EnumOption.OPTION_KeyboardOffset: //gotta figure out how to label this bad boy
                    break;
                case (int)EnumOption.OPTION_KeyboardLetters: //do we draw the letters on the keyboard?
                    break;
                case (int)EnumOption.OPTION_Language:
                    Globals.Settings.iLanguageID = iSelIndex;
                    break;
                default:
                    break;
            }
        }

    }

}
