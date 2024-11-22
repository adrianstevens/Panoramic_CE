using System.Collections;

namespace BlackJack
{

    public enum EnumCardSuit
    {
        Club,
        Diamond,
        Spade,
        Heart,

    };

    public enum EnumCardRank
    {
        Joker,
        Ace,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        Ten,
        Jack,
        Queen,
        King,
    };

    public class Card// : MonoBehaviour  
    {
        public static int NUM_SUITS = 4;
        public static int NUM_RANKS = 13;

        public const double HEIGHT = 120;
        public const double WIDTH = 98;

        public EnumCardRank Rank { get; set; }
        public EnumCardSuit Suit { get; set; }

        public Card()
        {
            Suit = EnumCardSuit.Diamond;
            Rank = EnumCardRank.Joker;
        }

        public int IndexInDeck
        {
            get;
            set;			// so it knows where it's located in the deck
        }

        public void SetCardRank(int value) { Rank = (EnumCardRank)value; }
        public void SetCardSuit(int value) { Suit = (EnumCardSuit)value; }

        public int GetCardRank() { return (int)Rank; }
        public int GetCardSuit() { return (int)Suit; }

        public int GetCardValue()
        {
            switch (Rank)
            {
                case EnumCardRank.Joker:
                case EnumCardRank.Ace:
                case EnumCardRank.Two:
                case EnumCardRank.Three:
                case EnumCardRank.Four:
                case EnumCardRank.Five:
                case EnumCardRank.Six:
                case EnumCardRank.Seven:
                case EnumCardRank.Eight:
                case EnumCardRank.Nine:
                case EnumCardRank.Ten:
                    return (int)Rank;
                case EnumCardRank.Jack:
                case EnumCardRank.Queen:
                case EnumCardRank.King:
                    return 10;
            }

            return 0;
        }

        public override string ToString()
        {
            return Rank + " of " + Suit;
        }
    }
}