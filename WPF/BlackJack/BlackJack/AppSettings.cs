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
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using BlackJack.Objects;
using BlackJack.Properties;
using System.Xml.Serialization;

namespace BlackJack
{
    public class AppSerialize
    {
        public bool bPlaySounds;
        public int iGameDif;
        public int iCountSys;
        public int iCardCountView;
    }


    public class AppSettings
    {
        public bool bPlaySounds;
        public const string SOUND_FLIP = "Assets/Sounds/crdflp2.wav";
        public const string SOUND_SHUFFLE = "Assets/Sounds/shuffle.wav";
        public const string SOUND_BET = "Assets/Sounds/bet.wav";
        public const string SOUND_BLACKJACK = "Assets/Sounds/bj.wav";
        public const string SOUND_CHA_CHING = "Assets/Sounds/chaching.wav";
        public const string SOUND_LOSE = "Assets/Sounds/loose.wav";
        public const string SOUND_ACHIEVE = "Assets/Sounds/achievement.wav";
        public const string SOUND_COUGH = "Assets/Sounds/cough.wav";
        public const string SOUND_CLAP = "Assets/Sounds/slow-clap.wav";

        const string REG_PlaySounds = "playsounds";
        const string REG_GameDifficulty = "gamedifficulty";
        const string REG_CountSystem = "countsystem";
        const string REG_CardCountView = "cardcountview";
        const string REG_NumDecks = "numdecks";
        const string REG_BurnFirst = "burnfirst";
        const string REG_PayoutMode = "payoutmode";
        const string REG_DealerHitOnSoft17 = "dealerhitonsoft17";
        const string REG_ShowScores = "showscores";
        const string REG_ShowDealerCard = "showdealercard";

        public HighScores HighScores = new HighScores();

        public Achievements Achievements = new Achievements();

        public AppSettings()
        {
            // set defaults
            bPlaySounds = true;
            CountSystem = EnumCardCount.HiLow;
            CardCountView = EnumCardCountView.On;

            Read();
        }

        public EnumCardCount CountSystem { get; set; }

        public EnumCardCountView CardCountView { get; set; }

        public void Read()
        {
           XmlSerializer ser = new XmlSerializer(typeof(AppSerialize));

           try
           {
               TextReader reader = new StreamReader("mainsettings.xml");

               AppSerialize cereal = (AppSerialize)ser.Deserialize(reader);

               bPlaySounds = cereal.bPlaySounds = bPlaySounds;
               Globals.Game.GameDifficulty = (EnumGameDifficulty)cereal.iGameDif;
               CountSystem = (EnumCardCount)cereal.iCountSys;
               CardCountView = (EnumCardCountView)cereal.iCardCountView;

               reader.Close();
           }
           catch (System.Exception ex)
           {
          	
           }
           
           HighScores.Read();

           Achievements.Read();
        }

        public void Save()
        {
            XmlSerializer ser = new XmlSerializer(typeof(AppSerialize));

            AppSerialize cereal = new AppSerialize();

            cereal.bPlaySounds = bPlaySounds;
            cereal.iGameDif = (int)Globals.Game.GameDifficulty;
            cereal.iCountSys = (int)CountSystem;
            cereal.iCardCountView = (int)CardCountView;

            try
            {
                TextWriter writer = new StreamWriter("mainsettings.xml");
                ser.Serialize(writer, cereal);
                writer.Close();
            }
            catch
            {

            }
            
            HighScores.Save();

            Achievements.Save();
        }
    }
}

