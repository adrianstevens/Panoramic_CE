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
using System.IO.IsolatedStorage;
using System.IO;
using System.Diagnostics;
using System.Xml.Serialization;

namespace BlackJack.Objects
{
    public class HighScores
    {
        const int NOT_IN_HISCORE_TABLE = -1;
        const string HIGHSCORE_FILE = "high_scores.xml";

        public HighScores()
        {
            m_arrScores = new List<TypeScore>();
        }

        bool Init(int iMaxScoreSaves)
        {
            m_iMaxScoreSaves = iMaxScoreSaves;
            return true;
        }

        public bool Save()
        {
            try
            {
                XmlSerializer ser = new XmlSerializer(typeof(List<TypeScore>));
                TextWriter writer = new StreamWriter("hs.xml");
                ser.Serialize(writer, m_arrScores);
                writer.Close();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("Could not save state: " + ex);
            }
            

            return true;
        }

        public void Read()
        {
            XmlSerializer ser = new XmlSerializer(typeof(List<TypeScore>));
            
            try
            {
                TextReader reader = new StreamReader("hs.xml");
                m_arrScores = (List<TypeScore>)ser.Deserialize(reader);
                reader.Close();
                
            }
            catch (System.Exception ex)
            {
                Debug.WriteLine("Could not read file storage: " + ex);
            }
        }

        private static int SortTypScore(TypeScore a, TypeScore b)
        {
            return a.Score - b.Score;
        }

        public int Add(string szName, int Score)//return the index - ie position in the highscore table -1 if failed
        {
            TypeScore sScore = new TypeScore();

            int iRet = -1;

            if (!string.IsNullOrEmpty(szName))
                sScore.Name = szName;

            sScore.Score = Score;

           // sScore.Time = DateTime.Now;

            m_arrScores.Add(sScore);

            m_arrScores.Sort(SortTypScore);

            iRet = m_arrScores.IndexOf(sScore);

            if (m_iMaxScoreSaves > 0 && m_iMaxScoreSaves - 1 < m_arrScores.Count - 1)
                m_arrScores.RemoveRange(m_iMaxScoreSaves - 1, m_arrScores.Count - 1);

            //this score didn't make it into the high score table (loser)
            if (iRet >= m_iMaxScoreSaves)
                iRet = NOT_IN_HISCORE_TABLE;

            m_iIndexOfLastAdded = iRet;//save the index of the last added score

            return iRet;
        }
        public TypeScore this[int index]
        {
            get { return m_arrScores[index]; }
        }
        public int Count
        {
            get { return m_arrScores.Count; }
        }

        int GetHiScore()
        {
            if (m_arrScores.Count == 0)
                return 0;

            return m_arrScores[0].Score;
        }

        int GetIndexOfLastScore() { return m_iIndexOfLastAdded; }

        // variables
        private List<TypeScore> m_arrScores;
        private int m_iMaxScoreSaves;		// how many max high scores you want to show
        private int m_iIndexOfLastAdded;
    }

    public class TypeScore
    {
        public string Name = "";			// the name of the lucky high scorer
        public int Score;			// will be a static chunk of memory (usually a struct of some sort)
       // public DateTime Time;			// the time of this high score
    };
}
