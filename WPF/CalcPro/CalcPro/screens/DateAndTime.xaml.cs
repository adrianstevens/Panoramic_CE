using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.ComponentModel;
using Microsoft.Windows.Controls;

namespace CalcPro
{
    public partial class DateAndTime : UserControl, ICalcProScreen
	{
        IssDateTime oDateTime = new IssDateTime();

        ICalcProBtn[] btnCalc = new ICalcProBtn[3];

        EnumInterval eInterval = EnumInterval.count; // so we know what's selected

        public DateAndTime()
		{
			// Required to initialize variables
			InitializeComponent();

            Globals.btnMan.SkinUpdate += UpdateSkin;
		}

        public void CloseSaveState()
        {
            if (!IsScreenLoaded())
                return;
        }
        public bool IsScreenLoaded()
        {
            if(gridMain.Children.Count > 0)
                return true;
            return false;
        }

        public void LoadScreen(bool bOnScreen)
        {
            if (bOnScreen == true)
            {
                //subscribe
                //Globals.Calc.DisplayUpdate += UpdateDisplay;
                //Globals.Calc.StateUpdate += UpdateCalcState;
            }
            else
            {
                //unsubscribe
                //Globals.Calc.DisplayUpdate -= UpdateDisplay;
            }
            if (IsScreenLoaded())
                return;

            Globals.btnMan.SetCalcType(EnumCalcType.CALC_DateTime);
            Globals.btnMan.CreateMainButtons(ref  gridMain);

            InitButtons();

            //and set the additional text fields
            SetAdditionalText();

            //localized strings please
            textBlockD.Text = Globals.rm.GetString("IDS_DATE_Days");
            textBlockH.Text = Globals.rm.GetString("IDS_DATE_Hours");
            textBlockM.Text = Globals.rm.GetString("IDS_DATE_Min");
            textBlockS.Text = Globals.rm.GetString("IDS_DATE_Sec");

            textFrom.Text = Globals.rm.GetString("IDS_DATE_Start");
            textDiff.Text = Globals.rm.GetString("IDS_DATE_Interval");
            textTo.Text = Globals.rm.GetString("IDS_DATE_End");

            
        }

        void SetAdditionalText()
        {
//            textBlockDOWFrom.Text = ((DateTime)DateFrom.Value).DayOfWeek.ToString();
//            textBlockDOWTo.Text = ((DateTime)DateTo.Value).DayOfWeek.ToString();


        }

        void OnReset()
        {

            oDateTime.ClearAll();

          //  DateFrom.Value = System.DateTime.Now;
          //  DateTo.Value = System.DateTime.Now;
          //  TimeFrom.Value = System.DateTime.Now;
          //  TimeTo.Value = System.DateTime.Now;
            textBlockDays.Text = "0";
            textBlockHours.Text = "0";
            textBlockMinutes.Text = "0";
            textBlockSeconds.Text = "0";

            SetAdditionalText();
        }

        private void OnMainButton(object sender, int iIndex)
        {
            TextBlock text = null;

            switch (eInterval)
            {
                case EnumInterval.day:
                    text = textBlockDays;
                    break;
                case EnumInterval.hour:
                    text = textBlockHours;
                    break;
                case EnumInterval.min:
                    text = textBlockMinutes;
                    break;
                case EnumInterval.sec:
                    text = textBlockSeconds;
                    break;
            }

            //buton conversion
            if (iIndex == 0)
                iIndex = 7;
            else if (iIndex == 1)
                iIndex = 4;
            else if (iIndex == 2)
                iIndex = 1;
            else if (iIndex == 3)
                iIndex = 0;

            else if (iIndex == 4)
                iIndex = 8;
            else if (iIndex == 5)
                iIndex = 5;
            else if (iIndex == 6)
                iIndex = 2;
            else if (iIndex == 7)
                iIndex = 10;

            else if (iIndex == 8)
                iIndex = 9;
            else if (iIndex == 9)
                iIndex = 6;
            else if (iIndex == 10)
                iIndex = 3;
            //11 is fine



            switch (iIndex)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                    if (text != null && text.Text.Length < 6)
                    {
                        if(text.Text == "0")
                            text.Text = iIndex.ToString();
                        else
                            text.Text += iIndex.ToString();
                    }
                    break;

                case 10:
                    OnReset();
                    break;

                case 11:
                    if (text != null && text.Text.Length > 0)
                    {
                        text.Text = text.Text.Substring(0, text.Text.Length - 1);
                    }
                    break;

            }



            if (iIndex == 10) //reset button
            {
                OnReset();
                
            }



        }

        void InitButtons()
        {
            UserControl uCon;//just a reference
            ICalcProBtn iBtn;

            //first things first we're going to take control of the main buttons
            for (int i = 0; i < gridMain.Children.Count; i++)
            {
                iBtn = (ICalcProBtn)gridMain.Children[i];

                int iIndex = i;

                iBtn.BtnSelected = delegate(object MySender, EventArgs eArg)
                {
                    OnMainButton(iBtn, iIndex);
                };
            }



            double dbMargin = Globals.MARGIN;

            

            string szTemp;

            for (int i = 0; i < 3; i++)
            {
                Globals.btnMan.CreateButton(ref btnCalc[i], 1);

                szTemp = "Calc" + i.ToString();

                //cast to a user control
                uCon = (UserControl)btnCalc[i];

                uCon.SetValue(FrameworkElement.NameProperty, szTemp);

                btnCalc[i].cBtn.iBtnIndex = 0;
                btnCalc[i].cBtn.iBtnType = 0;

                btnCalc[i].SetMainText("Calc");
                btnCalc[i].SetExpText("");

                int iTemp = i;

                btnCalc[i].BtnSelected += delegate(object MySender, EventArgs eArg)
                {
                    ClickButton(btnCalc[iTemp], iTemp);
                };

                Grid.SetColumn(uCon, 0);

                if (i == 0)
                {
                    uCon.Margin = new Thickness(3, 6, 0, -6);
                    Grid.SetRow(uCon, 1);
                }
                else if (i == 1)
                {
                    uCon.Margin = new Thickness(3, 0, 0, 0);
                    Grid.SetRow(uCon, 4);
                }
                else
                {
                    uCon.Margin = new Thickness(3, 6, 0, -6);
                    Grid.SetRow(uCon, 7);
                }

                gridTop.Children.Add(uCon);
            }
        }


        public void UpdateSkin(Color crBackground, EnumCalcProSkinColor eColor, EnumCalcProSkinStyle eStyle)
        {
            gridMain.Children.Clear();
        }

        private void ClickButton(ICalcProBtn sender, int iIndex)
        {
        //    UserControl uCon = (UserControl)sender;

            SetTextColors(10000);//anything other than 0-3 

            switch (iIndex)
            {
                case 0:
                    OnButtonFrom();
                    break;
                case 1:
                    OnButtonInt();
                    break;
                case 2:
                default:
                    OnButtonTo();
                    break;

            }
            
        }

        void OnButtonInt()
        {
            TimeSpan span = new TimeSpan();

            DateTime date = new DateTime();// = (DateTime)DateFrom.Value;
            DateTime time = new DateTime();// = (DateTime)TimeFrom.Value;
            date = new DateTime(date.Year, date.Month, date.Day, time.Hour, time.Minute, time.Second);
            oDateTime.SetStartDate(ref date);

            //date = (DateTime)DateTo.Value;
            //time = (DateTime)TimeTo.Value;
            date = new DateTime(date.Year, date.Month, date.Day, time.Hour, time.Minute, time.Second);
            oDateTime.SetEndDate(ref date);
            
            oDateTime.GetInterval(ref span);

            //and set the text blocks
            textBlockDays.Text = span.Days.ToString();
            textBlockHours.Text = span.Hours.ToString();
            textBlockMinutes.Text = span.Minutes.ToString();
            textBlockSeconds.Text = span.Seconds.ToString();

        }

        void OnButtonTo()
        {
            DateTime date = new DateTime();// (DateTime)DateFrom.Value;
            DateTime time = new DateTime(); //(DateTime)TimeFrom.Value;
            date = new DateTime(date.Year, date.Month, date.Day, time.Hour, time.Minute, time.Second);
            oDateTime.SetStartDate(ref date);

            int Days = Convert.ToInt32(textBlockDays.Text);
            int Hours = Convert.ToInt32(textBlockHours.Text);
            int Min = Convert.ToInt32(textBlockMinutes.Text);
            int Sec = Convert.ToInt32(textBlockSeconds.Text);

            TimeSpan span = new TimeSpan( oDateTime.GetCorrectedValue(Days, Hours, Min, Sec, EnumInterval.day),  oDateTime.GetCorrectedValue(Days, Hours, Min, Sec, EnumInterval.hour),  oDateTime.GetCorrectedValue(Days, Hours, Min, Sec, EnumInterval.min),  oDateTime.GetCorrectedValue(Days, Hours, Min, Sec, EnumInterval.sec));
            oDateTime.SetInterval(ref span);
            
            oDateTime.GetEndDate(ref date);
            //DateTo.Value = date;
            //TimeTo.Value = date;

            SetAdditionalText();

        }

        void OnButtonFrom()
        {
            DateTime date = new DateTime();// (DateTime)DateTo.Value;
            DateTime time = new DateTime();// (DateTime)TimeTo.Value;
            date = new DateTime(date.Year, date.Month, date.Day, time.Hour, time.Minute, time.Second);
            oDateTime.SetEndDate(ref date);

            int Days = Convert.ToInt32(textBlockDays.Text);
            int Hours = Convert.ToInt32(textBlockHours.Text);
            int Min = Convert.ToInt32(textBlockMinutes.Text);
            int Sec = Convert.ToInt32(textBlockSeconds.Text);

            TimeSpan span = new TimeSpan(oDateTime.GetCorrectedValue(Days, Hours, Min, Sec, EnumInterval.day), oDateTime.GetCorrectedValue(Days, Hours, Min, Sec, EnumInterval.hour), oDateTime.GetCorrectedValue(Days, Hours, Min, Sec, EnumInterval.min), oDateTime.GetCorrectedValue(Days, Hours, Min, Sec, EnumInterval.sec));
            oDateTime.SetInterval(ref span);

            oDateTime.GetStartDate(ref date);
            //DateFrom.Value = date;
            //TimeFrom.Value = date;

            SetAdditionalText();
        }

        void SetTextColors(int iSelected)
        {
            eInterval = (EnumInterval)iSelected;

            if (iSelected == (int)EnumInterval.day)
                textBlockDays.Foreground = Globals.Brush.GetBrush(Color.FromArgb(255,0,100,255));
            else
                textBlockDays.Foreground = Globals.Brush.GetBrush(Colors.Black);

            if (iSelected == (int)EnumInterval.hour)
                textBlockHours.Foreground = Globals.Brush.GetBrush(Color.FromArgb(255, 0, 100, 255));
            else
                textBlockHours.Foreground = Globals.Brush.GetBrush(Colors.Black);

            if (iSelected == (int)EnumInterval.min)
                textBlockMinutes.Foreground = Globals.Brush.GetBrush(Color.FromArgb(255, 0, 100, 255));
            else
                textBlockMinutes.Foreground = Globals.Brush.GetBrush(Colors.Black);

            if (iSelected == (int)EnumInterval.sec)
                textBlockSeconds.Foreground = Globals.Brush.GetBrush(Color.FromArgb(255, 0, 100, 255));
            else
                textBlockSeconds.Foreground = Globals.Brush.GetBrush(Colors.Black);
        }

        public void OnBackButton(object sender, CancelEventArgs e)
        {

        }

        private void textBlockDays_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            SetTextColors(0);
        }

        private void textBlockHours_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            SetTextColors(1);
        }

        private void textBlockMinutes_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            SetTextColors(2);
        }

        private void textBlockSeconds_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            SetTextColors(3);
        }

    /*    private void DateFrom_ValueChanged(object sender, DateTimeValueChangedEventArgs e)
        {
            SetAdditionalText();
        }

        private void DateTo_ValueChanged(object sender, DateTimeValueChangedEventArgs e)
        {
            SetAdditionalText();
        } */
	}
}