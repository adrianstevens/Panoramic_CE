using System;
using System.Windows;
using System.Collections.Generic;
using System.IO;
using System.Windows.Resources;
using System.IO.IsolatedStorage;
using System.Xml.Serialization;
using CalcPro;



    public class IssIniEx
    {

        Dictionary<string, Dictionary<string, string>> items = new Dictionary<string, Dictionary<string, string>>();

        public IssIniEx()
        {
        }
        public bool OpenFromResource(string szResName)
        {
#if DEBUG
            /*string szXmlFile = Path.GetFileName(szResName) + ".xml";
            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
                if (isf.FileExists(szXmlFile))
                    return OpenXml(szXmlFile);
            }*/
            System.Diagnostics.Debug.WriteLine("IssIniEx->OpenResourse - " + szResName);
#endif 
            

            Uri uri = new Uri(szResName, UriKind.RelativeOrAbsolute);

            StreamResourceInfo sri = App.GetResourceStream(uri);
            StreamReader sr = new StreamReader(sri.Stream);

            try
            {
                //StreamReader sr = new StreamReader(szResName, true);
                //StreamReader sr = new StreamReader(sri.Stream);

                string szData = sr.ReadToEnd();

                sr.Close();

                bool bReturn = PopulateArrays(szData);

#if DEBUG
                /*if (bReturn)
                    return SaveXml(szXmlFile);*/
#endif
                return bReturn;
            }
            catch (System.NullReferenceException)
            {
                return false;
            }
            
        }

        public bool IsInitialized()
        {
            return (items.Count > 0 ? true : false) ;
        }
        public bool OpenXml(string szFileName)
        {
            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
             /*   if (!isf.FileExists(szFileName))
                    return false;

                //If user choose to save, create a new file
                using (IsolatedStorageFileStream fs = isf.OpenFile(szFileName, FileMode.Open))
                {
                    //and serialize data
                    XmlSerializer ser = new XmlSerializer(typeof(Dictionary<string, Dictionary<string, string>>));
                    items = (Dictionary<string, Dictionary<string, string>>)ser.Deserialize(fs);

                    return true;
                } */
            }

            return false;
        }
        public bool SaveXml(string szFileName)
        {
        /*    if (items.Count == 0)
                return false;

            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
                if (isf.FileExists(szFileName))
                    isf.DeleteFile(szFileName);

                //If user choose to save, create a new file
                using (IsolatedStorageFileStream fs = isf.CreateFile(szFileName))
                {
                    //and serialize data
                    XmlSerializer ser = new XmlSerializer(typeof(Dictionary<string, Dictionary<string, string>>));
                    ser.Serialize(fs, items);

                    return true;
                }
            }  */
            return false;
        }
        public bool Open(string szFileName)
        {
            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
#if DEBUG
                /*string szXmlFile = Path.GetFileName(szFileName) + ".xml";
                if (isf.FileExists(szXmlFile))
                    return OpenXml(szXmlFile);   */ 
#endif 
               // bugbug
               /* if (!isf.FileExists(szFileName))
                    return false;

                using (IsolatedStorageFileStream openFile = isf.OpenFile(szFileName, FileMode.Open))
                {
                    StreamReader sr = new StreamReader(openFile, true);

                    string szData = sr.ReadToEnd();

                    sr.Close();
                    openFile.Close();

                    bool bReturn = PopulateArrays(szData);

                    return bReturn;  
                }  */
            }
            return false;
        }

        public bool Save(string szFileName)
        {
            return true;
        }

        public bool GetValue(ref string szDestination, string szSection, string szKey)
        {
            Dictionary<string, string> Values = null;

            if (!items.TryGetValue(szSection, out Values))
                return false;

            if (!Values.TryGetValue(szKey, out szDestination))
                return false;
            
            return true;
        }

        public bool GetValue(ref int iDestination, string szSection, string szKey)
        {
            string szTemp = null;

            if (GetValue(ref szTemp, szSection, szKey) == false)
                return false;

            iDestination = Convert.ToInt32(szTemp);

            return true;
        }
        
        private bool PopulateArrays(string szText)
        {
            string szCurrentSection = "";
            int iCurrentIndex = 0;

            string szLine;
            char[] charsToTrim = {'\t','\r','\n',' '};

            do
            {
                szLine = ReadLine(ref iCurrentIndex, szText);
                if (szLine == null)
                    break;

                szLine = szLine.Trim(charsToTrim);

                if (szLine.Length <= 2)
                    continue;
                
                if (szLine.StartsWith("[") && szLine.EndsWith("]"))
                {
                    szCurrentSection = szLine.Substring(1, szLine.Length-2);
                }
                else if(-1 != szLine.IndexOf("="))
                {
                    string[] szSplit = szLine.Split('=');
                    szSplit[0] = szSplit[0].Trim(charsToTrim);
                    szSplit[1] = szSplit[1].Trim(charsToTrim);

                    if (items.ContainsKey(szCurrentSection))
                    {
                        Dictionary<string, string> Values = items[szCurrentSection];
                        Values.Add(szSplit[0], szSplit[1]);
                    }
                    else
                    {
                        Dictionary<string, string> Values = new Dictionary<string, string>();
                        Values.Add(szSplit[0], szSplit[1]);
                        items.Add(szCurrentSection, Values);
                    }
                }


            } while (szLine != null);

            return true;
        }
        private string ReadLine(ref int iDataIndex, string szData)
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

            string szTemp = szData.Substring(iDataIndex, iIndex - iDataIndex + 1);

            //szTemp.Trim();

            //szTemp = szTemp.Replace(System.Environment.NewLine, string.Empty);

            iDataIndex = iIndex + 1;

            return szTemp;
        }
    }

