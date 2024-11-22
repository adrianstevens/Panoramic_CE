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
    public struct SkinColors
    {
        public Color Text;
        public Color TextDown;
        public Color Grad1Top;
        public Color Grad1Bottom;
        public Color Grad2Top;
        public Color Grad2Bottom;
        public Color DownColor;
        public Color Metro;
    };


    public class ButtonSkinClassic : IButtonSkin
    {
        protected class TypeDownClassic
        {
            public Brush brBorder { get; set; }
            public Brush brBorder2 { get; set; }
            public Brush brText { get; set; }
        }

        protected Border Border;
        protected Border Border2;
        protected TypeDownClassic _sDownClassic = null;

        protected SkinColors[] sBtnColors = new SkinColors[6];

        protected EnumCalcProSkinColor eCurrentColor = EnumCalcProSkinColor.Count;
        protected EnumCalcProSkinStyle eCurrentStyle = EnumCalcProSkinStyle.Count;

        public ButtonSkinClassic()
        {
        }

        protected bool LoadSkin(EnumCalcProSkinColor eSkin)
        {
            switch (eSkin)
            {
                default:
                case EnumCalcProSkinColor.Blue:
                    sBtnColors[0].Text = BrushFactory.ToColor(0xFF333333);
                    sBtnColors[0].TextDown = BrushFactory.ToColor(0xFF4A4A4A);
                    sBtnColors[0].Grad1Top = Colors.White;
                    sBtnColors[0].Grad1Bottom = BrushFactory.ToColor(0xFF494949);
                    sBtnColors[0].Grad2Top = BrushFactory.ToColor(0xFFD6D6D6);
                    sBtnColors[0].Grad2Bottom = BrushFactory.ToColor(0xFF858585);
                    sBtnColors[0].DownColor = BrushFactory.ToColor(0xFF959393);
                    sBtnColors[0].Metro = BrushFactory.ToColor(0xFFAFAFAF);

                    sBtnColors[1].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[1].TextDown = BrushFactory.ToColor(0xFF96A5BB);
                    sBtnColors[1].Grad1Top = BrushFactory.ToColor(0xFFD7E6FD);
                    sBtnColors[1].Grad1Bottom = BrushFactory.ToColor(0xFF243147);
                    sBtnColors[1].Grad2Top = BrushFactory.ToColor(0xFF92ACD4);
                    sBtnColors[1].Grad2Bottom = BrushFactory.ToColor(0xFF405C8B);
                    sBtnColors[1].DownColor = BrushFactory.ToColor(0xFF5F7699);
                    sBtnColors[1].Metro = BrushFactory.ToColor(0xFF6681AC);
                    
                    sBtnColors[2].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[2].TextDown = BrushFactory.ToColor(0xFF708098);
                    sBtnColors[2].Grad1Top = BrushFactory.ToColor(0xFF9DADC5);
                    sBtnColors[2].Grad1Bottom = BrushFactory.ToColor(0xFF14253D);
                    sBtnColors[2].Grad2Top = BrushFactory.ToColor(0xFF6782A6);
                    sBtnColors[2].Grad2Bottom = BrushFactory.ToColor(0xFF203B62);
                    sBtnColors[2].DownColor = BrushFactory.ToColor(0xFF485F7E);
                    sBtnColors[2].Metro = BrushFactory.ToColor(0xFF29446A);
                    
                    sBtnColors[3].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[3].TextDown = BrushFactory.ToColor(0xFFE3A98B);
                    sBtnColors[3].Grad1Top = BrushFactory.ToColor(0xFFFFC1A2);
                    sBtnColors[3].Grad1Bottom = BrushFactory.ToColor(0xFF4B2411);
                    sBtnColors[3].Grad2Top = BrushFactory.ToColor(0xFFD89170);
                    sBtnColors[3].Grad2Bottom = BrushFactory.ToColor(0xFF854125);
                    sBtnColors[3].DownColor = BrushFactory.ToColor(0xFFA76A4D);
                    sBtnColors[3].Metro = BrushFactory.ToColor(0xFF995538);

                    sBtnColors[4].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[4].TextDown = BrushFactory.ToColor(0xFFB0C580);
                    sBtnColors[4].Grad1Top = BrushFactory.ToColor(0xFFBACF86);
                    sBtnColors[4].Grad1Bottom = BrushFactory.ToColor(0xFF2D4332);
                    sBtnColors[4].Grad2Top = BrushFactory.ToColor(0xFF90B25A);
                    sBtnColors[4].Grad2Bottom = BrushFactory.ToColor(0xFF5D8134);
                    sBtnColors[4].DownColor = BrushFactory.ToColor(0xFF69833F);
                    sBtnColors[4].Metro = BrushFactory.ToColor(0xFF638638);
                    
                    sBtnColors[5].Text = BrushFactory.ToColor(0xFF393939);
                    sBtnColors[5].TextDown = BrushFactory.ToColor(0xFF685C3B);
                    sBtnColors[5].Grad1Top = BrushFactory.ToColor(0xFFFFEEBD);
                    sBtnColors[5].Grad1Bottom = BrushFactory.ToColor(0xFF90722D);
                    sBtnColors[5].Grad2Top = BrushFactory.ToColor(0xFFEFCE82);
                    sBtnColors[5].Grad2Bottom = BrushFactory.ToColor(0xFFB89A47);
                    sBtnColors[5].DownColor = BrushFactory.ToColor(0xFFB69F68);
                    sBtnColors[5].Metro = BrushFactory.ToColor(0xFFC6A757);

                    break;
                case EnumCalcProSkinColor.BlueSteel:
                    sBtnColors[0].Text = BrushFactory.ToColor(0xFF202525);
                    sBtnColors[0].TextDown = BrushFactory.ToColor(0xFF3A4247);
                    sBtnColors[0].Grad1Top = BrushFactory.ToColor(0xFFC2CBD0);
                    sBtnColors[0].Grad1Bottom = BrushFactory.ToColor(0xFF333537);
                    sBtnColors[0].Grad2Top = BrushFactory.ToColor(0xFFE7E7E7);
                    sBtnColors[0].Grad2Bottom = BrushFactory.ToColor(0xFF8C8E84);
                    sBtnColors[0].DownColor = BrushFactory.ToColor(0xFFA9ADAF);
                    sBtnColors[0].Metro = BrushFactory.ToColor(0xFFACADA7);

                    sBtnColors[1].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[1].TextDown = BrushFactory.ToColor(0xFF99A4AA);
                    sBtnColors[1].Grad1Top = BrushFactory.ToColor(0xFFB7C2C8);
                    sBtnColors[1].Grad1Bottom = BrushFactory.ToColor(0xFF292E35);
                    sBtnColors[1].Grad2Top = BrushFactory.ToColor(0xFF8A9DA8);
                    sBtnColors[1].Grad2Bottom = BrushFactory.ToColor(0xFF4B5664);
                    sBtnColors[1].DownColor = BrushFactory.ToColor(0xFF6C7D87);
                    sBtnColors[1].Metro = BrushFactory.ToColor(0xFF515D6A);
                    
                    sBtnColors[2].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[2].TextDown = BrushFactory.ToColor(0xFF99A4AA);
                    sBtnColors[2].Grad1Top = BrushFactory.ToColor(0xFFB7C2C8);
                    sBtnColors[2].Grad1Bottom = BrushFactory.ToColor(0xFF131C21);
                    sBtnColors[2].Grad2Top = BrushFactory.ToColor(0xFF8A969D);
                    sBtnColors[2].Grad2Bottom = BrushFactory.ToColor(0xFF202E37);
                    sBtnColors[2].DownColor = BrushFactory.ToColor(0xFF707C83);
                    sBtnColors[2].Metro = BrushFactory.ToColor(0xFF34414A);
                    
                    sBtnColors[3].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[3].TextDown = BrushFactory.ToColor(0xFFF4AA98);
                    sBtnColors[3].Grad1Top = BrushFactory.ToColor(0xFFF7EBE0);
                    sBtnColors[3].Grad1Bottom = BrushFactory.ToColor(0xFF451806);
                    sBtnColors[3].Grad2Top = BrushFactory.ToColor(0xFFF9B09F);
                    sBtnColors[3].Grad2Bottom = BrushFactory.ToColor(0xFF8D2D08);
                    sBtnColors[3].DownColor = BrushFactory.ToColor(0xFFB86F5F);
                    sBtnColors[3].Metro = BrushFactory.ToColor(0xFFAA5131);

                    sBtnColors[4].Text = BrushFactory.ToColor(0xFF2F2F2A);
                    sBtnColors[4].TextDown = BrushFactory.ToColor(0xFF6C693F);
                    sBtnColors[4].Grad1Top = BrushFactory.ToColor(0xFFFFFDE1);
                    sBtnColors[4].Grad1Bottom = BrushFactory.ToColor(0xFF29280F);
                    sBtnColors[4].Grad2Top = BrushFactory.ToColor(0xFFE4DFA4);
                    sBtnColors[4].Grad2Bottom = BrushFactory.ToColor(0xFFA59E10);
                    sBtnColors[4].DownColor = BrushFactory.ToColor(0xFFABA566);
                    sBtnColors[4].Metro = BrushFactory.ToColor(0xFFB5AF38);
                    
                    sBtnColors[5].Text = BrushFactory.ToColor(0xFF1E2C31);
                    sBtnColors[5].TextDown = BrushFactory.ToColor(0xFF44606A);
                    sBtnColors[5].Grad1Top = BrushFactory.ToColor(0xFFE0F1F7);
                    sBtnColors[5].Grad1Bottom = BrushFactory.ToColor(0xFF102A33);
                    sBtnColors[5].Grad2Top = BrushFactory.ToColor(0xFF8CDBF7);
                    sBtnColors[5].Grad2Bottom = BrushFactory.ToColor(0xFF275F72);
                    sBtnColors[5].DownColor = BrushFactory.ToColor(0xFF5495AD);
                    sBtnColors[5].Metro = BrushFactory.ToColor(0xFF5193AA);

                    break;
                case EnumCalcProSkinColor.Grey:
                    sBtnColors[0].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[0].TextDown = BrushFactory.ToColor(0xFFC2C2C2);
                    sBtnColors[0].Grad1Top = BrushFactory.ToColor(0xFFDEDEDE);
                    sBtnColors[0].Grad1Bottom = BrushFactory.ToColor(0xFF212121);
                    sBtnColors[0].Grad2Top = BrushFactory.ToColor(0xFFBCBCBC);
                    sBtnColors[0].Grad2Bottom = BrushFactory.ToColor(0xFF585858);
                    sBtnColors[0].DownColor = BrushFactory.ToColor(0xFF898989);
                    sBtnColors[0].Metro = BrushFactory.ToColor(0xFF8C8C8C);

                    sBtnColors[1].Text = BrushFactory.ToColor(0xFF86CA4B);
                    sBtnColors[1].TextDown = BrushFactory.ToColor(0xFF668948);
                    sBtnColors[1].Grad1Top = BrushFactory.ToColor(0xFFA3A3A3);
                    sBtnColors[1].Grad1Bottom = BrushFactory.ToColor(0xFF252525);
                    sBtnColors[1].Grad2Top = BrushFactory.ToColor(0xFF8D8D8D);
                    sBtnColors[1].Grad2Bottom = BrushFactory.ToColor(0xFF333333);
                    sBtnColors[1].DownColor = BrushFactory.ToColor(0xFF626262);
                    sBtnColors[1].Metro = BrushFactory.ToColor(0xFF484848);
                    
                    sBtnColors[2].Text = BrushFactory.ToColor(0xFF7BAA52);
                    sBtnColors[2].TextDown = BrushFactory.ToColor(0xFF608142);
                    sBtnColors[2].Grad1Top = BrushFactory.ToColor(0xFF787878);
                    sBtnColors[2].Grad1Bottom = BrushFactory.ToColor(0xFF141414);
                    sBtnColors[2].Grad2Top = BrushFactory.ToColor(0xFF686868);
                    sBtnColors[2].Grad2Bottom = BrushFactory.ToColor(0xFF2F2F2F);
                    sBtnColors[2].DownColor = BrushFactory.ToColor(0xFF4B4A4A);
                    sBtnColors[2].Metro = BrushFactory.ToColor(0xFF303030);
                    
                    sBtnColors[3].Text = BrushFactory.ToColor(0xFF1D1F1D);
                    sBtnColors[3].TextDown = BrushFactory.ToColor(0xFF3D3024);
                    sBtnColors[3].Grad1Top = BrushFactory.ToColor(0xFFE7AA94);
                    sBtnColors[3].Grad1Bottom = BrushFactory.ToColor(0xFF4D240A);
                    sBtnColors[3].Grad2Top = BrushFactory.ToColor(0xFFDE9273);
                    sBtnColors[3].Grad2Bottom = BrushFactory.ToColor(0xFF7A3E20);
                    sBtnColors[3].DownColor = BrushFactory.ToColor(0xFFA56A4F);
                    sBtnColors[3].Metro = BrushFactory.ToColor(0xFFA05F41);

                    sBtnColors[4].Text = BrushFactory.ToColor(0xFF292C29);
                    sBtnColors[4].TextDown = BrushFactory.ToColor(0xFF2C4D2C);
                    sBtnColors[4].Grad1Top = BrushFactory.ToColor(0xFFCEE7B5);
                    sBtnColors[4].Grad1Bottom = BrushFactory.ToColor(0xFF314D10);
                    sBtnColors[4].Grad2Top = BrushFactory.ToColor(0xFFBDE39C);
                    sBtnColors[4].Grad2Bottom = BrushFactory.ToColor(0xFF73A242);
                    sBtnColors[4].DownColor = BrushFactory.ToColor(0xFF7A955D);
                    sBtnColors[4].Metro = BrushFactory.ToColor(0xFF87B35A);
                    
                    sBtnColors[5].Text = BrushFactory.ToColor(0xFF202320);
                    sBtnColors[5].TextDown = BrushFactory.ToColor(0xFF3C3D33);
                    sBtnColors[5].Grad1Top = BrushFactory.ToColor(0xFFFBE4B5);
                    sBtnColors[5].Grad1Bottom = BrushFactory.ToColor(0xFF4D3F15);
                    sBtnColors[5].Grad2Top = BrushFactory.ToColor(0xFFDEBE73);
                    sBtnColors[5].Grad2Bottom = BrushFactory.ToColor(0xFF8B7533);
                    sBtnColors[5].DownColor = BrushFactory.ToColor(0xFFA99159);
                    sBtnColors[5].Metro = BrushFactory.ToColor(0xFFB09650);
                    break;
                case EnumCalcProSkinColor.Pink:
                    sBtnColors[0].Text = BrushFactory.ToColor(0xFF291115);
                    sBtnColors[0].TextDown = BrushFactory.ToColor(0xFF9D3A56);
                    sBtnColors[0].Grad1Top = Colors.White;
                    sBtnColors[0].Grad1Bottom = BrushFactory.ToColor(0xFF391921);
                    sBtnColors[0].Grad2Top = BrushFactory.ToColor(0xFFFDF0F5);
                    sBtnColors[0].Grad2Bottom = BrushFactory.ToColor(0xFFD098AA);
                    sBtnColors[0].DownColor = BrushFactory.ToColor(0xFFEBC6D4);
                    sBtnColors[0].Metro = BrushFactory.ToColor(0xFFF5E1E8);

                    sBtnColors[1].Text = BrushFactory.ToColor(0xFF3B1A20);
                    sBtnColors[1].TextDown = BrushFactory.ToColor(0xFF975F69);
                    sBtnColors[1].Grad1Top = BrushFactory.ToColor(0xFFF1CBDB);
                    sBtnColors[1].Grad1Bottom = BrushFactory.ToColor(0xFF5E3845);
                    sBtnColors[1].Grad2Top = BrushFactory.ToColor(0xFFE7AEC6);
                    sBtnColors[1].Grad2Bottom = BrushFactory.ToColor(0xFFD07796);
                    sBtnColors[1].DownColor = BrushFactory.ToColor(0xFFC491A7);
                    sBtnColors[1].Metro = BrushFactory.ToColor(0xFFD07996);
                    
                    sBtnColors[2].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[2].TextDown = BrushFactory.ToColor(0xFFDAACB4);
                    sBtnColors[2].Grad1Top = BrushFactory.ToColor(0xFFF5CEDF);
                    sBtnColors[2].Grad1Bottom = BrushFactory.ToColor(0xFF501424);
                    sBtnColors[2].Grad2Top = BrushFactory.ToColor(0xFFE79FBA);
                    sBtnColors[2].Grad2Bottom = BrushFactory.ToColor(0xFFC22F5E);
                    sBtnColors[2].DownColor = BrushFactory.ToColor(0xFFC47C97);
                    sBtnColors[2].Metro = BrushFactory.ToColor(0xFFC23261);
                    
                    sBtnColors[3].Text = BrushFactory.ToColor(0xFF291115);
                    sBtnColors[3].TextDown = BrushFactory.ToColor(0xFF9D3A56);
                    sBtnColors[3].Grad1Top = BrushFactory.ToColor(0xFFF5CEDF);
                    sBtnColors[3].Grad1Bottom = BrushFactory.ToColor(0xFF3B0E1A);
                    sBtnColors[3].Grad2Top = BrushFactory.ToColor(0xFFD489A5);
                    sBtnColors[3].Grad2Bottom = BrushFactory.ToColor(0xFF9B1D45);
                    sBtnColors[3].DownColor = BrushFactory.ToColor(0xFFA9657E);
                    sBtnColors[3].Metro = BrushFactory.ToColor(0xFFB34C6F);

                    sBtnColors[4].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[4].TextDown = BrushFactory.ToColor(0xFFCBE9AD);
                    sBtnColors[4].Grad1Top = BrushFactory.ToColor(0xFFCEE7B5);
                    sBtnColors[4].Grad1Bottom = BrushFactory.ToColor(0xFF314D10);
                    sBtnColors[4].Grad2Top = BrushFactory.ToColor(0xFFBDE39C);
                    sBtnColors[4].Grad2Bottom = BrushFactory.ToColor(0xFF73A242);
                    sBtnColors[4].DownColor = BrushFactory.ToColor(0xFF7A955D);
                    sBtnColors[4].Metro = BrushFactory.ToColor(0xFF7BA84C);
                    
                    sBtnColors[5].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[5].TextDown = BrushFactory.ToColor(0xFFE0C0C9);
                    sBtnColors[5].Grad1Top = BrushFactory.ToColor(0xFFF5CEDF);
                    sBtnColors[5].Grad1Bottom = BrushFactory.ToColor(0xFF4D2439);
                    sBtnColors[5].Grad2Top = BrushFactory.ToColor(0xFFCE92A5);
                    sBtnColors[5].Grad2Bottom = BrushFactory.ToColor(0xFF815164);
                    sBtnColors[5].DownColor = BrushFactory.ToColor(0xFFB17D8D);
                    sBtnColors[5].Metro = BrushFactory.ToColor(0xFF88576A);

                    break;


                case EnumCalcProSkinColor.Black:
                    sBtnColors[0].Text = BrushFactory.ToColor(0xFF202525);
                    sBtnColors[0].TextDown = BrushFactory.ToColor(0xFF3A4247);
                    sBtnColors[0].Grad1Top = BrushFactory.ToColor(0xFFC2CBD0);
                    sBtnColors[0].Grad1Bottom = BrushFactory.ToColor(0xFF333537);
                    sBtnColors[0].Grad2Top = BrushFactory.ToColor(0xFFE7E7E7);
                    sBtnColors[0].Grad2Bottom = BrushFactory.ToColor(0xFF8C8E84);
                    sBtnColors[0].DownColor = BrushFactory.ToColor(0xFFA9ADAF);
                    sBtnColors[0].Metro = BrushFactory.ToColor(0xFFB4B5AF);

                    sBtnColors[1].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[1].TextDown = BrushFactory.ToColor(0xFF606060);
                    sBtnColors[1].Grad1Top = BrushFactory.ToColor(0xFFBCBCBC);
                    sBtnColors[1].Grad1Bottom = BrushFactory.ToColor(0xFF000000);
                    sBtnColors[1].Grad2Top = BrushFactory.ToColor(0xFF505050);
                    sBtnColors[1].Grad2Bottom = BrushFactory.ToColor(0xFF000000);
                    sBtnColors[1].DownColor = BrushFactory.ToColor(0xFF373737);
                    sBtnColors[1].Metro = BrushFactory.ToColor(0xFF000000);

                    sBtnColors[2].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[2].TextDown = BrushFactory.ToColor(0xFF8CA5D8);
                    sBtnColors[2].Grad1Top = BrushFactory.ToColor(0xFFAAC0E5);
                    sBtnColors[2].Grad1Bottom = BrushFactory.ToColor(0xFF101927);
                    sBtnColors[2].Grad2Top = BrushFactory.ToColor(0xFF6B91CF);
                    sBtnColors[2].Grad2Bottom = BrushFactory.ToColor(0xFF103D80);
                    sBtnColors[2].DownColor = BrushFactory.ToColor(0xFF4F70A5);
                    sBtnColors[2].Metro = BrushFactory.ToColor(0xFF245091);

                    sBtnColors[3].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[3].TextDown = BrushFactory.ToColor(0xFFBE7575);
                    sBtnColors[3].Grad1Top = BrushFactory.ToColor(0xFFD4A7A7);
                    sBtnColors[3].Grad1Bottom = BrushFactory.ToColor(0xFF221919);
                    sBtnColors[3].Grad2Top = BrushFactory.ToColor(0xFFCA605D);
                    sBtnColors[3].Grad2Bottom = BrushFactory.ToColor(0xFF8A0400);
                    sBtnColors[3].DownColor = BrushFactory.ToColor(0xFFA14644);
                    sBtnColors[3].Metro = BrushFactory.ToColor(0xFF981A16);

                    sBtnColors[4].Text = BrushFactory.ToColor(0xFF393939);
                    sBtnColors[4].TextDown = BrushFactory.ToColor(0xFF685C3B);
                    sBtnColors[4].Grad1Top = BrushFactory.ToColor(0xFFFFEEBD);
                    sBtnColors[4].Grad1Bottom = BrushFactory.ToColor(0xFF90722D);
                    sBtnColors[4].Grad2Top = BrushFactory.ToColor(0xFFEFCE82);
                    sBtnColors[4].Grad2Bottom = BrushFactory.ToColor(0xFFB89A47);
                    sBtnColors[4].DownColor = BrushFactory.ToColor(0xFFB69F68);
                    sBtnColors[4].Metro = BrushFactory.ToColor(0xFFC6A757);

                    sBtnColors[5].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[5].TextDown = BrushFactory.ToColor(0xFFCBE9AD);
                    sBtnColors[5].Grad1Top = BrushFactory.ToColor(0xFFCEE7B5);
                    sBtnColors[5].Grad1Bottom = BrushFactory.ToColor(0xFF314D10);
                    sBtnColors[5].Grad2Top = BrushFactory.ToColor(0xFFBDE39C);
                    sBtnColors[5].Grad2Bottom = BrushFactory.ToColor(0xFF73A242);
                    sBtnColors[5].DownColor = BrushFactory.ToColor(0xFF7A955D);
                    sBtnColors[5].Metro = BrushFactory.ToColor(0xFF7CA94D);

                    break;
                case EnumCalcProSkinColor.Brown:
                    sBtnColors[0].Text = BrushFactory.ToColor(0xFF202525);
                    sBtnColors[0].TextDown = BrushFactory.ToColor(0xFF3A4247);
                    sBtnColors[0].Grad1Top = BrushFactory.ToColor(0xFFC2CBD0);
                    sBtnColors[0].Grad1Bottom = BrushFactory.ToColor(0xFF333537);
                    sBtnColors[0].Grad2Top = BrushFactory.ToColor(0xFFE7E7E7);
                    sBtnColors[0].Grad2Bottom = BrushFactory.ToColor(0xFF8C8E84);
                    sBtnColors[0].DownColor = BrushFactory.ToColor(0xFFA9ADAF);
                    sBtnColors[0].Metro = BrushFactory.ToColor(0xFFB6B7B2);

                    sBtnColors[1].Text = BrushFactory.ToColor(0xFF1B1814);
                    sBtnColors[1].TextDown = BrushFactory.ToColor(0xFF50493E);
                    sBtnColors[1].Grad1Top = BrushFactory.ToColor(0xFFEBE1D8);
                    sBtnColors[1].Grad1Bottom = BrushFactory.ToColor(0xFF25211A);
                    sBtnColors[1].Grad2Top = BrushFactory.ToColor(0xFFAFA49A);
                    sBtnColors[1].Grad2Bottom = BrushFactory.ToColor(0xFF60584A);
                    sBtnColors[1].DownColor = BrushFactory.ToColor(0xFF8B7E73);
                    sBtnColors[1].Metro = BrushFactory.ToColor(0xFF897F73);

                    sBtnColors[2].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[2].TextDown = BrushFactory.ToColor(0xFFA59E94);
                    sBtnColors[2].Grad1Top = BrushFactory.ToColor(0xFFC4C0BC);
                    sBtnColors[2].Grad1Bottom = BrushFactory.ToColor(0xFF221F19);
                    sBtnColors[2].Grad2Top = BrushFactory.ToColor(0xFF948980);
                    sBtnColors[2].Grad2Bottom = BrushFactory.ToColor(0xFF332E26);
                    sBtnColors[2].DownColor = BrushFactory.ToColor(0xFF7E7167);
                    sBtnColors[2].Metro = BrushFactory.ToColor(0xFF423C34);

                    sBtnColors[3].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[3].TextDown = BrushFactory.ToColor(0xFFBE7575);
                    sBtnColors[3].Grad1Top = BrushFactory.ToColor(0xFFD4A7A7);
                    sBtnColors[3].Grad1Bottom = BrushFactory.ToColor(0xFF221919);
                    sBtnColors[3].Grad2Top = BrushFactory.ToColor(0xFFCA605D);
                    sBtnColors[3].Grad2Bottom = BrushFactory.ToColor(0xFF8A0400);
                    sBtnColors[3].DownColor = BrushFactory.ToColor(0xFFA14644);
                    sBtnColors[3].Metro = BrushFactory.ToColor(0xFF941410);

                    sBtnColors[4].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[4].TextDown = BrushFactory.ToColor(0xFFD8B493);
                    sBtnColors[4].Grad1Top = BrushFactory.ToColor(0xFFEDD3A8);
                    sBtnColors[4].Grad1Bottom = BrushFactory.ToColor(0xFF251401);
                    sBtnColors[4].Grad2Top = BrushFactory.ToColor(0xFFD48641);
                    sBtnColors[4].Grad2Bottom = BrushFactory.ToColor(0xFFB25900);
                    sBtnColors[4].DownColor = BrushFactory.ToColor(0xFFB16A2D);
                    sBtnColors[4].Metro = BrushFactory.ToColor(0xFFBC6816);

                    sBtnColors[5].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[5].TextDown = BrushFactory.ToColor(0xFF8CA5D8);
                    sBtnColors[5].Grad1Top = BrushFactory.ToColor(0xFFAAC0E5);
                    sBtnColors[5].Grad1Bottom = BrushFactory.ToColor(0xFF101927);
                    sBtnColors[5].Grad2Top = BrushFactory.ToColor(0xFF6B91CF);
                    sBtnColors[5].Grad2Bottom = BrushFactory.ToColor(0xFF103D80);
                    sBtnColors[5].DownColor = BrushFactory.ToColor(0xFF4F70A5);
                    sBtnColors[5].Metro = BrushFactory.ToColor(0xFF1B478A);

                    break;
                case EnumCalcProSkinColor.Orange:
                    sBtnColors[0].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[0].TextDown = BrushFactory.ToColor(0xFF606060);
                    sBtnColors[0].Grad1Top = BrushFactory.ToColor(0xFFBCBCBC);
                    sBtnColors[0].Grad1Bottom = BrushFactory.ToColor(0xFF000000);
                    sBtnColors[0].Grad2Top = BrushFactory.ToColor(0xFF505050);
                    sBtnColors[0].Grad2Bottom = BrushFactory.ToColor(0xFF000000);
                    sBtnColors[0].DownColor = BrushFactory.ToColor(0xFF373737);
                    sBtnColors[0].Metro = BrushFactory.ToColor(0xFF000000);

                    sBtnColors[1].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[1].TextDown = BrushFactory.ToColor(0xFFF1C896);
                    sBtnColors[1].Grad1Top = BrushFactory.ToColor(0xFFFDBFA0);
                    sBtnColors[1].Grad1Bottom = BrushFactory.ToColor(0xFF4C2512);
                    sBtnColors[1].Grad2Top = BrushFactory.ToColor(0xFFD38C6E);
                    sBtnColors[1].Grad2Bottom = BrushFactory.ToColor(0xFF823F23);
                    sBtnColors[1].DownColor = BrushFactory.ToColor(0xFF9B5E44);
                    sBtnColors[1].Metro = BrushFactory.ToColor(0xFFA56044);

                    sBtnColors[2].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[2].TextDown = BrushFactory.ToColor(0xFFD8B493);
                    sBtnColors[2].Grad1Top = BrushFactory.ToColor(0xFFEDD3A8);
                    sBtnColors[2].Grad1Bottom = BrushFactory.ToColor(0xFF251401);
                    sBtnColors[2].Grad2Top = BrushFactory.ToColor(0xFFD48641);
                    sBtnColors[2].Grad2Bottom = BrushFactory.ToColor(0xFFB25900);
                    sBtnColors[2].DownColor = BrushFactory.ToColor(0xFFB16A2D);
                    sBtnColors[2].Metro = BrushFactory.ToColor(0xFFB55E07);

                    sBtnColors[3].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[3].TextDown = BrushFactory.ToColor(0xFFBE7575);
                    sBtnColors[3].Grad1Top = BrushFactory.ToColor(0xFFD4A7A7);
                    sBtnColors[3].Grad1Bottom = BrushFactory.ToColor(0xFF221919);
                    sBtnColors[3].Grad2Top = BrushFactory.ToColor(0xFFCA605D);
                    sBtnColors[3].Grad2Bottom = BrushFactory.ToColor(0xFF8A0400);
                    sBtnColors[3].DownColor = BrushFactory.ToColor(0xFFA14644);
                    sBtnColors[3].Metro = BrushFactory.ToColor(0xFF941410);

                    sBtnColors[4].Text = BrushFactory.ToColor(0xFF291500);
                    sBtnColors[4].TextDown = BrushFactory.ToColor(0xFF663400);
                    sBtnColors[4].Grad1Top = BrushFactory.ToColor(0xFFF3E7DB);
                    sBtnColors[4].Grad1Bottom = BrushFactory.ToColor(0xFF461A08);
                    sBtnColors[4].Grad2Top = BrushFactory.ToColor(0xFFF7AE9D);
                    sBtnColors[4].Grad2Bottom = BrushFactory.ToColor(0xFF8C2D08);
                    sBtnColors[4].DownColor = BrushFactory.ToColor(0xFFAD6858);
                    sBtnColors[4].Metro = BrushFactory.ToColor(0xFFED7E5A);

                    sBtnColors[5].Text = BrushFactory.ToColor(0xFF31261A);
                    sBtnColors[5].TextDown = BrushFactory.ToColor(0xFF504436);
                    sBtnColors[5].Grad1Top = BrushFactory.ToColor(0xFFFDEBBA);
                    sBtnColors[5].Grad1Bottom = BrushFactory.ToColor(0xFF90732E);
                    sBtnColors[5].Grad2Top = BrushFactory.ToColor(0xFFEDCC80);
                    sBtnColors[5].Grad2Bottom = BrushFactory.ToColor(0xFFB69846);
                    sBtnColors[5].DownColor = BrushFactory.ToColor(0xFFC49F4A);
                    sBtnColors[5].Metro = BrushFactory.ToColor(0xFFC3A455);

                    break;
				case EnumCalcProSkinColor.Green:
                    sBtnColors[0].Text = BrushFactory.ToColor(0xFF202525);
                    sBtnColors[0].TextDown = BrushFactory.ToColor(0xFF3A4247);
                    sBtnColors[0].Grad1Top = BrushFactory.ToColor(0xFFC2CBD0);
                    sBtnColors[0].Grad1Bottom = BrushFactory.ToColor(0xFF333537);
                    sBtnColors[0].Grad2Top = BrushFactory.ToColor(0xFFE7E7E7);
                    sBtnColors[0].Grad2Bottom = BrushFactory.ToColor(0xFF8C8E84);
                    sBtnColors[0].DownColor = BrushFactory.ToColor(0xFFA9ADAF);
                    sBtnColors[0].Metro = BrushFactory.ToColor(0xFFB2B3AD);

                    sBtnColors[1].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[1].TextDown = BrushFactory.ToColor(0xFFCBE9AD);
                    sBtnColors[1].Grad1Top = BrushFactory.ToColor(0xFFCEE7B5);
                    sBtnColors[1].Grad1Bottom = BrushFactory.ToColor(0xFF314D10);
                    sBtnColors[1].Grad2Top = BrushFactory.ToColor(0xFFBDE39C);
                    sBtnColors[1].Grad2Bottom = BrushFactory.ToColor(0xFF73A242);
                    sBtnColors[1].DownColor = BrushFactory.ToColor(0xFF7A955D);
                    sBtnColors[1].Metro = BrushFactory.ToColor(0xFF84B057);

                    sBtnColors[2].Text = BrushFactory.ToColor(0xFF1D2F04);
                    sBtnColors[2].TextDown = BrushFactory.ToColor(0xFF405A1B);
                    sBtnColors[2].Grad1Top = BrushFactory.ToColor(0xFFBACF86);
                    sBtnColors[2].Grad1Bottom = BrushFactory.ToColor(0xFF2D4332);
                    sBtnColors[2].Grad2Top = BrushFactory.ToColor(0xFF90B25A);
                    sBtnColors[2].Grad2Bottom = BrushFactory.ToColor(0xFF5D8134);
                    sBtnColors[2].DownColor = BrushFactory.ToColor(0xFF69833F);
                    sBtnColors[2].Metro = BrushFactory.ToColor(0xFF65883A);

                    sBtnColors[3].Text = BrushFactory.ToColor(0xFF2F2F2A);
                    sBtnColors[3].TextDown = BrushFactory.ToColor(0xFF6C693F);
                    sBtnColors[3].Grad1Top = BrushFactory.ToColor(0xFFFFFDE1);
                    sBtnColors[3].Grad1Bottom = BrushFactory.ToColor(0xFF29280F);
                    sBtnColors[3].Grad2Top = BrushFactory.ToColor(0xFFE4DFA4);
                    sBtnColors[3].Grad2Bottom = BrushFactory.ToColor(0xFFA59E10);
                    sBtnColors[3].DownColor = BrushFactory.ToColor(0xFFABA566);
                    sBtnColors[3].Metro = BrushFactory.ToColor(0xFFBAB444);

                    sBtnColors[4].Text = BrushFactory.ToColor(0xFF1B1814);
                    sBtnColors[4].TextDown = BrushFactory.ToColor(0xFF50493E);
                    sBtnColors[4].Grad1Top = BrushFactory.ToColor(0xFFEBE1D8);
                    sBtnColors[4].Grad1Bottom = BrushFactory.ToColor(0xFF25211A);
                    sBtnColors[4].Grad2Top = BrushFactory.ToColor(0xFFAFA49A);
                    sBtnColors[4].Grad2Bottom = BrushFactory.ToColor(0xFF60584A);
                    sBtnColors[4].DownColor = BrushFactory.ToColor(0xFF8B7E73);
                    sBtnColors[4].Metro = BrushFactory.ToColor(0xFF746B5E);

                    sBtnColors[5].Text = BrushFactory.ToColor(0xFFFFFFFF);
                    sBtnColors[5].TextDown = BrushFactory.ToColor(0xFFB0C580);
                    sBtnColors[5].Grad1Top = BrushFactory.ToColor(0xFFC4DE82);
                    sBtnColors[5].Grad1Bottom = BrushFactory.ToColor(0xFF0F1F0D);
                    sBtnColors[5].Grad2Top = BrushFactory.ToColor(0xFFA5BA60);
                    sBtnColors[5].Grad2Bottom = BrushFactory.ToColor(0xFF314915);
                    sBtnColors[5].DownColor = BrushFactory.ToColor(0xFF6A7C31);
                    sBtnColors[5].Metro = BrushFactory.ToColor(0xFF31502C);

                    break;
             //   default:
            //      return false;
            }
            return true;
        }

                       
        virtual public bool CreateButtonSkin(ref Grid LayoutRoot, ref TextBlock MainText, ref TextBlock SubText, ref TextBlock SecondText, ref TextBlock SecExpText, EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eStyle, ref int iSkinIndex)
        {
            if (eSkinColor != eCurrentColor)//only need to load in the colors 
            {
                if (LoadSkin(eSkinColor) == false)
                    return false;
            }

            /****************************************************/
            Border = new Border();
            Border.Margin = new Thickness(Globals.MARGIN);
            Border.CornerRadius = new CornerRadius(6);
            Border.Background = Globals.Brush.GetGradientBrush(sBtnColors[iSkinIndex].Grad1Top, sBtnColors[iSkinIndex].Grad1Bottom);
            /****************************************************/
            Border2 = new Border();
            Border2.Margin = new Thickness(Globals.MARGIN+2);
            Border2.CornerRadius = new CornerRadius(6);
            Border2.Background = Globals.Brush.GetGradientBrush(sBtnColors[iSkinIndex].Grad2Top, sBtnColors[iSkinIndex].Grad2Bottom);
            /*****************************************************/

            LayoutRoot.Children.Add(Border);
            LayoutRoot.Children.Add(Border2);

            MainText.Foreground = Globals.Brush.GetBrush(sBtnColors[iSkinIndex].Text);
            SubText.Foreground = Globals.Brush.GetBrush(sBtnColors[iSkinIndex].Text);

            SecondText.Foreground = Globals.Brush.GetBrush(sBtnColors[iSkinIndex].Text);
            SecExpText.Foreground = Globals.Brush.GetBrush(sBtnColors[iSkinIndex].Text);

            return true;
        }

        virtual public void DrawDownState(ref Grid LayoutRoot, ref TextBlock MainText, ref TextBlock SubText, ref TextBlock SecondText, ref TextBlock SecExpText, EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eStyle, ref int iSkinIndex)
        {
            // save the down state
            _sDownClassic = new TypeDownClassic();
            _sDownClassic.brBorder = Border.Background;
            _sDownClassic.brBorder2 = Border2.Background;
            _sDownClassic.brText = MainText.Foreground;

            GradientBrush br = Border.Background as GradientBrush;
            GradientBrush br2 = Border2.Background as GradientBrush;

            Border.Background = Globals.Brush.GetGradientBrush(br.GradientStops[1].Color, br.GradientStops[0].Color);

            MainText.Foreground = SubText.Foreground = SecondText.Foreground = SecExpText.Foreground = Globals.Brush.GetBrush(sBtnColors[iSkinIndex].TextDown);
            Border2.Background = Globals.Brush.GetGradientBrush(sBtnColors[iSkinIndex].DownColor, br2.GradientStops[1].Color);
        }

        virtual public bool DrawUpState(ref Grid LayoutRoot, ref TextBlock MainText, ref TextBlock SubText, ref TextBlock SecondText, ref TextBlock SecExpText, EnumCalcProSkinColor eSkinColor, EnumCalcProSkinStyle eStyle, ref int iSkinIndex)
        {
            if (_sDownClassic == null)
                return false;

            Border.Background = _sDownClassic.brBorder;
            Border2.Background = _sDownClassic.brBorder2;
            MainText.Foreground = _sDownClassic.brText;
            SubText.Foreground = _sDownClassic.brText;
            SecondText.Foreground = _sDownClassic.brText;
            SecExpText.Foreground = _sDownClassic.brText;

            _sDownClassic = null;

            return true;
        }
    }
}
