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
using System.Windows.Threading;
using System.Windows.Resources;
using System.IO.IsolatedStorage;

using System.Resources;
using System.Globalization;
using System.Reflection;
using System.Threading;
using System.ComponentModel;

namespace CalcPro
{
    public partial class CurrencyConverter : UserControl, ICalcProScreen
	{
        private ObjCurrency _oCurrency = new ObjCurrency();
        private ICalcProBtn _btnFrom = null;
        private ICalcProBtn _btnTo = null;
        private ICalcProBtn _btnUpdate = null;
        private ICalcProDisplay _dispFrom = null;
        private ICalcProDisplay _dispTo = null;
        private ICalcProDisplay _dispUpdate = null;

		public CurrencyConverter()
		{
			// Required to initialize variables
			InitializeComponent();
			
            this.LayoutRoot.DataContext = _oCurrency;

		}

        private void SetFlag(string szAbv, int iFlag)
        {
            ImageBrush brush = new ImageBrush();
            brush.ImageSource = new BitmapImage(new Uri(@"/Assets/Flags/" + szAbv + ".jpg", UriKind.Relative));
            brush.Stretch = Stretch.None;

            if (iFlag == 1)
                gridFlag1.Background = brush;
            else
                gridFlag2.Background = brush;
        }



        public void CloseSaveState()
        {
            if (!IsScreenLoaded())
                return;

            _oCurrency.SaveFavorites();
        }
        public bool IsScreenLoaded()
        {
            return gridMain.Children.Count > 0;
        }

        void SetFontColors()
        {
            txtTo.Foreground = Globals.btnMan.brMainTextColor;
            txtFrom.Foreground = Globals.btnMan.brMainTextColor;
            txtToSym.Foreground = Globals.btnMan.brAltTextColor;
            txtFromSym.Foreground = Globals.btnMan.brAltTextColor;
            txtToBid.Foreground = Globals.btnMan.brAltTextColor;
            txtFromBid.Foreground = Globals.btnMan.brAltTextColor;
            txtUpdate.Foreground = Globals.btnMan.brMainTextColor;
            txtUpdateStatus.Foreground = Globals.btnMan.brAltTextColor;

            // set the font too
            Globals.btnMan.SetFontItem(ref txtTo);
            Globals.btnMan.SetFontItem(ref txtFrom);
            Globals.btnMan.SetFontItem(ref txtToSym);
            Globals.btnMan.SetFontItem(ref txtFromSym);
            Globals.btnMan.SetFontItem(ref txtToBid);
            Globals.btnMan.SetFontItem(ref txtFromBid);
            Globals.btnMan.SetFontItem(ref txtUpdate);
            Globals.btnMan.SetFontItem(ref txtUpdateStatus);
        }

        public void LoadScreen(bool bOnScreen)
        {
            if (!_oCurrency.IsInitialized())
                _oCurrency.Initialize();

            if (bOnScreen == true)
            {
                //subscribe
                Globals.Calc.DisplayUpdate += UpdateDisplay;
                //Globals.Calc.StateUpdate += UpdateCalcState;

                //create an image brush
                SetFlag(_oCurrency.szFromSym, 1);
                SetFlag(_oCurrency.szToSym, 2);
            }
            else
            {
                //unsubscribe
                Globals.Calc.DisplayUpdate -= UpdateDisplay;
            }

            if (IsScreenLoaded())
                return;

            SetFontColors();

            //txtFromBid.Visibility = Visibility.Collapsed;



            Globals.btnMan.SetCalcType(EnumCalcType.CALC_Currency);
            Globals.btnMan.CreateMainButtons(ref  gridMain);

            Globals.btnMan.CreateButton(ref _btnFrom, 4);
            Globals.btnMan.CreateButton(ref _btnTo, 4);
            Globals.btnMan.CreateButton(ref _btnUpdate, 1);

            UserControl uCon;//just a reference

            //FROM

            //cast to a user control
            uCon = (UserControl)_btnFrom;
            uCon.SetValue(FrameworkElement.NameProperty, "From");

            _btnFrom.SetMainText(Globals.rm.GetString("IDS_BTN_From"));
            _btnFrom.SetExpText("");
            

            _btnFrom.BtnSelected += delegate(object MySender, EventArgs eArg)
            {
                LaunchMenu(true);
            };

            Grid.SetColumn(uCon, 4);
            Grid.SetRow(uCon, 0);

            gridBtnFrom.Children.Add(uCon);


            //TO
            //cast to a user control
            uCon = (UserControl)_btnTo;

            uCon.SetValue(FrameworkElement.NameProperty, "To");

            _btnTo.SetMainText(Globals.rm.GetString("IDS_BTN_To"));
            _btnTo.SetExpText("");

            _btnTo.BtnSelected += delegate(object MySender, EventArgs eArg)
            {
                LaunchMenu(false);
            };

            gridBtnTo.Children.Add(uCon);

            //UPDATE
            //cast to a user control
            uCon = (UserControl)_btnUpdate;

            uCon.SetValue(FrameworkElement.NameProperty, "Update");

            _btnUpdate.SetMainText(Globals.rm.GetString("IDS_BTN_Update"));
            _btnUpdate.SetExpText("");

            _btnUpdate.BtnSelected += delegate(object MySender, EventArgs eArg)
            {
                OnBtnUpdate();
            };

            gridBtnUpdate.Children.Add(uCon);

            _dispFrom = Globals.btnMan.CreateDisplay(ref gridDisplayFrom, "DisplayFrom", EnumCalcDisplay.Currency);
            _dispTo = Globals.btnMan.CreateDisplay(ref gridDisplayTo, "DisplayTo", EnumCalcDisplay.Currency);
            _dispUpdate = Globals.btnMan.CreateDisplay(ref gridDisplayUpdate, "DisplayUpdate", EnumCalcDisplay.Currency);

            // now display the text
            txtTo.Visibility = Visibility.Visible;
            txtFrom.Visibility = Visibility.Visible;
            txtToSym.Visibility = Visibility.Visible;
            txtFromSym.Visibility = Visibility.Visible;
            txtToBid.Visibility = Visibility.Visible;
            txtUpdateStatus.Visibility = Visibility.Visible;
            txtUpdate.Visibility = Visibility.Visible;

        }
        public void UpdateDisplay(bool bUpdate, int iBtnIndex, int iBtnId)
        {
            _oCurrency.UpdateItems();
        }
        public void UpdateSkin(Color crBackground, EnumCalcProSkinColor eColor, EnumCalcProSkinStyle eStyle)
        {
            SetFontColors();
        }
        public void OnBtnUpdate()
        {
            if (_oCurrency.eCurrencyUpdate != EnumUpdateCurrency.Updated)
                return;

            // update the UI
            _oCurrency.eCurrencyUpdate = EnumUpdateCurrency.Updating;

            WebClient web = new WebClient();

            web.DownloadProgressChanged += new DownloadProgressChangedEventHandler(webClient_DownloadProgressChanged);
            web.OpenReadCompleted += new OpenReadCompletedEventHandler(webClient_OpenReadCompleted);

            CultureInfo c = System.Threading.Thread.CurrentThread.CurrentUICulture;

            string szURL = "http://www.panoramicsoft.com/mobileapps/calcpro/currency";

            switch (c.TwoLetterISOLanguageName.ToLower())
            {
                case "ja":
                    szURL += "_ja";
                    break;
                case "fr":
                    szURL += "_fr";
                    break;
                case "de":
                    szURL += "_de";
                    break;
                case "es":
                    szURL += "_es";
                    break;
                case "pt":
                    szURL += "_pt";
                    break;
                default:
                    break;
            }

            szURL += "7.ini";

            web.OpenReadAsync(new Uri(szURL));
        }

        private void webClient_DownloadProgressChanged(object sender, DownloadProgressChangedEventArgs e)
        {
            //this.textBlock3.Text = "Downloading " + e.ProgressPercentage + "%";
        }

        private void webClient_OpenReadCompleted(object sender, OpenReadCompletedEventArgs e)
        {
            if (e.Error != null)
            {
                // update the UI
                _oCurrency.eCurrencyUpdate = EnumUpdateCurrency.ErrorUpdate;

                DispatcherTimer d = new DispatcherTimer();
                d.Interval = new TimeSpan(0, 0, 2); // 2 seconds
                d.Tick += new EventHandler(ResetUpdateTimer);
                d.Start();
                return;
            }

            /*

            // the zipped stream
            StreamResourceInfo zipFile = new StreamResourceInfo(e.Result, null);

            // the unzipped file
            StreamResourceInfo iniFile = Application.GetResourceStream(zipFile, new Uri(ObjCurrency.FILE_Currency, UriKind.Relative));

            // save it to file
            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
                if (isf.FileExists(ObjCurrency.FILE_Currency))
                    isf.DeleteFile(ObjCurrency.FILE_Currency);

                using (IsolatedStorageFileStream newFS = isf.CreateFile(ObjCurrency.FILE_Currency))
                {
                    byte[] buffer = new byte[4096];
                    int bytesread;

                    while ((bytesread = iniFile.Stream.Read(buffer, 0, buffer.Length)) != 0)
                    {
                        newFS.Write(buffer, 0, bytesread);
                    }

                    newFS.Close();
                }
            }

            // reinitialize
            _oCurrency.Initialize();
            */
        }

        private void ResetUpdateTimer(object o, EventArgs sender)
        {
            //button3.IsEnabled = true;
            DispatcherTimer d = (DispatcherTimer)o;
            d.Stop(); // kill the timer

            _oCurrency.eCurrencyUpdate = EnumUpdateCurrency.Updated;
        }

        private void LayoutRoot_Loaded(object sender, RoutedEventArgs e)
        {
            
        }
        private void LaunchMenu(bool bFrom)
        {
        /*    PopupMenu pop = new PopupMenu((FrameworkElement)(bFrom ? _btnFrom : _btnTo), Globals.rm.GetString(bFrom ? "IDS_BTN_From" : "IDS_BTN_To"));

            for (int i = 0; i < _oCurrency.iFavoriteCount; i++)
            {
                pop.AddItem(_oCurrency.GetFavItem(i));
            }

            pop.AddSeparator();

            for (int i = 0; i < _oCurrency.iCurrencyCount; i++)
            {
                pop.AddItem(_oCurrency.GetCurrencyItem(i));
            }

            int iIndex = _oCurrency.FindFavIndex(bFrom ? _oCurrency.iFromIndex : _oCurrency.iToIndex);
            pop.SetCurrentIndex(iIndex);
            pop.Launch();

            pop.SelectionChanged += delegate(object MySender, PopupMenuEventArgs eArg)
            {
                if (bFrom)
                {
                    _oCurrency.SetFromIndex(eArg.iSelected);
                    SetFlag(_oCurrency.szFromSym, 1);

                }
                else
                {
                    _oCurrency.SetToIndex(eArg.iSelected);
                    SetFlag(_oCurrency.szToSym, 2);

                }
            };
            */
        }

        public void OnBackButton(object sender, CancelEventArgs e)
        {

        }

        private void gridDisplayFrom_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {

        }

        private void gridDisplayFrom_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            _oCurrency.Switch();
        }

        private void gridDisplayTo_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            _oCurrency.Switch();
        }

        private void gridDisplayUpdate_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {

        }
	}
}