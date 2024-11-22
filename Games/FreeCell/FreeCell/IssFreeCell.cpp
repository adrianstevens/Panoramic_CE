#include "IssFreeCell.h"
#include "IssRegistry.h"
#include "IssString.h"

struct TypeGameState
{	//and save the game arrays
	byte			btColumns[CARD_COLUMNS][MAX_CARDS_PER_COLUMN];//array to store cards 
	byte			btFreeCells[4];
	byte			btAceRacks[4];
	byte			btSelected; // -1 for nothing selected
	byte			btCardsRemaining;
	int				iGameNumber;
	int				iGamesWon;
	int				iGamesLost;
};

CIssFreeCell::CIssFreeCell(void):
m_hWnd(NULL),
m_btCardsRemaining(0),
m_btSelected(NULL_CARD),
m_iGameNumber(-1),
m_eAutoPlay(AUTOPLAY_On),
m_bPlaySounds(TRUE),
m_bGameInPlay(FALSE),
m_dwTickCount(0),
m_eTableColor(BGCOLOR_Green),
m_iGamesLost(0),
m_iGamesWon(0),
m_iCurrentStreak(0),
m_iLongestStreak(0),
m_bIsSolvable(TRUE),
m_bShowAnimations(TRUE),
m_bStrict(FALSE),
m_bInverse(FALSE),
m_bShowTimer(FALSE),
m_eFCCount(FCCOUNT_4),
m_eGameType(FCTYPE_Freecell)
{
	LoadRegistry();
	if(m_btCardsRemaining == 0)
		NewGame();//we'll create a new game
	DeleteUndoArray();
}

CIssFreeCell::~CIssFreeCell(void)
{
	SaveRegistry();
	CIssString* oStr = CIssString::Instance();
	oStr->DeleteInstance();
	DeleteUndoArray();
}

BOOL CIssFreeCell::SelectGame(int iGame)
{
	if(iGame < 0 || iGame > SEED_MAX)
		return FALSE;

	if(m_btCardsRemaining == 52)
		m_bGameInPlay = FALSE;//don't penalize if they haven't started
		
	//set the game number
	m_iGameNumber = iGame;
	//this of course needs to be called before NewGame since it will always set m_bGameInPlay to TRUE
	if(m_bGameInPlay && m_bIsSolvable)
	{	
		m_iGamesLost++;
		m_iCurrentStreak = 0;
	}

	NewGame(FALSE);//don't need it too seed a new game
	return TRUE;
}

// This does the actual work of resetting the game.
void CIssFreeCell::NewGame(BOOL bSelNewGame)
{
	m_bIsSolvable = TRUE;
	DeleteUndoArray();
	
	if(m_btCardsRemaining == 52)
		m_bGameInPlay = FALSE;//don't penalize if they haven't started

	if(bSelNewGame)
	{
		int randMax = SEED_MAX;
		srand((unsigned)GetTickCount());
		m_iGameNumber = ((unsigned)GetTickCount()+(unsigned)rand())%randMax;
		srand(m_iGameNumber);

		if(m_bGameInPlay && m_bIsSolvable)//we'll be nice and not penalize for unsolvable games
		{
			m_iGamesLost++; //increment the counter
			m_iCurrentStreak = 0;
		}
	}

	switch(m_iGameNumber)
	{	//known unsolvable puzzles ... won't penalize for failure 
	case 11982:
	case 146692:
	case 186216:
	case 455889:
	case 495505:
	case 512118:
	case 517776:
	case 781948:
	case 1155215: 
	case 1254900:
	case 1387739:
	case 1495908:
	case 1573069:
	case 1631319:
	case 1633509:
	case 1662054:
        if(m_eGameType == FCTYPE_Freecell)
		    m_bIsSolvable = FALSE;
        else 
            m_bIsSolvable = TRUE;
		break;
	default:
		m_bIsSolvable = TRUE;
	}


	m_bGameInPlay = TRUE;

	m_eLastFrom = CL_None;
	m_eLastTo	= CL_None;
	
	int deck[52]; // 52 card deck
	int di=0;

	for (int top = 0; top < 4; top++ ) 
	{
		m_btAceRacks[top]	= CARD_BLANK + 1 + top;//for the symbols
		m_btFreeCells[top]	= CARD_BLANK;
	}

	if ( m_iGameNumber == -2 ) 
	{
		DealUnsolvable();
	}
	else 
	{
		// Create a deck
		for ( di=0; di < 52; di++ ) 
		{
			deck[di]=di+1;
		}
		di--;

		srand(m_iGameNumber);
		int wLeft = 52;
		for ( int j=0; j<MAX_CARDS_PER_COLUMN; j++ ) 
		{ // For each row
            if(m_eGameType != FCTYPE_FourColors)
            {
			    for ( int i=0; i<8; i++ ) 
			    { // For each column
				    if (wLeft > 0) 
				    {
					    int r = rand() % wLeft;
					    int suit = (deck[r]-1)%4;// if ( suit == 0 ) suit = 4;suit--;
					    int rank = (deck[r]-1)/4;// if ( suit == 3 ) rank--;

					    m_btColumns[i][j] = suit*13+rank+1;
					    deck[r] = deck[--wLeft];
				    } 
				    else m_btColumns[i][j] = NULL_CARD; // empty rest of tableaux
			    }
            }
            else
            {//FOUR COLORS HERE 
                for ( int i=0; i<4; i++ ) 
                { // For each column
                    if (wLeft > 0) 
                    {
                        int r = rand() % wLeft;
                        int suit = (deck[r]-1)%4;// if ( suit == 0 ) suit = 4;suit--;
                        int rank = (deck[r]-1)/4;// if ( suit == 3 ) rank--;

                        m_btColumns[i][j] = suit*13+rank+1;
                        deck[r] = deck[--wLeft];

                        m_btColumns[i+4][j] = NULL_CARD;
                    } 
                    else m_btColumns[i][j] = NULL_CARD; // empty rest of tableaux
                }
            }
		}
	}

	//lets cheat this up for testing
//	for(int i = 0; i < 7; i++)
//	{
//		m_btColumns[6][bt] = NULL_CARD;
//		m_btColumns[7][bt] = NULL_CARD;
//	}

	m_btSelected		= NULL_CARD; // No card is m_btSelected
	m_btCardsRemaining	= 52;
}

// looks stupid but we'll leave it in for now....might be needed for game generation
void CIssFreeCell::DealUnsolvable() 
{
	int l=14;
	int r=7;
	for ( int j=0; j<=24; j++ ) { // For each column > - - - - - - - - v
		for ( int i=0; i<=7; i++ ) { // For each row
			m_btColumns[i][j]=0;
			if ( i < 4 && l > 7 ) { // First four
				int ml=l;
				if ( l == 14 ) ml=1;
				m_btColumns[i][j]=ml+(13*i);
			}
			else if ( j <= 5 && r > 1 ) { // Second four
				m_btColumns[i][j]=r+(13*(i-4));
			}
		}
		l--;
		r--;
	}
}

BOOL CIssFreeCell::ReloadGame()
{
	return TRUE;
}




BOOL CIssFreeCell::IsCardBlack(byte btCard)
{
	return btCard >= 14 && btCard <= 39 ? FALSE : TRUE;
}


//numbered from 1 to 52
byte CIssFreeCell::GetCardNum(byte btCard)// Return the number of the card
{
	if(btCard > 52)//for non-card values .. this is correct
		return 0;
	byte btNum = btCard % 13;
	if (btNum == 0) 
		btNum=13;
	return btNum;
}

byte CIssFreeCell::GetCardSuit(byte btCard)
{
    if(btCard > 52 || btCard < 1)//for non-card values .. this is correct
        return -1;
    byte btSuit = (btCard -1) / 13;
    if(btSuit > 3)
        btSuit = 3;
    return btSuit;
}

BOOL CIssFreeCell::ValidateAceRack(byte btCol, byte btCard)
{
	if ( GetCardNum(btCard) == 1 ) 
	{ // The aces
		if ( m_btAceRacks[btCol] == 0 ) 
			return 1; // Column is empty
		else return 0;
	}
	else if ( m_btAceRacks[btCol] == 0 ) 
		return 0; // Since it's not an ace, if the column is empty, bail.
	else if ( btCard > 52 ) 
	{
		return 0;
	}
	else if ( btCard == m_btAceRacks[btCol]+1 ) 
		return 1;
	return 0;
}

BOOL CIssFreeCell::OnFreeCell(int iFreeCell)
{
	if(m_bGameInPlay == FALSE)
		return FALSE;
	if(iFreeCell < 0 || iFreeCell > 3)
		return FALSE;

	if(m_btSelected == NULL_CARD)
	{
		//check and see if there's a card to select
		byte btCardNum = GetCardNum(m_btFreeCells[iFreeCell]);
		if(btCardNum > 0)
			m_btSelected = (int)CL_FreeCell1+iFreeCell;
		return FALSE;
	}
	//can't move onto a card
	if(m_btFreeCells[iFreeCell] != CARD_BLANK)
	{	
		//so here we can check if we're selecting the card that's already selected an unselect rather than post an error
		if(m_btSelected - (int)CL_FreeCell1 == iFreeCell)
		{
			m_btSelected = -1;
			return FALSE;
		}
		else
		{
			MoveError();	
			return FALSE;
		}
	}
	//finally ... lets move onto the FreeCell
	SaveUndoInfo(); // only if we've made it this far
	if(m_btSelected < CARD_COLUMNS)
	{
		if(!MoveCardFromColumntoFreeCell(m_btSelected, (byte)iFreeCell, TRUE))
		{
			MoveError();
			return FALSE;
		}
		
	}
	//if we're here the only other possible move to a freecell is from a freecell
	else if(!MoveCardFromFreeCelltoFreeCell(m_btSelected - CL_FreeCell1, (byte)iFreeCell, TRUE))
	{
		MoveError();
		return FALSE;
	}

	//we made it through so we must be good
//	m_sUndo.SetUndo();//valid move .. allow the undo
	return TRUE;
}

BOOL CIssFreeCell::MoveCardFromFreeCelltoFreeCell(byte btFrom, byte btTo, BOOL bMakeMove /* = FALSE */)
{
	if(btFrom < 0 || btFrom >= 4)
		return FALSE;
	if(btTo < 0 || btTo >= GetNumFC())
		return FALSE;
	if(m_btFreeCells[btTo] != CARD_BLANK)
		return FALSE;
	if(bMakeMove==FALSE)
		return TRUE;
	//move the card
	m_btFreeCells[btTo]		= m_btFreeCells[btFrom];
	m_btFreeCells[btFrom]	= CARD_BLANK;
	ValidMove(btFrom+CL_Column8, btTo+CL_Column8);//all good
	return TRUE;
}

BOOL CIssFreeCell::MoveCardFromColumntoFreeCell(byte btColumn, byte btFreeCell, BOOL bMakeMove /* = FALSE */)
{
	if(btColumn < 0 || btColumn >= CARD_COLUMNS)
		return FALSE;
	if(btFreeCell < 0 || btFreeCell >= GetNumFC()) //easy enough
		return FALSE;
	if(m_btFreeCells[btFreeCell] != CARD_BLANK)
		return FALSE;
	if(bMakeMove==FALSE)
		return TRUE;
	//move the card
	m_btFreeCells[btFreeCell] = m_btColumns[btColumn][GetNumCardsInColumn(btColumn)-1];
	m_btColumns[btColumn][GetNumCardsInColumn(btColumn)-1] = NULL_CARD; //blank it out
	ValidMove(btColumn, btFreeCell+CL_Column8);
	return TRUE;
}


BOOL CIssFreeCell::OnAceRack(int iAce)
{
	if(m_bGameInPlay == FALSE)
		return FALSE;
	if(m_btSelected == NULL_CARD)
		return FALSE;
	if(iAce < 0 || iAce > 3)
		return FALSE;
	
	if(m_btSelected  <= CL_Column8)
	{
		SaveUndoInfo();
		if(!MoveCardFromColumntoAceRack(m_btSelected, (byte)iAce, TRUE))
		{
			MoveError();	
		}
		else
		{
		//	m_sUndo.SetUndo();
			return TRUE;
		}
	}
	else if(m_btSelected <= CL_FreeCell4)
	{
		SaveUndoInfo();
		if(!MoveCardFromFreeCelltoAceRack(m_btSelected - CL_FreeCell1, (byte)iAce, TRUE))
		{
			MoveError();
		}
		else
		{
		//	m_sUndo.SetUndo();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CIssFreeCell::MoveCardFromColumntoAceRack(byte btFrom, byte btAce, BOOL bMakeMove /* = FALSE */, BOOL bIsAutoMove/* = FALSE*/)
{
	if(btFrom < 0 || btAce < 0)
		return FALSE;
	if(btFrom >= CARD_COLUMNS || btAce >= 4)
		return FALSE;

	byte btFromIndex	= GetNumCardsInColumn(btFrom)-1;

	byte btCardFrom		= m_btColumns[btFrom][btFromIndex];
	byte btCardTo		= m_btAceRacks[btAce];

	byte btValueFrom = GetCardNum(btCardFrom);

	BOOL bMoveCard = FALSE;
	if(m_btAceRacks[btAce] >= CARD_BLANK && 
		btValueFrom == 1) 
	{
		bMoveCard = TRUE;
	}
	else if(btCardFrom == btCardTo + 1 &&
			btCardFrom/13 == btCardTo/13)//same suit
	{
		bMoveCard = TRUE;
	}
	else if(btCardFrom == btCardTo + 1 &&
		btCardFrom%13 == 0)//its the king
	{
		bMoveCard = TRUE;
	}
	
	if(bMoveCard)
	{
		if(bMakeMove)//so we can use the function for valid move testing
		{
			m_btAceRacks[btAce] = m_btColumns[btFrom][GetNumCardsInColumn(btFrom)-1];
			m_btColumns[btFrom][GetNumCardsInColumn(btFrom)-1] = NULL_CARD;
			m_btCardsRemaining--;
			if(bIsAutoMove)
				SendMessage(m_hWnd, WM_ANI_COLUMN_ACE, (WPARAM)btFrom, (LPARAM)btAce);
			ValidMove(btFrom, btAce + CL_FreeCell4);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CIssFreeCell::MoveCardFromFreeCelltoColumn(byte btFreeCell, byte btTo, BOOL bMakeMove /* = FALSE */)
{
	if(btFreeCell < 0 || btTo < 0)
		return FALSE;
	if(btFreeCell >= 4 || btTo >= GetNumColumns())
		return FALSE;

	if(GetNumCardsInColumn(btTo) > 0)
	{
		byte btToIndex	= GetNumCardsInColumn(btTo)-1;
		
		byte btCardFrom	= m_btFreeCells[btFreeCell];
		byte btCardTo	= m_btColumns[btTo][btToIndex];

		byte btValueFrom= GetCardNum(btCardFrom);
		byte btValueTo	= GetCardNum(btCardTo);

		if(btValueTo <= btValueFrom)
			return FALSE;

		if(btValueTo != btValueFrom +1)
			return FALSE;
        if(CanBuild(btCardFrom, btCardTo) == FALSE)
            return FALSE;


		//and move
		if(bMakeMove)
		{
			m_btColumns[btTo][btToIndex+1] = m_btFreeCells[btFreeCell];
			m_btFreeCells[btFreeCell] = CARD_BLANK;
			ValidMove(btFreeCell + CL_Column8, btTo);
		}
	}
	else if(bMakeMove)
	{//empty column
		m_btColumns[btTo][0] = m_btFreeCells[btFreeCell];
		m_btFreeCells[btFreeCell] = CARD_BLANK;
		ValidMove(btFreeCell+CL_Column8, btTo);
	}
	return TRUE;
}

BOOL CIssFreeCell::MoveCardFromFreeCelltoAceRack(byte btFreeCell, byte btAce, BOOL bMakeMove /* = FALSE */, BOOL bIsAutoMove /* = FALSE */)
{
	if(btFreeCell < 0 || btAce < 0)
		return FALSE;
	if(btFreeCell >= 4 || btAce >= 4)
		return FALSE;

	byte btCardFrom	= m_btFreeCells[btFreeCell];
	byte btCardTo	= m_btAceRacks[btAce];

	byte btValueFrom = GetCardNum(btCardFrom);

	BOOL bMoveCard = FALSE;

	if(m_btAceRacks[btAce] >= CARD_BLANK && 
	btValueFrom == 1) 
	{
		bMoveCard = TRUE;
	}
	else if(btCardFrom == btCardTo + 1 &&
		btCardFrom/13 == btCardTo/13)//same suit
	{
		bMoveCard = TRUE;
	}
	else if(btCardFrom == btCardTo + 1 &&
		btCardFrom%13 == 0)//its the king
	{
		bMoveCard = TRUE;
	}

	if(bMoveCard)
	{
		if(bMakeMove)
		{
			m_btAceRacks[btAce] = m_btFreeCells[btFreeCell];
			m_btFreeCells[btFreeCell] = CARD_BLANK;
			m_btCardsRemaining--;
			if(bIsAutoMove)
				SendMessage(m_hWnd, WM_ANI_FREECELL_ACE, (WPARAM)btFreeCell, (LPARAM)btAce);
			ValidMove(btFreeCell+CL_Column8, btAce+CL_FreeCell4);
		}
		return TRUE;
	}
	return FALSE;	
}

BOOL CIssFreeCell::OnTable()
{
	m_btSelected = -1;
	return TRUE;
}

BOOL CIssFreeCell::OnColumn(int iColumn)
{
	if(m_bGameInPlay == FALSE)
		return FALSE; //since I think this is going bad somewhere
	BOOL bRet = FALSE;
	//check the range
	if(iColumn < 0 || iColumn >= CARD_COLUMNS)
		return bRet;

	if(m_btSelected == NULL_CARD) //we just need to select something and move on
	{	//make sure there's something in the column
		if(GetNumCardsInColumn(iColumn) == 0)
			return bRet; //no cards in the column so don't do anything
		m_btSelected = iColumn;
		m_dwTickCount = GetTickCount();
		return bRet;//select and return
	}
	if(m_btSelected == iColumn)//double tap
	{
		SaveUndoInfo();
		DWORD dwTick = GetTickCount();
		if(dwTick - m_dwTickCount > DOUBLECLICK_TIMEOUT)
		{
			m_btSelected = -1;		
			return bRet;//too slow
		}
		//check if we can put the card on the ace rack
		BOOL bValidMove = FALSE;
		int i;

		if(bValidMove == FALSE)
		{
			for(i = 0; i < 4; i++)
			{
				if(MoveCardFromColumntoFreeCell(m_btSelected, i, TRUE))
				{
					bValidMove = TRUE;
					break;
				}
			}
		}
		
		if(bValidMove == FALSE)
		{
			MoveError();//can't put it on the ACE rack
		}
		else
		{
			bRet = TRUE;
		}
	}

	//otherwise we have to make a move from column to column
	else if(m_btSelected < CARD_COLUMNS)
	{
		SaveUndoInfo();
		if(!MoveCardFromColumntoColumn(m_btSelected, iColumn, TRUE))
		{//check if we can move more than one card
			if(!MoveStackFromColumntoColumn(m_btSelected, iColumn, TRUE))
				MoveError(); //ok NOW we've failed
			else
				bRet = TRUE;
		}
		else
			bRet = TRUE;
	}
	else if(m_btSelected <= (int)CL_FreeCell4)
	{
		SaveUndoInfo();
		if(!MoveCardFromFreeCelltoColumn(m_btSelected-(int)CL_FreeCell1, iColumn, TRUE))
			MoveError();
		else
			bRet = TRUE;
	}
//	if(bRet)
//		m_sUndo.SetUndo();//allow the undo since it was a valid move
	return bRet;
}

BOOL CIssFreeCell::MoveCardFromColumntoColumn(byte btFrom, byte btTo, BOOL bMakeMove /* = FALSE */, BOOL bMulti)
{
	if(btFrom >= GetNumColumns() || btTo >= GetNumColumns())
		return FALSE;

	byte btToIndex	= GetNumCardsInColumn(btTo)-1;
	byte btFromIndex= GetNumCardsInColumn(btFrom)-1;

	byte btCardFrom	= m_btColumns[btFrom][btFromIndex];
	byte btCardTo	= m_btColumns[btTo][btToIndex];

	byte btValueFrom= GetCardNum(btCardFrom);
	byte btValueTo	= GetCardNum(btCardTo);

	if(btValueFrom == NULL_CARD)
		return FALSE;

    //check for strict requirements
    if(m_bStrict == TRUE &&
        btValueTo == NULL_CARD &&
        btValueFrom != 13)
        return FALSE;

	if(GetNumCardsInColumn(btTo) > 0)
	{
		if(btValueTo <= btValueFrom && btCardTo)
			return FALSE;

		if(btValueTo != btValueFrom +1)
			return FALSE;
        if(CanBuild(btCardFrom, btCardTo) == FALSE)
            return FALSE;
		
		if(bMakeMove)
		{
			//and move
			m_btColumns[btTo][btToIndex+1] = m_btColumns[btFrom][btFromIndex];
			m_btColumns[btFrom][btFromIndex] = NULL_CARD;
			ValidMove(btFrom, btTo);
		}
	}
	else//moving to a blank spot
	{
		if(bMakeMove == FALSE)
			return TRUE;//can always move something

		//we have to see how many cards are continuous ... and then go from there
		int iContinuousCards = 1; //always one
		//changed from i > 1 to i > 0 in v 0.8.0 so we can move stacks from blank to blank spots
		//aces are 1, 14, 27, 40
        if(bMulti)
        {
		    for(int i=btFromIndex; i > 0 ; i--)//start from the top and work down
		    {	
			    if(m_btColumns[btFrom][i]%13 != (m_btColumns[btFrom][i-1]-1)%13 || //if they're continuous
				    //m_btColumns[btFrom][i]%13 != m_btColumns[btFrom][i-1]%13 + 12)//in case we hit a king
                    m_btColumns[btFrom][i-1]%13 == 1)//in case we hit an ace
				    break;
                if(CanBuild(m_btColumns[btFrom][i], m_btColumns[btFrom][i-1]) == FALSE)
                    break;
			    iContinuousCards++;
		    }
        }

        //we'll pass in the stack size in the bool :)
        if(bMulti > 1 && iContinuousCards > bMulti)
            iContinuousCards = bMulti;

		//now .. if iCont is bigger than 1 and we can move more than 1 card ... gotta ask
		//(number of empty freecells + 1) * 2 ^ (num empty columns) .. minus 1 cause we're using it
		int iNumMoves = 1;
		int iEmptyColumns = 0;
		int iEmptyFreeCells = GetNumEmptyFreeCells();
		for(int i = 0; i < CARD_COLUMNS; i++)
		{
			if(GetNumCardsInColumn(i) == 0)
				iEmptyColumns++;
		} 

        if(m_bStrict)
		    iNumMoves = (iEmptyFreeCells + 1);
        else
            iNumMoves = (iEmptyFreeCells + 1)*(int)pow(2, iEmptyColumns - 1);

		//move the stack
		if(iNumMoves > 1 && iContinuousCards > 1)
		{
			if(iNumMoves > iContinuousCards)
				iNumMoves = iContinuousCards;
			//move iNumMoves cards to the stack
			for(int j = 0; j < iNumMoves; j++)
			{	//no really ... below is correct
				m_btColumns[btTo][j] = m_btColumns[btFrom][btFromIndex+j-iNumMoves+1];
				m_btColumns[btFrom][btFromIndex+j-iNumMoves+1] = NULL_CARD;
			}
		}
		else	//move one card
		{
			m_btColumns[btTo][0] = m_btColumns[btFrom][btFromIndex];
			m_btColumns[btFrom][btFromIndex] = NULL_CARD;
		}
		ValidMove(btFrom, btTo);
		return TRUE;//we're done with moving onto a blank
	}

	return TRUE;
}

BOOL CIssFreeCell::MoveStackFromColumntoColumn(byte btFrom, byte btTo, BOOL bMakeMove /* = FALSE */)
{	//the usual checks
	if(btFrom >= GetNumColumns() || btTo >= GetNumColumns())
		return FALSE;
	
	if(GetNumCardsInColumn(btTo) > 0)
	{
		byte btToIndex		= GetNumCardsInColumn(btTo)-1;
		byte btFromIndex	= GetNumCardsInColumn(btFrom)-1;

		byte btCardFrom		= m_btColumns[btFrom][btFromIndex];
		byte btCardTo		= m_btColumns[btTo][btToIndex];

		byte btValueFrom	= GetCardNum(btCardFrom);
		byte btValueTo		= GetCardNum(btCardTo);

		if(btValueFrom == NULL_CARD)
			return FALSE;//just makes things simple

        //check for strict requirements
        if(m_bStrict == TRUE &&
            btValueTo == NULL_CARD &&
            btValueFrom != 13)
            return FALSE;//might be wrong ..
	
		if(btValueTo != NULL_CARD)
		{
			//ok..now we compare the values to see how deep we should go
			int iDiff = btValueTo - btValueFrom;
			if(iDiff <= 0)
				return FALSE;//wrong direction

			//is the to card the correct color or suit??
            if(CanBuild(m_btColumns[btFrom][btFromIndex-iDiff+1], m_btColumns[btTo][btToIndex]) == FALSE)
                return FALSE;

			int i;

			//now we use this number and check if the cards are continuous
			for(i = 0; i < iDiff-1; i++)
			{
                if(CanBuild(m_btColumns[btFrom][btFromIndex-i], m_btColumns[btFrom][btFromIndex-i-1]) == FALSE)
                    return FALSE;
  				if(GetCardNum(m_btColumns[btFrom][btFromIndex-i]) != GetCardNum(m_btColumns[btFrom][btFromIndex-i-1])-1)
					return FALSE;
                if(GetCardNum(m_btColumns[btFrom][btFromIndex-i]) == 1)
                    return FALSE;//we've hit an ace
			}

			//ok we've made it this far so the cards are in the correct order to move 
			//so now ... can we make the move???


			//we need to consider empty columns
			int iEmptyColumns = 0;
			int iEmptyFreeCells = GetNumEmptyFreeCells();
			for(int i = 0; i < CARD_COLUMNS; i++)
			{
				if(GetNumCardsInColumn(i) == 0)
					iEmptyColumns++;
			}

			//yeah formula - http://searchwarp.com/swa39437.htm
			//The formula for how many cards you can move is:
			//(number of empty freecells + 1) * 2 ^ (num empty columns) 
            int iNumMoves = 0;

            if(m_bStrict)
                iNumMoves = (iEmptyFreeCells + 1);
            else
                iNumMoves = (iEmptyFreeCells + 1)*(int)pow(2, iEmptyColumns);

			//if(iDiff > (iEmptyFreeCells + 1)*pow(2, iEmptyColumns))
            if(iDiff > iNumMoves)
				return FALSE;

			if(bMakeMove)
			{
				EnumAutoPlay eTemp = m_eAutoPlay;
				m_eAutoPlay = AUTOPLAY_Off;
				//ok ... now we know we can move the column
				for(i=0; i < iDiff; i++)
				{
					m_btColumns[btTo][btToIndex+iDiff-i] = m_btColumns[btFrom][btFromIndex-i];
					m_btColumns[btFrom][btFromIndex-i] = NULL_CARD;
					ValidMove(btFrom, btTo);
				}
				m_eAutoPlay = eTemp;//this is so it doesn't try to add cards to the ace rack mid-move
				CheckAceRack();//since we're not doing it in ValidMove
			}
		}
	
	}
	else
	{	//shouldn't ever get here I don't think...check it later yo
		return FALSE; //figure this out later....
	}
	return TRUE;
}

int CIssFreeCell::GetNumEmptyFreeCells()
{
	int iCount = 0;
	for(int i = 0; i < GetNumFC(); i++)
	{
		if(m_btFreeCells[i] == CARD_BLANK)
			iCount++;
	}

	return iCount;
}

void CIssFreeCell::CalcCardsRemaining()
{
	m_btCardsRemaining = 0;
	for(int i = 0; i < CARD_COLUMNS; i++)
		m_btCardsRemaining += GetNumCardsInColumn(i);
	for(int i = 0; i < 4; i++)
		if(m_btFreeCells[i] != CARD_BLANK)
			m_btCardsRemaining++;
}

void CIssFreeCell::ValidMove(byte btFrom, byte btTo)
{
	m_eLastTo	= (EnumCardLocations)btTo;
	m_eLastFrom	= (EnumCardLocations)btFrom;
	m_btSelected = NULL_CARD;
	CheckAceRack();//update the ACE rack if necessary
	CalcCardsRemaining();
	CheckGameState();//and finally ... see if we can still play
//	m_sUndo.bCanUndo = TRUE;
}

void CIssFreeCell::MoveError()
{
	DeleteLastUndo();
	m_btSelected = NULL_CARD;//clear out the selected card on an error
	PostMessage(m_hWnd, WM_INVALID_MOVE, NULL, NULL); //may use parameters later
}

int	CIssFreeCell::GetNumCardsInColumn(byte btColumn)
{
	int iCount = 0;

	for(int i = 0; i < MAX_CARDS_PER_COLUMN; i++)
	{
		if(m_btColumns[btColumn][i] == NULL_CARD)
			break;
		iCount++;
	}

	return iCount;	
}

BOOL CIssFreeCell::CheckAceRack()
{
	//better autoplay logic at end of section 2
	//http://www.solitairelaboratory.com/fcfaq.html
	//if a card is within two of the opposite color and within 3 of the same color move it up
	BOOL bAllAcesUp		= TRUE;
	BOOL bRet			= FALSE;
	if(m_eAutoPlay == AUTOPLAY_Off)
		return bRet;

	int iCard;
	int iCardValue;
	int iCardsInColumn;
	int iLowestAceRackValue		= 13;//start high and bring er down
	int iLowestBlackAceRack		= 13;
	int iLowestRedAceRack		= 13;
	int i;
	int iTemp;
	BOOL bIsBlack;

	for(i = 0; i < 4; i++)
	{
		iTemp = GetCardNum(m_btAceRacks[i]);
		if(iTemp == 0)
			bAllAcesUp = FALSE; //so we can't use the advanced logic yet
		if(iTemp < iLowestAceRackValue)
			iLowestAceRackValue = iTemp;
		bIsBlack = IsCardBlack(m_btAceRacks[i]);
		if(bIsBlack)
		{
			if(iTemp < iLowestBlackAceRack)
				iLowestBlackAceRack = iTemp;
		}
		else
		{
			if(iTemp < iLowestRedAceRack)
				iLowestRedAceRack = iTemp;
		}
	}

	BOOL bAttemptMove = FALSE;

	//check the columns
	for(i = 0; i < CARD_COLUMNS; i++)//check all columns
	{
		bAttemptMove = FALSE;
		iCardsInColumn = GetNumCardsInColumn(i);
		iCard = m_btColumns[i][iCardsInColumn-1];
		iCardValue = GetCardNum(iCard);
		bIsBlack = IsCardBlack(iCard);
		if(iCardValue > iLowestAceRackValue && iCardValue < iLowestAceRackValue + 3)//if its two away try and move it
			bAttemptMove = TRUE;
		else if(bIsBlack == TRUE && 
			m_eAutoPlay == AUTOPLAY_On &&
			bAllAcesUp &&
			iCardValue > iLowestBlackAceRack && 
			iCardValue < iLowestBlackAceRack + 4 && //only move up more cards if we have all aces in place
			iCardValue > iLowestRedAceRack && 
			iCardValue < iLowestRedAceRack + 3)//if its two away try and move it
			bAttemptMove = TRUE;
		else if(bIsBlack == FALSE && 
			m_eAutoPlay == AUTOPLAY_On &&
			bAllAcesUp &&
			iCardValue > iLowestBlackAceRack && 
			iCardValue < iLowestBlackAceRack + 3 &&
			iCardValue > iLowestRedAceRack && 
			iCardValue < iLowestRedAceRack + 4)//if its 2 or 3 away try and move it
			bAttemptMove = TRUE;
		
		if(bAttemptMove)
		{
            //try to hit the specific location ... in order of the deck
            byte btCard = m_btColumns[i][GetNumCardsInColumn(i)-1];
            byte btValue = GetCardNum(btCard);
            byte btSuit = GetCardSuit(btCard);

            if(btValue == 1)//ace
            {
                if(MoveCardFromColumntoAceRack(i,btSuit,TRUE, TRUE))
                {
                    bRet = TRUE;//we moved at least one card...this may not matter
                    break;
                }
            }

            if(bRet == FALSE)
			for(int j = 0; j < 4; j++)
			{
				if(MoveCardFromColumntoAceRack(i,j,TRUE, TRUE))
				{
					bRet = TRUE;//we moved at least one card...this may not matter
					break;
				}
			}
		}
	}
	//and check free cells
	for(i = 0; i < 4; i++)//check all columns
	{
		iCard = m_btFreeCells[i];
		if(iCard == NULL_CARD)
			continue;

		bAttemptMove = FALSE;
		iCardValue = GetCardNum(iCard);
		bIsBlack = IsCardBlack(iCard);
		if(iCardValue > iLowestAceRackValue && iCardValue < iLowestAceRackValue + 3)//if its an ACE or a TWO try to move it
			bAttemptMove = TRUE;
		else if(bIsBlack == TRUE && 
			iCardValue > iLowestBlackAceRack && 
			iCardValue < iLowestBlackAceRack + 4 &&
			iCardValue > iLowestRedAceRack && 
			iCardValue < iLowestRedAceRack + 3)//if its two away try and move it
			bAttemptMove = TRUE;
		else if(bIsBlack == FALSE && 
			iCardValue > iLowestBlackAceRack && 
			iCardValue < iLowestBlackAceRack + 3 &&
			iCardValue > iLowestRedAceRack && 
			iCardValue < iLowestRedAceRack + 4)//if its two away try and move it
			bAttemptMove = TRUE;
		
		if(bAttemptMove)
		{
			for(int j = 0; j < 4; j++)
			{
				if(MoveCardFromFreeCelltoAceRack(i,j,TRUE, TRUE))
				{
					bRet = TRUE;//we moved at least one card...this may not matter
					break;
				}
			}
		}
	}	

	return bRet;
}


/*
Check for:
Cards Remaining*
Avaliable FreeCells
Valid Moves from FreeCells to Columns*
Valid Moves from FreeCells to AceRack
Valid Moves from Columns to AceRack*
Valid Moves from Column to Column

*/
EnumGameState CIssFreeCell::CheckForValidMoves()
{
	BOOL bRet = FALSE;

	if(m_btCardsRemaining == 0)
	{	//we win!!
		return GSTATE_Win;
	}	

	if(GetNumEmptyFreeCells() > 0)
		return GSTATE_InPlay;

	int i,j;

	//the columns
	for(i = 0; i < CARD_COLUMNS; i++)
	{
		for(j = 0; j < CARD_COLUMNS; j++)
		{
			if(i==j)
				continue;
			if(MoveCardFromColumntoColumn(i,j,FALSE))
				return GSTATE_InPlay;
			if(MoveStackFromColumntoColumn(i,j,FALSE))
				return GSTATE_InPlay;
		}
	}

	//free cells and ace rack moves
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < CARD_COLUMNS; j++)
		{
			if(MoveCardFromFreeCelltoColumn(i, j, FALSE))
				return GSTATE_InPlay;
			if(MoveCardFromColumntoAceRack(j, i, FALSE))
				return GSTATE_InPlay;
		}

		for(j = 0; j < 4; j++)
		{
			if(MoveCardFromFreeCelltoAceRack(i,j,FALSE))
				return GSTATE_InPlay;
		}
	}
	return GSTATE_Lose; //no valid moves
}

//Call this after every valid move
void CIssFreeCell::CheckGameState()
{
	//now check for valid moves and end game conditions
	EnumGameState eState = CheckForValidMoves();

	switch(eState)
	{
	case GSTATE_InPlay: //pump the dialog to update/animate
						//doesn't matter if there's nothing to do...it'll just redraw
		PostMessage(m_hWnd, WM_PROCESS_MOVE, NULL, NULL);
	default:
		break;
	case GSTATE_Lose://as you'd suspect
		if(m_bGameInPlay)
		{	//don't forget to update the stats
			m_iGamesLost++;
			m_iCurrentStreak = 0;
			m_bGameInPlay = FALSE;
			DeleteUndoArray();
			PostMessage(m_hWnd, WM_GAME_LOSE, NULL, NULL);
		}
		break;
	case GSTATE_Win:
		if(m_bGameInPlay)
		{
			m_bGameInPlay = FALSE;
			m_iGamesWon++;//hehe ... should be done BEFORE the  message
			m_iCurrentStreak++;
			if(m_iCurrentStreak > m_iLongestStreak)
				m_iLongestStreak = m_iCurrentStreak;
			DeleteUndoArray();
			SendMessage(m_hWnd, WM_GAME_WIN, NULL, NULL);//its a send not a post so we can stop the animation .. yeah I don't know either
		}
		break;
	}

}

BOOL CIssFreeCell::CanUndo()
{
	if(m_arrUndo.GetSize() > 0)
		return TRUE;
	return FALSE;
}

BOOL CIssFreeCell::OnUndo()
{
	if(CanUndo() == FALSE)
		return FALSE;
//	m_sUndo.bCanUndo = FALSE;

	TypeFreeCellUndo* sUndo = m_arrUndo[m_arrUndo.GetSize() - 1];

    //quick brute force check
    //I don't feel like figuring out where the bad values are coming from
    BOOL bRet = FALSE;
    for(int i = 0; i < 4; i++)
    {
        if(sUndo->btFreeCells[i] > 0 && sUndo->btFreeCells[i] < CARD_BLANK)
            bRet = TRUE;
        if(sUndo->btColumns[i][0] > 0 && sUndo->btColumns[i][0] < CARD_BLANK)
            bRet = TRUE;
        if(sUndo->btColumns[i+4][0] > 0 && sUndo->btColumns[i+4][0] < CARD_BLANK)
            bRet = TRUE;
    }

    if(bRet == FALSE)
	{
		DeleteLastUndo();
        return FALSE;
	}

	memcpy(m_btColumns, sUndo->btColumns, sizeof(byte)*CARD_COLUMNS*MAX_CARDS_PER_COLUMN);
	memcpy(m_btFreeCells, sUndo->btFreeCells, sizeof(byte)*4);
	memcpy(m_btAceRacks, sUndo->btAceRacks, sizeof(byte)*4);

	//recount and such?
	CalcCardsRemaining();

	//and deselect 
	m_btSelected = NULL_CARD;

	DeleteLastUndo();

	return TRUE;
}

void CIssFreeCell::LoadRegistry()
{
	DWORD dwValue;

	if(S_OK == GetKey(REG_KEY,_T("ShowAnimations"),dwValue))
		m_bShowAnimations	= (BOOL)dwValue;
	if(S_OK == GetKey(REG_KEY,_T("GameInPlay"),dwValue))
		m_bGameInPlay	= (BOOL)dwValue;
	if(S_OK == GetKey(REG_KEY,_T("AutoPlay"),dwValue))
		m_eAutoPlay		= (EnumAutoPlay)dwValue;
	if(S_OK == GetKey(REG_KEY,_T("Sounds"),dwValue))
		m_bPlaySounds	= (BOOL)dwValue;
	if(S_OK == GetKey(REG_KEY,_T("Skin"),dwValue))
		m_eTableColor	= (EnumBackgroundColor)dwValue;

    if(S_OK == GetKey(REG_KEY,_T("Inverse"),dwValue))
        m_bInverse	= (BOOL)dwValue;
    if(S_OK == GetKey(REG_KEY,_T("Strict"),dwValue))
        m_bStrict	= (BOOL)dwValue;
    if(S_OK == GetKey(REG_KEY,_T("NumCells"),dwValue))
        m_eFCCount	= (EnumFreeCellCount)dwValue;
    if(S_OK == GetKey(REG_KEY,_T("GameType"),dwValue))
        m_eGameType	= (EnumFreecellType)dwValue;
    if(S_OK == GetKey(REG_KEY,_T("Timer"),dwValue))
        m_bShowTimer	= (BOOL)dwValue;

	if(S_OK == GetKey(REG_KEY,_T("GW"),dwValue))
		m_iGamesWon	= (int)dwValue;
	if(S_OK == GetKey(REG_KEY,_T("GL"),dwValue))
		m_iGamesLost	= (int)dwValue;
	if(S_OK == GetKey(REG_KEY,_T("CS"),dwValue))
		m_iCurrentStreak	= (int)dwValue;
	if(S_OK == GetKey(REG_KEY,_T("LS"),dwValue))
		m_iLongestStreak	= (int)dwValue;

	// if there was a game then retrieve the game state
	if(m_bGameInPlay)
	{
		TypeGameState sGs;

		// get our single Game State values
		dwValue = sizeof(TypeGameState);
		if(S_OK == GetKey(REG_KEY, _T("GameState"), (LPBYTE)&sGs, dwValue))
		{
			// set our member variables
			m_btCardsRemaining	= sGs.btCardsRemaining;
			m_btSelected		= sGs.btSelected;
			m_iGameNumber		= sGs.iGameNumber;
			memcpy(m_btAceRacks, sGs.btAceRacks, sizeof(byte)*4);
			memcpy(m_btFreeCells, sGs.btFreeCells, sizeof(byte)*4);
			memcpy(m_btColumns, sGs.btColumns, sizeof(byte)*MAX_CARDS_PER_COLUMN*8);
		}
	}
}

void CIssFreeCell::SaveRegistry()
{
	DWORD dwValue;

	dwValue = (DWORD)m_bShowAnimations;
	SetKey(REG_KEY, _T("ShowAnimations"), dwValue);
	dwValue = (DWORD)m_bGameInPlay;
	SetKey(REG_KEY, _T("GameInPlay"), dwValue);
	dwValue = (DWORD)m_eAutoPlay;
	SetKey(REG_KEY, _T("AutoPlay"), dwValue);
	dwValue = (DWORD)m_bPlaySounds;
	SetKey(REG_KEY, _T("Sounds"), dwValue);
	dwValue = (DWORD)m_eTableColor;
	SetKey(REG_KEY, _T("Skin"), dwValue);

    dwValue = (DWORD)m_bInverse;
    SetKey(REG_KEY, _T("Inverse"), dwValue);
    dwValue = (DWORD)m_bStrict;
    SetKey(REG_KEY, _T("Strict"), dwValue);
    dwValue = (EnumFreeCellCount)m_eFCCount;
    SetKey(REG_KEY, _T("NumCells"), dwValue);
    dwValue = (EnumFreecellType)m_eGameType;
    SetKey(REG_KEY, _T("GameType"), dwValue);
    dwValue = (DWORD)m_bShowTimer;
    SetKey(REG_KEY, _T("Timer"), dwValue);
    
	dwValue = (DWORD)m_iGamesWon;
	SetKey(REG_KEY, _T("GW"), dwValue);
	dwValue = (DWORD)m_iGamesLost;
	SetKey(REG_KEY, _T("GL"), dwValue);
	dwValue = (DWORD)m_iCurrentStreak;
	SetKey(REG_KEY, _T("CS"), dwValue);
	dwValue = (DWORD)m_iLongestStreak;
	SetKey(REG_KEY, _T("LS"), dwValue);

	
	if(m_bGameInPlay)
	{
		TypeGameState sGs;
		memcpy(sGs.btAceRacks, m_btAceRacks, sizeof(byte)*4);
		memcpy(sGs.btFreeCells, m_btFreeCells, sizeof(byte)*4);
		memcpy(sGs.btColumns, m_btColumns, sizeof(byte)*MAX_CARDS_PER_COLUMN*8);
		sGs.btCardsRemaining	= m_btCardsRemaining;
		sGs.btSelected			= m_btSelected;
		sGs.iGameNumber			= m_iGameNumber;

		dwValue = sizeof(TypeGameState);
		SetKey(REG_KEY, _T("GameState"), (LPBYTE)&sGs, dwValue);	
	}
}

void CIssFreeCell::SaveUndoInfo()
{
	TypeFreeCellUndo* sUndo = new TypeFreeCellUndo;

	memcpy(sUndo->btColumns, m_btColumns, sizeof(byte)*CARD_COLUMNS*MAX_CARDS_PER_COLUMN);
	memcpy(sUndo->btFreeCells, m_btFreeCells, sizeof(byte)*4);
	memcpy(sUndo->btAceRacks, m_btAceRacks, sizeof(byte)*4);

	m_arrUndo.AddElement(sUndo);

//	m_sUndo.ClearUndo();//can only undo AFTER we've completed a move
}

void CIssFreeCell::AutoPlay()
{
	if(m_eAutoPlay != AUTOPLAY_Off)
		return;
	m_eAutoPlay = AUTOPLAY_Safe;
	CheckAceRack();
	m_eAutoPlay = AUTOPLAY_Off;



}

BOOL CIssFreeCell::IsCardLocked(byte btColumn, byte btIndex)
{
    //top card ... never locked
    if(btIndex == GetNumCardsInColumn(btColumn) - 1)
        return FALSE;

    //out of range 
    if(btIndex >= GetNumCardsInColumn(btColumn) || GetNumCardsInColumn(btColumn) == 0)
        return TRUE;

    //now we gotta do some work
    for(int i = btIndex; i < GetNumCardsInColumn(btColumn) - 1; i++)
    {
        if(GetCardNum(m_btColumns[btColumn][i]) != (GetCardNum(m_btColumns[btColumn][i+1]) + 1))
            return TRUE;
    }
    return FALSE;
}

BOOL CIssFreeCell::MoveCard(byte btFrom, byte btTo, byte btDepth)
{
    //range and can't move from the ace rack
    if(btFrom < 0 || btFrom >= CL_AceRack1 ||
        btTo < 0 || btTo >= CL_None)
        return FALSE;

    //no stacks to strange places ....
    if((btDepth > 0 && btFrom > CL_Column8) ||
        (btDepth > 0 && btTo > CL_Column8))
        return FALSE;

    SaveUndoInfo();

    if(btFrom < CL_FreeCell1)//columns
    {   //from column
        if(btTo < CL_FreeCell1)//columns
        {
            if(btDepth > 0)
            {
                //are we moving to an empty spot?
                if(GetNumCardsInColumn(btTo) == 0)
                    return MoveCardFromColumntoColumn(btFrom, btTo, TRUE, btDepth+1);

                //ok ... here we need to check if our card is place able on the location                
                //this is ugly but correct
                if(GetCardNum(m_btColumns[btTo][GetNumCardsInColumn(btTo)-1]) !=
                    GetCardNum(m_btColumns[btFrom][GetNumCardsInColumn(btFrom)-1-btDepth])+1)
                    return FALSE;

                return MoveStackFromColumntoColumn(btFrom, btTo, TRUE);
            }
            else
            {   //single card
                return MoveCardFromColumntoColumn(btFrom, btTo, TRUE, FALSE);
            }
        }
        if(btTo < CL_AceRack1)
            return MoveCardFromColumntoFreeCell(btFrom, btTo - CL_FreeCell1, TRUE);
        return MoveCardFromColumntoAceRack(btFrom, btTo - CL_AceRack1, TRUE);
    }

    //otherwise we're moving from the freecell
    if(btTo < CL_FreeCell1)
        return MoveCardFromFreeCelltoColumn(btFrom - CL_FreeCell1, btTo, TRUE);
    if(btTo < CL_AceRack1)
        return MoveCardFromFreeCelltoFreeCell(btFrom - CL_FreeCell1, btTo - CL_FreeCell1, TRUE);
    return MoveCardFromFreeCelltoAceRack(btFrom - CL_FreeCell1, btTo - CL_AceRack1, TRUE);

    //won't get here ....
    return FALSE;
}

void CIssFreeCell::SetGameType(EnumFreecellType eType)
{
    if(eType == m_eGameType)
        return;

    m_eGameType = eType;
    if(IsGameInPlay())
    {
        NewGame(FALSE); //I'm too nice ...
    }
}

int CIssFreeCell::GetNumFC()
{
    switch(m_eFCCount)
    {
    case FCCOUNT_1:
        return 1;
    	break;
    case FCCOUNT_2:
        return 2;
        break;
    case FCCOUNT_3:
        return 3;
        break;
    case FCCOUNT_4:
    default:
        return 4;
        break;
    }
}

//can we stack from onto to based on current game rules
//color, suit, etc depending on game .. doesn't check value
BOOL CIssFreeCell::CanBuild(byte btFrom, byte btTo)
{
    switch(m_eGameType)
    {
    case FCTYPE_Kingcell:
        return TRUE;//anything goes 
    	break;
    case FCTYPE_Freecell:
    case FCTYPE_FourColors:
        //check color
        if(IsCardBlack(btFrom) != IsCardBlack(btTo))
            return TRUE;//gotta alternate
        break;
    case FCTYPE_Bakers:
        if(GetCardSuit(btFrom) == GetCardSuit(btTo))
            return TRUE;//gotta be the same suit
        break;
    }
    return FALSE;

}

int CIssFreeCell::GetNumColumns()
{
    if(m_eGameType == FCTYPE_FourColors)
        return 7;
    return 8;
}

void CIssFreeCell::DeleteUndoArray()
{
	TypeFreeCellUndo* sUndo;

	for(int i = 0; i < m_arrUndo.GetSize(); i++)
	{
		sUndo = m_arrUndo[i];
		if(sUndo)
		{
			delete sUndo;
			sUndo = NULL;
		}
	}
	m_arrUndo.RemoveAll();
}

void CIssFreeCell::DeleteLastUndo()
{
	if(m_arrUndo.GetSize() == 0)
		return;

	TypeFreeCellUndo* sUndo = m_arrUndo[m_arrUndo.GetSize()-1];

	if(sUndo == NULL)
		return;

	delete sUndo;
	sUndo = NULL;

	m_arrUndo.RemoveElementAt(m_arrUndo.GetSize() - 1);
}