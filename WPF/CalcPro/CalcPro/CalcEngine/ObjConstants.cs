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
using System.Windows.Resources;
using System.IO;
using System.IO.IsolatedStorage;
using System.Text;
using System.Collections.Generic;
using System.Xml.Serialization;
using System.ComponentModel;

namespace CalcPro
{
    public class ObjConstants
    {
        /// <summary>
        ///  Variables
        /// </summary>
        public int iLastLevel { get; set; }
        public int iLastIndex { get; set; }
        public class FavoriteItem
        {
            public string szName { get; set; }
            public string szGroup { get; set; }
            public string szConstant { get; set; }
        }
        private class ConstantItem
        {
            public string szName { get; set; }
            public string szConstant { get; set; }
            public bool bIsGroup { get; set; }
            public int iStartIndex { get; set; }
            public int iEndIndex { get; set; }

            public ConstantItem()
            {
                bIsGroup = false;
                iStartIndex = -1;
                iEndIndex = -1;
            }
        }
        private string szData = null;
        private const int MAX_Items = 12;
        private const string FILE_Favorites = "FavConstants.txt";
        private int[] iItems;
        private List<ConstantItem>[] arrItems;
        private List<FavoriteItem> arrFavorites;

        /// <summary>
        /// Public Functions
        /// </summary>
        public ObjConstants()
        {
            iLastIndex = 0;
            iLastLevel = 0;
            iItems = new int[MAX_Items];
            arrItems = new List<ConstantItem>[MAX_Items];
            arrFavorites = new List<FavoriteItem>();

            for (int i = 0; i < MAX_Items; i++)
            {
                iItems[i] = -1;
                arrItems[i] = new List<ConstantItem>();
            }           
        }
        public int GetGroupCount(int iLevel)
        {
            if (iLevel < 0 || iLevel >= MAX_Items)
                return 0;

            // make sure we're initialized
            InitGroup();

            if (iLevel == 0)
            {
                if (arrItems[iLevel].Count == 0)
                    PopulateGroup(0, 0);
            }

            return arrItems[iLevel].Count;
        }
        public string GetGroupName(int iLevel, int iIndex)
        {
            if (iLevel < 0 || iLevel >= MAX_Items)
                return null;

            ConstantItem item = arrItems[iLevel][iIndex];
            if (item == null)
                return null;

            return item.szName;
        }
        public string GetGroupConstant(int iLevel, int iIndex)
        {
            if (iLevel < 0 || iLevel >= MAX_Items)
                return null;

            ConstantItem item = arrItems[iLevel][iIndex];
            if (item == null || item.szConstant == null)
                return null;

            // save the last entry so we can add it to our favorites after
            iLastIndex = iIndex;
            iLastLevel = iLevel;

            return item.szConstant;
        }
        public string GetSelectedSection(int iLevel)
        {
            if (iLevel < 0 || iLevel > 2)
                return null;

            ConstantItem item = arrItems[iLevel][iItems[iLevel]];
            if (item == null || item.szName == null)
                return null;

            return item.szName;
        }
        public bool IsGroupAConstant(int iLevel, int iIndex)
        {
            if (iLevel < 0 || iLevel >= MAX_Items)
                return false;

            try
            {
                ConstantItem item = arrItems[iLevel][iIndex];

                return !item.bIsGroup;
            }
            catch (System.ArgumentOutOfRangeException)
            {
                return false;
            }            
        }
        public bool SetGroup(int iLevel, int iIndex)
        {
            if (iLevel < 0 || iLevel >= MAX_Items || iIndex < 0)
                return false;

            // make sure our previous array has values
            if (iLevel > 0 && (iItems[iLevel - 1] == -1 || arrItems[iLevel - 1].Count == 0))
                return false;

            for (int i = iLevel; i < MAX_Items; i++)
                iItems[i] = -1;

            iItems[iLevel] = iIndex;

            // populate the next box
            if (iLevel < MAX_Items + 1 && !IsGroupAConstant(iLevel, iIndex))
                PopulateGroup(iLevel + 1, iIndex);

            return true;
        }

        public bool SaveFavorites()
        {
            if (arrFavorites.Count == 0)
                return false;

            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
                //If user choose to save, create a new file
             /*   using (IsolatedStorageFileStream fs = isf.CreateFile(FILE_Favorites))
                {
                    //and serialize data
                    XmlSerializer ser = new XmlSerializer(typeof(List<FavoriteItem>));
                    ser.Serialize(fs, arrFavorites);
                }  */
            }
            return true;
        }

        public bool ReadFavorites()
        {
            arrFavorites.Clear();

            // so it doesn't crash in the designer
        //    if (!DesignerProperties.IsInDesignTool)
            {
                //using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
                {
                /*    if (isf.FileExists(FILE_Favorites))
                    {
                        //If user choose to save, create a new file
                        using (IsolatedStorageFileStream fs = isf.OpenFile(FILE_Favorites, FileMode.Open))
                        {
                            //and serialize data
                            XmlSerializer ser = new XmlSerializer(typeof(List<FavoriteItem>));
                            arrFavorites = (List<FavoriteItem>)ser.Deserialize(fs);
                        }
                    } */
                }
            }

            if (arrFavorites.Count == 0)
            {
                SetDefaultFavorites();
            }
            return true;
        }
        public bool AddToFavorites()
        {
            if (iLastIndex == -1 || iLastLevel == -1 || !IsGroupAConstant(iLastLevel, iLastIndex))
                return false;

            if (arrFavorites.Count == 0)
                ReadFavorites();

            ConstantItem item = arrItems[iLastLevel][iLastIndex];

            string szGroup = "";

            // build the group name here
            for (int i = 0; i < iLastLevel; i++)
            {
                ConstantItem sName = arrItems[i][iItems[i]];

                if (i != 0)
                    szGroup += "→";

                szGroup += sName.szName;
            }

            // first check if it's already in the group
            for (int i = 0; i < arrFavorites.Count; i++ )
            {
                FavoriteItem sCheck = arrFavorites[i];
                if (sCheck.szName == item.szName && sCheck.szConstant == item.szConstant)
                {
                    // move to the first in line
                    arrFavorites.RemoveAt(i);
                    arrFavorites.Insert(0, sCheck);
                    return true;
                }
            }


            FavoriteItem sNew = new FavoriteItem();
            sNew.szName = item.szName;
            sNew.szGroup = szGroup;
            sNew.szConstant = item.szConstant;
            arrFavorites.Insert(0, sNew);

            if (arrFavorites.Count > MAX_Items)
                arrFavorites.RemoveAt(arrFavorites.Count - 1);

            SaveFavorites();
            return true;
        }
        public int GetFavoritesCount()
        {
            if (arrFavorites.Count == 0)
                ReadFavorites();

            return arrFavorites.Count;
        }
        public string GetFavoriteName(int iIndex)
        {
            FavoriteItem item = arrFavorites[iIndex];
            if (item == null)
                return null;
            return item.szName;
        }
        public string GetFavoriteGroup(int iIndex)
        {
            FavoriteItem item = arrFavorites[iIndex];
            if (item == null)
                return null;
            return item.szGroup;
        }
        public string GetFavoriteConstant(int iIndex)
        {
            FavoriteItem item = arrFavorites[iIndex];
            if (item == null)
                return null;
            return item.szConstant;
        }

        /// <summary>
        /// Private functions
        /// </summary>
        /// <param name="iStart"></param>
        private void DeleteArray(int iStart)
        {
            for (int i = iStart; i < MAX_Items; i++)
            {
                arrItems[i].Clear();
                iItems[i] = -1;
            }
        }
        private void PopulateGroup(int iLevel, int iPreviousGroupIndex)
        {
            iLastLevel = iLevel;
            iLastIndex = 0;

            // remove everything going forward
            DeleteArray(iLevel);

            int iStartIndex = 0;
            int iEndIndex = szData.Length;

            if (iLevel > 0)
            {
                ConstantItem item = arrItems[iLevel - 1][iPreviousGroupIndex];
                iStartIndex = item.iStartIndex;
                iEndIndex = item.iEndIndex;
            }

            string szGroup = "GROUP";
            for (int i = 0; i < iLevel + 1; i++)
            {
                szGroup = "*" + szGroup + "*";
            }

            szGroup += "\r\n";

            int iCurrentIndex = szData.IndexOf(szGroup, iStartIndex);
            if (iCurrentIndex == -1 || iCurrentIndex >= iEndIndex)
            {
                // read in constants
                iCurrentIndex = iStartIndex;
                string szLine;
                while ((szLine = ReadLine(ref iCurrentIndex)) != null)
                {
                    int iNewStarCount = 0;

                    if (IsGroup(szLine, ref iNewStarCount))
                        break;

                    // we are an item so read two lines worth of data
                    string szConstant = ReadLine(ref iCurrentIndex);
                    if (szConstant.Length == 0)
                        break;

                    ConstantItem newItem = new ConstantItem();

                    newItem.szName = szLine;
                    newItem.szConstant = szConstant;
                    arrItems[iLevel].Add(newItem);
                }
            }
            else
            {
                // read in some groups
                while (iCurrentIndex != -1 && iCurrentIndex < iEndIndex)
                {
                    string szGroupName;
                    iCurrentIndex = iCurrentIndex + 5 + 2 * (iLevel + 1) + 2;
                    szGroupName = ReadLine(ref iCurrentIndex);
                    if (szGroupName.Length == 0)
                        break;

                    ConstantItem newGroup = new ConstantItem();
                    newGroup.szName = szGroupName;
                    newGroup.bIsGroup = true;
                    newGroup.iStartIndex = iCurrentIndex;
                    newGroup.iEndIndex = iEndIndex;
                    arrItems[iLevel].Add(newGroup);

                    if (arrItems[iLevel].Count > 1)
                    {
                        // set the proper size for the previous index
                        ConstantItem Previous = arrItems[iLevel][arrItems[iLevel].Count - 2];
                        if (Previous == null)
                            break;
                        Previous.iEndIndex = iCurrentIndex - 1;
                    }

                    iCurrentIndex = szData.IndexOf(szGroup, iCurrentIndex);
                }
            }
        }
        private string ReadLine(ref int iDataIndex)
        {
            if (iDataIndex >= szData.Length)
                return null;

            int iIndex = -1;

            // so we skip multiple \n
            while (true)
            {
                iIndex = szData.IndexOf("\n", iDataIndex);
                if (iIndex == -1)
                    return null;

                if (iIndex != iDataIndex)
                    break;

                iDataIndex++;
                if (iDataIndex >= szData.Length)
                    return null;
            }

            string szTemp = szData.Substring(iDataIndex, iIndex - iDataIndex+1);

            szTemp = szTemp.Replace(System.Environment.NewLine, string.Empty);

            iDataIndex = iIndex + 1;

            return szTemp;
        }
        private bool IsGroup(string szLine, ref int iStarCount)
        {
            if (szLine.Length == 0)
                return false;

            bool bRet = false;
            if (szLine.StartsWith("*") && szLine.EndsWith("*"))
            {
                bRet = true;
                iStarCount = 0;
                for (int i = 0; i < szLine.Length; i++)
                {
                    if (szLine[i] != '*')
                        break;
                    iStarCount++;
                }
            }
            return bRet;
        }
        private void SetDefaultFavorites()
        {
            FavoriteItem sConst = new FavoriteItem();//Adrian said to leave this on March 11, 2010
            sConst.szName      = "Elementary charge (C)";
            sConst.szGroup = "Physics→Electromagnetic";
            sConst.szConstant  = "1.60217648740e-019";
            arrFavorites.Add(sConst);

            sConst = new FavoriteItem();
            sConst.szName      = "Electron mass (kg)";
            sConst.szGroup = "Physics→Atomic and Nuclear";
            sConst.szConstant  = "9.1093821545e-031";
            arrFavorites.Add(sConst);

            sConst = new FavoriteItem();
            sConst.szName      = "Proton mass (kg)";
            sConst.szGroup = "Physics→Atomic and Nuclear";
            sConst.szConstant  = "1.67262163783e-027";
            arrFavorites.Add(sConst);

            sConst = new FavoriteItem();
            sConst.szName      = "Neutron mass (kg)";
            sConst.szGroup = "Physics→Atomic and Nuclear";
            sConst.szConstant  = "1.6749286e-027";
            arrFavorites.Add(sConst);

            sConst = new FavoriteItem();
            sConst.szName      = "Standard gravity (m/s²)";
            sConst.szGroup = "Physics→Common Values";
            sConst.szConstant  = "9.80665";
            arrFavorites.Add(sConst);

            sConst = new FavoriteItem();
            sConst.szName      = "Speed of sound (m/s)";
            sConst.szGroup = "Physics→Common Values";
            sConst.szConstant  = "343";
            arrFavorites.Add(sConst);

            sConst = new FavoriteItem();
            sConst.szName      = "Speed of light (m/s)";
            sConst.szGroup = "Physics→Universal";
            sConst.szConstant  = "299792458";
            arrFavorites.Add(sConst);

            sConst = new FavoriteItem();
            sConst.szName      = "Gravitation (m³/kg·s²)";
            sConst.szGroup = "Physics→Universal";
            sConst.szConstant  = "6.6742867e-011";
            arrFavorites.Add(sConst);

            sConst = new FavoriteItem();
            sConst.szName      = "Planck (J·s)";
            sConst.szGroup = "Physics→Universal";
            sConst.szConstant  = "6.6260689633e-034";
            arrFavorites.Add(sConst);

            sConst = new FavoriteItem();
            sConst.szName      = "Gas constant R (J/°K mol)";
            sConst.szGroup = "Physics→Physico-chemical";
            sConst.szConstant  = "8.31447215";
            arrFavorites.Add(sConst);

            sConst = new FavoriteItem();
            sConst.szName      = "Boltzmann k (J/K)";
            sConst.szGroup = "Physics→Physico-chemical";
            sConst.szConstant  = "1.3806505e-023";
            arrFavorites.Add(sConst);

            sConst = new FavoriteItem();
            sConst.szName      = "Avagadro's number";
            sConst.szGroup = "Physics→Physico-chemical";
            sConst.szConstant  = "6.0221367e+023";
            arrFavorites.Add(sConst);
        }
        private void InitGroup()
        {
            if (szData != null)
                return;

            Uri uri = new Uri("Assets/constants.txt", UriKind.RelativeOrAbsolute);

            StreamResourceInfo sri = App.GetResourceStream(uri);

            StreamReader sr = new StreamReader(sri.Stream);

            szData = sr.ReadToEnd();

            sr.Close();

        }

        
    }
}
