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
using System.Xml.Serialization;
using System.IO;

namespace Sudoku.Engine
{
    public enum EnumSudokuType
    {
        STYPE_Sudoku,
        STYPE_Killer,
        STYPE_Kakuro,
        STYPE_KenKen,
        STYPE_Greater,
        STYPE_Count,
    };

    public class BoardTileEventArgs : EventArgs
    {
        public int x { get; set; }
        public int y { get; set; }
    }

    public class SEngine
    {
        private const string FILE_BestTimes = "BestTimes.txt";
        private const string FILE_AvgTimes = "AvgTimes.txt";
        private const string FILE_PlayCounts = "PlayCounts.txt";

        static int DEFAULT_TIME = 3599;

        SudokuGame m_oSudoku = new SudokuGame();
        KakuroGame m_oKakuro = new KakuroGame();
        KillerGame m_oKiller = new KillerGame();
        KenKenGame m_oKenKen = new KenKenGame();
        GreaterGame m_oGreater = new GreaterGame();

        EnumSudokuType m_eGameType;

        EnumDifficulty m_eDiff; 
        bool m_bShowHints;
        bool m_bIsGameInPlay;
        int m_iGameTimer;
        bool m_bIsBoardSet;     // this is to overcome tombstoning while in the game screen to force a reload

        int[,] m_iAvgTimes = new int[(int)EnumSudokuType.STYPE_Count, (int)EnumDifficulty.DIFF_UnKnown];//unknown is the same as the count
        int[,] m_iBestTimes = new int[(int)EnumSudokuType.STYPE_Count, (int)EnumDifficulty.DIFF_UnKnown];//unknown is the same as the count
        int[,] m_iGameCounts = new int[(int)EnumSudokuType.STYPE_Count, (int)EnumDifficulty.DIFF_UnKnown];//count of games played per type


        // http://www.c-sharpcorner.com/uploadfile/ddutta/eventhandlinginnetusingcs11092005052726am/eventhandlinginnetusingcs.aspx

        public EventHandler<BoardTileEventArgs> UpdateTile;  
        public EventHandler<EventArgs> UpdateAll;
        public EventHandler<EventArgs> GameComplete;

        public EnumDifficulty      GetDifficulty(){return m_eDiff;}

        public EnumSudokuType GetGameType() { return m_eGameType; }
        public void SetGameType(EnumSudokuType eType) { m_eGameType = eType; }

        public bool GetShowHints() { return m_bShowHints; }
        public void SetShowHints(bool bShow) { m_bShowHints = bShow; }

        public bool GetIsGameInPlay() { return m_bIsGameInPlay; }
        public void SetIsGameInPlay(bool bIsGameInPlay) { m_bIsGameInPlay = bIsGameInPlay; }

        public bool GetIsBoardSet() { return m_bIsBoardSet; }

        public int GetGameTimer() { return m_iGameTimer; }
        public void SetGameTimer(int iGameTimer) { m_iGameTimer = iGameTimer; }
        public void IncGameTimer() { m_iGameTimer++; }

        public int GetBestTime(EnumSudokuType eType, EnumDifficulty eDiff) { return m_iBestTimes[(int)eType, (int)eDiff]; }
        public int GetGameCount(EnumSudokuType eType, EnumDifficulty eDiff) { return m_iGameCounts[(int)eType, (int)eDiff]; }
        public int GetAvgTime(EnumSudokuType eType, EnumDifficulty eDiff) { return m_iAvgTimes[(int)eType, (int)eDiff]; }

        public string GetBestTimeString(EnumSudokuType eType, EnumDifficulty eDiff)
        {
            int iTemp = GetBestTime(eType, eDiff);
            //format as a time

            return String.Format("{0}:{1:00}", (iTemp / 60), iTemp % 60);
            //return (iTemp / 60).ToString() + ":" + (iTemp % 60).ToString();
        }

        public string GetAvgTimeString(EnumSudokuType eType, EnumDifficulty eDiff)
        {
            int iTemp = GetAvgTime(eType, eDiff);
            //format as a time
            return String.Format("{0}:{1:00}", (iTemp / 60), iTemp % 60);
            //return (iTemp / 60).ToString() + ":" + (iTemp % 60).ToString();
        }

        public SEngine()
        {
            m_eGameType = EnumSudokuType.STYPE_Kakuro;
            m_bShowHints = false;
            m_bIsGameInPlay = false;
            m_eDiff = EnumDifficulty.DIFF_UnKnown;
            m_iGameTimer = 0;
            m_bIsBoardSet = false;
            
            for(int i = 0; i < (int)EnumSudokuType.STYPE_Count; i++)
            {
                for(int j = 0; j < (int)EnumDifficulty.DIFF_UnKnown; j++)
                {
                    m_iBestTimes[i,j] = DEFAULT_TIME; //59:59
                    m_iAvgTimes[i,j] = DEFAULT_TIME;
                    m_iGameCounts[i,j] = 0;
                }
            }

            LoadState();
        }

        public void SaveState()
        {
            //int[,] m_iAvgTimes = new int[(int)EnumSudokuType.STYPE_Count, (int)EnumDifficulty.DIFF_UnKnown];//unknown is the same as the count

            List<int> arrBestTimes = new List<int>();
            List<int> arrAvgTimes = new List<int>();
            List<int> arrGameCounts = new List<int>();


            for (int y = 0; y < (int)EnumDifficulty.DIFF_UnKnown; y++)
            {
                for (int x = 0; x < (int)EnumSudokuType.STYPE_Count; x++)    
                {
                    arrBestTimes.Add(m_iBestTimes[x, y]);
                    arrAvgTimes.Add(m_iAvgTimes[x, y]);
                    arrGameCounts.Add(m_iGameCounts[x, y]);
                }
            }

            try
            {
                XmlSerializer ser = new XmlSerializer(typeof(List<int>));
                TextWriter writer = new StreamWriter(FILE_BestTimes);
                ser.Serialize(writer, arrBestTimes);
                writer.Close();

                writer = new StreamWriter(FILE_AvgTimes);
                ser.Serialize(writer, arrAvgTimes);
                writer.Close();

                writer = new StreamWriter(FILE_PlayCounts);
                ser.Serialize(writer, arrGameCounts);
                writer.Close();
            }
            catch (System.Exception ex)
            {
            	
            }
        }

        public void LoadState()
        {
            XmlSerializer ser = new XmlSerializer(typeof(List<int>));

            try
            {
                TextReader reader = new StreamReader(FILE_BestTimes);

                //and serialize data
                List<int> arrBestTimes = (List<int>)ser.Deserialize(reader);

                if (arrBestTimes.Count == (int)EnumSudokuType.STYPE_Count * (int)EnumDifficulty.DIFF_UnKnown)
                {
                    for (int y = 0; y < (int)EnumDifficulty.DIFF_UnKnown; y++)
                    {
                        for (int x = 0; x < (int)EnumSudokuType.STYPE_Count; x++)
                        {
                            m_iBestTimes[x, y] = arrBestTimes[y * (int)EnumSudokuType.STYPE_Count + x];
                        }
                    }
                }

                reader.Close();

                reader = new StreamReader(FILE_AvgTimes);

                //and serialize data
                List<int> arrAvgTimes = (List<int>)ser.Deserialize(reader);

                if (arrAvgTimes.Count == (int)EnumSudokuType.STYPE_Count * (int)EnumDifficulty.DIFF_UnKnown)
                {
                    for (int y = 0; y < (int)EnumDifficulty.DIFF_UnKnown; y++)
                    {
                        for (int x = 0; x < (int)EnumSudokuType.STYPE_Count; x++)
                        {
                            m_iAvgTimes[x, y] = arrAvgTimes[y * (int)EnumSudokuType.STYPE_Count + x];
                        }
                    }
                }

                reader.Close();

                reader = new StreamReader(FILE_PlayCounts);

                //and serialize data
                List<int> arrPlayCounts = (List<int>)ser.Deserialize(reader);

                if (arrPlayCounts.Count == (int)EnumSudokuType.STYPE_Count * (int)EnumDifficulty.DIFF_UnKnown)
                {
                    for (int y = 0; y < (int)EnumDifficulty.DIFF_UnKnown; y++)
                    {
                        for (int x = 0; x < (int)EnumSudokuType.STYPE_Count; x++)
                        {
                            m_iGameCounts[x, y] = arrPlayCounts[y * (int)EnumSudokuType.STYPE_Count + x];
                        }
                    }
                }

                reader.Close();
            }
            catch 
            {
            	
            }
            m_bIsGameInPlay = true;
        }

#region SEEngine 

        public void SaveGameState()
        {
            SaveState();

            if (!m_bIsGameInPlay)
                return; // don't need to save anything

            switch (m_eGameType)
            {
                case EnumSudokuType.STYPE_Sudoku:
                    m_oSudoku.SaveGameState();
                    break;
                case EnumSudokuType.STYPE_Killer:
                    m_oKiller.SaveGameState();
                    break;
                case EnumSudokuType.STYPE_Kakuro:
                    m_oKakuro.SaveGameState();
                    break;
                case EnumSudokuType.STYPE_KenKen:
                    m_oKenKen.SaveGameState();
                    break;
                case EnumSudokuType.STYPE_Greater:
                    m_oGreater.SaveGameState();
                    break;
                default:
                    break;
            }
        }

        public void LoadGameState()
        {
            if (!m_bIsGameInPlay)
                return; // nothing to load

            m_bIsBoardSet = true;

            SetDifficulty(m_eDiff);

            switch (m_eGameType)
            {
                case EnumSudokuType.STYPE_Sudoku:                    
                    m_oSudoku.LoadGameState();
                    break;
                case EnumSudokuType.STYPE_Killer:
                    m_oKiller.LoadGameState();
                    break;
                case EnumSudokuType.STYPE_Kakuro:
                    m_oKakuro.LoadGameState();
                    break;
                case EnumSudokuType.STYPE_KenKen:
                    m_oKenKen.LoadGameState();
                    break;
                case EnumSudokuType.STYPE_Greater:
                    m_oGreater.LoadGameState();
                    break;
                default:
                    break;
            }
        }
        
        public int GetCellsFree()
        {
            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                return m_oSudoku.GetNumFreeCells();
            case EnumSudokuType.STYPE_Killer:
                return m_oKiller.GetNumFreeCells();
            case EnumSudokuType.STYPE_Kakuro:
                return m_oKakuro.GetNumFreeCells();
            case EnumSudokuType.STYPE_KenKen:
                return m_oKenKen.GetNumFreeCells();
            case EnumSudokuType.STYPE_Greater:
                return m_oGreater.GetNumFreeCells();
            default:
                return -1;
            }
        }

        public int GetGridSize()
        {
            if(m_eGameType == EnumSudokuType.STYPE_Kakuro)
                return m_oKakuro.GetBoardSize();
            return 9;
        }

        public bool NewGame()
        {
            m_bIsGameInPlay = true;
            m_bIsBoardSet = true;
            m_iGameTimer = 0;
            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                return m_oSudoku.NewGame();
            case EnumSudokuType.STYPE_Killer:
                m_oKiller.NewGame(m_oKiller.GetDifficulty());
                return true;
            case EnumSudokuType.STYPE_Kakuro:
                return m_oKakuro.NewGame();
            case EnumSudokuType.STYPE_KenKen:
                m_oKenKen.NewGame(m_oKenKen.GetDifficulty());
                return true;
            case EnumSudokuType.STYPE_Greater:
                m_oGreater.NewGame();
                return true;
            default:
                return false;
            }
        }

        public bool IsGameComplete(int iGameTime)
        {
            bool bRet = false;
            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                bRet = m_oSudoku.IsGameComplete();
                break;
            case EnumSudokuType.STYPE_Killer:
                bRet =  m_oKiller.IsGameComplete();
                break;
            case EnumSudokuType.STYPE_Kakuro:
                bRet = m_oKakuro.IsGameComplete();
                break;
            case EnumSudokuType.STYPE_KenKen:
                bRet = m_oKenKen.IsGameComplete();
                break;
            case EnumSudokuType.STYPE_Greater:
                bRet = m_oGreater.IsGameComplete();
                break;
            default:
                return false;
            }

            if(bRet == true)
                CheckTime(iGameTime);

            m_bIsGameInPlay = !bRet;

            return bRet;
        }

        public void CheckTime(int iGameTime)
        {
            if(iGameTime < m_iBestTimes[(int)GetGameType(),(int)GetDifficulty()])
                m_iBestTimes[(int)GetGameType(),(int)GetDifficulty()] = iGameTime;

            //increase the game count
            m_iGameCounts[(int)GetGameType(),(int)GetDifficulty()]++;

            //and calc the average time
            if(m_iGameCounts[(int)GetGameType(),(int)GetDifficulty()] == 1)
            {
                m_iAvgTimes[(int)GetGameType(),(int)GetDifficulty()] = iGameTime;
            }
            else
            {
                int iTemp = m_iGameCounts[(int)GetGameType(),(int)GetDifficulty()];
                iTemp--;
                iTemp = iTemp * m_iAvgTimes[(int)GetGameType(),(int)GetDifficulty()] + iGameTime;

                m_iAvgTimes[(int)GetGameType(),(int)GetDifficulty()] = iTemp / m_iGameCounts[(int)GetGameType(),(int)GetDifficulty()];
            }
        }

        public void RestartGame()
        {
            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                m_oSudoku.RestartGame();
                break;
            case EnumSudokuType.STYPE_Killer:
                m_oKiller.RestartGame();
                break;
            case EnumSudokuType.STYPE_Kakuro:
                m_oKakuro.RestartGame();
                break;
            case EnumSudokuType.STYPE_KenKen:
                m_oKenKen.RestartGame();
                break;
            case EnumSudokuType.STYPE_Greater:
                m_oGreater.RestartGame();
                break;
            default:
                break;
            }

            m_bIsBoardSet = true;
            m_bIsGameInPlay = true;
            m_iGameTimer = 0;
        }

        public bool OnSetGuess(int iX, int iY, int iGuess)
        {
            bool bRet = false;

            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                    bRet = m_oSudoku.OnSetGuess(iX, iY, iGuess);
                break;
            case EnumSudokuType.STYPE_Killer:
                bRet = m_oKiller.OnSetGuess(iX, iY, iGuess);
                break;
            case EnumSudokuType.STYPE_Kakuro:
                if(m_oKakuro.GetTileType(iX, iY) == EnumTilePiece.TP_Guess ||
                    m_oKakuro.GetTileType(iX, iY) == EnumTilePiece.TP_PencilMark)
                    bRet = m_oKakuro.SetGuessPiece(iGuess, iX, iY);
                break;
            case EnumSudokuType.STYPE_KenKen:
                bRet = m_oKenKen.OnSetGuess(iX, iY, iGuess);
                break;
            case EnumSudokuType.STYPE_Greater:
                bRet = m_oGreater.OnSetGuess(iX, iY, iGuess);
                break;
            default:
                break;
            }

            if (bRet)
            {
                BoardUpdateTile(iX, iY);
                BoardUpdateAll();
                CheckComplete();
            }
    
            return bRet;
        }

        public bool OnSetPencilMark(int iX, int iY, int iPencil)
        {
            bool bRet = false;

            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                bRet = m_oSudoku.TogglePencilMarks(iX, iY, iPencil);
                break;
            case EnumSudokuType.STYPE_Killer:
                bRet = m_oKiller.TogglePencilMarks(iX, iY, iPencil);
                break;
            case EnumSudokuType.STYPE_Kakuro:
                if(m_oKakuro.GetTileType(iX, iY) == EnumTilePiece.TP_Guess ||
                    m_oKakuro.GetTileType(iX, iY) == EnumTilePiece.TP_PencilMark)
                    bRet = m_oKakuro.TogglePencil(iPencil, iX, iY);
                break;
            case EnumSudokuType.STYPE_KenKen:
                bRet = m_oKenKen.TogglePencilMarks(iX, iY, iPencil);
                break;
            case EnumSudokuType.STYPE_Greater:
                bRet = m_oGreater.TogglePencilMarks(iX, iY, iPencil);
                break;
            default:
                break;
            }

            if (bRet)
            {
                BoardUpdateTile(iX, iY);
            }

            return bRet;
        }


        public POINT GetLastCorrectGuess()
        {
            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                return m_oSudoku.GetLastCorrectGuess();
                
            case EnumSudokuType.STYPE_Killer:
                return m_oKiller.GetLastCorrectGuess();
                
            case EnumSudokuType.STYPE_Kakuro:
                return m_oKakuro.GetLastCorrectGuess();
                
            case EnumSudokuType.STYPE_KenKen:
                return m_oKenKen.GetLastCorrectGuess();
                
            case EnumSudokuType.STYPE_Greater:
                return m_oGreater.GetLastCorrectGuess();
                
            default:
                POINT pt;
                pt.x=-1; pt.y = -1;
                return pt;
                
            }
        }

        public POINT GetLastHint()
        {
            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                return m_oSudoku.GetLastHint();
                
            case EnumSudokuType.STYPE_Killer:
                return m_oKiller.GetLastHint();
                
            case EnumSudokuType.STYPE_Kakuro:
                return m_oKakuro.GetLastHint();
                
            case EnumSudokuType.STYPE_KenKen:
                return m_oKenKen.GetLastHint();
                
            case EnumSudokuType.STYPE_Greater:
                return m_oGreater.GetLastHint();
                
            default:
                POINT pt;
                pt.x=-1; pt.y = -1;
                return pt;
                
            }
        }

        public POINT GetLastWrongGuess()
        {
            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                return m_oSudoku.GetLastWrongGuess();
                
            case EnumSudokuType.STYPE_Killer:
                return m_oKiller.GetLastWrongGuess();
                
            case EnumSudokuType.STYPE_KenKen:
                return m_oKenKen.GetLastWrongGuess();
                
            case EnumSudokuType.STYPE_Greater:
                return m_oGreater.GetLastWrongGuess();
                
            case EnumSudokuType.STYPE_Kakuro:
                 //BUGBUG return m_oKakuro.GetLastWrongGuess();
                //break;
            default:
                POINT pt;
                pt.x=-1; pt.y = -1;
                return pt;
            }
        }

        public bool IsLocked(int iX, int iY)
        {
            if(m_eGameType == EnumSudokuType.STYPE_Sudoku)
                return m_oSudoku.IsLocked(iX, iY);
            else if(m_eGameType == EnumSudokuType.STYPE_Killer)
                return m_oKiller.IsLocked(iX, iY);
            else if(m_eGameType == EnumSudokuType.STYPE_KenKen)
                return m_oKenKen.IsLocked(iX, iY);
            else if(m_eGameType == EnumSudokuType.STYPE_Greater)
                return m_oGreater.IsLocked(iX, iY);
            return false;//no locked cells for kakuro ... 
        }

        public bool IsGiven(int iX, int iY)
        {
            if(m_eGameType == EnumSudokuType.STYPE_Sudoku)
                return m_oSudoku.IsGiven(iX, iY);
            else if(m_eGameType == EnumSudokuType.STYPE_Killer)
                return m_oKiller.IsGiven(iX, iY);
            else if(m_eGameType == EnumSudokuType.STYPE_KenKen)
                return m_oKenKen.IsGiven(iX, iY);
            else if(m_eGameType == EnumSudokuType.STYPE_Greater)
                return m_oGreater.IsGiven(iX, iY);//might do this for difficulty ... hmmm
            return false;//no given cells for kakuro ... 
        }

        public int GetSolution(int iX, int iY)
        {
            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                return m_oSudoku.GetSolution(iX, iY);
                
            case EnumSudokuType.STYPE_Killer:
                return m_oKiller.GetSolution(iX, iY);
                
            case EnumSudokuType.STYPE_Kakuro:
                if(m_oKakuro.GetTileType(iX, iY) == EnumTilePiece.TP_Guess)
                    return m_oKakuro.GetSolution(m_oKakuro.GetGuessPiece(iX, iY));
                return 0;
                
            case EnumSudokuType.STYPE_KenKen:
                return m_oKenKen.GetSolution(iX, iY);
                
            case EnumSudokuType.STYPE_Greater:
                return m_oGreater.GetSolution(iX, iY);
                
            default:
                return 0;
                
            }
        }

        public int GetGuess(int iX, int iY)
        {
            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                return m_oSudoku.GetGuess(iX, iY);
                
            case EnumSudokuType.STYPE_Killer:
                return m_oKiller.GetGuess(iX, iY);
                
            case EnumSudokuType.STYPE_Kakuro:
                if(m_oKakuro.GetTileType(iX, iY) == EnumTilePiece.TP_Guess)
                    return m_oKakuro.GetGuess(m_oKakuro.GetGuessPiece(iX, iY));
                return 0;
                
            case EnumSudokuType.STYPE_KenKen:
                return m_oKenKen.GetGuess(iX, iY);
                
            case EnumSudokuType.STYPE_Greater:
                return m_oGreater.GetGuess(iX, iY);
                
            default:
                return 0;
                
            }
        }

        public int GetOutline(int iX, int iY)
        {
            if(m_eGameType == EnumSudokuType.STYPE_Killer)
                return m_oKiller.GetOutline(iX, iY); 
            if(m_eGameType == EnumSudokuType.STYPE_KenKen)
                return m_oKenKen.GetOutline(iX, iY);

            return Globals.KOUTLINE_None;
        }

        public int GetTotal(int iX, int iY)
        {
            if(m_eGameType == EnumSudokuType.STYPE_Killer)
                return m_oKiller.GetTotal(iX, iY);

            if(m_eGameType == EnumSudokuType.STYPE_KenKen)
                return GetKenKenAnswer(iX, iY);

            return -1;
        }

        public int GetKenKenAnswer(int iX, int iY)
        {
            if(m_eGameType == EnumSudokuType.STYPE_KenKen)
                return m_oKenKen.GetMathAnswer(iX, iY);
            else 
                return -1;
        }

        public EnumOperator GetKenKenOp(int iX, int iY)
        {
            if(m_eGameType == EnumSudokuType.STYPE_KenKen)
                return m_oKenKen.GetOp(iX, iY);
            
             return EnumOperator.OP_None;
        }

        public int GetSamuIndex(int iX, int iY)
        {
            if(iX < 0 || iY < 0 || iX >= 9 || iY >= 9)
                return -1;
            if(m_eGameType == EnumSudokuType.STYPE_Killer)
                return m_oKiller.GetSamuIndex(iX, iY);
            if(m_eGameType == EnumSudokuType.STYPE_KenKen)
                return m_oKenKen.GetSamuIndex(iX, iY);

            return -1;
        }

        public int GetSamuValue(int iX, int iY)
        {
            if(iX < 0 || iY < 0 || iX >= 9 || iY >= 9)
                return -1;
            if(m_eGameType == EnumSudokuType.STYPE_Killer)
                return m_oKiller.GetSamuValue(iX, iY);
            if(m_eGameType == EnumSudokuType.STYPE_KenKen)
                return m_oKenKen.GetSamuValue(iX, iY);

            return -1;
        }

        public int GetHorzTotal(int iX, int iY)
        {
            if(m_eGameType != EnumSudokuType.STYPE_Kakuro)
                return 0;
            if(m_oKakuro.GetTileType(iX, iY) != EnumTilePiece.TP_Kakuro)
                return 0;
            return m_oKakuro.GetHorzTotal(m_oKakuro.GetKakuroPiece(iX, iY));
        }

        public int GetVertTotal(int iX, int iY)
        {
            if(m_eGameType != EnumSudokuType.STYPE_Kakuro)
                return 0;
            if(m_oKakuro.GetTileType(iX, iY) != EnumTilePiece.TP_Kakuro)
                return 0;
            return m_oKakuro.GetVertTotal(m_oKakuro.GetKakuroPiece(iX, iY));
        }

        public bool IsBlank(int iX, int iY)
        {
            if(m_eGameType != EnumSudokuType.STYPE_Kakuro)
                return false;
            if(m_oKakuro.GetTileType(iX, iY) == EnumTilePiece.TP_Blank)
                return true;
            return false;
        }

        public bool IsKakuro(int iX, int iY)
        {
            if(m_eGameType != EnumSudokuType.STYPE_Kakuro)
                return false;
            if(m_oKakuro.GetTileType(iX, iY) == EnumTilePiece.TP_Kakuro)
                return true;
            return false;
        }

        public bool SetBoardSize(EnumSize eSize)
        {
            if(m_eGameType != EnumSudokuType.STYPE_Kakuro)
                return false;

            m_oKakuro.SetSize(eSize);
            return true;
        }

        public EnumSize GetBoardSize()
        {
            if (m_eGameType != EnumSudokuType.STYPE_Kakuro)
                return EnumSize.KSIZE_9;


            return m_oKakuro.GetSize();
        }

        public bool SetDifficulty(EnumDifficulty eDiff)
        {

            m_oSudoku.SetDifficulty(eDiff);
            m_oKiller.SetDifficulty(eDiff);
            m_oKenKen.SetDifficulty(eDiff);
            m_oGreater.SetDifficulty(eDiff);

            switch(eDiff)
            {
            case EnumDifficulty.DIFF_Medium:
                m_oKakuro.SetLevel(EnumLevel.LVL_Medium);
    	        break;
            case EnumDifficulty.DIFF_Hard:
            case EnumDifficulty.DIFF_VeryHard:
            case EnumDifficulty.DIFF_Hardcore:
                m_oKakuro.SetLevel(EnumLevel.LVL_Hard);
                break;
            default:
                m_oKakuro.SetLevel(EnumLevel.LVL_Easy);
                break;
            }

            m_eDiff = eDiff;
            return true;
        }

        public bool Solve()
        {
            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                m_oSudoku.SolveGame();
                break;
            case EnumSudokuType.STYPE_Killer:
                m_oKiller.SolveGame();
                break;
            case EnumSudokuType.STYPE_Kakuro:
                m_oKakuro.SolveGame();
                break;
            case EnumSudokuType.STYPE_KenKen:
                m_oKenKen.SolveGame();
                break;
            case EnumSudokuType.STYPE_Greater:
                m_oGreater.SolveGame();
                break;
            }

            m_bIsGameInPlay = false;

            if (UpdateAll != null)
            {
                UpdateAll(this, new EventArgs());
            }

            return true;
        }

        public bool Hint()
        {
            bool bRet = false;

            POINT ptLast;

            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                    bRet = m_oSudoku.Hint();
                    ptLast = m_oSudoku.GetLastHint();
                    break;
            case EnumSudokuType.STYPE_Killer:
                    bRet = m_oKiller.Hint();
                    ptLast = m_oKiller.GetLastHint();
                    break;
            case EnumSudokuType.STYPE_Kakuro:
                    bRet = m_oKakuro.Hint();
                    ptLast = m_oKakuro.GetLastHint();
                    break;
            case EnumSudokuType.STYPE_KenKen:
                    bRet = m_oKenKen.Hint();
                    ptLast = m_oKenKen.GetLastHint();
                    break;
            case EnumSudokuType.STYPE_Greater:
                    bRet = m_oGreater.Hint();
                    ptLast = m_oGreater.GetLastHint();
                    break;
             default:
                    ptLast = new POINT();//supress the warning
                    break;
            }

            if (bRet)
            {
                BoardUpdateTile(ptLast.x, ptLast.y);
                BoardUpdateAll();
                if(CheckComplete() == false)
                    m_iGameTimer += 10;
            }

            return bRet;
        }

        public bool Undo()
        {
            bool bRet = false;

            POINT ptLast;

            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                bRet = m_oSudoku.Undo();
                ptLast = m_oSudoku.GetLastMove();
                break;
                
            case EnumSudokuType.STYPE_Killer:
                bRet = m_oKiller.Undo();
                ptLast = m_oKiller.GetLastMove();
                break;
            case EnumSudokuType.STYPE_Kakuro:
                bRet = m_oKakuro.Undo();
                ptLast = m_oKakuro.GetLastMove();
                break;
            case EnumSudokuType.STYPE_KenKen:
                bRet = m_oKenKen.Undo();
                ptLast = m_oKenKen.GetLastMove();
                break;
            case EnumSudokuType.STYPE_Greater:
                bRet = m_oGreater.Undo();
                ptLast = m_oGreater.GetLastMove();
                break;
            default:
                ptLast = new POINT();//supress the warning
                break;
            }

            if (bRet)
            {
                BoardUpdateTile(ptLast.x, ptLast.y);
                BoardUpdateAll();
            }
            return bRet;
        }

        public int GetPencilMarks(int iX, int iY)
        {
            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                return m_oSudoku.GetPencilMarks(iX, iY);
                
            case EnumSudokuType.STYPE_Killer:
                return m_oKiller.GetPencilMarks(iX, iY);
                
            case EnumSudokuType.STYPE_Kakuro:
                return m_oKakuro.GetPencilMarks(iX, iY);
                  
            case EnumSudokuType.STYPE_KenKen:
                return m_oKenKen.GetPencilMarks(iX, iY);
                
            case EnumSudokuType.STYPE_Greater:
                return m_oGreater.GetPencilMarks(iX, iY);
                
            }
            return 0;
        }
        public void ClearPencilMarks(int iX, int iY)
        {
            switch (m_eGameType)
            {
                case EnumSudokuType.STYPE_Sudoku:
                    m_oSudoku.SetPencilMarks(iX, iY, 0);
                    break;
                case EnumSudokuType.STYPE_Killer:
                    m_oKiller.SetPencilMarks(iX, iY, 0);
                    break;
                case EnumSudokuType.STYPE_Kakuro:
                    m_oKakuro.ClearPencil(iX, iY);
                    break;
                case EnumSudokuType.STYPE_KenKen:
                    m_oKenKen.SetPencilMarks(iX, iY, 0);
                    break;
                case EnumSudokuType.STYPE_Greater:
                    m_oGreater.SetPencilMarks(iX, iY, 0);
                    break;
            }
        }

        public void ClearPencilMarks()
        {
            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                m_oSudoku.ClearAllPencilMarks();
                break;
            case EnumSudokuType.STYPE_Killer:
                m_oKiller.ClearAllPencilMarks();
                break;
            case EnumSudokuType.STYPE_Kakuro:
                m_oKakuro.ClearAllPencilMarks();
                break;
            case EnumSudokuType.STYPE_KenKen:
                m_oKenKen.ClearAllPencilMarks();
                break;
            case EnumSudokuType.STYPE_Greater:
                m_oGreater.ClearAllPencilMarks();
                break;
            }
        }

        public bool GetAutoPencilMarks()
        {
            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                return m_oSudoku.GetAutoPencil();
                
            case EnumSudokuType.STYPE_Killer:
                return m_oKiller.GetAutoPencil();
                
            case EnumSudokuType.STYPE_Kakuro:
                return m_oKakuro.GetAutoPencil();
                
            case EnumSudokuType.STYPE_KenKen:
                return m_oKenKen.GetAutoPencil();
                
            case EnumSudokuType.STYPE_Greater:
                return m_oGreater.GetAutoPencil();
                
            }
            return false;
        }

        public bool GetAutoRemovePencil()
        {
            //they're all set so this is fine
            return m_oSudoku.GetAutoRemovePencil();
        }

        public void SetAutoPencilMarks(bool bAuto)
        {
            m_oSudoku.SetAutoPencilMarks(bAuto);
            m_oKiller.SetAutoPencilMarks(bAuto);
            m_oKakuro.SetAutoPencilMarks(bAuto);
            m_oKenKen.SetAutoPencilMarks(bAuto);
            m_oGreater.SetAutoPencilMarks(bAuto);
        }

        public void SetAutoRemovePencil(bool bAuto)
        {
            m_oSudoku.SetAutoRemovePencilMarks(bAuto);
            m_oKiller.SetAutoRemovePencilMarks(bAuto);
            m_oKakuro.SetAutoRemovePencilMarks(bAuto);
            m_oKenKen.SetAutoRemovePencilMarks(bAuto);
            m_oGreater.SetAutoRemovePencilMarks(bAuto);
        }

        public void SetShowLocked(bool bShowLocked)
        {
           m_oSudoku.SetLocked(bShowLocked);
           m_oKiller.SetLocked(bShowLocked);
           m_oKenKen.SetLocked(bShowLocked);
           m_oGreater.SetLocked(bShowLocked);
        }

        public bool GetShowErrors()
        {
            return m_oSudoku.GetShowErrors();//good enough ... 
        }

        public void SetShowErrors(bool bShow)
        {
            m_oSudoku.SetShowErrors(bShow);
            m_oKiller.SetShowErrors(bShow);
            m_oKakuro.SetShowErrors(bShow);
            m_oKenKen.SetShowErrors(bShow);
            m_oGreater.SetShowErrors(bShow);
        }

        public bool DrawTotal(int iX, int iY)
        {
            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                break;
            case EnumSudokuType.STYPE_Killer:
                return m_oKiller.DrawTotal(iX, iY);    
                
            case EnumSudokuType.STYPE_Kakuro:
                break;
            case EnumSudokuType.STYPE_KenKen:
                return m_oKenKen.DrawTotal(iX, iY);
                
            case EnumSudokuType.STYPE_Greater:
                break;
            }
            return false;
        }

        public bool IsPencilMarkValid(int iValue, int iX, int iY)
        {
            if(IsGiven(iX, iY) ||
                IsLocked(iX, iY) ||
                IsKakuro(iX, iY) || 
                IsBlank(iX, iY) )
                return false;

            if(m_bShowHints == false)
                return true;//all good then

            switch(m_eGameType)
            {
            case EnumSudokuType.STYPE_Sudoku:
                return m_oSudoku.IsPencilMarkValid(iX, iY, iValue);
                
            case EnumSudokuType.STYPE_Killer:
                return m_oKiller.IsPencilMarkValid(iX, iY, iValue);
                
            case EnumSudokuType.STYPE_Kakuro:
                return m_oKakuro.IsPencilMarkPossible(iX, iY, iValue);
                
            case EnumSudokuType.STYPE_KenKen:
                return m_oKenKen.IsPencilMarkValid(iX, iY, iValue);
                
            case EnumSudokuType.STYPE_Greater:
                return m_oGreater.IsPencilMarkValid(iX, iY, iValue);
                
            }
            return false;
        }

        public EnumGreaterDir GetGVertical(int iX, int iY)
        {
            if(m_eGameType != EnumSudokuType.STYPE_Greater)
                return EnumGreaterDir.GDIR_Unknown;
            return m_oGreater.GetGVertical(iX, iY);
        }

        public EnumGreaterDir GetGHorizontal(int iX, int iY)
        {
            if(m_eGameType != EnumSudokuType.STYPE_Greater)
                return EnumGreaterDir.GDIR_Unknown;
            return m_oGreater.GetGHorizontal(iX, iY);
        }
#endregion

        void BoardUpdateTile(int iX, int iY)
        {
            if (UpdateTile != null)
            {
                UpdateTile(this, new BoardTileEventArgs
                {
                    x = iX,
                    y = iY,
                });
            }
        }

        void BoardUpdateAll()
        {
            //gonna try it this way ...
            if (UpdateAll != null)
            {
                UpdateAll(this, new EventArgs());
            }
        }

        bool CheckComplete()
        {
            if (IsGameComplete(m_iGameTimer))
            {
                if (GameComplete != null)
                {
                    GameComplete(this, new EventArgs());
                    return true;
                }
            }
            return false;
        }
    }
}
 