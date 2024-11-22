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
using System.Windows.Media.Imaging;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.IsolatedStorage;
using System.Xml.Serialization;

namespace BlackJack.Objects
{
    public enum EnumAchievement
    {
        Woooh,
        OnFire,
        TheSplit,
        TwentyFiveK,
        FiftyK,
        SeventyFiveK,
        OneHundredK,
        LuckyBreak,
        TripUp,
        RunningCount,
        HighRoller,
        IHate8s,
        TheGambler,
        DoubleTrouble,
        MaxAchievment
    }

    public enum EnumAchievementValue
    {
        Five = 5, Ten = 10, Fifteen = 15, Twenty = 20, TwentyFive = 25
    }


    [XmlInclude(typeof(WooohAchievement))]
    [XmlInclude(typeof(IHate8sAchievement))]
    [XmlInclude(typeof(OnFireAchievement))]
    [XmlInclude(typeof(TheSplitAchievement))]
    [XmlInclude(typeof(TwentyFiveKAchievement))]
    [XmlInclude(typeof(FifthKAchievement))]
    [XmlInclude(typeof(SeventyFiveKAchievement))]
    [XmlInclude(typeof(OneHunderedKAchievement))]
    [XmlInclude(typeof(LuckyBreakAchievement))]
    [XmlInclude(typeof(TripUpAchievement))]
    [XmlInclude(typeof(RunningCountAchievement))]
    [XmlInclude(typeof(HighRollerAchievement))]
    [XmlInclude(typeof(TheGamblerAchievement))]
    [XmlInclude(typeof(DoubleTroubleAchievement))]
    public abstract class Achievement
    {
        protected BitmapImage _ptsImage;
        protected BitmapImage _ptsImageGrey;
        protected string _desc;
        protected string _title;

        public Achievement() {
            switch (Points)
            {
                case EnumAchievementValue.Five:
                    _ptsImage = Globals.Gui.Skin.imgPnts5;
                    _ptsImageGrey = Globals.Gui.Skin.imgPnts5Grey;
                    break;
                case EnumAchievementValue.Ten:
                    _ptsImage = Globals.Gui.Skin.imgPnts10;
                    _ptsImageGrey = Globals.Gui.Skin.imgPnts10Grey;
                    break;
                case EnumAchievementValue.Fifteen:
                    _ptsImage = Globals.Gui.Skin.imgPnts15;
                    _ptsImageGrey = Globals.Gui.Skin.imgPnts15Grey;
                    break;
                case EnumAchievementValue.Twenty:
                    _ptsImage = Globals.Gui.Skin.imgPnts20;
                    _ptsImageGrey = Globals.Gui.Skin.imgPnts20Grey;
                    break;
                case EnumAchievementValue.TwentyFive:
                    _ptsImage = Globals.Gui.Skin.imgPnts25;
                    _ptsImageGrey = Globals.Gui.Skin.imgPnts25Grey;
                    break;
            }
        }

        [XmlIgnore]
        public string Title { get { return _title; } }
        [XmlIgnore]
        public string Desc { get { return _desc; } }

        [XmlIgnore]
        public string Color { get { return Completed ? "Yellow" : "White"; } }

        public abstract EnumAchievement Value { get; }

        [XmlIgnore]
        public abstract BitmapImage Image { get; }

        public abstract bool Check(ResultEventArgs args);

        [XmlIgnore]
        public double Locked
        {
            get
            {
                return Completed ? 0 : 0.9;
            }
        }

        [XmlIgnore]
        public abstract EnumAchievementValue Points
        {
            get;
        }
        [XmlIgnore]
        public BitmapImage PointImage
        {
            get
            {
                return Completed ? _ptsImage : _ptsImageGrey;
            }
        }
        public bool Completed { get; set; }
    }

    public class IHate8sAchievement : Achievement
    {
        private int _8sCount;

        public IHate8sAchievement()
        {
            _desc = Globals.Res.GetString("IDS_ACH1_DESC");
            _title = Globals.Res.GetString("IDS_ACH1_TITLE");
        }
        public override BitmapImage Image
        {
            get { return Globals.Gui.Skin.imgAchieve8s; }
        }

        public override EnumAchievementValue Points
        {
            get { return EnumAchievementValue.Five; }
        }

        public override EnumAchievement Value
        {
            get { return EnumAchievement.IHate8s; }
        }

        public override bool Check(ResultEventArgs args)
        {
            int check8s = 0;

            // check player hand
            foreach (var c in Globals.Game.PlayerHand)
            {
                // look for two cards with a value of 8
                if (c.GetCardValue() == 8 && ++check8s == 2)
                {
                    // check if we've had a pair of 8s twice
                    if (++_8sCount == 2)
                    {
                        Completed = true;
                        return true;
                    }

                }
            }

            check8s = 0;

            // check split hand
            foreach (var c in Globals.Game.SplitHand)
            {
                // look for two cards with a value of 8
                if (c.GetCardValue() == 8 && ++check8s == 2)
                {
                    // check if we've had a pair of 8s twice
                    if (++_8sCount == 2)
                    {
                        Completed = true;
                        return true;
                    }

                }
            }
            return false;
        }
    }

    public class WooohAchievement : Achievement
    {
        public WooohAchievement()
        {
            _desc = Globals.Res.GetString("IDS_ACH2_DESC");
            _title = Globals.Res.GetString("IDS_ACH2_TITLE");
        }

        public override BitmapImage Image
        {
            get { return Globals.Gui.Skin.imgAchieveWooh; }
        }

        public override EnumAchievementValue Points
        {
            get { return EnumAchievementValue.Ten; }
        }

        public override EnumAchievement Value
        {
            get { return EnumAchievement.Woooh; }
        }

        public override bool Check(ResultEventArgs args)
        {
            int count5 = 0;

            // check single deck game
            // removed - Adrian .. way to hard
            //if (Globals.Game.CardSet.m_iNumDecks != 1)
            //    return false;

            // check for a 21 player hand
            if (Globals.Game.PlayerHand.GetScore() == 21)
            {
                // count cards with a value of 5
                foreach (var c in Globals.Game.PlayerHand)
                {
                    if(c.GetCardValue() == 5)
                    {
                        ++count5;
                    }
                }

                // check if all 5s made an appearance
                if (count5 == 4)
                {
                    Completed = true;
                    return true;
                }
            }

            count5 = 0;

            // check for 21 in split hand
            if (Globals.Game.SplitHand.GetScore() == 21)
            {
                // count cards with value of 5
                foreach (var c in Globals.Game.SplitHand)
                {
                    if(c.GetCardValue() == 5)
                    {
                        ++count5;
                    }
                }

                // check if all 5s made an appearance
                if (count5 == 4)
                {
                    Completed = true;
                    return true;
                }
            }
            return false;
        }
    }

    public class OnFireAchievement : Achievement
    {
        private int _onFireCount;

        public OnFireAchievement()
        {
            _desc = Globals.Res.GetString("IDS_ACH3_DESC");
            _title = Globals.Res.GetString("IDS_ACH3_TITLE");
        }

        public override BitmapImage Image
        {
            get { return Globals.Gui.Skin.imgAchieveFire; }
        }

        public override EnumAchievementValue Points
        {
            get { return EnumAchievementValue.Fifteen; }
        }

        public override EnumAchievement Value
        {
            get { return EnumAchievement.OnFire; }
        }

        public override bool Check(ResultEventArgs args)
        {
            // if we're not in black in any hand reset counter and return
            if (args.Player != EnumGameResult.BlackJack && args.Split != EnumGameResult.BlackJack)
            {
                _onFireCount = 0;
                return false;
            }

            // increment count twice if both hands are in blackjack, otherwise increment once
            int mod = (args.Player == EnumGameResult.BlackJack && args.Split == EnumGameResult.BlackJack) ? 2 : 1;

            _onFireCount += mod;

            // check if we've had a blackjack 3 times in a row
            if (_onFireCount >= 3)
            {
               Completed = true;
            }

            return Completed;
        }
    }

    public class TheSplitAchievement : Achievement
    {
        public TheSplitAchievement()
        {
            _desc = Globals.Res.GetString("IDS_ACH4_DESC");
            _title = Globals.Res.GetString("IDS_ACH4_TITLE");
        }

        public override BitmapImage Image
        {
            get { return Globals.Gui.Skin.imgAchieveSplit; }
        }
        public override EnumAchievementValue Points
        {
            get { return EnumAchievementValue.Twenty; }
        }
        
        public override EnumAchievement Value
        {
            get { return EnumAchievement.TheSplit; }
        }
        public override bool Check(ResultEventArgs args)
        {
            // check if both hands are 21
            if (Globals.Game.PlayerHand.GetScore() == 21 && Globals.Game.SplitHand.GetScore() == 21)
            {
                Completed = true;
            }
            return Completed;
        }
    }

    public class TwentyFiveKAchievement : Achievement
    {
        public TwentyFiveKAchievement()
        {
            _desc = Globals.Res.GetString("IDS_ACH5_DESC");
            _title = Globals.Res.GetString("IDS_ACH5_TITLE");
        }

        public override BitmapImage Image
        {
            get { return Globals.Gui.Skin.imgAchieve25k; }
        }
        public override EnumAchievementValue Points
        {
            get { return EnumAchievementValue.Five; }
        }
        public override EnumAchievement Value
        {
            get { return EnumAchievement.TwentyFiveK; }
        }
        public override bool Check(ResultEventArgs args)
        {
            // make sure we're in normal difficulty
            //if (Globals.Game.GameDifficulty != EnumGameDifficulty.Normal)
            //    return false;

            // check if our bankroll 25k+
            if (Globals.Game.Bank >= 25000)
            {
                Completed = true;
            }

            return Completed;
        }
    }

    public class FifthKAchievement : Achievement
    {
        public FifthKAchievement()
        {
            _desc = Globals.Res.GetString("IDS_ACH6_DESC");
            _title = Globals.Res.GetString("IDS_ACH6_TITLE");
        }

        public override BitmapImage Image
        {
            get { return Globals.Gui.Skin.imgAchieve50k; }
        }
        public override EnumAchievementValue Points
        {
            get { return EnumAchievementValue.Ten; }
        }
        public override EnumAchievement Value
        {
            get { return EnumAchievement.FiftyK; }
        }
        public override bool Check(ResultEventArgs args)
        {
            // ensure game difficulty is normal
            //if (Globals.Game.GameDifficulty != EnumGameDifficulty.Normal)
            //    return false;

            // check if bank roll is 50k+
            if (Globals.Game.Bank >= 50000)
            {
                Completed = true;
            }

            return Completed;
        }
    }

    public class SeventyFiveKAchievement : Achievement
    {
        public SeventyFiveKAchievement()
        {
            _desc = Globals.Res.GetString("IDS_ACH7_DESC");
            _title = Globals.Res.GetString("IDS_ACH7_TITLE");
        }

        public override BitmapImage Image
        {
            get { return Globals.Gui.Skin.imgAchieve75g; }
        }
        public override EnumAchievementValue Points
        {
            get { return EnumAchievementValue.Ten; }
        }
        public override EnumAchievement Value
        {
            get { return EnumAchievement.SeventyFiveK; }
        }
        public override bool Check(ResultEventArgs args)
        {
            // ensure game difficulty is expert
            //if (Globals.Game.GameDifficulty != EnumGameDifficulty.Expert)
            //    return false;

            // check bank roll is 75K+
            if (Globals.Game.Bank >= 75000)
            {
                Completed = true;
            }

            return Completed;
        }
    }

    public class OneHunderedKAchievement : Achievement
    {
        public OneHunderedKAchievement()
        {
            _desc = Globals.Res.GetString("IDS_ACH8_DESC");
            _title = Globals.Res.GetString("IDS_ACH8_TITLE");
        }

        public override BitmapImage Image
        {
            get { return Globals.Gui.Skin.imgAchieve100k; }
        }
        public override EnumAchievementValue Points
        {
            get { return EnumAchievementValue.Fifteen; }
        }
        public override EnumAchievement Value
        {
            get { return EnumAchievement.OneHundredK; }
        }
        public override bool Check(ResultEventArgs args)
        {
            // check difficulty is expert
            if (Globals.Game.GameDifficulty != EnumGameDifficulty.Expert)
                return false;

            // check bank roll is 100K+
            if (Globals.Game.Bank >= 100000)
            {
                Completed = true;
            }

            return Completed;
        }
    }

    public class LuckyBreakAchievement : Achievement
    {
        public LuckyBreakAchievement()
        {
            _desc = Globals.Res.GetString("IDS_ACH9_DESC");
            _title = Globals.Res.GetString("IDS_ACH9_TITLE");
        }
        public override BitmapImage Image
        {
            get { return Globals.Gui.Skin.imgAchieveLucky; }
        }
        public override EnumAchievementValue Points
        {
            get { return EnumAchievementValue.TwentyFive; }
        }
        public override EnumAchievement Value
        {
            get { return EnumAchievement.LuckyBreak; }
        }
        public override bool Check(ResultEventArgs args)
        {
            // check if player has won
            if (args.Player == EnumGameResult.Win)
            {
                // check if we won with 8 or more cards
                if (Globals.Game.PlayerHand.Count >= 8)
                {
                    Completed = true;
                }
            }

            // check is split hand one
            if (args.Split == EnumGameResult.Win)
            {
                // check if we won with 8 or more cards
                if (Globals.Game.SplitHand.Count >= 8)
                {
                    Completed = true;
                }
            }

            return Completed;
        }
    }

    public class TripUpAchievement : Achievement
    {
        public int _dealerBustCount;

        public TripUpAchievement()
        {
            _desc = Globals.Res.GetString("IDS_ACH10_DESC");
            _title = Globals.Res.GetString("IDS_ACH10_TITLE");
        }
        public override BitmapImage Image
        {
            get { return Globals.Gui.Skin.imgAchieveTrip; }
        }
        public override EnumAchievementValue Points
        {
            get { return EnumAchievementValue.Fifteen; }
        }
        public override EnumAchievement Value
        {
            get { return EnumAchievement.TripUp; }
        }
        public override bool Check(ResultEventArgs args)
        {
            // if the dealer hasn't busted, reset counter and short circuit
            if (Globals.Game.DealerHand.GetScore() <= 21)
            {
                _dealerBustCount = 0;
                return false;
            }

            // check if we have busted 5 times in a row
            if (++_dealerBustCount == 5)
            {
                Completed = true;
            }

            return Completed;
        }
    }

    public class RunningCountAchievement : Achievement
    {
        private int _winCount;

        public RunningCountAchievement()
        {
            _desc = Globals.Res.GetString("IDS_ACH11_DESC");
            _title = Globals.Res.GetString("IDS_ACH11_TITLE");
        }
        public override BitmapImage Image
        {
            get { return Globals.Gui.Skin.imgAchieveRunning; }
        }
        public override EnumAchievementValue Points
        {
            get { return EnumAchievementValue.Twenty; }
        }
        public override EnumAchievement Value
        {
            get { return EnumAchievement.RunningCount; }
        }
        public override bool Check(ResultEventArgs args)
        {
            // if we're not card counting, short circuit
            if (Globals.Settings.CardCountView == EnumCardCountView.Off)
            {
                _winCount = 0;
                return false;
            }
            
            // if either hand has not won or blackjacked, short circuit
            if(args.Player != EnumGameResult.Win && args.Player != EnumGameResult.BlackJack &&
                args.Split != EnumGameResult.Win && args.Split != EnumGameResult.BlackJack)
            {
                _winCount = 0;
                return false;
            }

            // check if we've won 10 times in a row
            if (++_winCount == 10)
            {
                Completed = true;
            }

            return Completed;
        }  
    }

    public class HighRollerAchievement : Achievement
    {
        public HighRollerAchievement()
        {
            _desc = Globals.Res.GetString("IDS_ACH12_DESC");
            _title = Globals.Res.GetString("IDS_ACH12_TITLE");
        }
        public override BitmapImage Image
        {
            get { return Globals.Gui.Skin.imgAchieveHr; }
        }
        public override EnumAchievementValue Points
        {
            get { return EnumAchievementValue.Ten; }
        }
        public override EnumAchievement Value
        {
            get { return EnumAchievement.HighRoller; }
        }
        public override bool Check(ResultEventArgs args)
        {
            // ensure a high count
            if (Globals.Game.GetCardCount() < 4)
                return false;

            // check if we've maxed our bet
            if (!Globals.Game.Show25() && !Globals.Game.Show50() &&
                !Globals.Game.Show100() && !Globals.Game.Show500())
            {
                Completed = true;
            }

            return Completed;
        }
    }

    public class TheGamblerAchievement : Achievement
    {
        public TheGamblerAchievement()
        {
            _desc = Globals.Res.GetString("IDS_ACH13_DESC");
            _title = Globals.Res.GetString("IDS_ACH13_TITLE");
        }

        public override BitmapImage Image
        {
            get { return Globals.Gui.Skin.imgAchieveGam; }
        }
        public override EnumAchievementValue Points
        {
            get { return EnumAchievementValue.Five; }
        }
        public override EnumAchievement Value
        {
            get { return EnumAchievement.TheGambler; }
        }
        public override bool Check(ResultEventArgs args)
        {
            if (Completed) return false;

            
            
            //gross
            if (Globals.Game.PlayerHand.LastCard == null)
                Completed = true;

            // null result means we've checked somewhere other than the end of the hand (should be on a hit)
            // check for a score greater than 18
            else if ((args == null || args.Player == EnumGameResult.NULL) && 
                (Globals.Game.PlayerHand.GetScore() - Globals.Game.PlayerHand.LastCard.GetCardValue()) >= 18)
            {
                Completed = true;
            }
            if (Globals.Game.IsSplit && args.Split == EnumGameResult.NULL && Globals.Game.SplitHand.LastCard != null
                && (Globals.Game.SplitHand.GetScore() - Globals.Game.SplitHand.LastCard.GetCardValue()) >= 18)
            {
                Completed = true;
            }

            return Completed;
        }
    }

    public class DoubleTroubleAchievement : Achievement
    {
        private int _doubleCount;
        private bool fDoublePlr;
        private bool fDoubleSplit;

        public DoubleTroubleAchievement()
        {
            _desc = Globals.Res.GetString("IDS_ACH14_DESC");
            _title = Globals.Res.GetString("IDS_ACH14_TITLE");
        }

        public override BitmapImage Image
        {
            get { return Globals.Gui.Skin.imgAchieveDouble; }
        }
        public override EnumAchievementValue Points
        {
            get { return EnumAchievementValue.Fifteen; }
        }
        public override EnumAchievement Value
        {
            get { return EnumAchievement.DoubleTrouble; }
        }
        public override bool Check(ResultEventArgs args)
        {
            if (Completed) return false;

            if (!Globals.Game.IsDoubleDown)
            {
                fDoublePlr = fDoubleSplit = false;
                return false;
            }

            // if method is called on a double click, check if score is greater than 12 and short circuit
            if(fDoublePlr)
            {
                    if ((Globals.Game.PlayerHand.GetScore() - Globals.Game.PlayerHand.LastCard.GetCardValue()) >= 12)
                    {
                        Completed = (++_doubleCount >= 5);
                    }

            }
            else if (args.Player == EnumGameResult.Win)// && ++_doubleCount >= 5)
            {
                fDoublePlr = true;
            }
            if(fDoubleSplit)
            {
                    if ((Globals.Game.SplitHand.GetScore() - Globals.Game.SplitHand.LastCard.GetCardValue()) >= 12)
                    {
                        Completed = (++_doubleCount >= 5);
                    }
            }
            else if (args.Split == EnumGameResult.Win)// && ++_doubleCount >= 5)
            {
                fDoubleSplit = true;
            }
            
            return Completed;
        }
    }

    public class Achievements : IEnumerable<Achievement>
    {
        const string ACHIEVEMENT_FILE = "achievements.xml";

        Achievement[] _list;

        public EventHandler<EventArgs> OnCompleted;

        public Achievements()
        {
            _list = new Achievement[(int)EnumAchievement.MaxAchievment];

            Add(new IHate8sAchievement());
            Add(new WooohAchievement());
            Add(new OnFireAchievement());
            Add(new TheSplitAchievement());
            Add(new TwentyFiveKAchievement());
            Add(new FifthKAchievement());
            Add(new SeventyFiveKAchievement());
            Add(new OneHunderedKAchievement());
            Add(new LuckyBreakAchievement());
            Add(new TripUpAchievement());
            Add(new RunningCountAchievement());
            Add(new HighRollerAchievement());
            Add(new TheGamblerAchievement());
            Add(new DoubleTroubleAchievement());
        }

        public void Add(Achievement ach)
        {
            _list[(int)ach.Value] = ach;
        }
        public Achievement this[EnumAchievement index]
        {
            get { return _list[(int)index]; }
            set { _list[(int)index] = value; }
        }

        public bool IsCompleted(EnumAchievement a)
        {
            return _list[(int)a].Completed;
        }

        public int TotalPoints()
        {
            int total = 0;

            foreach (var a in _list)
            {
                if (a.Completed)
                {
                    total += (int) a.Points;
                }
            }

            return total;
        }

        public void Check(ResultEventArgs args)
        {
            foreach (var a in _list)
            {
                if (a.Completed) continue;

                if (a.Check(args) && OnCompleted != null)
                    OnCompleted(a, EventArgs.Empty);
            }
        }

        public void Check(EnumAchievement value, ResultEventArgs args)
        {
            var a = _list[(int)value];

            if (a.Check(args) && OnCompleted != null)
                OnCompleted(a, EventArgs.Empty);
        }


        #region IEnumerable<Achievement> Members

        public IEnumerator<Achievement> GetEnumerator()
        {
            foreach (var c in _list)
                yield return c;
        }

        #endregion

        #region IEnumerable Members

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            foreach (var c in _list)
                yield return c;
        }

        #endregion

        public bool Save()
        {

                XmlSerializer ser = new XmlSerializer(typeof(Achievement[]));

                try
                {
                   TextWriter writer = new StreamWriter(ACHIEVEMENT_FILE);

                   ser.Serialize(writer, _list);

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
            try
            {
                //If user choose to save, create a new file
                XmlSerializer ser = new XmlSerializer(typeof(Achievement[]));
                TextReader reader = new StreamReader(ACHIEVEMENT_FILE);

                _list = (Achievement[])ser.Deserialize(reader);

                reader.Close();

            }
            catch (Exception ex)
            {
                Debug.WriteLine("cannot read achievements: " + ex);
            }
        }
    }
}
