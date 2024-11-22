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
using System.Text;
using System.IO;
using System.Windows.Resources;
using System.IO.IsolatedStorage;
using System.Xml.Serialization;
using System.Collections.Generic;

namespace Sudoku.Engine
{
    public enum EnumConstants
    {
        CONST_BoardSize = 11,
    }

    public enum EnumTilePiece
    {
	    TP_Blank,
	    TP_Kakuro,
	    TP_PencilMark,
	    TP_Guess,
    };

    public enum EnumSize
    {
        KSIZE_5,
        KSIZE_7,
        KSIZE_9,
    };

    public class KakuroGame
    {
        /// <summary>
        /// Private Variables
        /// </summary>
        /// 
        Random rand = new Random(System.DateTime.Today.Minute + System.Environment.TickCount);

        private const string FILE_KakuroBoard = "Kakurob.txt";
        private const string FILE_KakuroValues = "Kakurov.txt";

        private int m_iBoardSize;
        private int[,] m_iTile;
        private EnumLevel m_eLevel;
        private EnumSize m_eSize;
        private bool m_bIsGameInPlay;
        private bool m_bAutoPencilMarks;
        private bool m_bShowErrors;
        private bool m_bAutoRemovePencilMarks;
        private int m_iNumFreeCells;

        int m_iMistakeCounter;						// number of mistakes ... we'll give an accuracy rating at the end 
        int m_iTotalPlayableCells = 0;					// number of cells the user must solve ... used for the accuracy percentage
        int m_iNumSeconds;

        POINT m_ptLastCorrectGuess = new POINT();					// we'll use this for the correct guess animation
        POINT m_ptLastHint = new POINT();

        TypeLastMove m_sLastMove;

        public KakuroGame()
        {
            m_iBoardSize = 0;
            m_eLevel     = EnumLevel.LVL_Easy;
            m_eSize      = EnumSize.KSIZE_7;
            m_iTile      = new int[(int)EnumConstants.CONST_BoardSize, (int)EnumConstants.CONST_BoardSize];
        }


        public int					GetNumFreeCells(){return GetFreeCells();}

        public int GetFreeCells() { return m_iNumFreeCells; }
        public int GetNumSeconds() { return m_iNumSeconds; }
        public int GetBoardSize() { return m_iBoardSize; }
        public POINT GetLastHint(){return m_ptLastHint;}

        public bool IsGameInPlay() { return m_bIsGameInPlay; }
        public EnumLevel GetLevel() { return m_eLevel; }
        public EnumSize GetSize() { return m_eSize; }
        public bool GetAutoPencil() { return m_bAutoPencilMarks; }

        public bool GetShowErrors() { return m_bShowErrors; }


        public void StopCurrentGame() { m_bIsGameInPlay = false; }
        public void SetLevel(EnumLevel eLevel) { m_eLevel = eLevel; }
        public void SetSize(EnumSize eSize) { m_eSize = eSize; }
        public void SetAutoPencilMarks(bool bAuto) { m_bAutoPencilMarks = bAuto; /*if (bAuto)AutoFillPencilMarks();*/ }
        public void SetAutoRemovePencilMarks(bool bAuto) { m_bAutoRemovePencilMarks = bAuto; }
        public void SetShowErrors(bool bShowErrors) { m_bShowErrors = bShowErrors; }

        public virtual void SaveGameState()
        {
            List<int> arrTiles = new List<int>();
            List<int> arrOtherValues = new List<int>();

            for (int y = 0; y < (int)EnumConstants.CONST_BoardSize; y++)
            {
                for (int x = 0; x < (int)EnumConstants.CONST_BoardSize; x++)
                {
                    arrTiles.Add(m_iTile[x, y]);
                }
            }

            arrOtherValues.Add(m_iNumSeconds);
            arrOtherValues.Add(m_iNumFreeCells);
            arrOtherValues.Add(Convert.ToInt32(m_bAutoPencilMarks));
            arrOtherValues.Add(Convert.ToInt32(m_bShowErrors));
            arrOtherValues.Add(Convert.ToInt32(m_bAutoRemovePencilMarks));
            arrOtherValues.Add(m_iTotalPlayableCells);
            arrOtherValues.Add(m_iMistakeCounter);

            try
            {
                XmlSerializer ser = new XmlSerializer(typeof(List<int>));
                TextWriter writer = new StreamWriter(FILE_KakuroBoard);
                ser.Serialize(writer, arrTiles);
                writer.Close();

                writer = new StreamWriter(FILE_KakuroValues);
                ser.Serialize(writer, arrOtherValues);
                writer.Close();
            }
            catch (System.Exception ex)
            {
            	
            }
        }

        public virtual void LoadGameState()
        {
            switch (m_eSize)
            {
                case EnumSize.KSIZE_5:
                    m_iBoardSize = 6;
                    break;
                case EnumSize.KSIZE_7:
                    m_iBoardSize = 8;
                    break;
                case EnumSize.KSIZE_9:
                    m_iBoardSize = 10;
                    break;
                default:
                    break;
            }

            XmlSerializer ser = new XmlSerializer(typeof(List<int>));

            try
            {
                TextReader reader = new StreamReader(FILE_KakuroBoard);

                //and serialize data
                List<int> arrTiles = (List<int>)ser.Deserialize(reader);

                if (arrTiles.Count == (int)EnumConstants.CONST_BoardSize * (int)EnumConstants.CONST_BoardSize)
                {
                    for (int y = 0; y < (int)EnumConstants.CONST_BoardSize; y++)
                    {
                        for (int x = 0; x < (int)EnumConstants.CONST_BoardSize; x++)
                        {
                            m_iTile[x, y] = arrTiles[y * (int)EnumConstants.CONST_BoardSize + x];
                        }
                    }
                }

                reader.Close();

                reader = new StreamReader(FILE_KakuroValues);

                //and serialize data
                List<int> arrOtherValues = (List<int>)ser.Deserialize(reader);

                if (arrOtherValues.Count == 7)
                {
                    m_iNumSeconds = arrOtherValues[0];
                    m_iNumFreeCells = arrOtherValues[1];
                    m_bAutoPencilMarks = Convert.ToBoolean(arrOtherValues[2]);
                    m_bShowErrors = Convert.ToBoolean(arrOtherValues[3]);
                    m_bAutoRemovePencilMarks = Convert.ToBoolean(arrOtherValues[4]);
                    m_iTotalPlayableCells = arrOtherValues[5];
                    m_iMistakeCounter = arrOtherValues[6];
                }
                m_bIsGameInPlay = true;

                reader.Close();
            }
            catch (System.Exception ex)
            {
            	
            }
        }

        public string GetPuzzle(EnumLevel eLevel, EnumSize eSize)
        {
            string txt = "Assets/Kakuro/K";

            switch(eSize)
	        {
	        case EnumSize.KSIZE_5:
		        txt += "5";
		        m_iBoardSize = 6;
		        break;
	        case EnumSize.KSIZE_7:
		        txt += "7";
		        m_iBoardSize = 8;
		        break;
	        case EnumSize.KSIZE_9:
	        default:
		        txt += "9";
		        m_iBoardSize = 10;
	            break;
	        }

	        switch(eLevel)
	        {
	        case EnumLevel.LVL_Easy:
		        txt += "E";
		        break;
	        case EnumLevel.LVL_Medium:
		        txt += "N";
		        break;
	        case EnumLevel.LVL_Hard:
		        txt += "H";
		        break;
	        }

            txt += ".TXT";

            //Uri uri = new Uri("Assets/Kakuro/K7E.TXT", UriKind.RelativeOrAbsolute);
            Uri uri = new Uri(txt, UriKind.RelativeOrAbsolute);

            StreamResourceInfo sri = App.GetResourceStream(uri);

            StreamReader sr = new StreamReader(sri.Stream);

            txt = sr.ReadToEnd();

            sr.Close();

            int iLineLength = ((m_iBoardSize - 1) * (m_iBoardSize - 1) + 2);
            int iNumPuzzles = txt.Length / iLineLength;

            int iPuzzleIndex = rand.Next(0, iNumPuzzles - 1);

            return txt.Substring(iPuzzleIndex * iLineLength, iLineLength - 2);

        }

        private void RandomizeBoard(string szBoard)
        {
            if (szBoard == null)
                return;

            int iType = rand.Next(0, 8);

            switch (iType)
            {
                case 0://rotate 180
                    Rotate180(szBoard);
                    break;
                case 1:
                    Rotate90(szBoard);
                    break;
                case 2:
                    Rotate270(szBoard);
                    break;
                case 3:
                    MirrorXY(szBoard);
                    break;
                case 4:
                    MirrorXY(szBoard);
                    Rotate90(szBoard);
                    break;
                case 5:
                    MirrorXY(szBoard);
                    Rotate180(szBoard);
                    break;
                case 6:
                    MirrorXY(szBoard);
                    Rotate270(szBoard);
                    break;
                case 7:	//do nothing
                default:
                    break;
            }


        }

        private void MirrorXY(string szBoard)
        {
            if (szBoard == null)
                return;

            int iX, iY;

            StringBuilder s = new StringBuilder(szBoard);

            for (int y = 0; y < m_iBoardSize - 1; y++)
            {
                for (int x = 0; x < m_iBoardSize - 1; x++)
                {
                    iX = y;
                    iY = x;
                    s[x + y * (m_iBoardSize - 1)] = szBoard[iX + iY * (m_iBoardSize - 1)];

                }
            }

            szBoard = s.ToString();
        }

        private void Rotate90(string szBoard)
        {
            if (szBoard == null)
                return;

            int iX, iY;

            StringBuilder s = new StringBuilder(szBoard);

            for (int y = 0; y < m_iBoardSize - 1; y++)
            {
                for (int x = 0; x < m_iBoardSize - 1; x++)
                {
                    iX = m_iBoardSize - 2 - y;
                    iY = x;
                    s[x + y * (m_iBoardSize - 1)] = szBoard[iX + iY * (m_iBoardSize - 1)];

                }
            }

            szBoard = s.ToString();
        }

        private void Rotate180(string szBoard)
        {
            if (szBoard == null)
                return;

            StringBuilder s = new StringBuilder(szBoard);
            char szTemp;
            int iLen = szBoard.Length;
            int iIndex = iLen;


            if (iIndex % 2 == 1)
                iIndex--;
            iIndex /= 2;
            for (int i = 0; i < iIndex; i++)
            {
                szTemp = s[i];
                s[i] = s[iLen - 1 - i];
                s[iLen - 1 - i] = szTemp;
            }

            szBoard = s.ToString();
        }

        private void Rotate270(string szBoard)
        {
            if (szBoard == null)
                return;

            int iX, iY;

            StringBuilder s = new StringBuilder(szBoard);

            for (int y = 0; y < m_iBoardSize - 1; y++)
            {
                for (int x = 0; x < m_iBoardSize - 1; x++)
                {
                    iX = y;
                    iY = m_iBoardSize - 2 - x;
                    s[x + y * (m_iBoardSize - 1)] = szBoard[iX + iY * (m_iBoardSize - 1)];

                }
            }

            szBoard = s.ToString();
        }

        private int GetType(int d)
        {
            return (d & 3);
        }

        public int GetPencil(int d)
        {
            if (d < 0)
                return 0;
            return (d & 2044) >> 2;
        }

        public int GetGuess(int d)
        {
            if (d < 0)
                return 0;
            return (d & 30720) >> 11;
        }

        public int GetSolution(int d)
        {
            return (d & 491520) >> 15;
        }

        public int GetHorzTotal(int d)
        {
            return (d & 252) >> 2;
        }

        public int GetVertTotal(int d)
        {
            return (d & 16128) >> 8;
        }

        public EnumTilePiece GetType(int iRow, int iColumn)
        {
            int iVal = m_iTile[iRow, iColumn];
            return (EnumTilePiece)GetType(iVal);
        }

        public int GetSolution(int iRow, int iColumn)
        {
            int iVal = m_iTile[iRow, iColumn];
            return GetSolution(iVal);
        }

        private void SetType(ref int iCurrent, int iType)
        {
            // zero out
            iCurrent &= ~3;
            iCurrent |= (iType & 3);
        }

        private void SetPencil(ref int iCurrent, int iPencil)
        {
            // zero out
            iCurrent &= ~2044;
            iCurrent |= ((iPencil << 2) & 2044);
        }

    /*    bool IsValid(int iX, int iY)
        {
            if (iX > m_iBoardSize || iY > m_iBoardSize)
                return false;

            if (iX < 0 || iY < 0)
                return false;

            return true;
        }

        int GetTile(int iX, int iY)
        {
            if (!IsValid(iX, iY))
                return 0;

            return m_iTile[iX, iY];
        }

        void SetTile(int iX, int iY, int iTile)
        {
            if (!IsValid(iX, iY))
                return;

            m_iTile[iX, iY] = iTile;
        }

        public void SetPencilMarks(int iX, int iY, int iPencilMarks)
        {
            int iTile = GetTile(iX, iY);
            iTile &= ~523264;
            iTile |= ((iPencilMarks << 10) & 523264);
            SetTile(iX, iY, iTile);
        }*/

        private void SetGuess(ref int iCurrent, int iGuess)
        {
            // zero out
            iCurrent &= ~30720;
            iCurrent |= ((iGuess << 11) & 30720);
        }

        private void SetSolution(ref int iCurrent, int iSolution)
        {
            // zero out
            iCurrent &= ~491520;
            iCurrent |= ((iSolution << 15) & 491520);
        }

        private void SetHorzTotal(ref int iCurrent, int iHorzTotal)
        {
            // zero out
            iCurrent &= ~252;
            iCurrent |= ((iHorzTotal << 2) & 252);
        }

        private void SetVertTotal(ref int iCurrent, int iVertTotal)
        {
            // zero out
            iCurrent &= ~16128;
            iCurrent |= ((iVertTotal << 8) & 16128);
        }

        public bool NewGame()
        {
            string szBoard = GetPuzzle(m_eLevel, m_eSize);
            if (szBoard == null)
                return false;

            m_iNumSeconds = 0;
            m_iMistakeCounter = 0;

            m_ptLastCorrectGuess.x = 0;
            m_ptLastCorrectGuess.y = 0;

            m_ptLastHint.x = -1;
            m_ptLastHint.y = -1;

            // rotate, mirror, etc
            RandomizeBoard(szBoard);

            int iValue = 0;
            int iIndex = 0;

            for (int y = 0; y < m_iBoardSize; y++)
            {
                for (int x = 0; x < m_iBoardSize; x++)
                {
                    // if this is a Kakuro piece ... edge piece
                    iIndex = (x - 1) + (y - 1) * (m_iBoardSize - 1);
                    if (x > 0 && y > 0)
                        iValue = szBoard[iIndex] - 48;
                    else
                        iValue = -1;

                    if (iValue < 1)//edge piece 
                    {
                        SetType(ref m_iTile[x, y], (int)EnumTilePiece.TP_Kakuro);
                        //zero em out for now ... set later
                        SetHorzTotal(ref m_iTile[x, y], 0);
                        SetVertTotal(ref m_iTile[x, y], 0);
                    }
                    else
                    {
                        SetType(ref m_iTile[x, y], (int)EnumTilePiece.TP_PencilMark);
                        SetSolution(ref m_iTile[x, y], iValue);
                        SetGuess(ref m_iTile[x, y], 0);
                        SetPencil(ref m_iTile[x, y], 0);
                    }
                }
            }

            //we'll set the sums in these loops
            for (int y = 0; y < m_iBoardSize; y++)
            {
                for (int x = 0; x < m_iBoardSize; x++)
                {
                    if (GetType(m_iTile[x, y]) == (int)EnumTilePiece.TP_Kakuro)
                    {
                        int iHTotal = 0;
                        int iVTotal = 0;

                        // count up the horizontal sum
                        for (int i = x + 1; i < m_iBoardSize; i++)
                        {
                            // if it's another kakuro piece we're done counting
                            if (GetType(m_iTile[i, y]) == (int)EnumTilePiece.TP_Kakuro)
                                break;
                            else if (y == 0)	// we don't count the first row
                                break;

                            // if this is a Kakuro piece
                            iIndex = (i - 1) + (y - 1) * (m_iBoardSize - 1);
                            if (iIndex > -1)
                                iValue = szBoard[iIndex] - 48;

                            iHTotal += iValue;
                        }

                        // count up the vertical sum
                        for (int i = y + 1; i < m_iBoardSize; i++)
                        {
                            // if it's another kakuro piece we're done counting
                            if (GetType(m_iTile[x, i]) == (int)EnumTilePiece.TP_Kakuro)
                                break;
                            else if (x == 0)	// we don't count the first column
                                break;

                            // if this is a Kakuro piece
                            iIndex = (x - 1) + (i - 1) * (m_iBoardSize - 1);
                            if (iIndex > -1)
                                iValue = szBoard[iIndex] - 48;

                            iVTotal += iValue;
                        }

                        if (iVTotal == 0 && iHTotal == 0)
                            SetType(ref m_iTile[x, y], (int)EnumTilePiece.TP_Blank);

                        //and finally .... set the totals
                        SetHorzTotal(ref m_iTile[x, y], iHTotal);
                        SetVertTotal(ref m_iTile[x, y], iVTotal);

                    }
                }
            }

            m_bIsGameInPlay = true;
            CalcFreeCells();

            AutoFillPencilMarks();

            //set the total free cells
            m_iTotalPlayableCells = GetFreeCells();


            return true;
        }

        public void RestartGame()
        {	//we need to erase all guesses ... restore to only given values
	        m_ptLastCorrectGuess.x	= 0;
	        m_ptLastCorrectGuess.y	= 0;
            m_ptLastHint.x          = -1;
            m_ptLastHint.y          = -1;
	        m_iMistakeCounter		= 0;
	        m_iNumSeconds			= 0;
            m_sLastMove.eLastMove = EnumLastMove.LASTMOVE_None;


	        //and now set all of the guess pieces back to pencil marks
	        for(int x = 0; x < m_iBoardSize; x++)
	        {
		        for(int y = 0; y < m_iBoardSize; y++)
		        {
			        if(GetType(m_iTile[x,y]) == (int)EnumTilePiece.TP_Guess)
                        SetType(ref m_iTile[x, y], (int)EnumTilePiece.TP_PencilMark);
		        }
	        }

	        m_bIsGameInPlay = true;
	        CalcFreeCells();
	        AutoFillPencilMarks();
        }

        public bool IsGameComplete()
        {
	        if(m_bIsGameInPlay == false)
		        return true;

	        if(m_iNumFreeCells > 0)
		        return false;

	        //lets actually check then
	        for(int x = 1; x < m_iBoardSize; x++)//1 because we'll skip the boarders
	        {
		        for(int y=1; y < m_iBoardSize; y++)
		        {
                    if (GetType(m_iTile[x, y]) == (int)EnumTilePiece.TP_Guess)
			        {	//make sure the guess equals the solution
				        if(GetSolution(m_iTile[x,y]) != GetGuess(m_iTile[x,y]))
					        return false;
			        }
		        }
	        }
	        //we win!
	        m_bIsGameInPlay = false;
	        return true;
        }

        public int GetGuessPiece(int iX, int iY)
        {
	        if(!IsTileValid(iX, iY))
		        return -1;

	        return m_iTile[iX,iY];

        }



        public int GetKakuroPiece(int iX, int iY)
        {
	        if(!IsTileValid(iX, iY))
		        return -1;

	        return m_iTile[iX,iY];

        }

        public EnumTilePiece GetTileType(int iX, int iY)
        {
	        if(IsTileValid(iX, iY) == false)
		        return EnumTilePiece.TP_Blank;

	        return (EnumTilePiece)GetType(m_iTile[iX,iY]);
        }

        bool IsTileValid(int iX, int iY)
        {
	        if(iX > m_iBoardSize ||
		        iY > m_iBoardSize)
		        return false;

	        if(iX < 0 ||
		        iY < 0)
		        return false;

	        return true;
        }

        public int GetPencilMarks(int iX, int iY)
        {
            if (GetTileType(iX, iY) != EnumTilePiece.TP_PencilMark)
                return 0;
            return GetPencil(GetGuessPiece(iX, iY));
        }

        public bool TogglePencil(int iNum, int iX, int iY)
        {
	        if(iNum < 1 || iNum > 9 || !IsTileValid(iX, iY))
		        return false;

	        int iTemp = (int)(Math.Pow(2, iNum-1));

	        if(GetType(m_iTile[iX,iY]) != (int)EnumTilePiece.TP_PencilMark)
	        {
                SetType(ref m_iTile[iX, iY], (int)EnumTilePiece.TP_PencilMark);
		        RecalcPencilMarks(iX, iY);
	        }

        //	TypePieceGuess* sGuess = GetGuessPiece(iX, iY);

	        int iPencil = GetPencil(m_iTile[iX,iY]);

	        if((iPencil & iTemp) == iTemp)
		        iPencil -= iTemp;
	        else
		        iPencil += iTemp;

	        SetPencil(ref m_iTile[iX,iY], iPencil);

	        CalcFreeCells();

            return true;
        }

        public void ClearPencil(int iX, int iY)
        {
	        if(!IsTileValid(iX, iY))
		        return;

            SetType(ref m_iTile[iX, iY], (int)EnumTilePiece.TP_PencilMark);
	        SetPencil(ref m_iTile[iX,iY], 0);
        }

        public bool SetGuessPiece(int iNum, int iX, int iY)
        {
	        if(iNum < 1 || iNum > 9 || !IsTileValid(iX, iY))
		        return false;

            m_sLastMove.eLastMove = EnumLastMove.LASTMOVE_Guess;
            m_sLastMove.iTile = m_iTile[iX,iY];
            m_sLastMove.iX = iX;
            m_sLastMove.iY = iY;

	        if(GetType(m_iTile[iX,iY]) != (int)EnumTilePiece.TP_Guess)
                SetType(ref m_iTile[iX, iY], (int)EnumTilePiece.TP_Guess);

	        int iGuess		= GetGuess(m_iTile[iX,iY]);
	        int iSolution	= GetSolution(m_iTile[iX,iY]);

	        if(iGuess == iNum)
	        {
		        iGuess = 0;
                SetType(ref m_iTile[iX, iY], (int)EnumTilePiece.TP_PencilMark);
	        }
	        else
	        {
		        iGuess = iNum;
		        //if(iGuess == iSolution || m_bShowErrors == false)
		        {
			        m_ptLastCorrectGuess.x = iX;
			        m_ptLastCorrectGuess.y = iY;
		        }
		        if(iGuess != iSolution)
			        m_iMistakeCounter++;
	        }

	        //and finally...set the guess
	        SetGuess(ref m_iTile[iX,iY], iGuess);
	        CalcFreeCells();
	        RecalcPencilMarks(iX, iY);
            return true;
        }

        public POINT GetLastCorrectGuess()
        {
	        POINT pt = m_ptLastCorrectGuess;

	        //erases it when you ask
	        m_ptLastCorrectGuess.x = 0;
	        m_ptLastCorrectGuess.y = 0;
	        return pt;
        }

        public bool IsPencilMarkPossible(int iX, int iY, int iValue)
        {
	        if(iValue < 1 || iValue > 9 || !IsTileValid(iX, iY))
		        return false;



	        if(GetType(m_iTile[iX,iY]) == (int)EnumTilePiece.TP_Blank ||
		        GetType(m_iTile[iX,iY]) == (int)EnumTilePiece.TP_Kakuro)
		        return false;

	        //now we actually check the pencil mark

	        if(!IsPencilValueValid(iValue, FindHorzTotal(iX, iY, false), FindHorzCWSize(iX, iY, false)))
		        return false;
	        if(!IsPencilValueValid(iValue, FindVertTotal(iX, iY, false), FindVertCWSize(iX, iY, false)))
		        return false;

	        return true;

        }


        bool IsPencilMarkValid(int iX, int iY, int iValue)
        {
	        if(IsPencilMarkPossible(iX, iY, iValue)==false)
		        return false;

	        if(iValue < 1 || iValue > 9 || !IsTileValid(iX, iY))
		        return false;

	        if(GetType(m_iTile[iX,iY]) == (int)EnumTilePiece.TP_Blank ||
		        GetType(m_iTile[iX,iY]) == (int)EnumTilePiece.TP_Kakuro)
		        return false;

	        //now we actually check the pencil mark
            if (!IsPencilValueValid(iValue, FindHorzTotal(iX, iY, true), FindHorzCWSize(iX, iY, true)))
		        return false;
            if (!IsPencilValueValid(iValue, FindVertTotal(iX, iY, true), FindVertCWSize(iX, iY, true)))
		        return false;

	        int iValueBin = (int)(Math.Pow(2, iValue));

	        int iHorz = GetHorzGuesses(iX, iY);
	        int iVert = GetVertGuesses(iX, iY);
	        if((iHorz & iValueBin) == iValueBin ||
		        (iVert & iValueBin )== iValueBin)
		        return false;

	        return true;
        }

        int FindHorzTotal(int iX, int iY, bool bCheat)
        {
	        if(!IsTileValid(iX, iY))
		        return 0;	

	        if(GetType(m_iTile[iX,iY])== (int)EnumTilePiece.TP_Blank)
		        return 0;

	        int iTotal = 0;

	        int iTempX = iX;

	        while(GetType(m_iTile[iTempX,iY]) != (int)EnumTilePiece.TP_Kakuro)
		        iTempX--;

	        iTotal = GetHorzTotal(m_iTile[iTempX,iY]);
	        iTempX++;

	        //Now...if we're doing the cheater option....go back through the group and subtract off guesses
	        if(bCheat)
	        {
		        while(GetType(m_iTile[iTempX,iY]) != (int)EnumTilePiece.TP_Kakuro &&
			        GetType(m_iTile[iTempX,iY]) != (int)EnumTilePiece.TP_Blank)
		        {
			        if(iTempX >= m_iBoardSize)
				        break;
			        if(GetType(m_iTile[iTempX,iY]) == (int)EnumTilePiece.TP_Guess)
				        iTotal -= GetGuess(m_iTile[iTempX,iY]);
			
			        iTempX++;
		        }
		        //and finally...if we've selected a cell that already has a total......re-add it 
		        if(GetType(m_iTile[iX,iY]) == (int)EnumTilePiece.TP_Guess)
			        iTotal += GetGuess(m_iTile[iX,iY]);
	        }

	        return iTotal;
        }


        int FindVertTotal(int iX, int iY, bool bCheat)
        {
	        if(!IsTileValid(iX, iY))
		        return 0;	

	        if(GetType(m_iTile[iX,iY]) == (int)EnumTilePiece.TP_Blank)
		        return 0;

	        int iTotal = 0;

	        int iTempY = iY;

	        while(GetType(m_iTile[iX,iTempY]) != (int)EnumTilePiece.TP_Kakuro)
		        iTempY--;

	        iTotal = GetVertTotal(m_iTile[iX,iTempY]);

	        //Now...if we're doing the cheater option....go back through the group and subtract off guesses
	        if(bCheat)
	        {
		        iTempY++;

		        while(GetType(m_iTile[iX,iTempY]) != (int)EnumTilePiece.TP_Kakuro &&
			        GetType(m_iTile[iX,iTempY]) != (int)EnumTilePiece.TP_Blank)
		        {
			        if(iTempY >= m_iBoardSize)
				        break;
			        if(GetType(m_iTile[iX,iTempY]) == (int)EnumTilePiece.TP_Guess)
				        iTotal -= GetGuess(m_iTile[iX,iTempY]);
			        iTempY++;
		        }

		        //and finally...if we've selected a cell that already has a total...re-add it 
		        if(GetType(m_iTile[iX,iY]) == (int)EnumTilePiece.TP_Guess)
			        iTotal += GetGuess(m_iTile[iX,iY]);
	        }

	        return iTotal;

        }

        int FindHorzCWSize(int iX, int iY, bool bCheat)
        {
	        if(!IsTileValid(iX, iY))
		        return 0;	

	        int iTempX	= iX;
	        int iSize	= 0;

	        while(GetType(m_iTile[iTempX,iY]) != (int)EnumTilePiece.TP_Kakuro)
		        iTempX--;

	        iTempX++;

	        if(bCheat)
	        {
		        while(GetType(m_iTile[iTempX,iY]) != (int)EnumTilePiece.TP_Kakuro &&
			        GetType(m_iTile[iTempX,iY]) != (int)EnumTilePiece.TP_Blank)
		        {
			        if(iTempX >= m_iBoardSize)
				        break;
			        if(GetType(m_iTile[iTempX,iY]) == (int)EnumTilePiece.TP_PencilMark)
				        iSize++;
			        iTempX++;
		        }

		        //and finally...if the cell has a guess in it we still need to count it for the selection screen
		        if(GetType(m_iTile[iX,iY]) == (int)EnumTilePiece.TP_Guess)
			        iSize++;
	        }
	        else//count up the guess cells and the pencil mark cells
	        {
		        while(GetType(m_iTile[iTempX,iY]) != (int)EnumTilePiece.TP_Kakuro &&
			        GetType(m_iTile[iTempX,iY]) != (int)EnumTilePiece.TP_Blank)
		        {
			        if(iTempX >= m_iBoardSize)
				        break;
			        iSize++;
			        iTempX++;
		        }
	        }

	        return iSize;
        }

        int FindVertCWSize(int iX, int iY, bool bCheat)
        {
	        if(!IsTileValid(iX, iY))
		        return 0;	

	        int iTempY	= iY;
	        int iSize	= 0;

	        while(GetType(m_iTile[iX,iTempY]) != (int)EnumTilePiece.TP_Kakuro)
		        iTempY--;

	        iTempY++;

	        if(bCheat)
	        {
		        while(GetType(m_iTile[iX,iTempY]) != (int)EnumTilePiece.TP_Kakuro &&
			        GetType(m_iTile[iX,iTempY]) != (int)EnumTilePiece.TP_Blank)
		        {
			        if(iTempY >= m_iBoardSize)
				        break;
			        if(GetType(m_iTile[iX,iTempY]) == (int)EnumTilePiece.TP_PencilMark)
				        iSize++;
			        iTempY++;
		        }

		        //and finally...if the cell has a guess in it we still need to count it for the selection screen
		        if(GetType(m_iTile[iX,iY]) == (int)EnumTilePiece.TP_Guess)
			        iSize++;
	        }
	        else//count up the guess cells and the pencil mark cells
	        {
		        while(GetType(m_iTile[iX,iTempY]) != (int)EnumTilePiece.TP_Kakuro &&
			        GetType(m_iTile[iX,iTempY]) != (int)EnumTilePiece.TP_Blank)
		        {
			        if(iTempY >= m_iBoardSize)
				        break;
			        iSize++;
			        iTempY++;
		        }
	        }

	        return iSize;
        }

        int GetHorzGuesses(int iX, int iY)
        {
	        if(!IsTileValid(iX, iY))
		        return 0;	

	        int iTempX	= iX;
	        int iGuesses= 0;

	        while(GetType(m_iTile[iTempX,iY]) != (int)EnumTilePiece.TP_Kakuro)
		        iTempX--;

	        iTempX++;

	        while(GetType(m_iTile[iTempX,iY]) != (int)EnumTilePiece.TP_Kakuro &&
		        GetType(m_iTile[iTempX,iY]) != (int)EnumTilePiece.TP_Blank)
	        {
		        if(iTempX >= m_iBoardSize)
			        break;
		        else if(GetType(m_iTile[iTempX,iY]) == (int)EnumTilePiece.TP_Guess)
			        iGuesses += (int)Math.Pow(2, GetGuess(m_iTile[iTempX,iY]));
		        iTempX++;
	        }
	
	        //and finally...if its already been guessed
	        if(GetType(m_iTile[iX,iY]) == (int)EnumTilePiece.TP_Guess)
		        iGuesses -= (int)Math.Pow(2, GetGuess(m_iTile[iX,iY]));

	        return iGuesses;
        }

        int GetVertGuesses(int iX, int iY)
        {
	        if(!IsTileValid(iX, iY))
		        return 0;	

	        int iTempY	= iY;
	        int iGuesses= 0;

	        while(GetType(m_iTile[iX,iTempY]) != (int)EnumTilePiece.TP_Kakuro)
		        iTempY--;

	        iTempY++;

	        while(GetType(m_iTile[iX,iTempY]) != (int)EnumTilePiece.TP_Kakuro &&
		        GetType(m_iTile[iX,iTempY]) != (int)EnumTilePiece.TP_Blank)
	        {
		        if(iTempY >= m_iBoardSize)
			        break;
		        else if(GetType(m_iTile[iX,iTempY]) == (int)EnumTilePiece.TP_Guess)
			        iGuesses += (int)Math.Pow(2, GetGuess(m_iTile[iX,iTempY]));
		        iTempY++;
	        }

	        //and finally...if its already been guessed
	        if(GetType(m_iTile[iX,iY]) == (int)EnumTilePiece.TP_Guess)
		        iGuesses -= (int)Math.Pow(2, GetGuess(m_iTile[iX,iY]));
	        return iGuesses;
        }

        bool IsPencilValueValid(int iValue, int iTotal, int iGroupSize)
        {
	        if(iTotal < 1 || 
		        iGroupSize < 1)
		        return false;

        //	int iUpperLimit = 0;
        //	int iLowerLimit = 0;

	        switch(iGroupSize)
	        {
	        case 1:
		        if(iValue != iTotal)
			        return false;
		        return true;
		        
	        default://lets use the table instead ... should be more accurate
		        {
			        int iPosValues = GetGuessPossibilities(iGroupSize, iTotal);
			        return IsGuessValid(iValue, iPosValues);
		        }
		        
	        }
        //	return false;
        }

        void IncrementTimer()
        {
	        if(m_bIsGameInPlay)
		        m_iNumSeconds++;
        }

        void CalcFreeCells()
        {
	        if(!m_bIsGameInPlay)
		        return;

	        m_iNumFreeCells = 0;
	        for(int x=0; x<m_iBoardSize; x++)
	        {
		        for(int y=0; y<m_iBoardSize; y++)
		        {
			        if(GetType(m_iTile[x,y]) == (int)EnumTilePiece.TP_PencilMark)
				        m_iNumFreeCells++;
		        }
	        }
        }



        void AutoFillPencilMarks()
        {	
	        if(!m_bAutoPencilMarks)
		        return;

	        for(int x = 0; x < m_iBoardSize; x++)
	        {
		        for(int y = 0; y < m_iBoardSize; y++)
		        {
			        if(GetType(m_iTile[x,y]) == (int)EnumTilePiece.TP_PencilMark)
			        {
				        ClearPencil(x, y);	
				        for(int i = 0; i < 9; i++)
				        {
					        if(IsPencilMarkValid(x,y,i+1))
					        {
						        TogglePencil(i+1, x, y);
					        }
				        }
			        }
		        }
	        }
        }

        void RecalcPencilMarks(int iX, int iY)
        {
	        if(!m_bAutoPencilMarks && !m_bAutoRemovePencilMarks )
		        return;

            //don't clear out the pencil marks if you guessed wrong
            if(!m_bAutoPencilMarks && m_bAutoRemovePencilMarks && m_bShowErrors &&
                GetGuess(GetGuessPiece(iX, iY)) != GetSolution(GetGuessPiece(iX, iY)))
                return;

            int iValue = GetGuess(GetGuessPiece(iX, iY));
            if(iValue < 0 && !m_bAutoPencilMarks)
                return;

	        if(!IsTileValid(iX, iY))
		        return;	

	        int iTempX	= iX;
	        int iTempY  = iY;

	        while(GetType(m_iTile[iTempX,iY]) != (int)EnumTilePiece.TP_Kakuro)
		        iTempX--;

	        iTempX++;

	        while(GetType(m_iTile[iTempX,iY]) != (int)EnumTilePiece.TP_Kakuro &&
		        GetType(m_iTile[iTempX,iY]) != (int)EnumTilePiece.TP_Blank)
	        {
		        if(iTempX >= m_iBoardSize)
			        break;
		        else if(GetType(m_iTile[iTempX,iY]) == (int)EnumTilePiece.TP_PencilMark)
		        {
                    if(m_bAutoPencilMarks)
                    {
			            ClearPencil(iTempX, iY);	
			            for(int i = 0; i < 9; i++)
			            {
				            if(IsPencilMarkValid(iTempX,iY,i+1))
				            {
					            TogglePencil(i+1, iTempX, iY);
				            }
			            }
                    }
                    else
                    {   //auto remove
                        int iTemp = (int)(Math.Pow(2, iValue-1));
                        int iPencil = GetPencil(m_iTile[iTempX,iY]);

                        if((iPencil & iTemp) == iTemp)
                        {
                            iPencil -= iTemp;
                            SetPencil(ref m_iTile[iTempX,iY], iPencil);
                        }
                    }
		        }
		        iTempX++;
	        }

	        while(GetType(m_iTile[iX,iTempY]) != (int)EnumTilePiece.TP_Kakuro)
		        iTempY--;

	        iTempY++;

	        while(GetType(m_iTile[iX,iTempY]) != (int)EnumTilePiece.TP_Kakuro &&
		        GetType(m_iTile[iX,iTempY]) != (int)EnumTilePiece.TP_Blank)
	        {
		        if(iTempY >= m_iBoardSize)
			        break;
		        else if(GetType(m_iTile[iX,iTempY]) == (int)EnumTilePiece.TP_PencilMark)
		        {
                    if(m_bAutoPencilMarks)
                    {
			            ClearPencil(iX, iTempY);
			            for(int i = 0; i < 9; i++)
			            {
				            if(IsPencilMarkValid(iX,iTempY,i+1))
				            {
					            TogglePencil(i+1, iX, iTempY);
				            }
			            }
                    }
                    else
                    {   //auto remove
                        int iTemp = (int)(Math.Pow(2, iValue-1));
                        int iPencil = GetPencil(m_iTile[iX,iTempY]);

                        if((iPencil & iTemp) == iTemp)
                        {
                            iPencil -= iTemp;
                            SetPencil(ref m_iTile[iX,iTempY], iPencil);
                        }
                    }

		        }
		        iTempY++;
	        }
        }

        double GetAccuracy()
        {
	        double dbMistakes	= m_iMistakeCounter;
	        double dbMovesMade	= m_iTotalPlayableCells - m_iNumFreeCells;

	        if(dbMovesMade == 0)
		        return 0.0;
	        if(dbMistakes == 0)
		        return 100.0;
	        if(dbMovesMade == dbMistakes)//I don't like divide by 0s....
		        return 0.0;

	        return (dbMovesMade - dbMistakes)/dbMovesMade * 100;
        }

        int GetGuessPossibilities(int iNumCells, int iTotal)
        {
	        int iGuess = 511; // default to all possibilities

	        for(int i=0; i< Globals.sNumList.GetLength(0); i++)
	        {
                if (iNumCells == Globals.sNumList[i, 0] && iTotal == Globals.sNumList[i, 1])
		        {
                    iGuess = Globals.sNumList[i,2];
			        break;
		        }
	        }
	        return iGuess;
        }

        //Guess... user guess
        //NumberToCheck ... value from the array 
        bool IsGuessValid(int iGuess, int iNumberToCheck)
        {
            if ((iNumberToCheck & (int)Math.Pow(2, 9 - iGuess)) > 0)
                return true;
            return false;
        }

        public bool ClearAllPencilMarks()
        {
	        if(m_bAutoPencilMarks)
		        return false;

	        //ok..this is a little worse...go through every cell, check if its a pencil mark or guess...and clear it
	        for(int iX = 1; iX < m_iBoardSize; iX++)
	        {
		        for(int iY = 1; iY < m_iBoardSize; iY++)
		        {
			        if(GetType(m_iTile[iX,iY]) == (int)EnumTilePiece.TP_PencilMark ||
				        GetType(m_iTile[iX,iY]) == (int)EnumTilePiece.TP_Guess)
			        {
				        SetPencil(ref m_iTile[iX,iY], 0);
			        }
		        }
	        }

	        return true;
        }


        public void SolveGame()
        {
            for(int y = 0; y < m_iBoardSize; y++)
            {
                for(int x = 0; x < m_iBoardSize; x++)
                {
                    if(GetType(m_iTile[x,y]) == (int)EnumTilePiece.TP_PencilMark ||
                        (GetType(m_iTile[x,y]) == (int)EnumTilePiece.TP_Guess &&
                        GetGuess(GetGuessPiece(x, y)) != GetSolution(GetGuessPiece(x, y)))    )
                    {   //if its pencil marks or a wrong guess
                        SetGuess(ref m_iTile[x,y], GetSolution(m_iTile[x,y]));
                        SetType(ref m_iTile[x,y], (int)EnumTilePiece.TP_Guess);
                    }
                }
            }
            CalcFreeCells();
        }


        public bool Hint()
        {
	        if(m_bIsGameInPlay == false)
		        return false;
	
	        bool	bFound	= false;
	        int		iLoop	= 0;
	        int		iRandX	= -1;
	        int		iRandY	= -1;

	        while(bFound == false)
	        {
		        if(iLoop > 250)
			        return false;//too many tries

                iRandX = rand.Next() % m_iBoardSize;
		        iRandY = rand.Next()%m_iBoardSize;

		        if(GetType(m_iTile[iRandX,iRandY]) == (int)EnumTilePiece.TP_PencilMark)
		        {
			        SetGuess(ref m_iTile[iRandX,iRandY], GetSolution(m_iTile[iRandX,iRandY]));
			        SetType(ref m_iTile[iRandX,iRandY], (int)EnumTilePiece.TP_Guess);
			        m_ptLastCorrectGuess.x	= iRandX;
			        m_ptLastCorrectGuess.y	= iRandY;
                    m_ptLastHint.x          = iRandX;
                    m_ptLastHint.y          = iRandY;
                    //PostMessage(m_hWnd, WM_UPDATECELL, iRandX, iRandY);
			        bFound = true;
		        }
	        }

            CalcFreeCells();

	        m_iMistakeCounter++; //you cheat..you pay the price

	        return true;
        }

        public bool Undo()
        {
            if(m_sLastMove.eLastMove == EnumLastMove.LASTMOVE_None)
                return false;
 
            int iCell = m_iTile[m_sLastMove.iX,m_sLastMove.iY];

            m_iTile[m_sLastMove.iX,m_sLastMove.iY] = m_sLastMove.iTile;
            m_sLastMove.iTile = iCell;

            RecalcPencilMarks(m_sLastMove.iX, m_sLastMove.iY);
            CalcFreeCells();
            //lets just always post
            //PostMessage(m_hWnd, WM_UNDOMOVE, m_sLastMove.iX, m_sLastMove.iY);
            return true;
        }
        public POINT GetLastMove()
        {
            POINT ptRet = new POINT();
            ptRet.x = m_sLastMove.iX;
            ptRet.y = m_sLastMove.iY;

            return ptRet;
        }


    }
}
