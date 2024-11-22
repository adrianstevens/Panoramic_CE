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
using System.IO;
using System.IO.IsolatedStorage;
using System.Text;
using System.Collections.Generic;
using System.ComponentModel;
using System.Resources;
using System.Reflection;
using System.Xml.Serialization;


namespace CalcPro
{
    public class TypeCurrency
    {
        public string szName { get; set; }
        public string szAbvName { get; set; }
        public string szSymbol { get; set; }
        public double dbExchangeRate { get; set; }
        public double dbBid { get; set; }
        public double dbAsk { get; set; }
    }
    public enum EnumUpdateCurrency
    {
        Updated,
        Updating,
        ErrorUpdate,
    }
    public class ObjCurrency: INotifyPropertyChanged
    {
        public const int NUM_Favs = 4;

        public const string FILE_Currency = "currency-nozip.ini";
        public event PropertyChangedEventHandler PropertyChanged;
        private List<TypeCurrency> _arrItems = new List<TypeCurrency>();
        private List<int> _arrFavs = new List<int>();
        
               
        private DateTime _dtUpdate;
        private EnumUpdateCurrency _eCurrencyUpdate = EnumUpdateCurrency.Updated;
        
        private const string FILE_Favorites = "FavCurrency.txt";
        private const string FILE_FromIndex = "FromIndex.txt";
        private const string FILE_ToIndex   = "ToIndex.txt";

        public ObjCurrency()
        {

        }

        public EnumUpdateCurrency eCurrencyUpdate
        {
            get { return _eCurrencyUpdate; }
            set
            {
                _eCurrencyUpdate = value;
                NotifyPropertyChanged("szLastUpdateTime");
                NotifyPropertyChanged("szLastUpdateDate");
            }
        }
        public int iFromIndex
        {
            get { return _iCurrentFrom; }
            set
            {
                _iCurrentFrom = value;
                UpdateItems();
            }
        }
        private int _iCurrentFrom = 0;

        public int iToIndex
        {
            get { return _iCurrentTo; }
            set
            {
                _iCurrentTo = value;
                UpdateItems();//just easier
            }
        }
        private int _iCurrentTo = 1;

        public int iCurrencyCount
        {
            get { return _arrItems.Count; }
        }
        public int iFavoriteCount
        {
            get { return _arrFavs.Count; }
        }
        public string szLastUpdateTime
        {
            get
            {
                switch (eCurrencyUpdate)
                {
                    case EnumUpdateCurrency.Updated:
                        return string.Format(Globals.rm.GetString("IDS_MSG_Updated"), _dtUpdate.ToShortTimeString());
                    case EnumUpdateCurrency.ErrorUpdate:
                        return Globals.rm.GetString("IDS_ERROR_UnableToConnect");
                    case EnumUpdateCurrency.Updating:
                    default:
                        return Globals.rm.GetString("IDS_MSG_Connecting");
                }
                
            }
        }
        public string szLastUpdateDate
        {
            get
            {
                if (eCurrencyUpdate != EnumUpdateCurrency.Updated)
                    return "";

                return _dtUpdate.ToShortDateString();
            }
        }		
        public string szFrom
        {
            get
            {
                if (_arrItems.Count == 0 || Globals.Display.szResult == "")
                    return "";

                return _arrItems[_iCurrentFrom].szSymbol + CurrencyFormatBasic(Globals.Display.szResult);
            }
        }
        public string szTo
        {
            get
            {
                if (_arrItems.Count == 0 || Globals.Display.szResult == "" || _arrItems[_iCurrentFrom].dbExchangeRate == 0.00)
                    return "0.00";

                double dbResult = 0;
                Double.TryParse(Globals.Display.szResult, out dbResult);
                
                if (dbResult == 0)
                    return _arrItems[_iCurrentTo].szSymbol+"0.00";

                double dbAnswer = dbResult * _arrItems[_iCurrentTo].dbExchangeRate / _arrItems[_iCurrentFrom].dbExchangeRate;
                return _arrItems[_iCurrentTo].szSymbol + CurrencyFormatBasic(dbAnswer.ToString());
            }
        }      		
		public string szToBid
        {
            get
            {
                if (_arrItems.Count == 0 || Globals.Display.szResult == "" || _arrItems[_iCurrentFrom].dbExchangeRate == 0.00)
                    return "";

                double dbResult = 0;
                Double.TryParse(Globals.Display.szResult, out dbResult);

                if (dbResult == 0)
                    return "";

                double dbBid = dbResult * _arrItems[_iCurrentTo].dbBid / _arrItems[_iCurrentFrom].dbExchangeRate;
                double dbAsk = dbResult * _arrItems[_iCurrentTo].dbAsk / _arrItems[_iCurrentFrom].dbExchangeRate;

                return string.Format(Globals.rm.GetString("IDS_INFO_Bid"), CurrencyFormatBasic(dbBid.ToString())) + "\r\n" + string.Format(Globals.rm.GetString("IDS_INFO_Ask"), CurrencyFormatBasic(dbAsk.ToString()));

            }
        }
		public string szFromBid
        {
            get
            {
                if (_arrItems.Count == 0)
                    return "";

                return string.Format(Globals.rm.GetString("IDS_INFO_Bid"), _arrItems[_iCurrentFrom].dbBid) + "\r\n" + string.Format(Globals.rm.GetString("IDS_INFO_Ask"), _arrItems[_iCurrentFrom].dbAsk);
            }
        }
 		public string szFromSym
        {
            get
            {
                if (_arrItems.Count == 0)
                    return "";
                return _arrItems[_iCurrentFrom].szAbvName;
            }
        }
        public string szToSym
        {
            get
            {
                if (_arrItems.Count == 0)
                    return "";

                return _arrItems[_iCurrentTo].szAbvName;
            }
        }

        public void SaveIndexes()
        {
            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
           /*     using (IsolatedStorageFileStream fs = isf.CreateFile(FILE_ToIndex))
                {
                    //and serialize data
                    XmlSerializer ser = new XmlSerializer(typeof(int));
                    ser.Serialize(fs, iToIndex);
                }

                using (IsolatedStorageFileStream fs = isf.CreateFile(FILE_FromIndex))
                {
                    //and serialize data
                    XmlSerializer ser = new XmlSerializer(typeof(int));
                    ser.Serialize(fs, iFromIndex);
                }  */
            }

        }


        public bool SaveFavorites()
        {
            if (_arrFavs.Count == 0)
                return false;

            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
                //If user choose to save, create a new file
          /*      using (IsolatedStorageFileStream fs = isf.CreateFile(FILE_Favorites))
                {
                    //and serialize data
                    XmlSerializer ser = new XmlSerializer(typeof(List<int>));
                    ser.Serialize(fs, _arrFavs);
                }   */
            }
            return true;
        }

        void ReadIndexes()
        {
            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
            /*    if (isf.FileExists(FILE_ToIndex))
                {
                    //If user choose to save, create a new file
                    using (IsolatedStorageFileStream fs = isf.OpenFile(FILE_ToIndex, FileMode.Open))
                    {
                        //and serialize data
                        XmlSerializer ser = new XmlSerializer(typeof(int));
                        iToIndex = (int)ser.Deserialize(fs);
                    }
                }  */

              /*  if (isf.FileExists(FILE_FromIndex))
                {
                    //If user choose to save, create a new file
                    using (IsolatedStorageFileStream fs = isf.OpenFile(FILE_FromIndex, FileMode.Open))
                    {
                        //and serialize data
                        XmlSerializer ser = new XmlSerializer(typeof(int));
                        iFromIndex = (int)ser.Deserialize(fs);
                    }
                }  */
            }




        }


        bool ReadFavorites()
        {
            _arrFavs.Clear();

            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
             /*   if (isf.FileExists(FILE_Favorites))
                {
                    //If user choose to save, create a new file
                    using (IsolatedStorageFileStream fs = isf.OpenFile(FILE_Favorites, FileMode.Open))
                    {
                        //and serialize data
                        XmlSerializer ser = new XmlSerializer(typeof(List<int>));
                        _arrFavs = (List<int>)ser.Deserialize(fs);

                        // use the first two in the favs
                        if (_arrFavs.Count > 1)
                        {
                            iFromIndex = _arrFavs[0];
                            iToIndex = _arrFavs[1];
                        }
                    }
                }  */
            }

            // set some defaults
            if (_arrFavs.Count == 0)
            {
                iFromIndex = FindAbvItem("USD");
                iToIndex = FindAbvItem("GBP");
                _arrFavs.Add(iFromIndex);
                _arrFavs.Add(iToIndex);
                _arrFavs.Add(FindAbvItem("CAD"));
                _arrFavs.Add(FindAbvItem("EUR"));
            }
            return true;
        }
        public bool IsInitialized()
        {
            return _arrItems.Count > 0 ? true : false;
        }
        public bool Initialize()
        {
            IssIniEx ini = new IssIniEx();

            string[] szFavAbv = new string[NUM_Favs];
            string szCurFrom, szCurTo;

            if (_arrItems.Count > 0)
            {
                szCurFrom = _arrItems[_iCurrentFrom].szAbvName; 
                szCurTo = _arrItems[_iCurrentTo].szAbvName;
            }
            else
            {
                szCurFrom = "";
                szCurTo = "";
            }


            //save the favorites abreviations if they exisist
            for (int i = 0; i < NUM_Favs; i++)
            {
                if (_arrItems.Count > 0)
                    szFavAbv[i] = _arrItems[_arrFavs[i]].szAbvName;
                else
                    szFavAbv[i] = "";
            }

            _arrItems.Clear();

            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
          /*      if (!isf.FileExists(FILE_Currency))
                {
                    string szRes = "Assets/" + FILE_Currency;
                    if (!ini.OpenFromResource(szRes))
                        return false;
                }
                else
                {
                    if (!ini.Open(FILE_Currency))
                        return false;
                }  */
            }

            if (!ini.IsInitialized())
                return false;

            string szTemp = "";

            if (!ini.GetValue(ref szTemp, "Info", "utc"))
            {
                _dtUpdate = DateTime.Now;
            }
            else
            {
                double db = Convert.ToDouble(szTemp);

                DateTime converted = new DateTime(1970, 1, 1, 0, 0, 0, 0);

                //add the timestamp to the value
                DateTime newDateTime = converted.AddSeconds(db);

                //return the value in string format
                _dtUpdate = newDateTime;//.ToLocalTime();              
            }

            /*if (!ini.GetValue(ref szTemp, "Info", "lastupdate"))
                _dtUpdate = DateTime.Now;
            else
            {
                try
                {
                    _dtUpdate = DateTime.Parse(szTemp);
                }
                catch
                {
                    _dtUpdate = DateTime.Now;
                }
            }*/
                        

            if (!ini.GetValue(ref szTemp, "Info", "numcurrencies"))
                return false;

            int iCount = Convert.ToInt32(szTemp);
            double dbOut;

            for (int i = 1; i < iCount+1; i++)
            {
                if(!ini.GetValue(ref szTemp, "Currencies", i.ToString()))
                    continue;

                // short,long,symbol,exchange,bid,ask
                string[] szItems = szTemp.Split(',');

                TypeCurrency newItem = new TypeCurrency();

                newItem.szAbvName = szItems[0];
                newItem.szName = szItems[1];
                newItem.szSymbol = szItems[2];
                if (szItems[3] != "N/A")
                {
                    if (Double.TryParse(szItems[3], out dbOut))
                        newItem.dbExchangeRate = dbOut;
                    else
                        continue;
                    //newItem.dbExchangeRate = Convert.ToDouble(szItems[3]);
                }
                if (szItems[4] != "N/A")
                {
                    if (Double.TryParse(szItems[4], out dbOut))
                        newItem.dbBid = dbOut;
                    //newItem.dbBid = Convert.ToDouble(szItems[4]);
                }
                if (szItems[5] != "N/A")
                {
                    if (Double.TryParse(szItems[5], out dbOut))
                        newItem.dbAsk = dbOut;
                    //newItem.dbAsk = Convert.ToDouble(szItems[5]);
                }


                if (newItem.dbExchangeRate == 0)
                {
                    if (newItem.szAbvName == ("USD"))
                    {
                        newItem.dbExchangeRate = 1.0;//good ol US .. should probably fix this in the desktop ver
                    }
                    else
                    {
                        System.Diagnostics.Debug.WriteLine("Failed to get exchange rate of currency " + newItem.szAbvName);
                        continue;
                    }
                }




                _arrItems.Add(newItem);
            }


            _arrItems.Sort(delegate(TypeCurrency  cur1, TypeCurrency cur2)
            {
                return cur1.szAbvName.CompareTo(cur2.szAbvName);
            });



            // Update the currency
            eCurrencyUpdate = EnumUpdateCurrency.Updated;

            if (_arrFavs.Count == 0)
            {
                ReadFavorites();
                ReadIndexes();
            }
            else if(szFavAbv[0].Length > 0)//reverse lookup
            {
                for (int i = 0; i < NUM_Favs; i++)
                {
                    _arrFavs[i] = FindAbvItem(szFavAbv[i]);
                }
                _iCurrentTo = FindAbvItem(szCurTo);
                _iCurrentFrom = FindAbvItem(szCurFrom);
            }

            return true;
        }
        public void UpdateItems()
        {
            NotifyPropertyChanged("szFrom");
            NotifyPropertyChanged("szTo");
            NotifyPropertyChanged("szFromSym");
            NotifyPropertyChanged("szToSym");
            NotifyPropertyChanged("szFromBid");
            NotifyPropertyChanged("szToBid");
        }
        public string GetCurrencyItem(int iIndex)
        {
            return "(" + _arrItems[iIndex].szAbvName + ") " + _arrItems[iIndex].szName;
        }
        public string GetFavItem(int iIndex)
        {
            int iNewIndex = _arrFavs[iIndex];
            return "(" + _arrItems[iNewIndex].szAbvName + ") " + _arrItems[iNewIndex].szName;
        }
        public int FindFavIndex(int iIndex)
        {
            for (int i = 0; i < _arrFavs.Count; i++)
            {
                if (_arrFavs[i] == iIndex)
                    return i;
            }

            // didn't find it so go to the actual item, shouldn't happen
            return iIndex + _arrFavs.Count + 1;
        }
        public void SetFromIndex(int iIndex)
        {
            // if we selected within the favs list
            if (iIndex < _arrFavs.Count)
            {
                iFromIndex = _arrFavs[iIndex];
                SaveIndexes();
                return;
            }

            // adjust for favs to get real index
            iIndex = iIndex - _arrFavs.Count - 1;

            iFromIndex = iIndex;
            SetFavIndex(iIndex);
        }
        public void SetToIndex(int iIndex)
        {
            // if we selected within the favs list
            if (iIndex < _arrFavs.Count)
            {
                iToIndex = _arrFavs[iIndex];
                SaveIndexes();
                return;
            }

            // adjust for favs to get real index
            iIndex = iIndex - _arrFavs.Count - 1;

            iToIndex = iIndex;
            SetFavIndex(iIndex);
        }

        private bool SetFavIndex(int iIndex)
        {
            // is it already on the list?
            for (int i = 0; i < _arrFavs.Count; i++)
            {
                if (_arrFavs[i] == iIndex)
                {
                    return false;
                }
            }

            // add to the begginging
            _arrFavs.Insert(0, iIndex);

            // remove the ones off the end
            for (int i = _arrFavs.Count - 1; i > NUM_Favs - 1; i--)
            {
                _arrFavs.RemoveAt(i);
            }

            // save the new favorites
            SaveFavorites();
            SaveIndexes();
            return true;
        }
        private void NotifyPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this,
                    new PropertyChangedEventArgs(propertyName));
            }

        }
        private int FindAbvItem(string szAbv)
        {
            for (int i = 0; i < _arrItems.Count; i++)
            {
                if (szAbv == _arrItems[i].szAbvName)
                    return i;
            }
            return 0;
        }
        private string CurrencyFormatBasic(string szNumber)
        {
            if (szNumber.IndexOf("E") != -1 ||
                szNumber.IndexOf("e") != -1)
                return szNumber;

            int iDec = szNumber.IndexOf(".");
            int iLen = szNumber.Length;

            if (iDec == -1)
                return szNumber + ".00";
            else if (iDec == iLen - 1)
                return szNumber + "00";
            else if (iDec == iLen - 2)
                return szNumber + "0";
            else if (iDec == iLen - 3)// (iLen - iDec < 4)//all reeady perfect
                return szNumber;
            else //remove some excess decimal places
                return szNumber.Remove(iDec + 3, iLen - iDec - 3);
        }

        public void Switch()
        {
            string szTemp = Globals.Display.szResult;

            double dbResult = 0;
            Double.TryParse(Globals.Display.szResult, out dbResult);

            if (dbResult != 0)
            {
                dbResult = dbResult * _arrItems[_iCurrentTo].dbExchangeRate / _arrItems[_iCurrentFrom].dbExchangeRate;
            }

            //conversion
            int iTemp = iFromIndex;

            SetFromIndex(iToIndex + _arrFavs.Count + 1);
            SetToIndex(iTemp + _arrFavs.Count + 1);

           // iFromIndex = iToIndex;
           // iToIndex = iTemp;
           

            //value
            Globals.Calc.AddString(dbResult.ToString());
            UpdateItems();

        }
    }
}
