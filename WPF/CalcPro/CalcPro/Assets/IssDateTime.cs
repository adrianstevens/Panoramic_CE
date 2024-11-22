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

//namespace CalcPro.CalcEngine
//{
    public enum EnumInterval
    {
        day,
        hour,
        min,
        sec,
        count,
    }

    public class IssDateTime
    {
        public static int NUM_SEC_PER_YEAR = 31536000;
        public static int NUM_SEC_PER_DAY = 86400;
        public static int NUM_SEC_PER_HOUR = 3600;
        public static int NUM_SEC_PER_MIN = 60;

        DateTime sStartDate;
        DateTime sEndDate;
        TimeSpan sInterval;

        public IssDateTime()
        {
            ClearAll();

            CalcInterval();
        }

        public bool SetStartDate(ref DateTime newTime)
        {
            sStartDate = newTime;
            TimeSpan newSpan = new TimeSpan(0, 0, sStartDate.Second);
            sStartDate = sStartDate.Subtract(newSpan);

            return true;
        }

        public bool SetEndDate(ref DateTime newTime)
        {
            sEndDate = newTime;
            TimeSpan newSpan = new TimeSpan(0, 0, sEndDate.Second);
            sEndDate = sEndDate.Subtract(newSpan);
            return true;
        }

        public bool SetInterval(ref TimeSpan newTime)
        {
            sInterval = newTime;
            return true;
        }

        public bool GetStartDate(ref DateTime startTime)
        {
            CalcStartDate();
            startTime = sStartDate;
            return true;
        }

        public bool GetEndDate(ref DateTime endTime)
        {
            CalcEndDate();
            endTime = sEndDate;
            return true;
        }


        public bool GetInterval(ref TimeSpan intTime)
        {
            CalcInterval();
            intTime = sInterval;
            return true;
        }

        bool ClearStartDate()
        {
            sStartDate = System.DateTime.Now;
            TimeSpan newSpan = new TimeSpan(0, 0, sStartDate.Second);

            sStartDate = sStartDate.Subtract(newSpan);
            return true;
        }

        bool ClearEndDate()
        {
            sEndDate = System.DateTime.Now;
            TimeSpan newSpan = new TimeSpan(0, 0, sEndDate.Second);
            
            sEndDate = sEndDate.Subtract(newSpan);
            return true;
        }

        bool ClearInterval()
        {//we'll set this one to all 0s 
            sInterval = new TimeSpan(0, 0, 0);
            return true;
        }

        public bool ClearAll()
        {
            ClearStartDate();
            ClearEndDate();
            ClearInterval();

            return true;
        }

        bool CalcStartDate()
        {
            ClearStartDate();

            try 
            {
                sStartDate = sEndDate.Subtract(sInterval);
                return true;
            }
            catch
            {
                ClearAll();
                return false;
            }

        }

        bool CalcEndDate()
        {
            ClearEndDate();
            try
            {
                sEndDate = sStartDate.Add(sInterval);
                return true;
            }
            catch
            {
                ClearAll();
                return false;
            }
        }

        bool CalcInterval()
        {
            ClearInterval();

            try
            {
                sInterval = sEndDate.Subtract(sStartDate);
                return true;
            }
            catch
            {
                ClearAll();
                return false;
            }
        }

        //probably should have passed the values in to make this code more re-usable but oh well
        bool IsEndAfterStart()
        {
            long iStart, iEnd;

            iStart = sStartDate.ToFileTimeUtc();
            iEnd = sEndDate.ToFileTimeUtc();

            if (iStart > iEnd)
                return false;

            //if they're equal I'm still returning true
            return true;
        }

        bool IsLeapYear(int iYear)
        {
            //I'll assume the C# is as good if not better ...
            return System.DateTime.IsLeapYear(iYear);
            
            /*
            //Leap Years - every 4 years except for years dividable by 100 ... 
            //unless they're also divisible by 400 ... in which case they are

            //gregorian style
            if ((iYear) % 400 == 0)
                return true;

            if ((iYear) % 100 == 0)
                return false;

            if ((iYear % 4) == 0)
                return true;

            return false;*/
        }

        int GetDaysInYear(int iYear)
        {
            int iDays = 365;

            //pretty sure there's another special case 
            if (IsLeapYear(iYear))
                iDays++;

            return iDays;
        }

        public int GetCorrectedValue(int Days, int Hours, int Min, int Sec, EnumInterval eInt)
        {
            while (Sec > 59)
            {
                Sec -= 60;
                Min++;
            }

            while (Min > 59)
            {
                Min -= 60;
                Hours++;
            }

            while(Hours > 23)
            {
                Hours -= 24;
                Days++;
            }

            switch (eInt)
            {
                case EnumInterval.day:
                    return Days;
                case EnumInterval.hour:
                    return Hours;
                case EnumInterval.min:
                    return Min;
                case EnumInterval.sec:
                    return Sec;
            }
            return 0;
        }
    }
//}
