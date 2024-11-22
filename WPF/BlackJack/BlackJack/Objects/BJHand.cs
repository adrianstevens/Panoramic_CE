using System.Collections;
using System.Collections.Generic;
using System.IO.IsolatedStorage;
using System.Xml.Serialization;

namespace BlackJack
{
    public class BJHand : IEnumerable<Card>
    {

        public int MAX_CARDS_IN_HAND = 11;

        List<Card> m_oCards = new List<Card>();

        // Use this for initialization
        public void Start()
        {
            m_oCards = new List<Card>();
        }

        public int Count
        {
            get { return m_oCards.Count; }
        }

        public void Save(string filename)
        {
            using (var store = IsolatedStorageFile.GetUserStoreForApplication())
            {
            /*    var fs = store.CreateFile(filename);

                var ser = new XmlSerializer(m_oCards.GetType());

                ser.Serialize(fs, m_oCards);*/

            }
        }

        public void Read(string filename)
        {
            using (var store = IsolatedStorageFile.GetUserStoreForApplication())
            {
             /*   var fs = store.OpenFile(filename, System.IO.FileMode.Open);

                var ser = new XmlSerializer(typeof(List<Card>));

                m_oCards = (List<Card>)ser.Deserialize(fs);*/
            }
        }

        public Card this[int iCardIndex]
        {
            get
            {

                if (iCardIndex < 0 || iCardIndex >= m_oCards.Count)
                    return null;

                return m_oCards[iCardIndex];
            }
        }

        public int GetScore()
        {
            int iLow = 0;
            int iHi = 0;

            GetScore(out iLow, out iHi);

            if (iHi > 21)
                return iLow;
            return iHi;
        }

        public bool GetScore(out int iLowScore, out int iHighScore)
        {
            int iTemp = 0;
            bool bAce = false;

            iLowScore = 0;
            iHighScore = 0;

            // calculate low score
            foreach(var c in m_oCards)
            {
                iTemp = c.GetCardRank();
                if (iTemp == 1)
                    bAce = true;
                if (iTemp > 10)
                    iTemp = 10;
                iLowScore += iTemp;
            }

            if (bAce == true &&
                (iLowScore + 10) < 22)
            {
                iHighScore = iLowScore + 10;
                return true;
            }
            else
            {
                iHighScore = iLowScore;
                return false;
            }

        }

        public int LastIndex 
        { 
            get 
            { 
                return Count - 1; 
            } 
        }

        public Card LastCard 
        { 
            get
            { 
                if(m_oCards.Count == 0) return null;
                return m_oCards[LastIndex]; 
            } 
        }

        //reference variable so it'll get changed (I think...)
        public void AddCard(Card oCard)
        {
            m_oCards.Add(oCard);
        }

        public void RemoveCard(int iCardIndex)
        {
            m_oCards.RemoveAt(iCardIndex);
        }

        public void ClearHand()
        {
            m_oCards.Clear();
        }

        #region IEnumerable<Card> Members

        public IEnumerator<Card> GetEnumerator()
        {
            foreach (var c in m_oCards)
                yield return c;
        }

        #endregion

        #region IEnumerable Members

        IEnumerator IEnumerable.GetEnumerator()
        {
            foreach (var c in m_oCards)
                yield return c;
        }

        #endregion
    }
}
