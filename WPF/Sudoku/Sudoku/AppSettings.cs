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
using System.Collections.Generic;
using System.Xml.Serialization;
using System.IO;

namespace Sudoku
{
   

    public class AppSettings
    {
        public EnumSudokuSkins eSkin;
        public EnumBackground eBackground;
        public int iSelectorX;
        public int iSelectorY;
        public bool bShowHighlights;
        public bool bDrawBlanks;
        public bool bPlaySounds;
        public bool bShowErrors
        {
            get { return Globals.Game.GetShowErrors(); }
            set { Globals.Game.SetShowErrors(value); }
        }
        public bool bLockCompleted
        {
            get { return _bLockCompleted; }
            set
            {
                Globals.Game.SetShowLocked(value);
                _bLockCompleted = value;
            }
        }
        public bool bShowHints
        {
            get { return Globals.Game.GetShowHints(); }
            set { Globals.Game.SetShowHints(value); }
        }
        public bool bAutoRemovePencil
        {
            get { return _bAutoRemovePencil; }
            set
            {
                Globals.Game.SetAutoRemovePencil(value);
                _bAutoRemovePencil = value;
            }
        }
        public Engine.EnumSudokuType eGameType
        {
            get { return Globals.Game.GetGameType(); }
            set { Globals.Game.SetGameType(value); }
        }
        public Engine.EnumDifficulty eDifficulty
        {
            get { return Globals.Game.GetDifficulty(); }
            set { Globals.Game.SetDifficulty(value); }
        }
        public Engine.EnumSize eGameSize
        {
            get { return Globals.Game.GetBoardSize(); }
            set { Globals.Game.SetBoardSize(value); }
        }
        public bool bIsGameInPlay
        {
            get { return Globals.Game.GetIsGameInPlay(); }
            set { Globals.Game.SetIsGameInPlay(value); }
        }
        public bool bAutoPencil
        {
            get { return Globals.Game.GetAutoPencilMarks(); }
            set { Globals.Game.SetAutoPencilMarks(value); }
        }
        public int iGameTimer
        {
            get { return Globals.Game.GetGameTimer(); }
            set { Globals.Game.SetGameTimer(value); }
        }


        private bool _bAutoRemovePencil;
        private bool _bLockCompleted;

        private const string REG_Skin = "skin";
        private const string REG_Background = "background";
        private const string REG_SelectorX = "selectorX";
        private const string REG_SelectorY = "selectorY";
        private const string REG_ShowHighLights = "showhighlights";
        private const string REG_DrawBlanks = "drawblanks";
        private const string REG_PlaySounds = "PlaySounds";
        private const string REG_ShowErrors = "showerrors";
        private const string REG_LockCompleted = "lockcompleted";
        private const string REG_ShowHints = "showhints";
        private const string REG_AutoRemovePencil = "autoremovepencil";
        private const string REG_GameType = "GameType";
        private const string REG_Difficulty = "Difficulty";
        private const string REG_IsGameInPlay = "IsGameInPlay";
        private const string REG_BoardSize = "BoardSize";
        private const string REG_GameTimer = "GameTimer";
        private const string REG_AutoPencil = "autopencil";
       

        public AppSettings()
        {
            // set defaults
            eSkin = EnumSudokuSkins.Glass;
            eBackground = EnumBackground.BlueGreen;
            iSelectorX = 0;
            iSelectorY = 0;
            bShowHighlights = false;
            bDrawBlanks = false;
            bPlaySounds = true;
            bShowErrors = true;
            bLockCompleted = true;
            bShowHints = false;
            bAutoRemovePencil = false;
            bAutoPencil = false;

            Read();
        }

        public void Read()
        {
            XmlSerializer ser = new XmlSerializer(typeof(List<int>));

            List<int> arrValues = null;

            try
            {
                TextReader reader = new StreamReader("Options.xml");

                //and serialize data
                arrValues = (List<int>)ser.Deserialize(reader);

            }
            catch (System.Exception ex)
            {

            }

            if (arrValues == null || arrValues.Count < 1)
                return;

            int iIndex = 0;


            eSkin = (EnumSudokuSkins)arrValues[iIndex++];
            eBackground = (EnumBackground)arrValues[iIndex++];
            iSelectorX = arrValues[iIndex++];
            iSelectorY = arrValues[iIndex++];
            bShowHighlights = Convert.ToBoolean(arrValues[iIndex++]);
            bDrawBlanks = Convert.ToBoolean(arrValues[iIndex++]);
            bPlaySounds = Convert.ToBoolean(arrValues[iIndex++]);
            bShowErrors = Convert.ToBoolean(arrValues[iIndex++]);
            bLockCompleted = Convert.ToBoolean(arrValues[iIndex++]);
            bShowHints = Convert.ToBoolean(arrValues[iIndex++]);
            bAutoRemovePencil = Convert.ToBoolean(arrValues[iIndex++]);
            eGameType = (Sudoku.Engine.EnumSudokuType)arrValues[iIndex++];
            eDifficulty = (Engine.EnumDifficulty)arrValues[iIndex++];
            eGameSize = (Engine.EnumSize)arrValues[iIndex++];
            bIsGameInPlay = Convert.ToBoolean(arrValues[iIndex++]);
            iGameTimer = arrValues[iIndex++];
            bAutoPencil = Convert.ToBoolean(arrValues[iIndex++]);
        }

        public void Save()
        {
            List<int> arrValues = new List<int>();

            arrValues.Add((int)eSkin);
            arrValues.Add((int)eBackground);
            arrValues.Add(iSelectorX);
            arrValues.Add(iSelectorY);
            arrValues.Add(Convert.ToInt32(bShowHighlights));
            arrValues.Add(Convert.ToInt32(bDrawBlanks));
            arrValues.Add(Convert.ToInt32(bPlaySounds));
            arrValues.Add(Convert.ToInt32(bShowErrors));
            arrValues.Add(Convert.ToInt32(bLockCompleted));
            arrValues.Add(Convert.ToInt32(bShowHints));
            arrValues.Add(Convert.ToInt32(bAutoRemovePencil));
            arrValues.Add((int)eGameType);
            arrValues.Add((int)eDifficulty);
            arrValues.Add((int)eGameSize);
            arrValues.Add(Convert.ToInt32(bIsGameInPlay));
            arrValues.Add(iGameTimer);
            arrValues.Add(Convert.ToInt32(bAutoPencil));

            try
            {
                XmlSerializer ser = new XmlSerializer(typeof(List<int>));
                TextWriter writer = new StreamWriter("Options.xml");
                ser.Serialize(writer, arrValues);
                writer.Close();
            }
            catch (System.Exception ex)
            {
            }
        }     
    }
}

