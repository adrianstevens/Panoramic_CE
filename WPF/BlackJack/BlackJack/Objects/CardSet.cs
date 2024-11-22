using System.Collections;
using System.IO.IsolatedStorage;
using System.Xml.Serialization;
using System.Collections.Generic;
using System;
#if DEBUG
using System.Windows.Controls.Primitives;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Media;
using BlackJack.Objects;
#endif

namespace BlackJack
{
    public class TypeCardSetState
    {
        public bool m_bUseJokers;
        public int m_iNumCards;
        public int m_iNumDecks;
        public int m_iCardIndex;

        public Card[] m_oCards;

        public void CopyFrom(TypeCardSetState orig)
        {
            m_bUseJokers = orig.m_bUseJokers;
            m_iCardIndex = orig.m_iCardIndex;
            m_iNumCards = orig.m_iNumCards;
            m_iNumDecks = orig.m_iNumDecks;

            m_oCards = new Card[orig.m_oCards.Length];

            Array.Copy(orig.m_oCards, m_oCards, m_oCards.Length);
        }
    };

    public class CardSet : TypeCardSetState
    {
        const int MAX_DECKS = 8;
        const int NUM_SHUFFLES = 127;	// card swaps per deck

        const int CARDS_IN_DECK = 52;

        public CardSet()
        {
            Start(false);
        }
        // Use this for initialization
        public void Start(bool useJokers)
        {
            m_bUseJokers = useJokers;

            Reset();
            m_iNumDecks = 0;
            ChangeNumDecks(1);

        }

        public void Start()
        {
            Start(false);
        }

        public int GetCardsLeft() { return m_iNumCards - m_iCardIndex; }

        void Reset()
        {
            m_oCards = new Card[CARDS_IN_DECK];

            m_iCardIndex = 0;
            m_iNumCards = m_oCards.Length;
            m_iNumDecks = 1;

            m_bUseJokers = false;

        }

        public void ChangeNumDecks(int iNumDecks)
        {
            if (iNumDecks < 1 ||
                iNumDecks > MAX_DECKS ||
                iNumDecks == m_iNumDecks)
            {
                return;
            }

            int iCardsPerDeck = CARDS_IN_DECK;
            if (m_bUseJokers)
                iCardsPerDeck += 2;

            m_iNumDecks = iNumDecks;
            //set the number of cards
            m_iNumCards = iNumDecks * iCardsPerDeck;

            m_oCards = new Card[m_iNumCards];

            // make sure to set where in the deck these cards are
            for (int i = 0; i < m_iNumCards; i++)
            {
                m_oCards[i] = new Card();
                m_oCards[i].IndexInDeck = i;
            }

            //Always Initialize the deck(s) 
            InitializeDeck();
            Shuffle();
        }


        public void Shuffle()
        {
            // pick two random cards and swap them, repeat
            m_iCardIndex = 0; //KINDA need this....

            //Seed the randomness with the seed of randomness
            //Random.seed(1);

            Card oTempCard;
            oTempCard = new Card();//ookay

            int iNumSwaps = m_iNumDecks * NUM_SHUFFLES;
            int iSwap1, iSwap2;

            for (int i = 0; i < iNumSwaps; i++)
            {
                iSwap1 = Globals.Random.Next(0, m_iNumCards);
                iSwap2 = Globals.Random.Next(0, m_iNumCards);

                //iSwap1 = 5;
                //iSwap2 = 6;

                if (iSwap1 == iSwap2)
                    continue;

                if (iSwap1 >= m_iNumCards)
                    continue;

                if (iSwap2 >= m_iNumCards)
                    continue;

                //simple swap
                oTempCard.Rank = m_oCards[iSwap1].Rank;
                oTempCard.Suit = m_oCards[iSwap1].Suit;

                m_oCards[iSwap1].Rank = m_oCards[iSwap2].Rank;
                m_oCards[iSwap1].Suit = m_oCards[iSwap2].Suit;

                m_oCards[iSwap2].Rank = oTempCard.Rank;
                m_oCards[iSwap2].Suit = oTempCard.Suit;
            }
        }

#if DEBUG
  /*      public void ShuffleDebug(object o, PopupMenuEventArgs args)
        {
            int index = 0;
            Card tmp;

            switch ((EnumAchievement) args.iSelected)
            {
                case EnumAchievement.IHate8s:
                    while (index < 8)
                    {
                        tmp = m_oCards[index++];
                        tmp.Rank = EnumCardRank.Eight;
                    }
                    break;
                case EnumAchievement.DoubleTrouble:
                case EnumAchievement.SeventyFiveK:
                case EnumAchievement.OneHundredK:
                case EnumAchievement.TwentyFiveK:
                case EnumAchievement.RunningCount:
                    for (var i = 0; i < m_iNumCards / 2; i++)
                    {
                        m_oCards[index++].Rank = EnumCardRank.Seven;
                        m_oCards[index++].Rank = EnumCardRank.Six;
                    }
                    break;
                case EnumAchievement.HighRoller:
                     for (var i = 0; i < m_iNumCards/2; i++)
                    {
                        m_oCards[index++].Rank = EnumCardRank.Ten;
                        m_oCards[index++].Rank = EnumCardRank.Six;
                    }
                   break;
                case EnumAchievement.LuckyBreak:
                   m_oCards[index++].Rank = EnumCardRank.Ace;
                   m_oCards[index++].Rank = EnumCardRank.Nine;
                   m_oCards[index++].Rank = EnumCardRank.Ace;
                   m_oCards[index++].Rank = EnumCardRank.Nine;
                   m_oCards[index++].Rank = EnumCardRank.Ace;
                   m_oCards[index++].Rank = EnumCardRank.Ace;
                   m_oCards[index++].Rank = EnumCardRank.Two;
                   m_oCards[index++].Rank = EnumCardRank.Two;
                   m_oCards[index++].Rank = EnumCardRank.Two;
                   m_oCards[index++].Rank = EnumCardRank.Two;
                   m_oCards[index++].Rank = EnumCardRank.Three;
                   m_oCards[index++].Rank = EnumCardRank.Three;
                   m_oCards[index++].Rank = EnumCardRank.Three;
                    break;
                case EnumAchievement.OnFire:
                    m_oCards[index++].Rank = EnumCardRank.Ten;
                    m_oCards[index++].Rank = EnumCardRank.Ten;
                    m_oCards[index++].Rank = EnumCardRank.Ace;
                    m_oCards[index++].Rank = EnumCardRank.Ace;
                    for (var i = 0; i < 3; i++)
                    {
                        m_oCards[index++].Rank = EnumCardRank.Ten;
                        index++;
                        m_oCards[index++].Rank = EnumCardRank.Ace;
                        index++;
                    }
                    break;
                case EnumAchievement.TheGambler:
                    m_oCards[index++].Rank = EnumCardRank.Nine;
                    index++;
                    m_oCards[index++].Rank = EnumCardRank.Nine;
                    index++;
                    m_oCards[index++].Rank = EnumCardRank.Three;
                    break;
                case EnumAchievement.TheSplit:
                    m_oCards[index++].Rank = EnumCardRank.Seven;
                    index++;
                    m_oCards[index++].Rank = EnumCardRank.Seven;
                    index++;
                    m_oCards[index++].Rank = EnumCardRank.Seven;
                    m_oCards[index++].Rank = EnumCardRank.Seven;
                    m_oCards[index++].Rank = EnumCardRank.Seven;
                    m_oCards[index++].Rank = EnumCardRank.Seven;
                    break;
                case EnumAchievement.TripUp:
                    for (var i = 0; i < 5; i++)
                    {
                        index++;
                        m_oCards[index++].Rank = EnumCardRank.Six;
                        index++;
                        m_oCards[index++].Rank = EnumCardRank.Six;
                        m_oCards[index++].Rank = EnumCardRank.Ten;
                    }
                    break;
                case EnumAchievement.Woooh:
                    m_oCards[index++].Rank = EnumCardRank.Five;
                    index++;
                    m_oCards[index++].Rank = EnumCardRank.Five;
                    index++;
                    m_oCards[index++].Rank = EnumCardRank.Five;
                    m_oCards[index++].Rank = EnumCardRank.Five;
                    m_oCards[index++].Rank = EnumCardRank.Ace;
                    break;
            }
        }*/
#endif

        void InitializeDeck()
        {
            int iCardsInDeck = CARDS_IN_DECK;
            m_iCardIndex = 0;

            if (m_bUseJokers == true)
            {
                iCardsInDeck = CARDS_IN_DECK + 2;
            }
            // go through number of decks
            for (int i = 0; i < m_iNumDecks; i++)
            {
                // go through suits
                for (int j = 0; j < Card.NUM_SUITS; j++)
                {
                    // go through card ranks
                    for (int k = 0; k < Card.NUM_RANKS; k++)
                    {
                        m_oCards[k + j * Card.NUM_RANKS + i * iCardsInDeck].SetCardSuit(j);
                        m_oCards[k + j * Card.NUM_RANKS + i * iCardsInDeck].SetCardRank(k + 1);	// correct for joker in enum
                    }
                }
                // add jokers at the end of each additional deck
                if (m_bUseJokers == true)
                {
                    m_oCards[(i + 1) * iCardsInDeck - 2].Rank = (int)EnumCardRank.Joker;
                    m_oCards[(i + 1) * iCardsInDeck - 1].Rank = (int)EnumCardRank.Joker;
                }
            }
        }

        public Card GetNextCard() //(BOOL bNoShuffle /* = FALSE */)
        {
            if (m_iCardIndex >= m_iNumCards + 1)
                return m_oCards[0];

            m_iCardIndex++;
            return m_oCards[m_iCardIndex - 1];

        }

        Card GetCardFromIndex(int iIndex)
        {
            if (iIndex < 0 || iIndex >= m_iNumCards)
                return m_oCards[0];

            return m_oCards[iIndex];
        }


        void UndoNextCard()
        {
            if (m_iCardIndex > 0)
                m_iCardIndex--;
        }


        public bool Save(string filename)
        {
           using (var store = IsolatedStorageFile.GetUserStoreForApplication())
            {
            /*    var fs = store.CreateFile(filename);

                var ser = new XmlSerializer(typeof(TypeCardSetState));

                var tmp = new TypeCardSetState();
                tmp.CopyFrom(this);

                ser.Serialize(fs, tmp);*/

                return true;
            }
        }

        public bool Read(string filename)
        {
           using (var store = IsolatedStorageFile.GetUserStoreForApplication())
            {
            /*    var fs = store.OpenFile(filename, System.IO.FileMode.Open);

                var ser = new XmlSerializer(typeof(TypeCardSetState));

                var tmp = (TypeCardSetState)ser.Deserialize(fs);

                CopyFrom(tmp);*/

                return true;
            }
        }
    }

}
