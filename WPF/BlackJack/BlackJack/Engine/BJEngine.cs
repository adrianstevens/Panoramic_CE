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
using System.IO.IsolatedStorage;
using System.Xml.Serialization;
using System.Diagnostics;
using System.IO;
using System.ComponentModel;
using System.Media;
using System.Windows.Resources;

namespace BlackJack
{
    public enum StartingBank
    {   //b is because we can't start with numbers
        b10000,
        b5000,
        b1000,
        b500,
        b250,
        b100,
        Count,
    };

    public enum EnumGameDifficulty
    {
        Easy,
        Normal,
        Expert,
        Learn,
        Custom,

    };

    public enum EnumGameState
    {
        Bet,
        Deal,
        PlayerFirstMove,
        PlayerMove,
        SplitMove,
        DealerMove,
        EndOfGame,
    };

    public enum EnumGameResult
    {
        BlackJack,
        Win,
        Draw,
        Surrender,
        Lose,
        NULL, //for the ol split hand
    };

    public enum EnumCardCount
    {
        WizardAce5,
        KO,
        HiLow,
        HiOpt1,
        HiOpt2,
        Zen,
        OmegaII,
        Count,
    };

    public enum EnumNextMove
    {   //small for my arrays
        Stand,//stand
        Hit,//hit
        Double,
        DoubleOrHit,//double or hit
        DoubleOrStand,//double or stand
        Split,//split
        Surrender,//surrender ... otherwise hit
        Insurance,//insurance ... never without card counting
        Nothing,
    };

    public enum EnumCardCountView
    {
        On,
        Off,
        SemiHidden,
    };

    /*
    struct TypeOptions
    {
        EnumCardCountView eCardCountView;
        EnumCardCount eCardCount;
        EnumGameDifficulty eGameDifficulty;
        bool bPlaySounds;
        int dwLang;
    };*/

    public class TypeGameState
    {

        public int Num25Chips { get; set; }
        public int Num50Chips { get; set; }
        public int Num100Chips { get; set; }
        public int Num500Chips { get; set; }

        public int m_iCardCount { get; set; } //http://en.wikipedia.org/wiki/Card_counting

        public EnumGameDifficulty GameDifficulty
        { get; set; }

        public int BetAmount
        {
            get;
            set;
        }
        public int Bank
        {
            get;
            set;
        }

        public EnumGameState GameState { get; set; }

        public int BankHi { get; set; } //we'll keep track for the hiscore table

        public bool IsSplit { get; set; }

        public ResultEventArgs LastResult { get; set; }

        public int NumDecks { get; set; }
        public bool BurnFirstCard { get; set; }//toss the first card away on a shuffle
        public bool DealerHitsOnSoft17 { get; set; }
        public bool BlackJack2to1 { get; set; } //normal is 3 to 2
        public bool ShowScores { get; set; }
        public bool ShowDealerCard { get; set; }
        public StartingBank BankRoll { get; set; }

        public void CopyFrom(TypeGameState orig)
        {
            BurnFirstCard = orig.BurnFirstCard;
            DealerHitsOnSoft17 = orig.DealerHitsOnSoft17;
            BlackJack2to1 = orig.BlackJack2to1;
            ShowScores = orig.ShowScores;
            ShowDealerCard = orig.ShowDealerCard;

            Num25Chips  = orig.Num25Chips;

            Num50Chips = orig.Num50Chips;

            Num100Chips = orig.Num100Chips;

            Num500Chips = orig.Num500Chips;

            m_iCardCount = orig.m_iCardCount;

            GameDifficulty = orig.GameDifficulty;

            GameState = orig.GameState;

            BetAmount = orig.BetAmount;

            Bank = orig.Bank;

            BankHi = orig.BankHi;

            LastResult = orig.LastResult;

            IsSplit = orig.IsSplit;

            NumDecks = orig.NumDecks;

            BankRoll = orig.BankRoll;
        }
    }

    [XmlInclude(typeof(TypeGameState))]
    public class BJEngine : TypeGameState
    {

        #region Events
        public EventHandler EndGame;
        public EventHandler DealerFlip;
        public EventHandler Shuffle;
        public EventHandler<ResultEventArgs> Results;
        #endregion

        const string GAME_FILE = "gamestate.xml";
        const string PLAYER_FILE = "player.xml";
        const string DEALER_FILE = "dealer.xml";
        const string SPLIT_FILE = "split.xml";
        const string DECK_FILE = "deck.xml";

        public const int MAX_CHIPS_PER_STACK = 15;

        public const int BANK_DEFAULT = 100;
        public const int HISCORE_DEFAULT = 100;

        public BJEngine()
        {
            DealerHand = new BJHand();
            PlayerHand = new BJHand();
            SplitHand = new BJHand();
            CardSet = new CardSet();
            PlayEndSound = false;
            SetDifficulty(EnumGameDifficulty.Learn);
            Reset();
            
            // defaults
            NumDecks = 4;
            BurnFirstCard = true;
            BlackJack2to1 = false;
            DealerHitsOnSoft17 = false;
            CardsScores21 = false;
            ShowScores = true;
            AllowSurrender = true;
            ShowDealerCard = false;
            BankRoll = StartingBank.b1000;
            
            Read();
        }
        public BJHand DealerHand { get; protected set; }
        public BJHand PlayerHand { get; protected set; }
        public BJHand SplitHand { get; protected set; }
        public CardSet CardSet { get; protected set; }


        //public EnumGameState GameState { get; set; }
        
        public int NumDealerCardsDealt { get; set; }
        public int NumPlayerCardsDealt { get; set; }
        public int NumSplitCardsDealt { get; set; }

        

        public void NewGame()
        {
            NewGame(true);
        }
        public void NewGame(bool bCheckHiScore)
        {
            if (BankHi > BJEngine.HISCORE_DEFAULT && bCheckHiScore)
            {
                int HiScore = BankHi;
                Globals.Settings.HighScores.Add(null, HiScore);
            }
            Reset();
        }

        int GetBankDefault()
        {
            int iBankRoll = BANK_DEFAULT;

            switch (BankRoll)
            {
                case StartingBank.b100:
                    iBankRoll = 100;
                    break;
                case StartingBank.b250:
                    iBankRoll = 250;
                    break;
                case StartingBank.b500:
                    iBankRoll = 500;
                    break;
                case StartingBank.b1000:
                default://////////////////////////
                    iBankRoll = 1000;
                    break;
                case StartingBank.b5000:
                    iBankRoll = 5000;
                    break;
                case StartingBank.b10000:
                    iBankRoll = 10000;
                    break;
            }

            return iBankRoll;

        }


        public void Reset()
        {
            NewHandReset();

            int iBankRoll = GetBankDefault();

            

            Bank = iBankRoll;
            BankHi = BJEngine.HISCORE_DEFAULT;
            PreviousBank = iBankRoll;
            BetAmount = 0;
            PlayEndSound = true;

            Num25Chips = 0;
            Num50Chips = 0;
            Num100Chips = 0;
            Num500Chips = 0;

            m_iCardCount = 0;

            GameState = EnumGameState.Bet;
            CardSet.ChangeNumDecks(NumDecks);
            CardSet.Shuffle();
        }


        public void Save()
        {
        /*    using (var store = IsolatedStorageFile.GetUserStoreForApplication())
            {
                try
                {
                    var stream = store.CreateFile(GAME_FILE);

                    var ser = new XmlSerializer(typeof(TypeGameState));

                    TypeGameState tmp = new TypeGameState();
                    tmp.CopyFrom(this);
                    ser.Serialize(stream, tmp);
                    
                    stream.Close();


                    PlayerHand.Save(PLAYER_FILE);
                    DealerHand.Save(DEALER_FILE);
                    SplitHand.Save(SPLIT_FILE);
                    CardSet.Save(DECK_FILE);

                }
                catch (Exception ex)
                {
                    Debug.WriteLine("Could not save state: " + ex);
                }
            }*/
        }

        public void Read()
        {
         /*   if (DesignerProperties.IsInDesignTool)
                return;

            using (var store = IsolatedStorageFile.GetUserStoreForApplication())
            {
                try
                {
                    using (var stream = store.OpenFile(GAME_FILE, System.IO.FileMode.Open))
                    {

                        var ser = new XmlSerializer(typeof(TypeGameState));

                        TypeGameState sState = (TypeGameState)ser.Deserialize(stream);

                        CopyFrom(sState);

                        PlayerHand.Read(PLAYER_FILE);
                        DealerHand.Read(DEALER_FILE);
                        SplitHand.Read(SPLIT_FILE);
                        CardSet.Read(DECK_FILE);
                    }
                }
                catch (Exception ex)
                {
                    Debug.WriteLine("Could not read game state " + ex);
                    Reset();
                }
            }*/
        }

        //Basically we make a function for every possible Button
        public bool AddBetAmount(int iAmount)
        {
            if ((GameState != EnumGameState.Bet) &&
                    (GameState != EnumGameState.EndOfGame))
                return false;
            else if (GameState == EnumGameState.EndOfGame)
                GameState = EnumGameState.Bet;

            if (iAmount < 0 ||
                iAmount + BetAmount > Bank)
                return false;

            BetAmount += iAmount;
            //Bank -= iAmount;

            return true;
        }

        public bool Add25ChipBet()
        {
            if (Show25() == false)
                return false;


            if (AddBetAmount(25))
            {
                Num25Chips++;
                if (Num25Chips > MAX_CHIPS_PER_STACK)
                {
                    Num25Chips -= 2;
                    Num50Chips++;

                    if (Num50Chips > MAX_CHIPS_PER_STACK)
                    {
                        Num50Chips -= 2;
                        Num100Chips++;

                        if (Num100Chips > MAX_CHIPS_PER_STACK)
                        {
                            Num100Chips -= 5;
                            Num500Chips++;
                        }
                    }
                }
                return true;
            }
            return false;
        }

        public bool Add50ChipBet()
        {
            if (Show50() == false)
                return false;

            if (AddBetAmount(50))
            {
                Num50Chips++;

                if (Num50Chips > MAX_CHIPS_PER_STACK)
                {
                    Num50Chips -= 2;
                    Num100Chips++;

                    if (Num100Chips > MAX_CHIPS_PER_STACK)
                    {
                        Num100Chips -= 5;
                        Num500Chips++;
                    }


                }
                return true;
            }
            return false;
        }

        public bool Add100ChipBet()
        {
            if (Show100() == false)
                return false;

            if (AddBetAmount(100))
            {
                if (Num100Chips < MAX_CHIPS_PER_STACK)
                {
                    Num100Chips++;

                    if (Num100Chips > MAX_CHIPS_PER_STACK)
                    {
                        Num100Chips -= 5;
                        Num500Chips++;
                    }

                    return true;
                }
            }
            return false;
        }

        public bool Add500ChipBet()
        {
            if (Show500() == false)
                return false;

            if (AddBetAmount(500))
            {
                if (Num500Chips < MAX_CHIPS_PER_STACK)
                {
                    Num500Chips++;
                    return true;
                }
            }
            return true;
        }

        public bool DealerCardFaceUp()
        {
            if (GameDifficulty == EnumGameDifficulty.Easy)
                return true;

            switch (GameState)
            {
                case EnumGameState.Bet:
                    break;
                case EnumGameState.Deal:
                    if (ShowDealerCard != false)
                        return true;
                    break;
                case EnumGameState.PlayerFirstMove:
                case EnumGameState.PlayerMove:
                case EnumGameState.SplitMove:
                    return false;
                case EnumGameState.DealerMove:
                case EnumGameState.EndOfGame:
                    break;
            }
            return true;
        }


        public bool ResetBetAmount()
        {
            if ((GameState != EnumGameState.Bet) &&
            (GameState != EnumGameState.EndOfGame))
                return false;
            else if (GameState == EnumGameState.EndOfGame)
                GameState = EnumGameState.Bet;

            if (BetAmount < 1)
                return false;

            //not anymore
           // Bank += BetAmount;

            BetAmount = 0;

            Num25Chips = 0;
            Num50Chips = 0;
            Num100Chips = 0;
            Num500Chips = 0;

            return true;
        }

        public bool Deal()
        {
            if (ShowDealButton() == false)
                return false;

            if (GameState != EnumGameState.Bet &&
                GameState != EnumGameState.EndOfGame)
                return false;

            //decrease on a new hand
            Bank -= BetAmount;

            GameState = EnumGameState.Deal;
            NewHandReset();
            PlayEndSound = true;
            Card nextCard = null;

            //Deal two cards to Player and Dealer
            //might need more control here....or maybe the interface can worry about the animation

            nextCard = CardSet.GetNextCard();
            AddCardToCount(nextCard);
            PlayerHand.AddCard(nextCard);

            nextCard = CardSet.GetNextCard();
            AddCardToCount(nextCard);
            DealerHand.AddCard(nextCard);

            nextCard = CardSet.GetNextCard();
            AddCardToCount(nextCard);
            PlayerHand.AddCard(nextCard);

            nextCard = CardSet.GetNextCard();
            AddCardToCount(nextCard);
            DealerHand.AddCard(nextCard);

            GameState = EnumGameState.PlayerFirstMove;

            return CheckGameStatus();
        }

        public bool PlayDealerHand()
        {
            Card oCard = null;

            if (GameState == EnumGameState.DealerMove)
            {
                //check and see if we even need to play it out
                PlayerHand.GetScore(out m_iLow, out m_iHi);
                if (m_iLow > 21)
                {
                    if (m_iLow > 21)
                    {
                        if (IsSplit)
                            SplitHand.GetScore(out m_iLow, out m_iHi);
                        if (m_iLow > 21)//this is fine .. checks twice for one hand
                            //checks each hand for split
                            return true;
                    }
                }

                DealerHand.GetScore(out m_iLow, out m_iHi);

                //		while(m_iHi < 17) // have to hit
                while (1 == 1)
                {
                    if (m_iHi > 16 && m_iHi < 22)
                        break; //we're all good
                    if (m_iLow > 16)
                        break; //also good
                    oCard = CardSet.GetNextCard();
                    AddCardToCount(oCard);
                    DealerHand.AddCard(oCard);
                    DealerHand.GetScore(out m_iLow, out m_iHi);
                }

                //check for soft 17
                if (DealerHitsOnSoft17 &&
                    m_iLow == 7 && m_iHi == 17) // hit on soft 17 baby
                {
                    oCard = CardSet.GetNextCard();
                    AddCardToCount(oCard);
                    DealerHand.AddCard(oCard);
                }

                EvaluateScores();

                return true;
            }

            return false;
        }

        public bool CheckGameStatus()
        {
            //ie...check the scores for blackjacks, busts, etc

            int iTemp;
            DealerHand.GetScore(out m_iLow, out m_iHi);
            PlayerHand.GetScore(out m_iLow, out iTemp);

            //We'll call this constantly to see what's going on....in case of black jacks, busts, etc it will change the game state
            switch (GameState)
            {
                case EnumGameState.PlayerFirstMove:
                    //check for blackjack
                    {
                        if (iTemp == 21)
                            //blackjack
                            EvaluateScores();
                    }
                    break;
                case EnumGameState.PlayerMove:
                    {
                        if (IsSplit)
                        {
                            if (m_iLow == 21 || iTemp == 21)//check the first two calls .. this is right
                            {
                                Stand();
                                CheckGameStatus();
                            }
                        }
                        else
                        {
                            if (m_iLow > 21)//you're done sucka
                                EvaluateScores();
                            else if (m_iLow == 21 || iTemp == 21)
                                PlayDealerHand();
                        }
                    }
                    break;
                case EnumGameState.SplitMove:
                    {
                        SplitHand.GetScore(out m_iLow, out m_iHi);
                        if (m_iLow >= 21)
                            PlayDealerHand();
                        if (m_iLow == 21 || m_iHi == 21)
                            Stand();
                    }
                    break;
                case EnumGameState.DealerMove:
                    {
                        DealerHand.GetScore(out m_iLow, out m_iHi);
                        if (m_iLow >= 21)
                            EvaluateScores(); //now I know what you're thinking what about holding on 17??...dealt with 
                    }
                    break;
                default:
                    break;

            }
            return true;
        }

        public bool EvaluateScores()
        {
            PreviousBank = Bank;

            //Now we figure out if we've won and do the payouts
            int iPlayer, iDealer, iSplit;
            PlayerHand.GetScore(out m_iLow, out iPlayer);
            DealerHand.GetScore(out m_iLow, out iDealer);
            SplitHand.GetScore(out m_iLow, out iSplit);

            if (CardsScores21)
            {
                if (iPlayer < 22 &&
                    PlayerHand.Count == 5)
                    iPlayer = 21;

                if (iSplit < 22 &&
                    SplitHand.Count == 5)
                    iSplit = 21;

                if (iDealer < 22 &&
                    DealerHand.Count == 5)
                    iDealer = 21;
            }

            ResultSplit = EnumGameResult.NULL;

            //Special Cases first
            if (m_bSurrender)
            {
                m_bSurrender = false;
                ResultPlayer = EnumGameResult.Surrender;
            }
            else
            {	//Player Hand
                if (iPlayer > 21)
                    ResultPlayer = EnumGameResult.Lose;
                else if (iPlayer == iDealer)
                {
                    if (iDealer == 21)
                    {   //we both have bj ... bj doesn't count if you split
                        if (PlayerHand.Count == 2 && DealerHand.Count == 2 && IsSplit == false)
                            ResultPlayer = EnumGameResult.Draw;
                        //we have bj ... dealer doesn't ... will never happen 
                        else if (PlayerHand.Count == 2 && DealerHand.Count > 2 && IsSplit == false)
                            ResultPlayer = EnumGameResult.BlackJack;
                        //we don't have bj but the dealer does               
                        else if (DealerHand.Count == 2)
                            ResultPlayer = EnumGameResult.Lose;
                        else //neither has bj
                            ResultPlayer = EnumGameResult.Draw;
                    }
                    else //less than 21 .. just a draw
                    {
                        ResultPlayer = EnumGameResult.Draw;
                    }
                }
                else if (iPlayer == 21 &&
                    PlayerHand.Count == 2 &&
                    !IsSplit)
                    ResultPlayer = EnumGameResult.BlackJack;
                else if (iDealer > 21)
                    ResultPlayer = EnumGameResult.Win;
                else if (iDealer > iPlayer)
                    ResultPlayer = EnumGameResult.Lose;
                else if (iPlayer > iDealer)
                    ResultPlayer = EnumGameResult.Win;

                if (IsSplit)
                {
                    if (iSplit > 21)
                        ResultSplit = EnumGameResult.Lose;
                    else if (iSplit == iDealer)
                    {
                        //if the dealer has blackjack we lose
                        if (iDealer == 21 &&
                            DealerHand.Count == 2)
                            ResultSplit = EnumGameResult.Lose;
                        else//otherwise its just a tie
                            ResultSplit = EnumGameResult.Draw;
                    }
                    else if (iDealer > 21)
                        ResultSplit = EnumGameResult.Win;
                    else if (iDealer > iSplit)
                        ResultSplit = EnumGameResult.Lose;
                    else
                        ResultSplit = EnumGameResult.Win; //we have a higher score
                }
            }

            //Now lets do the payout...this could be in another function but doesn't really matter

            Bank += BetAmount; //correction for removing the bet when dealing
            switch (ResultPlayer)
            {
                case EnumGameResult.Win:
                    Bank += BetAmount;
                    break;
                case EnumGameResult.BlackJack:
                    if (BlackJack2to1)
                        Bank += 2 * BetAmount;
                    else
                        Bank += 3 * BetAmount / 2;
                    break;
                case EnumGameResult.NULL:
                case EnumGameResult.Draw:
                    //leave it
                    break;
                case EnumGameResult.Lose:
                    Bank -= BetAmount;
                    break;
                case EnumGameResult.Surrender:
                    Bank -= BetAmount / 2;
                    break;
            }

            switch (ResultSplit)
            {
                case EnumGameResult.Win:
                    Bank += BetAmount;
                    break;
                case EnumGameResult.BlackJack:
                    if (BlackJack2to1)
                        Bank += 2 * BetAmount;
                    else
                        Bank += 3 * BetAmount / 2;
                    break;
                case EnumGameResult.NULL:
                case EnumGameResult.Draw:
                    //leave it
                    break;
                case EnumGameResult.Lose:
                    Bank -= BetAmount;
                    break;
            }

            if (IsSplit)
                Bank += BetAmount;//when we split we take another bet out 
            //after the bet is reduced so we put that money back into the bank

            if (_insurance)
            {
                if (DealerHand.Count == 2 &&
                    iDealer == 21)
                    Bank += BetAmount; //Insurance Pays off sucka!
            }

            if (CalcDoubleDown)
            {
                //BetAmount /= 2;
                //Bank += BetAmount;
                CalcDoubleDown = false;
            }

            //should never happen ...
            while (Bank < 0)
            {
                BetAmount--;
                Bank++;
            }

            if (BetAmount > Bank)
            {
                //check for multiples of 25 ...
                BetAmount = (Bank / 25) * 25;
                MatchChipsToBet();
            }

            if (Bank == 0)
                MatchChipsToBet();

            //post the results message
            Results(this, new ResultEventArgs(ResultPlayer, ResultSplit));

            GameState = EnumGameState.EndOfGame;

            if (Bank < 25)//&& BetAmount == 0)
            {	//the game is over ... send a message to the dlg
                EndGame(this, EventArgs.Empty);
            }

            if ((Bank) > BankHi)
                BankHi = Bank;

            return true;
        }

        public bool HasGameStarted()
        {
            if (BetAmount == 0 && Bank == GetBankDefault())
                return false;
            else
                return true;
        }


        //Get some strings for the interface
        public void GetBank(ref string szBank)
        {
            szBank = Bank.ToString();
        }

        public void SetBetAmount(int iBet)
        {
            if (iBet > 0)
                BetAmount = iBet;

            MatchChipsToBet();
        }

        public void GetPlayerScore(ref string szPlayerScore)
        {
            if (ShowScores == false) szPlayerScore = string.Empty;

            else GetScore(ref szPlayerScore, PlayerHand);
        }
        public void GetDealerScore(ref string szDealerScore) //{GetScore(szDealerScore, &m_oDealerHand);};
        {
            szDealerScore = string.Empty;

            if (ShowDealerCard == true ||
                GameState == EnumGameState.EndOfGame)
                GetScore(ref szDealerScore, DealerHand);
        }
        public void GetSplitScore(ref string szSplitScore)
        {
            if (ShowScores == false) szSplitScore = string.Empty;
            else GetScore(ref szSplitScore, SplitHand);
        }

        public int GetCardCount()
        {
            if (GameDifficulty == EnumGameDifficulty.Easy)
                return m_iCardCount;

            //we don't know the dealer card value yet
            if (DealerHand[1] != null && DealerCardFaceUp() == false)
                return m_iCardCount - GetCardCountValue(DealerHand[1]);

            return m_iCardCount;
        }



        public bool Hit()
        {
            Card oCard = null;

            //Normal
            if (GameState == EnumGameState.PlayerMove ||
                GameState == EnumGameState.PlayerFirstMove)
            {
                oCard = CardSet.GetNextCard();
                AddCardToCount(oCard);
                PlayerHand.AddCard(oCard);

                //now check if we've busted for the correct game state
                PlayerHand.GetScore(out m_iLow, out m_iHi);
                if (m_iLow > 21 || //busted
                    (PlayerHand.Count == 5 && CardsScores21)) //done
                {
                    if (IsSplit)
                        GameState = EnumGameState.SplitMove;
                    else
                    {
                        EvaluateScores();
                        if (DealerFlip != null)
                            DealerFlip(this, EventArgs.Empty);
                    }

                }
                else if (m_iLow == 21 || m_iHi == 21)//no reason to hit again 
                {
                    return Stand();
                    /*    if(m_bSplit)
                        {
                            GameState = EnumGameState.SplitMove;
                        }
                        else
                        {
                            GameState = EnumGameState.DealerMove;
                        }*/
                }
                else
                {
                    GameState = EnumGameState.PlayerMove;
                }
            }


            //Split
            else if (GameState == EnumGameState.SplitMove)
            {
                oCard = CardSet.GetNextCard();
                AddCardToCount(oCard);
                SplitHand.AddCard(oCard);

                int iLow, iHi;
                PlayerHand.GetScore(out iLow, out iHi);
                SplitHand.GetScore(out m_iLow, out m_iHi);

                if (m_iLow > 21 && iLow > 21)
                {   //both hands are busted .. we're done
                    EvaluateScores();
                    if (DealerFlip != null)
                        DealerFlip(this, EventArgs.Empty);
                }
                else if (m_iLow > 21)
                {   //just the split hand has busted so dealer gets to play
                    return Stand(); //lazy but works
                }
                //if both hands have blackjack the dealer doesn't play
                else if (m_iLow == 21 && iLow == 21 && PlayerHand.Count == 2 && SplitHand.Count == 2)
                {
                    EvaluateScores();
                    if (DealerFlip != null)
                        DealerFlip(this, EventArgs.Empty);
                }

                //if we have 21 we auto stand
                else if (m_iLow == 21 || m_iHi == 21 ||
                    (SplitHand.Count == 5 && CardsScores21)) //done
                {
                    return Stand();
                    //dealer knows how to play .. it can flip itself
                }
            }
            else
            {
                return false;
            }
            return CheckGameStatus();
        }
        public bool Stand()
        {
            //Normal
            if (GameState == EnumGameState.PlayerFirstMove ||
                GameState == EnumGameState.SplitMove)
            {
                GameState = EnumGameState.DealerMove;
            }
            else if (GameState == EnumGameState.PlayerMove)
            {
                if (IsSplit)
                    GameState = EnumGameState.SplitMove;
                else
                    GameState = EnumGameState.DealerMove;
            }
            else
                return false;

            bool flip = GameState == EnumGameState.DealerMove;

            PlayDealerHand();

            //Make the dealer play out his hand
            if (flip && DealerFlip != null)
                DealerFlip(this, EventArgs.Empty);

            return true;
        }
        public bool Split()
        {
            Card oCard = null;

            if (ShowSplitButton())
            {	//we split!
                Bank -= BetAmount;

                SplitHand.AddCard(PlayerHand[1]);//copy the pointer to the card to the Split Hand
                PlayerHand.RemoveCard(1);	//delete the pointer to the Player Hand
                //Deal two more cards

                oCard = CardSet.GetNextCard();
                AddCardToCount(oCard);
                PlayerHand.AddCard(oCard);

                oCard = CardSet.GetNextCard();
                AddCardToCount(oCard);
                SplitHand.AddCard(oCard);

                IsSplit = true;
                GameState = EnumGameState.PlayerMove;
                return CheckGameStatus();
            }
            return false;
        }
        public bool Insurance()
        {
            if (ShowInsuranceButton())
            {
                Bank -= BetAmount / 2;
                _insurance = true;
                GameState = EnumGameState.PlayerMove;
            }
            return false;
        }
        public bool DoubleDown()//sideways card
        {
            if (ShowDoubleButton())
            {
                IsDoubleDown = true;
                CalcDoubleDown = IsDoubleDown;
                Bank -= BetAmount;
                //BetAmount *= 2; 

                Card oCard = null;

                oCard = CardSet.GetNextCard();
                AddCardToCount(oCard);
                PlayerHand.AddCard(oCard);

                GameState = EnumGameState.DealerMove;
                CheckGameStatus();
                PlayDealerHand();

                //ok this engine is kind of shit
                //int iLow, iHigh;
                //PlayerHand.GetScore(out iLow, out iHigh);
                //if (iHigh > 21)
                    EvaluateScores();
                return true;
            }
            return false;
        }
        public bool Surrender()  //pussy
        {
            if (ShowSurrenderButton())
            {
                m_bSurrender = true;
                EvaluateScores();
                return true;
            }
            return false;
        }
        public bool Bet()
        {
            if (GameState != EnumGameState.EndOfGame)
                return false;

            GameState = EnumGameState.Bet;

            return true;
        }

        //Functions to check states of certain BlackJack Events
        public bool ShowInsuranceButton()
        {
            if (GameState == EnumGameState.PlayerFirstMove &&
            Bank >= BetAmount / 2)
            {
                Card oCard;

                oCard = DealerHand[0];

                if (oCard.Rank == EnumCardRank.Ace)
                {
                    return true;
                }
            }

            return false;
        }
        public bool ShowSplitButton()
        {
            if (GameState == EnumGameState.PlayerFirstMove &&
            Bank >= BetAmount)
            {
//#if !DEBUG	
        Card oTempCard1;
		Card oTempCard2;

		oTempCard1 = PlayerHand[0];
		oTempCard2 = PlayerHand[1];

		if(oTempCard2.GetCardValue()== oTempCard1.GetCardValue() &&
			Bank >= BetAmount)
//#endif
                return true;
            }
            return false;
        }
        public bool ShowDoubleButton()
        {
            if (GameState == EnumGameState.PlayerFirstMove &&
            Bank >= BetAmount)
                return true;
            else
                return false;
        }
        public bool ShowSurrenderButton()
        {
            if (AllowSurrender == false)
                return false;

            if (GameState == EnumGameState.PlayerFirstMove)
            {
                return true;
            }
            return false;
        }
        public bool ShowDealButton()
        {
            if (BetAmount > 0) return true;

            return false;
        }

        //bet buttons
        public bool Show25()
        {
            if (Bank - BetAmount < 25)
                return false;

            if (Num25Chips >= MAX_CHIPS_PER_STACK)
                return false;

            return true;
        }
        public bool Show50()
        {
            if (Bank - BetAmount < 50)
                return false;

            if (Num50Chips >= MAX_CHIPS_PER_STACK)
                return false;

            return true;
        }

        public bool Show100()
        {
            if (Bank - BetAmount < 100)
                return false;

            if (Num100Chips >= MAX_CHIPS_PER_STACK)
                return false;

            return true;
        }

        public bool Show500()
        {
            if (Bank - BetAmount < 500)
                return false;

            if (Num500Chips >= MAX_CHIPS_PER_STACK)
                return false;

            return true;
        }

        public bool ShowDealerScore()
        {
            if (ShowScores == false)
            {
                if (GameState != EnumGameState.EndOfGame)
                    return false;
            }

            if (NumDealerCardsDealt < DealerHand.Count)
                return false;

            switch (GameState)
            {
                case EnumGameState.Bet:
                case EnumGameState.PlayerMove:
                case EnumGameState.SplitMove:
                case EnumGameState.DealerMove:
                    return false;
                case EnumGameState.PlayerFirstMove:
                case EnumGameState.Deal:
                case EnumGameState.EndOfGame:
                    return true;
            }

            return false;
        }

        //just easier here
        public bool IsBJTie()
        {
            if(NumDealerCardsDealt != 2 || NumPlayerCardsDealt != 2 || NumSplitCardsDealt != 0)
                return false;

            if (PlayerHand.GetScore() != 21 || DealerHand.GetScore() != 21)
                return false;

            return true;

        }

        public bool ShowPlayerScore()
        {
            if (ShowScores == false)
            {
                if (GameState != EnumGameState.EndOfGame)
                    return false;
            }

            if (NumPlayerCardsDealt < PlayerHand.Count)
                return false;

            switch (GameState)
            {
                case EnumGameState.PlayerMove:
                case EnumGameState.SplitMove:
                case EnumGameState.DealerMove:
                case EnumGameState.PlayerFirstMove:
                case EnumGameState.Deal:
                case EnumGameState.EndOfGame:
                    return true;
                default:
                case EnumGameState.Bet:
                    break;
            }

            return false;
        }
        public bool ShowSplitScore()
        {
            if (IsSplit == false)
                return false;

            else if (ShowScores == false)
            {
                if (GameState != EnumGameState.EndOfGame)
                    return false;
            }
            if (NumSplitCardsDealt < SplitHand.Count)
                return false;

            switch (GameState)
            {
                case EnumGameState.Bet:
                case EnumGameState.PlayerMove:
                case EnumGameState.SplitMove:
                case EnumGameState.DealerMove:
                //		return false;
                //		break;
                case EnumGameState.PlayerFirstMove:
                case EnumGameState.Deal:
                case EnumGameState.EndOfGame:
                    return true;
            }
            return false;
        }

        public bool ShowResults()
        {
            if (GameState == EnumGameState.EndOfGame)
                return true;
            return false;
        }


        public bool DealAnotherCard()
        {
            if (IsDoubleDown == true && NumPlayerCardsDealt == 2)
            {
                NumPlayerCardsDealt++;
                return true;
            }
            else if (NumDealerCardsDealt < DealerHand.Count)
            {
                NumDealerCardsDealt++;
                return true;
            }
            else if (NumPlayerCardsDealt < PlayerHand.Count)
            {
                NumPlayerCardsDealt++;
                return true;
            }
            else if (NumSplitCardsDealt < SplitHand.Count)
            {
                NumSplitCardsDealt++;
                return true;
            }
            return false;
        }

        public bool DealerBusts()
        {
            int hi, low;

            DealerHand.GetScore(out low, out hi);

            if (low > 21)
                return true;

            return false;
        }



        public bool SetDifficulty(EnumGameDifficulty eDiff)
        {
            /*switch (eDiff)
            {
                case EnumGameDifficulty.Easy:
                    NumDecks = 1;
                    BurnFirstCard = false;
                    BlackJack2to1 = true;
                    DealerHitsOnSoft17 = true;
                    CardsScores21 = true;
                    ShowScores = true;
                    AllowSurrender = true;
                    ShowDealerCard = true;
                    break;
                case EnumGameDifficulty.Normal:
                    NumDecks = 2;
                    BurnFirstCard = false;
                    BlackJack2to1 = false;
                    DealerHitsOnSoft17 = true;
                    CardsScores21 = false;
                    ShowScores = true;
                    AllowSurrender = true;
                    ShowDealerCard = false;
                    break;
                case EnumGameDifficulty.Expert:
                    NumDecks = 4;
                    BurnFirstCard = true;
                    BlackJack2to1 = false;
                    DealerHitsOnSoft17 = false;
                    CardsScores21 = false;
                    ShowScores = false;
                    AllowSurrender = false;
                    ShowDealerCard = false;
                    break;
                case EnumGameDifficulty.Learn:
                    NumDecks = 1;
                    BurnFirstCard = false;
                    BlackJack2to1 = true;
                    DealerHitsOnSoft17 = true;
                    CardsScores21 = false;
                    ShowScores = true;
                    AllowSurrender = true;
                    ShowDealerCard = false;
                    break;
                default:
                    return false;
            }*/

            GameDifficulty = eDiff;

            return true;
        }

        public EnumGameDifficulty GetDifficulty()
        {
            return GameDifficulty;
        }


        public bool IsFinishedDealing()
        {
            if (IsDoubleDown == true && NumPlayerCardsDealt == 2)
                return false;
            else if (NumDealerCardsDealt < DealerHand.Count)
                return false;
            else if (NumPlayerCardsDealt < PlayerHand.Count)
                return false;
            else if (NumSplitCardsDealt < SplitHand.Count)
                return false;
            return true;
        }


        static readonly EnumNextMove[,] ePairs = new EnumNextMove[10, 10]
    {   //a   , 2    , 3    , 4    , 5    , 6    , 7    , 8    , 9    , 10      //dealer shows ...
        {EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split},//a
        {EnumNextMove.Hit , EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit },//2
        {EnumNextMove.Hit , EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit },//3
        {EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit },//4
        {EnumNextMove.Hit , EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.Hit },//5
        {EnumNextMove.Hit , EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit },//6
        {EnumNextMove.Hit , EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit },//7
        {EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split},//8
        {EnumNextMove.Stand , EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Stand , EnumNextMove.Split, EnumNextMove.Split, EnumNextMove.Stand },//9
        {EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand }//10
    };

        static readonly EnumNextMove[,] eSoft = new EnumNextMove[10, 10]
    {   //a   , 2    , 3    , 4    , 5    , 6    , 7    , 8    , 9    , 10      //dealer shows ...
        {EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit },//a .. should split but we'll always hit if not
        {EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit },//2
        {EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit },//3
        {EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit },//4
        {EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit },//5
        {EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit },//6
        {EnumNextMove.Hit , EnumNextMove.Stand , EnumNextMove.DoubleOrStand, EnumNextMove.DoubleOrStand, EnumNextMove.DoubleOrStand, EnumNextMove.DoubleOrStand, EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Hit },//7
        {EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand },//8
        {EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand },//9
        {EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand }//10 this is blackjack holmes
    };

        //and finally ... based on the total values
        static readonly EnumNextMove[,] eHard = new EnumNextMove[9, 10]
    {   //a   , 2    , 3    , 4    , 5    , 6    , 7    , 8    , 9    , 10      //dealer shows ...
        {EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand },//17-20
        {EnumNextMove.Surrender, EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Hit , EnumNextMove.Surrender, EnumNextMove.Surrender, EnumNextMove.Surrender},//16
        {EnumNextMove.Hit , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Surrender},//15
        {EnumNextMove.Hit , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit },//13-14
        {EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Stand , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit },//12
        {EnumNextMove.Hit , EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit},//11
        {EnumNextMove.Hit , EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.Hit },//10
        {EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.DoubleOrHit, EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit },//9
        {EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit , EnumNextMove.Hit },//5-8
    };
        //add functions for advanced options if time permits

        public EnumNextMove GetNextMove()
        {
            EnumNextMove eMove = EnumNextMove.Nothing;

            BJHand oPlayerHand = null;

            if (GameState == EnumGameState.SplitMove)
                oPlayerHand = SplitHand;
            else
                oPlayerHand = PlayerHand;

            if (oPlayerHand == null)
                goto End;

            if (DealerHand.Count != 2 || oPlayerHand.Count < 2)
                goto End;

            if (GameState != EnumGameState.PlayerMove &&
                GameState != EnumGameState.PlayerFirstMove &&
                GameState != EnumGameState.SplitMove)
                goto End;



            //we'll do pairs first
            if (oPlayerHand.Count == 2 &&
                oPlayerHand[0].GetCardValue() == oPlayerHand[1].GetCardValue())
            {   //0 position is a joker for card value
                if (ShowSplitButton())
                {
                    eMove = ePairs[oPlayerHand[0].GetCardValue() - 1, DealerHand[0].GetCardValue() - 1];
                    goto End;
                }
                //otherwise we'll fall through to the hard and soft totals
            }

            //might as well do the soft totals next
            int iHi, iLow;
            oPlayerHand.GetScore(out iLow, out iHi);
            if (iLow != iHi && iHi < 22)
            {   //we have a valid soft score
                int iNonAce = iLow - 1;
                eMove = eSoft[iNonAce - 1, DealerHand[0].GetCardValue() - 1];
                goto End;
            }

            //otherwise we've got some hard totals going on
            int iIndex = 0;
            if (iLow > 16)
                iIndex = 0;
            else if (iLow > 15)
                iIndex = 1;
            else if (iLow > 14)
                iIndex = 2;
            else if (iLow > 12)
                iIndex = 3;
            else if (iLow > 11)
                iIndex = 4;
            else if (iLow > 10)
                iIndex = 5;
            else if (iLow > 9)
                iIndex = 6;
            else if (iLow > 8)
                iIndex = 7;
            else
                iIndex = 8;

            eMove = eHard[iIndex, DealerHand[0].GetCardValue() - 1];


        End:
            //now we'll correct the choices with two possible moves
            if (eMove == EnumNextMove.DoubleOrHit && ShowDoubleButton() == false)
                eMove = EnumNextMove.Hit;
            if (eMove == EnumNextMove.DoubleOrStand && ShowDoubleButton() == false)
                eMove = EnumNextMove.Stand;
            if (eMove == EnumNextMove.DoubleOrStand || eMove == EnumNextMove.DoubleOrHit)
                eMove = EnumNextMove.Double;
            if (eMove == EnumNextMove.Surrender && ShowSurrenderButton() == false)
                eMove = EnumNextMove.Hit;

            return eMove;
        }

        private void GetScore(ref string szScore, BJHand oHand)
        {
            szScore = string.Empty;

            if (ShowScores == false)
                return;

            oHand.GetScore(out m_iLow, out m_iHi);

            if (m_iHi < 22 &&
                m_iLow != m_iHi)
            {	//show both scores
                string sz2ndScore = m_iHi.ToString();
                szScore = m_iLow.ToString();

                szScore += " (";
                szScore += sz2ndScore;
                szScore += ")";
            }
            else
            {
                szScore = m_iLow.ToString();
            }

            if (m_iLow == 0)
                szScore = string.Empty;
        }

        private void NewHandReset()
        {
            IsSplit = false;
            IsDoubleDown = false;
            CalcDoubleDown = false;
            m_bSurrender = false;
            _insurance = false;

            //Clear the Hands
            DealerHand.ClearHand();
            PlayerHand.ClearHand();
            SplitHand.ClearHand();

            NumDealerCardsDealt = 0;
            NumPlayerCardsDealt = 0;
            NumSplitCardsDealt = 0;

            //and finally check if we need to shuffle the decks
            //maybe we should toss up an indicator??
            const int CARD_LIMIT = 20;
            if (CardSet.GetCardsLeft() <= CARD_LIMIT)
            {
                CardSet.Shuffle();
                m_iCardCount = 0;
                if (Shuffle != null)
                    Shuffle(this, EventArgs.Empty);
            }
        }

        private bool AddCardToCount(Card oCard)
        {
            if (oCard == null)
                return false;

            //subtract by one because 0 rank is Joker
            m_iCardCount += GetCardCountValue(oCard);

            return true;
        }

        static readonly int[,] _iCountValues = new int[(int)EnumCardCount.Count, 13]
    {   //Ace 2  3  4  5  6  7  8  9 10  J  Q  K
        {-1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},//wizard
        {-1, 1, 1, 1, 1, 1, 1, 0, 0,-1,-1,-1,-1},//KO
        {-1, 1, 1, 1, 1, 1, 0, 0, 0,-1,-1,-1,-1},//HiLow
        { 0, 0, 1, 1, 1, 1, 0, 0, 0,-1,-1,-1,-1},//HiOpt1
        { 0, 1, 1, 2, 2, 1, 1, 0, 0,-2,-2,-2,-2},//HiOpt2
        {-1, 1, 1, 2, 2, 2, 1, 0, 0,-2,-2,-2,-2},//Zen
        { 0, 1, 1, 2, 2, 2, 1, 0,-1,-2,-2,-2,-2}//OmegaII
    };

        private int GetCardCountValue(Card oCard)
        {
            return _iCountValues[(int)Globals.Settings.CountSystem, oCard.GetCardRank() - 1];
        }

        private void MatchChipsToBet()
        {
            int iTemp = BetAmount;

            //zero and recalculate
            Num500Chips = 0;
            Num100Chips = 0;
            Num50Chips = 0;
            Num25Chips = 0;

            while (iTemp >= 500 && Num500Chips < MAX_CHIPS_PER_STACK)
            {
                Num500Chips++;
                iTemp -= 500;
            }
            while (iTemp >= 100 && Num100Chips < MAX_CHIPS_PER_STACK)
            {
                Num100Chips++;
                iTemp -= 100;
            }
            while (iTemp >= 50 && Num50Chips < MAX_CHIPS_PER_STACK)
            {
                Num50Chips++;
                iTemp -= 50;
            }
            while (iTemp >= 25 && Num25Chips < MAX_CHIPS_PER_STACK)
            {
                Num25Chips++;
                iTemp -= 25;
            }

            if (iTemp > 0)
            {
                Bank += iTemp;//should never happen
                BetAmount -= iTemp;
            }
        }





        public bool CardsScores21 { get; private set; }
        public bool AllowSurrender { get; private set; }


        public bool PlayEndSound { get; private set; }


        public bool IsDoubleDown;
        private bool CalcDoubleDown { get; set; }

        private EnumGameResult ResultPlayer { get; set; }
        private EnumGameResult ResultSplit { get; set; }



        public int PreviousBank { get; private set; }
        int m_iLow, m_iHi; //temps but used ALL of the time

        bool m_bSurrender;
        private bool _insurance;

        public static void PlaySound(string res)
        {
            if (Globals.Settings.bPlaySounds == false)
                return;

            Uri uriSound = new Uri(@res, UriKind.Relative);

            StreamResourceInfo sri = App.GetResourceStream(uriSound);

            if (sri == null)
                return;

            var player = new SoundPlayer(sri.Stream);

            //var player = new SoundPlayer(res);
            player.Play();
            
         /*   SoundEffect sfx = SoundEffect.FromStream(TitleContainer.OpenStream(res));
            sfx.Play();
            FrameworkDispatcher.Update();*/
        }
    }

    public class ResultEventArgs : EventArgs
    {
        public ResultEventArgs()
        {
            Player = EnumGameResult.NULL;
            Split = EnumGameResult.NULL;
        }

        public ResultEventArgs(EnumGameResult player, EnumGameResult split)
        {
            Player = player;
            Split = split;
        }

        public static new ResultEventArgs Empty = new ResultEventArgs(EnumGameResult.NULL, EnumGameResult.NULL);

        public EnumGameResult Player { get; set; }
        public EnumGameResult Split { get; set; }
    }
}
