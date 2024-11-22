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
using System.IO;
using System.IO.IsolatedStorage;
using System.Xml.Serialization;
using System.Collections.Generic;

namespace Sudoku.Engine
{
    public struct POINT
    {
        public int x;
        public int y;
    }

    public class SudokuGame
    {
        

        static int POINTS_PER_CELL = 500;
        static int POINTS_PER_ERROR	= 100;
        static int POINTS_PER_HINT = 500;
 //       static int POINTS_LOCKSTATE = 100;
        static int POINTS_MIN =	25;
        static int SUDOKU_Size = 9;

        private const string FILE_SudokuBoard = "Sudokub.txt";
        private const string FILE_SudokuValues = "Sudokuv.txt";

        protected EnumDifficulty		m_eDifficulty;				// Difficulty of the Game
        protected TypeLastMove m_sLastMove;

        Random rand = new Random(System.DateTime.Today.Minute + System.Environment.TickCount);
    

	    public POINT m_ptLastCorrectGuess = new POINT();
        public POINT m_ptLastWrongGuess = new POINT();
        public POINT m_ptLastHint = new POINT();

        protected int[,] m_iTile = new int[SUDOKU_Size, SUDOKU_Size];	// Tiles [1 bit given,1 bit locked,4 bits solution,4 bits guess,9 bits pencilmarks]
        protected int m_iBoardSize;						// board size, for now it's regular 9 by 9
        protected int m_iNumFreeCells;					// how many free cells are there currently
        protected int m_iNumSeconds;						// Number of Seconds that have passed
        protected int m_iHighScore;						// High Score
        protected int m_iScore;
        protected int m_iMistakeCounter;
    		
	    protected bool m_bShowLockedState;					// Should we show the locked items
        protected bool m_bIsGameInPlay;					// is there a game currently in progress?
        protected bool m_bAutoPencilMarks;
        protected bool m_bShowErrors;
        protected bool m_bAutoRemovePencilMarks;           // if not auto it'll remove pencil marks as you solve (kinda cool)




        public virtual int	GetScore(){return m_iScore;}
        public bool GetAutoPencil() { return m_bAutoPencilMarks; }
        public bool GetAutoRemovePencil() { return m_bAutoRemovePencilMarks; }
	//    void				SetAutoPencilMarks(bool bAuto){m_bAutoPencilMarks = bAuto; if(bAuto)AutoFillPencilMarks();}
        public void SetAutoRemovePencilMarks(bool bAuto) { m_bAutoRemovePencilMarks = bAuto; }
        public bool GetShowErrors() { return m_bShowErrors; }
        public void SetShowErrors(bool bShowErrors) { m_bShowErrors = bShowErrors; }
        public void SetAutoPencilMarks(bool bAuto) { m_bAutoPencilMarks = bAuto; if (bAuto)AutoFillPencilMarks(); }

        public int GetBoardSize() { return m_iBoardSize; }
        public bool IsGameInPlay() { return m_bIsGameInPlay; }
        public int GetNumFreeCells() { return m_iNumFreeCells; }
        public int GetNumSeconds() { return m_iNumSeconds; }
            // Options stuff
        public EnumDifficulty GetDifficulty() { return m_eDifficulty; }
        public bool IsLocked() { return m_bShowLockedState; }
        public int GetHighScore() { return m_iHighScore; }
        public void SetDifficulty(EnumDifficulty eDifficulty) { m_eDifficulty = eDifficulty; }

        public SudokuGame()
        {
            m_iBoardSize = SUDOKU_Size;
            m_bIsGameInPlay = false;
            m_iMistakeCounter = 0;
            m_iNumFreeCells = 0;
            m_iScore = 0;
            m_iNumSeconds = 0;
            m_iHighScore = 0;
            m_bShowLockedState = true;
            m_bShowErrors = true;
            m_bAutoPencilMarks = false;
            m_bAutoRemovePencilMarks = false;
            m_eDifficulty = EnumDifficulty.DIFF_Easy;
            
            m_sLastMove.eLastMove = EnumLastMove.LASTMOVE_None;
	        m_ptLastCorrectGuess.x	= -1;
	        m_ptLastCorrectGuess.y	= -1;
	        m_ptLastWrongGuess.x	= -1;
	        m_ptLastWrongGuess.y	= -1;
	        m_ptLastHint.x			= -1;
	        m_ptLastHint.y			= -1;
        }

        protected void NewBoard()
        {
	        // reset our tile pieces
            for(int i = 0; i < 9; i++)
            {
                for(int j = 0; j < 9; j++)
                {
                    m_iTile[i,j] = 0;
                }
            }
            
	        m_iNumFreeCells = 0;
	        m_iNumSeconds	= 0;
        }

        public bool NewGame()
        {
            return NewGame(m_eDifficulty);
        }

        public virtual bool NewGame(EnumDifficulty eDiff)
        {
	        // start the wait cursor because this could take a few seconds

            // clear the board off, reset everything, then load a new puzzle
	        // get the new puzzle
	        int[,] iPuzzleArray = new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];
	        int[,] iSolutionArray = new int[Globals.NUM_ROWS,Globals.NUM_COLUMNS];
            SudokuPuzzle Puzzle = new SudokuPuzzle();
	        // create an array used to pass back some info here about the puzzle generating performance

            m_eDifficulty = eDiff;
	        Puzzle.GetNewPuzzle(m_eDifficulty, ref iPuzzleArray, ref iSolutionArray);

	        // reset our tile pieces
	        NewBoard();

	        // reset all the cells with the new puzzle
	        for (int i = 0; i < Globals.NUM_ROWS; i++)				// y
	        {
		        for (int j = 0; j < Globals.NUM_COLUMNS; j++)		// x
		        {
			        SetSolution(j, i, iSolutionArray[i,j]);
			        SetIsGiven(j, i, (iPuzzleArray[i,j]==0?false:true));
		        }
	        }

	        m_bIsGameInPlay = true;
	        m_iNumFreeCells = 0;
	        m_iNumSeconds	= 0;
	        m_iScore		= 0;
	        m_ptLastCorrectGuess.x	= -1;
	        m_ptLastCorrectGuess.y	= -1;
            m_sLastMove.eLastMove = EnumLastMove.LASTMOVE_None;

	        // check the full solution for any locked states
	        CheckSolution();

	        // see how many free cells there are
	        CalcFreeCells();
	        AutoFillPencilMarks();

	        return true;
        }

        public virtual void SaveGameState()
        {
            List<int> arrTiles = new List<int>();
            List<int> arrOtherValues = new List<int>();

            for(int y=0; y<SUDOKU_Size; y++)
            {
                for(int x=0; x<SUDOKU_Size; x++)
                {
                    arrTiles.Add(m_iTile[x,y]);
                }
            }

            arrOtherValues.Add(m_iNumSeconds);
            arrOtherValues.Add(m_iNumFreeCells);
            arrOtherValues.Add(m_iScore);

            try
            {
                XmlSerializer ser = new XmlSerializer(typeof(List<int>));
                TextWriter writer = new StreamWriter(FILE_SudokuBoard);
                ser.Serialize(writer, arrTiles);
                writer.Close();

                writer = new StreamWriter(FILE_SudokuValues);
                ser.Serialize(writer, arrOtherValues);
                writer.Close();
            }
            catch (System.Exception ex)
            {
            }
        }

        public virtual void LoadGameState()
        {
            XmlSerializer ser = new XmlSerializer(typeof(List<int>));

            try
            {
                TextReader reader = new StreamReader(FILE_SudokuBoard);

                //and serialize data
                List<int> arrTiles = (List<int>)ser.Deserialize(reader);

                if (arrTiles.Count == SUDOKU_Size * SUDOKU_Size)
                {
                    for (int y = 0; y < SUDOKU_Size; y++)
                    {
                        for (int x = 0; x < SUDOKU_Size; x++)
                        {
                            m_iTile[x, y] = arrTiles[y * SUDOKU_Size + x];
                        }
                    }
                }

                reader.Close();

                reader = new StringReader(FILE_SudokuValues);

                //and serialize data
                List<int> arrOtherValues = (List<int>)ser.Deserialize(reader);

                if (arrOtherValues.Count == 3)
                {
                    m_iNumSeconds = arrOtherValues[0];
                    m_iNumFreeCells = arrOtherValues[1];
                    m_iScore = arrOtherValues[2];

                    CalcFreeCells();
                }

                reader.Close();


            }
            catch (System.Exception ex)
            {
            	
            }
            m_bIsGameInPlay = true;
        }


        public POINT GetLastCorrectGuess()
        {
	        POINT pt = m_ptLastCorrectGuess;

	        //erases it when you ask
	        m_ptLastCorrectGuess.x = -1;
	        m_ptLastCorrectGuess.y = -1;
	        return pt;
        }

        public POINT GetLastWrongGuess()
        {
	        POINT pt = m_ptLastWrongGuess;

	        //erases it when you ask
	        m_ptLastWrongGuess.x = -1;
	        m_ptLastWrongGuess.y = -1;
	        return pt;
        }

        public POINT GetLastHint()
        {
	        POINT pt = m_ptLastHint;

	        //erases it when you ask ... don't see why (Nov 09)
	        //m_ptLastHint.x = -1;
	        //m_ptLastHint.y = -1;
	        return pt;
        }

        void StopGame()
        {
	        // let's save what's there just in case
	        if(m_bIsGameInPlay)
		    {
                //SaveRegistry();
            }

	        m_bIsGameInPlay	= false;
        }

        public void RestartGame()
        {
	        m_ptLastCorrectGuess.x	= -1;
	        m_ptLastCorrectGuess.y	= -1;
	        m_iMistakeCounter		= 0;
	        m_iScore				= 0;
            m_sLastMove.eLastMove = EnumLastMove.LASTMOVE_None;

	        m_bIsGameInPlay = true;
	        m_iNumSeconds	= 0;

            bool bShowLocked = m_bShowLockedState;
            m_bShowLockedState = false;

	        for(int x = 0; x < m_iBoardSize; x++)
	        {
		        for(int y=0; y < m_iBoardSize; y++)
		        {
			        SetGuess(x,y,0);
                    SetPencilMarks(x,y,0);
                    SetIsLocked(x, y, false);
		        }
	        }
            m_bShowLockedState = bShowLocked;
            AutoFillPencilMarks();
            
        }

        public bool IsGameComplete()
        {
	        if(m_bIsGameInPlay == false)
		        return true;

	        if(m_iNumFreeCells > 0)
		        return false;

	        //lets actually check then
	        for(int x = 0; x < m_iBoardSize; x++)
	        {
		        for(int y=0; y < m_iBoardSize; y++)
		        {
			        if(!IsGiven(x,y) && !IsLocked(x,y))
			        {
				        //make sure the guess equals the solution
				        if(GetSolution(x,y) != GetGuess(x,y))
					        return false;
			        }
		        }
	        }
	        //we win!
	        m_bIsGameInPlay = false;
	        return true;	
        }

        public void SolveGame()
        {
	        m_bIsGameInPlay = false;
	        m_iNumFreeCells	= 0;

	        // set the whole board the solution and locked (if applicable)
	        for(int x = 0; x < m_iBoardSize; x++)
	        {
		        for(int y=0; y < m_iBoardSize; y++)
		        {
                    if(IsGiven(x, y))
                        continue;
                    if(IsLocked(x, y))
                        continue;
                    if(GetGuess(x, y) == GetSolution(x, y))
                        continue;
                    //much better
			        SetGuess(x,y, GetSolution(x,y));
			        SetIsLocked(x,y, true);
		        }
	        }
        }


        public bool IsGiven(int iX, int iY)
        {
	        int iTile = GetTile(iX, iY);
	        if((iTile&1) > 0)
                return true;
            return false;
        }

        public bool IsLocked(int iX, int iY)
        {
	        int iTile = GetTile(iX, iY);

            if( ((iTile&2)>>1) > 0)
                return true;
            return false;
        }

        public int GetSolution(int iX, int iY)
        {
	        int iTile = GetTile(iX, iY);
	        return (iTile&60)>>2;
        }

        public int GetGuess(int iX, int iY)
        {
	        int iTile = GetTile(iX, iY);
	        return (iTile&960)>>6;
        }

        public int GetPencilMarks(int iX, int iY)
        {
	        int iTile = GetTile(iX, iY);
	        return (iTile&523264)>>10;
        }

        bool IsValid(int iX, int iY)
        {
	        if(iX > m_iBoardSize ||	iY > m_iBoardSize)
		        return false;

	        if(iX < 0 || iY < 0)
		        return false;

	        return true;
        }

        int	GetTile(int iX, int iY)
        {
	        if(!IsValid(iX, iY)) 
		        return 0;

	        return m_iTile[iX,iY];
        }

        protected void SetIsGiven(int iX, int iY, bool bIsGiven)
        {
	        int iTile = GetTile(iX, iY);
	        iTile	&= ~1;
	        iTile	|= ((bIsGiven?1:0)&1);
	        SetTile(iX, iY, iTile);
        }

        void SetIsLocked(int iX, int iY, bool bIsLocked)
        {
	        int iTile = GetTile(iX, iY);
	        iTile	&= ~2;

	        iTile	|= (((bIsLocked?1:0)<<1)&2);
	        SetTile(iX, iY, iTile);
        }

        protected void SetSolution(int iX, int iY, int iSolution)
        {
	        int iTile = GetTile(iX, iY);
	        iTile	&= ~60;
	        iTile	|= ((iSolution<<2)&60);
	        SetTile(iX, iY, iTile);
        }

        public bool OnSetGuess(int iX, int iY, int iGuess)
        {
	        // don't change anything if we're a given or if we're locked
	        if(IsGiven(iX,iY) || IsLocked(iX, iY))
		        return false;

            m_sLastMove.eLastMove = EnumLastMove.LASTMOVE_Guess;
            m_sLastMove.iTile = m_iTile[iX,iY];
            m_sLastMove.iX = iX;
            m_sLastMove.iY = iY;

	        // this is if we want to erase the current guess
	        if(iGuess == GetGuess(iX, iY))
		        iGuess = 0;

	        SetGuess(iX, iY, iGuess);

	        // we only care if the guess is valid
	        if(iGuess != 0)
	        {
		        if(GetSolution(iX, iY) == GetGuess(iX, iY) || m_bShowErrors == false)
		        {
			        m_ptLastCorrectGuess.x = iX;
			        m_ptLastCorrectGuess.y = iY;
		        }
		        else if(GetSolution(iX, iY) != GetGuess(iX, iY))
		        {
			        m_ptLastWrongGuess.x	= iX;
			        m_ptLastWrongGuess.y	= iY;
			        m_iMistakeCounter++;//might not use it but now its here
		        }
	        }

	        if(GetSolution(iX, iY) == GetGuess(iX, iY))
	        {
		        int iPoints = POINTS_PER_CELL - m_iNumSeconds;
		        if(m_bShowLockedState)
			        iPoints -= 100;//100 points off for the lock state
		        if(m_bAutoPencilMarks)
			        iPoints /= 2;//half points for auto pencil
		        if(iPoints < POINTS_MIN)
			        iPoints = POINTS_MIN;//min points (and no negatives for being slow)
		        m_iScore += iPoints;
	        }
	        else
	        {	//you're wrong and you lose points
		        m_iScore -= POINTS_PER_ERROR;
		        if(m_iScore < 0)
			        m_iScore = 0;
	        }

	        //update the highscore
	        if(m_iScore > m_iHighScore)
		        m_iHighScore = m_iScore;

	        RecalcPencilMarks(iX, iY);
	        CalcFreeCells();
            return true;
        }

        protected void SetGuess(int iX, int iY, int iGuess)
        {
	        int iTile = GetTile(iX, iY);
	        iTile	&= ~960;
	        iTile	|= ((iGuess<<6)&960);
	        SetTile(iX, iY, iTile);

            CheckSolution();
        }


        public void SetPencilMarks(int iX, int iY, int iPencilMarks)
        {
	        int iTile = GetTile(iX, iY);
	        iTile	&= ~523264;
	        iTile	|= ((iPencilMarks<<10)&523264);
	        SetTile(iX, iY, iTile);
        }

        void SetTile(int iX, int iY, int iTile)
        {
	        if(!IsValid(iX, iY))
		        return;

	        m_iTile[iX,iY] = iTile;
        }

        void IncrementTimer()
        {
	        if(m_bIsGameInPlay)
		        m_iNumSeconds++;
        }

        public void ClearPencilMarks(int iX, int iY)
        {
            SetPencilMarks(iX, iY, 0);
        }

        public bool TogglePencilMarks(int iX, int iY, int iNumber)
        {
	        if(iNumber < 1 || iNumber > 9 || !IsValid(iX, iY))
		        return false;

        /*    m_sLastMove.eLastMove = LASTMOVE_Pencil;
            m_sLastMove.iTile = m_iTile[iX,iY];
   
            m_sLastMove.iX = iX;
            m_sLastMove.iY = iY;*/

	        int iTemp = (int)(Math.Pow(2, iNumber-1));

	        // make sure to get rid of the guess just in case
            if(GetGuess(iX, iY) > 0)
            {
                SetGuess(iX, iY, 0);
            }
	        //RecalcPencilMarks(iX, iY);
	
	        int iPencil = GetPencilMarks(iX, iY);

	        if((iPencil & iTemp) == iTemp)
		        iPencil -= iTemp;
	        else
		        iPencil += iTemp;

	        SetPencilMarks(iX, iY, iPencil);

	        CalcFreeCells();

            return true;
        }

        protected void CalcFreeCells()
        {
	        m_iNumFreeCells = 0;
	        for(int x = 0; x < m_iBoardSize; x++)
	        {
		        for(int y=0; y < m_iBoardSize; y++)
		        {
			        if(!IsGiven(x,y) && GetGuess(x,y) == 0)
			        {
				        m_iNumFreeCells++;
			        }
		        }
	        }
        }

        protected void CheckSolution()
        {
	        for(int x = 0; x < m_iBoardSize; x++)
	        {
		        for(int y=0; y < m_iBoardSize; y++)
		        {
			        // it will return true if we have a full solution
			        if(CheckCellSolution(x,y))
				        return;
		        }
	        }
        }

        bool CheckCellSolution(int iX, int iY)
        {
	        bool bSolvedVal = false;
	        int iSolveCount = 0;

	        // check if column is solved
	        if(IsColumnSolved(iX))
	        {
		        iSolveCount++;
		        if(m_bShowLockedState)
		        {
			        for (int y = 0; y < GetBoardSize(); y++)
                    {
                        if(IsLocked(iX, y) == false)
                        {
				            SetIsLocked(iX, y, true);
                            if(m_bIsGameInPlay)
                            {
                                //PostMessage(m_hWnd, WM_UPDATECELL, iX, y);
                            }
                        }
                    }
		        }			
	        }

	        // check if row is solved
	        if(IsRowSolved(iY))
	        {
		        iSolveCount++;
		        if(m_bShowLockedState)
		        {
			        for (int x = 0; x < GetBoardSize(); x++)
                    {
                        if(IsLocked(x, iY) == false)
                        {
                            SetIsLocked(x, iY, true);
                            if(m_bIsGameInPlay)
                            {
                                //PostMessage(m_hWnd, WM_UPDATECELL, x, iY);
                            }
                        }
                    }
		        }
	        }

	        // check if subsection is solved
	        if (IsSubsectionSolved(iY, iX))
	        {
		        iSolveCount++;
		        if(m_bShowLockedState)
		        {
			        // set the lockstate on all cells in the subsection
			        int iRowSection		= GetSubsection(iY);
			        int iColumnSection	= GetSubsection(iX);
			        for (int y = iRowSection*3; y < iRowSection*3+3; y++)
			        {
				        for (int x = iColumnSection*3; x < iColumnSection*3+3; x++)
				        {
                            if(IsLocked(x, y) == false)
                            {
					            SetIsLocked(x,y, true);
                                if (m_bIsGameInPlay)
                                {
                                    //PostMessage(m_hWnd, WM_UPDATECELL, x, y);
                                }
                            }
				        }
			        }
		        }
	        }

	        // if all three are solved, check if the whole game is solved
	        if (iSolveCount == 3)
	        {
		        if (IsGameSolved())
		        {
			        bSolvedVal = true;
			        if(m_bShowLockedState)
			        {
				        // make sure to set everything as locked right now
				        for (int y = 0; y < m_iBoardSize; y++)
				        {
					        for (int x = 0; x < m_iBoardSize; x++)
					        {
						        SetIsLocked(x,y,true);
					        }
				        }
			        }
		        }
	        }
	        return bSolvedVal;
        }

        bool IsGameSolved()
        {
	        // check the whole board and see if we're solved
	        for (int y = 0; y < m_iBoardSize; y++)
	        {
		        for (int x = 0; x < m_iBoardSize; x++)
		        {
			        if(!IsSolved(x,y))
				        return false;
		        }
	        }
	        return true;
        }

        bool IsColumnSolved(int iColumnIndex)
        {
	        // go through each cell of the column and check if either the current guess is equal
	        // to the solution value, or if the cell value is given
	        for (int y = 0; y < m_iBoardSize; y++)
	        {
		        if (!IsSolved(iColumnIndex, y))
			        return false;
	        }
	        return true;
        }

        bool IsRowSolved(int iRowIndex)
        {
	        // go through each cell of the row and check if either the current guess is equal
	        // to the solution value, or if the cell value is given
	        for (int x = 0; x < m_iBoardSize; x++)
	        {
		        if (!IsSolved(x, iRowIndex))
			        return false;
	        }

	        return true;
        }

        bool IsSubsectionSolved(int iRowIndex, int iColumnIndex)
        {
	        // go through each cell of the subsection and check if either the current guess is equal
	        // to the solution value, or if the cell value is given

	        // consider the subsections to make up a 3x3 array in the puzzle, and find which subsection
	        // the given cell is in
	        int iSubRow		= GetSubsection(iRowIndex);
	        int iSubColumn	= GetSubsection(iColumnIndex);

	        // now iterate through all cells in a subsection to see if they are all solved
	        for (int y = iSubRow*3; y < iSubRow*3 + 3; y++)
	        {
		        for (int x = iSubColumn*3; x < iSubColumn*3 + 3; x++)
		        {
			        if (!IsSolved(x,y))
				        return false;
		        }
	        }
	        return true;
        }



        int GetSubsection(int iRowColumnIndex)
        {
	        int iRet = 0;
	        switch(iRowColumnIndex)
	        {	
	        case 0:
	        case 1:
	        case 2:
		        {
			        iRet = 0;
			        break;
		        }
	        case 3:
	        case 4:
	        case 5:
		        {
			        iRet = 1;
			        break;
		        }
	        case 6:
	        case 7:
	        case 8:
		        {
			        iRet = 2;
			        break;
		        }
	        }
	        return iRet;
        }

        bool IsSolved(int iX, int iY)
        {
	        return (IsGiven(iX, iY) || IsLocked(iX, iY) || GetSolution(iX, iY) == GetGuess(iX, iY));
        }

        void ResetLockedItems()
        {
	        // if we show the locked states
	        if(m_bShowLockedState)
		        CheckSolution();
	        else
	        {
		        // set all locked states to false
		        for (int y = 0; y < m_iBoardSize; y++)
		        {
			        for (int x = 0; x < m_iBoardSize; x++)
			        {
				        SetIsLocked(x,y, false);
			        }
		        }
	        }
        }

        public bool ClearAllPencilMarks()
        {
	        if(m_bAutoPencilMarks)
		        return false;

	        for(int x = 0; x < SUDOKU_Size; x++)
	        {
		        for(int y = 0; y < SUDOKU_Size; y++)
		        {
			        SetPencilMarks(x,y,0);//just do em all .. I don't care
		        }
	        }

	        return true;
        }

        public bool Hint()
        {
	        if(m_bIsGameInPlay == false)
		        return false;

            bool	bFound	= false;
	        //int		iLoop	= 0;
	        int		iRandX	= -1;
	        int		iRandY	= -1;

	        iRandX = rand.Next() % SUDOKU_Size;
            iRandY = rand.Next() % SUDOKU_Size;	


	        while(bFound == false)
	        {
		        if(IsGiven(iRandX, iRandY) ||
			        (GetGuess(iRandX, iRandY) == GetSolution(iRandX, iRandY)) )
		        {
			        iRandX++;
			        if(iRandX == SUDOKU_Size)
			        {
				        iRandX = 0;
				        iRandY++;
			        }
			        if(iRandY == SUDOKU_Size)
				        iRandY = 0;
			        continue;
		        }
		        //enjoy your freebee
		        //so we get the animation ... update the free cells, etc
		        SetGuess(iRandX, iRandY, GetSolution(iRandX, iRandY));
		        RecalcPencilMarks(iRandX, iRandY);
                CalcFreeCells();
		        CheckCellSolution(iRandX, iRandY);
		        m_ptLastHint.x	= iRandX;
		        m_ptLastHint.y	= iRandY;
		        m_iScore -= POINTS_PER_HINT;
                //PostMessage(m_hWnd, WM_UPDATECELL, iRandX, iRandY);
		        if(m_iScore < 0)
			        m_iScore = 0;
		        break;
	        }

	        return true;
        }

        protected void AutoFillPencilMarks()
        {
	        if(!m_bAutoPencilMarks || m_bIsGameInPlay == false)
		        return;

	        for(int x = 0; x < m_iBoardSize; x++)
	        {
		        for(int y = 0; y < m_iBoardSize; y++)
		        {
			        if(IsGiven(x,y))
				        continue;
			        if(GetGuess(x,y) != 0)
				        continue;
			        SetPencilMarks(x,y,0);//blank it out
			        for(int i =0; i < SUDOKU_Size; i++)//size ... same as the number of pencil mark options
			        {
				        if(IsPencilMarkValid(x,y,i+1))
				        {
					        TogglePencilMarks(x,y,i+1);
				        }
			        }
		        }
	        }
        }

        protected bool IsPencilValid(int iX, int iY, int iValue)
        {   //I couldn't figure out how to call the base class of a double in heritance and I didn't want to cast since this can be call a lot
            return this.IsPencilMarkValid(iX, iY, iValue);
        }

        public virtual bool IsPencilMarkValid(int iX, int iY, int iValue)
        {
	        int i;
	        //Check row
	        for(i = 0; i < SUDOKU_Size; i++)
	        {
		        if(i == iX)
			        continue;
		        if(IsGiven(i, iY))
		        {
			        if(GetSolution(i, iY) == iValue)
				        return false;
		        }
		        else if(GetGuess(i, iY) == iValue)
			        return false;
	        }


	        //check column
	        for(i = 0; i < SUDOKU_Size; i++)
	        {
		        if(i == iY)
			        continue;
		        if(IsGiven(iX, i))
		        {
			        if(GetSolution(iX, i) == iValue)
				        return false;
		        }
		        else if(GetGuess(iX, i) == iValue)
			        return false;
	        }

	        //check nanet (3x3)
	        for(int x = iX-iX%3; x < iX+(3-iX%3); x++)
	        {
		        for(int y = iY-iY%3; y < iY+(3-iY%3); y++)
		        {
			        if(IsGiven(x, y))
			        {
				        if(GetSolution(x, y) == iValue)
					        return false;
			        }
			        else if(GetGuess(x, y) == iValue)
				        return false;		

		        }
	        }
	        return true;
        }

        bool ReCalcCell(int iX, int iY, int iXCell, int iYCell)
        {
	        if(iX/3 != iXCell/3 && iY != iYCell)
		        return false;
	        if(iY/3 != iYCell/3 && iX != iXCell)
		        return false;

	        return true;
        }


        void RecalcPencilMarks(int iX, int iY)
        {
	        if(!m_bAutoPencilMarks && !m_bAutoRemovePencilMarks)
		        return;

            //don't clear out the pencil marks if you guessed wrong
            if(!m_bAutoPencilMarks && m_bAutoRemovePencilMarks && m_bShowErrors &&
                GetGuess(iX, iY) != GetSolution(iX, iY))
                return;


            if(m_bAutoPencilMarks)
            {
	            //only need to calc the row, column and nanet
	            //brute force for now
	            for(int x = 0; x < m_iBoardSize; x++)
	            {
		            for(int y = 0; y < m_iBoardSize; y++)
		            {
			            if(!ReCalcCell(x,y,iX,iY))
				            continue;//save a little work
			            if(IsGiven(x,y))
				            continue;
			            if(GetGuess(x,y) != 0)
				            continue;
			            SetPencilMarks(x,y,0);//blank it out
			            for(int i =0; i < SUDOKU_Size; i++)//size ... same as the number of pencil mark options
			            {
				            if(IsPencilMarkValid(x,y,i+1))
				            {
					            TogglePencilMarks(x,y,i+1);
				            }
			            }
		            }
	            }
            }

            //not needed twice
            else if(m_bAutoRemovePencilMarks)
            {
                POINT pt = new POINT();
                pt.x = iX;
                pt.y = iY;

                int iValue = GetGuess(iX, iY);
                if(iValue < 0)
                    return;//no guess

                //if(GetLastCorrectGuess != pt && GetLastWrongGuess() != pt)
                //    return;//wasn't a guess so I don't care ...

                for(int x = 0; x < m_iBoardSize; x++)
                {
                    for(int y = 0; y < m_iBoardSize; y++)
                    {
                        if(!ReCalcCell(x,y,iX,iY))
                            continue;//save a little work
                        if(IsGiven(x,y))
                            continue;
                        if(GetGuess(x,y) != 0)
                            continue;

                        int iTemp = (int)(Math.Pow(2, iValue-1));

                        int iPencil = GetPencilMarks(x, y);

                        if((iPencil & iTemp) == iTemp)
                        {
                            iPencil -= iTemp;
                            SetPencilMarks(x, y, iPencil);
                        }
                    }
                }
            }
        }

        public bool Undo()
        {
         //   OnSetGuess(m_sLastMove.iX,m_sLastMove.iY, 5);
         //   return true;

            if(m_sLastMove.eLastMove == EnumLastMove.LASTMOVE_None)
                return false;
            if(IsLocked(m_sLastMove.iX, m_sLastMove.iY))
                return false;//don't get to unlock ... to bad

            int iCell = GetTile(m_sLastMove.iX,m_sLastMove.iY);
    
            SetTile(m_sLastMove.iX,m_sLastMove.iY,m_sLastMove.iTile);
         //   SetGuess(m_sLastMove.iX,m_sLastMove.iY,1);
            m_sLastMove.iTile = iCell;
            //lets just always post
            RecalcPencilMarks(m_sLastMove.iX, m_sLastMove.iY);
            CalcFreeCells();
    
            //PostMessage(m_hWnd, WM_UNDOMOVE, m_sLastMove.iX, m_sLastMove.iY);
            return true;
        }

        public void SetLocked(bool bLocked)
        {
            m_bShowLockedState = bLocked;
            ResetLockedItems();
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
