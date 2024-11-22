#include "KakuroGame.h"
#include "IssString.h"
#include "IssRegistry.h"
#include "unzip.h"

#define ZIP_PASSWORD		_T("iSS26iSS")

struct TypeGameState	// used for saving a complete game set
{
	int iTiles[BOARD_SIZE][BOARD_SIZE];
	int iNumSeconds;		// seconds played in the game
	int iNumFreeCells;		// free cells in the game
	int iMistakeCounter;	// 
	int	iTotalPlayableCells;
	BOOL bAutoPencil;		// auto fill pencil marks
	BOOL bShowErrors;
    BOOL bAutoClearPencil;
};

CKakuroGame::CKakuroGame(void)
:m_hInst(NULL)
,m_iBoardSize(10)
,m_bIsGameInPlay(FALSE)
,m_bShowErrors(TRUE)
,m_eLevel(LVL_Easy)
,m_eSize(KSIZE_5)
,m_iNumSeconds(0)
,m_iNumFreeCells(0)
,m_bAutoPencilMarks(FALSE)
,m_bAutoRemovePencilMarks(FALSE)
,m_iMistakeCounter(0)
,m_iTotalPlayableCells(0)
,m_hWnd(NULL)
{
#ifdef DEBUG
    m_bAutoRemovePencilMarks = TRUE;
#endif

    m_sLastMove.eLastMove = LASTMOVE_None;
	m_oStr = CIssString::Instance();
	srand(GetTickCount());

	LoadRegistry();
}

void CKakuroGame::LoadRegistry()
{
	DWORD dwValue;

	if(S_OK == GetKey(REG_KEY,_T("Level"),dwValue))
		m_eLevel		= (EnumLevel)dwValue;

	if(S_OK == GetKey(REG_KEY,_T("KakuroGameInPlay"),dwValue))
		m_bIsGameInPlay	= (BOOL)dwValue;

	if(S_OK == GetKey(REG_KEY,_T("Size"),dwValue))
		m_eSize	        = (EnumSize)dwValue;

	switch(m_eSize)
	{
	case KSIZE_5:
		m_iBoardSize = 6;
		break;
	case KSIZE_7:
		m_iBoardSize = 8;
		break;
	case KSIZE_9:
		m_iBoardSize = 10;
	default:
	    break;
	}

	// if there was a game then retrieve the game state
	if(m_bIsGameInPlay)
	{
		TypeGameState sGs;

		// get our single Game State values
		dwValue = sizeof(TypeGameState);
		if(S_OK == GetKey(REG_KEY, _T("KakuroGameState"), (LPBYTE)&sGs, dwValue))
		{
			// set our member variables
			m_iNumFreeCells		= sGs.iNumFreeCells;
			m_iNumSeconds		= sGs.iNumSeconds;
			m_bAutoPencilMarks	= sGs.bAutoPencil;
			m_bShowErrors		= sGs.bShowErrors;
            m_bAutoRemovePencilMarks = sGs.bAutoClearPencil;
			m_iMistakeCounter	= sGs.iMistakeCounter;
			m_iTotalPlayableCells = sGs.iTotalPlayableCells;
			memcpy(m_iTile, sGs.iTiles, sizeof(int)*BOARD_SIZE*BOARD_SIZE);
		}
	}

}

CKakuroGame::~CKakuroGame(void)
{
	SaveRegistry();
	// destroy all the structures now
	Destroy();
}

void CKakuroGame::SaveRegistry()
{
	DWORD dwValue = (DWORD)m_bIsGameInPlay;
	SetKey(REG_KEY, _T("KakuroGameInPlay"), dwValue);
	dwValue = (DWORD)m_eLevel;
	SetKey(REG_KEY, _T("Level"), dwValue);
	dwValue = (DWORD)m_eSize;
	SetKey(REG_KEY, _T("Size"), dwValue);

	if(m_bIsGameInPlay)
	{
		TypeGameState sGs;
		sGs.iNumSeconds			= m_iNumSeconds;
		sGs.iNumFreeCells		= m_iNumFreeCells;
		sGs.bAutoPencil			= m_bAutoPencilMarks;
		sGs.bShowErrors			= m_bShowErrors;
        sGs.bAutoClearPencil    = m_bAutoRemovePencilMarks;
		sGs.iTotalPlayableCells = m_iTotalPlayableCells;
		sGs.iMistakeCounter		= m_iMistakeCounter;
		memcpy(sGs.iTiles, m_iTile, sizeof(int)*BOARD_SIZE*BOARD_SIZE);

		dwValue = sizeof(TypeGameState);
		SetKey(REG_KEY, _T("KakuroGameState"), (LPBYTE)&sGs, dwValue);	
	}
}

BOOL CKakuroGame::Destroy()
{
	return TRUE;
}

BOOL CKakuroGame::Initialize(HINSTANCE hInst, HWND hWnd)
{
	m_hInst = hInst;
    m_hWnd = hWnd;
//	Destroy();
	return TRUE;
}

TCHAR* CKakuroGame::GetPuzzle(EnumLevel eLevel, EnumSize eSize)
{
	TCHAR szResName[STRING_LARGE];
	m_oStr->StringCopy(szResName, _T("DAT_K"));

	switch(eSize)
	{
	case KSIZE_5:
		m_oStr->Concatenate(szResName, _T("5"));
		m_iBoardSize = 6;
		break;
	case KSIZE_7:
		m_oStr->Concatenate(szResName, _T("7"));
		m_iBoardSize = 8;
		break;
	case KSIZE_9:
	default:
		m_oStr->Concatenate(szResName, _T("9"));
		m_iBoardSize = 10;
	    break;
	}

	switch(eLevel)
	{
	case LVL_Easy:
		m_oStr->Concatenate(szResName, _T("E"));
		break;
	case LVL_Medium:
		m_oStr->Concatenate(szResName, _T("N"));
		break;
	case LVL_Hard:
		m_oStr->Concatenate(szResName, _T("H"));
		break;
	}

	//now lets load our zip file into a buffer
	HZIP hZip;

	// try and load up the text from the resource
	HRSRC hres = FindResource(m_hInst, szResName, _T("DAT"));
	if (hres == 0)
	{ 
		// did not find it
		return NULL;
	}

	//If resource is found a handle to the resource is returned
	//now just load the resource
	HGLOBAL    hbytes = LoadResource(m_hInst, hres);

	// Lock the resource
	LPVOID pdata	= LockResource(hbytes);
	UINT ziplen		= SizeofResource(m_hInst,hres);
	
	hZip = OpenZip(pdata, ziplen, 0);
	ZIPENTRY zEntry;
	GetZipItem(hZip, 0, &zEntry);
	char* zipBuf = new char[zEntry.unc_size];
	
	UnzipItem(hZip, 0, zipBuf, zEntry.unc_size);

	int iLineLength = ((m_iBoardSize-1)*(m_iBoardSize-1)+2);
	int iNumPuzzles = zEntry.unc_size/iLineLength;

	int iPuzzleIndex = rand()%(iNumPuzzles-1);

	TCHAR* szReturn = m_oStr->CreateAndCopy(zipBuf, iPuzzleIndex*iLineLength, iLineLength-2);

	//and finally ... lets copy some data into our szReturn
	

	CloseZip(hZip);

	delete [] zipBuf;
	zipBuf = NULL;

	return szReturn;
}

BOOL CKakuroGame::NewGame()
{
	//show the wait cursor
	SetCursor(LoadCursor(NULL, IDC_WAIT)); 
	ShowCursor(TRUE);

	Destroy();

	m_iNumSeconds			= 0;
	m_iMistakeCounter		= 0;

	m_ptLastCorrectGuess.x	= 0;
	m_ptLastCorrectGuess.y	= 0;

    m_ptLastHint.x          = -1;
    m_ptLastHint.y          = -1;
    

    m_sLastMove.eLastMove = LASTMOVE_None;

	TCHAR* szPuzzle = GetPuzzle(m_eLevel, m_eSize);
    if(szPuzzle == NULL)
        return FALSE;

    ZeroMemory(m_iTile, sizeof(int)*BOARD_SIZE*BOARD_SIZE);

	//rotate, mirror, etc
	RandomizeBoard(szPuzzle);
	
	int iValue = 0;
	int iIndex = 0;

    for(int y=0; y<m_iBoardSize; y++)
	{
		for(int x=0; x<m_iBoardSize; x++)
		{
			// if this is a Kakuro piece ... edge piece
			iIndex = (x-1)+(y-1)*(m_iBoardSize-1);
			if(x >0 && y > 0)
				iValue = szPuzzle[iIndex] - 48;
			else 
				iValue = -1;

			if(iValue < 1)//edge piece 
			{
				SetType(m_iTile[x][y], TP_Kakuro);
				//zero em out for now ... set later
				SetHorzTotal(m_iTile[x][y], 0);
				SetVertTotal(m_iTile[x][y], 0);
			}
			else
			{
				SetType(m_iTile[x][y], TP_PencilMark);
				SetSolution(m_iTile[x][y], iValue);
				SetGuess(m_iTile[x][y], 0);
				SetPencil(m_iTile[x][y], 0);
			}
		}
	}


	//we'll set the sums in these loops
	for(int y=0; y<m_iBoardSize; y++)
	{
		for(int x=0; x<m_iBoardSize; x++)
		{
			if(GetType(m_iTile[x][y]) == TP_Kakuro)
			{
				//TypePieceKakuro* sKakuro = GetKakuroPiece(m_sTile[x][y].iIndex);
				int iHTotal = 0;
				int iVTotal = 0;

				// count up the horizontal sum
				for(int i=x+1; i<m_iBoardSize; i++)
				{
					// if it's another kakuro piece we're done counting
					if(GetType(m_iTile[i][y]) == TP_Kakuro)
						break;
					else if(y == 0)	// we don't count the first row
						break;

					// if this is a Kakuro piece
					iIndex = (i-1)+(y-1)*(m_iBoardSize-1);
					if(iIndex > -1)
						iValue = szPuzzle[iIndex]-48;

					iHTotal += iValue;
				}

				// count up the vertical sum
				for(int i=y+1; i<m_iBoardSize; i++)
				{
					// if it's another kakuro piece we're done counting
					if(GetType(m_iTile[x][i]) == TP_Kakuro)
						break;
					else if(x == 0)	// we don't count the first column
						break;

					// if this is a Kakuro piece
					iIndex = (x-1)+(i-1)*(m_iBoardSize-1);
					if(iIndex > -1)
						iValue = szPuzzle[iIndex]-48;

					iVTotal += iValue;
				}

				if(iVTotal == 0 && iHTotal == 0)
					SetType(m_iTile[x][y], TP_Blank);

				//and finally .... set the totals
				SetHorzTotal(m_iTile[x][y], iHTotal);
				SetVertTotal(m_iTile[x][y], iVTotal);
					
			}
		}
	}

	m_bIsGameInPlay = TRUE;
	CalcFreeCells();

	AutoFillPencilMarks();

	if(szPuzzle)
		delete [] szPuzzle;
	szPuzzle = NULL;

	//set the total free cells
	m_iTotalPlayableCells = GetFreeCells();

	SaveRegistry();

	ShowCursor(FALSE);
	SetCursor(NULL);

	return TRUE;
}

void CKakuroGame::RandomizeBoard(TCHAR* szGame)
{
	if(szGame == NULL )
		return;

	int iType = rand()%8;

	switch(iType)
	{
	case 0://rotate 180
		Rotate180(szGame);
		break;
	case 1:
		Rotate90(szGame);
		break;
	case 2:
		Rotate270(szGame);
	    break;
	case 3:
		MirrorXY(szGame);
		break;
	case 4:
		MirrorXY(szGame);
		Rotate90(szGame);
		break;
	case 5:
		MirrorXY(szGame);
		Rotate180(szGame);
		break;
	case 6:
		MirrorXY(szGame);
		Rotate270(szGame);
	    break;
	case 7:	//do nothing
	default:
		break;
	}
}

TCHAR CKakuroGame::GetValueFromTCHAR(int iX, int iY, TCHAR* szGame)
{
	if(szGame == NULL)
		return 0;

	return szGame[iX+iY*(m_iBoardSize-1)];//board size includes top and left boarders
}

void CKakuroGame::MirrorXY(TCHAR* szGame)
{
	if(szGame == NULL)
		return;

	int iX;
	int iY;

	TCHAR* szCopy = m_oStr->CreateAndCopy(szGame);

	for(int y = 0; y < m_iBoardSize-1; y++)
	{
		for(int x = 0; x < m_iBoardSize-1; x++)
		{
			iX = y;
			iY = x;
			szGame[x+y*(m_iBoardSize-1)] = szCopy[iX+iY*(m_iBoardSize-1)];
		}
	}

	m_oStr->Delete(&szCopy);
}


void CKakuroGame::Rotate90(TCHAR* szGame)
{
	if(szGame == NULL)
		return;

	int iX;
	int iY;

	TCHAR* szCopy = m_oStr->CreateAndCopy(szGame);

	for(int y = 0; y < m_iBoardSize-1; y++)
	{
		for(int x = 0; x < m_iBoardSize-1; x++)
		{
			iX = m_iBoardSize-2-y;
			iY = x;
			szGame[x+y*(m_iBoardSize-1)] = szCopy[iX+iY*(m_iBoardSize-1)];
		}
	}

	m_oStr->Delete(&szCopy);
}

void CKakuroGame::Rotate180(TCHAR* szGame)
{
	if(szGame == NULL)
		return;
	
	TCHAR szTemp;
	int iLen = m_oStr->GetLength(szGame);
	int iIndex = iLen;
	if(iIndex%2 == 1)
		iIndex--;
	iIndex/=2;
	for(int i=0; i<iIndex;i++)
	{
		szTemp = szGame[i];
		szGame[i] = szGame[iLen-1-i];
		szGame[iLen-1-i] = szTemp;
	}
}

void CKakuroGame::Rotate270(TCHAR* szGame)
{
	if(szGame == NULL)
		return;

	int iX;
	int iY;

	TCHAR* szCopy = m_oStr->CreateAndCopy(szGame);

	for(int y = 0; y < m_iBoardSize-1; y++)
	{
		for(int x = 0; x < m_iBoardSize-1; x++)
		{
			iX = y;
			iY = m_iBoardSize-2-x;
			szGame[x+y*(m_iBoardSize-1)] = szCopy[iX+iY*(m_iBoardSize-1)];
		}
	}

	m_oStr->Delete(&szCopy);


}

void CKakuroGame::RestartGame()
{	//we need to erase all guesses ... restore to only given values
	m_ptLastCorrectGuess.x	= 0;
	m_ptLastCorrectGuess.y	= 0;
    m_ptLastHint.x          = -1;
    m_ptLastHint.y          = -1;
	m_iMistakeCounter		= 0;
	m_iNumSeconds			= 0;
    m_sLastMove.eLastMove = LASTMOVE_None;


	//and now set all of the guess pieces back to pencil marks
	for(int x = 0; x < m_iBoardSize; x++)
	{
		for(int y = 0; y < m_iBoardSize; y++)
		{
			if(GetType(m_iTile[x][y]) == TP_Guess)
				SetType(m_iTile[x][y], TP_PencilMark);
		}
	}

	m_bIsGameInPlay = TRUE;
	CalcFreeCells();
	AutoFillPencilMarks();
}

BOOL CKakuroGame::IsGameComplete()
{
	if(m_bIsGameInPlay == FALSE)
		return TRUE;

	if(m_iNumFreeCells > 0)
		return FALSE;

	//lets actually check then
	for(int x = 1; x < m_iBoardSize; x++)//1 because we'll skip the boarders
	{
		for(int y=1; y < m_iBoardSize; y++)
		{
			if(GetType(m_iTile[x][y]) == TP_Guess)
			{	//make sure the guess equals the solution
				if(GetSolution(m_iTile[x][y]) != GetGuess(m_iTile[x][y]))
					return FALSE;
			}
		}
	}
	//we win!
	m_bIsGameInPlay = FALSE;
	return TRUE;
}

int CKakuroGame::GetGuessPiece(int iX, int iY)
{
	if(!IsTileValid(iX, iY))
		return NULL;

	return m_iTile[iX][iY];

}



int CKakuroGame::GetKakuroPiece(int iX, int iY)
{
	if(!IsTileValid(iX, iY))
		return NULL;

	return m_iTile[iX][iY];

}

EnumTilePiece CKakuroGame::GetTileType(int iX, int iY)
{
	if(IsTileValid(iX, iY) == FALSE)
		return TP_Blank;

	return (EnumTilePiece)GetType(m_iTile[iX][iY]);
}

BOOL CKakuroGame::IsTileValid(int iX, int iY)
{
	if(iX > m_iBoardSize ||
		iY > m_iBoardSize)
		return FALSE;

	if(iX < 0 ||
		iY < 0)
		return FALSE;

	return TRUE;
}

void CKakuroGame::TogglePencil(int iNum, int iX, int iY)
{
	if(iNum < 1 || iNum > 9 || !IsTileValid(iX, iY))
		return;

	int iTemp = (int)(pow(2, iNum-1));

	if(GetType(m_iTile[iX][iY]) != TP_PencilMark)
	{
		SetType(m_iTile[iX][iY], TP_PencilMark);
		RecalcPencilMarks(iX, iY);
	}

//	TypePieceGuess* sGuess = GetGuessPiece(iX, iY);

	int iPencil = GetPencil(m_iTile[iX][iY]);

	if((iPencil & iTemp) == iTemp)
		iPencil -= iTemp;
	else
		iPencil += iTemp;

	SetPencil(m_iTile[iX][iY], iPencil);

	CalcFreeCells();
}

void CKakuroGame::ClearPencil(int iX, int iY)
{
	if(!IsTileValid(iX, iY))
		return;

	SetType(m_iTile[iX][iY], TP_PencilMark);
	SetPencil(m_iTile[iX][iY], 0);
}

void CKakuroGame::SetGuessPiece(int iNum, int iX, int iY)
{
	if(iNum < 1 || iNum > 9 || !IsTileValid(iX, iY))
		return;

    m_sLastMove.eLastMove = LASTMOVE_Guess;
    m_sLastMove.iTile = m_iTile[iX][iY];
    m_sLastMove.iX = iX;
    m_sLastMove.iY = iY;

	if(GetType(m_iTile[iX][iY]) != TP_Guess)
		SetType(m_iTile[iX][iY], TP_Guess);

	int iGuess		= GetGuess(m_iTile[iX][iY]);
	int iSolution	= GetSolution(m_iTile[iX][iY]);

	if(iGuess == iNum)
	{
		iGuess = 0;
		SetType(m_iTile[iX][iY], TP_PencilMark);
	}
	else
	{
		iGuess = iNum;
		//if(iGuess == iSolution || m_bShowErrors == FALSE)
		{
			m_ptLastCorrectGuess.x = iX;
			m_ptLastCorrectGuess.y = iY;
		}
		if(iGuess != iSolution)
			m_iMistakeCounter++;
	}

	//and finally...set the guess
	SetGuess(m_iTile[iX][iY], iGuess);
    PostMessage(m_hWnd, WM_UPDATECELL, iX, iY);
	CalcFreeCells();
	RecalcPencilMarks(iX, iY);
}

POINT CKakuroGame::GetLastCorrectGuess()
{
	POINT pt = m_ptLastCorrectGuess;

	//erases it when you ask
	m_ptLastCorrectGuess.x = 0;
	m_ptLastCorrectGuess.y = 0;
	return pt;
}

BOOL CKakuroGame::IsPencilMarkPossible(int iX, int iY, int iValue)
{
	if(iValue < 1 || iValue > 9 || !IsTileValid(iX, iY))
		return FALSE;



	if(GetType(m_iTile[iX][iY]) == TP_Blank ||
		GetType(m_iTile[iX][iY]) == TP_Kakuro)
		return FALSE;

	//now we actually check the pencil mark

	if(!IsPencilValueValid(iValue, FindHorzTotal(iX, iY, FALSE), FindHorzCWSize(iX, iY, FALSE)))
		return FALSE;
	if(!IsPencilValueValid(iValue, FindVertTotal(iX, iY, FALSE), FindVertCWSize(iX, iY, FALSE)))
		return FALSE;

	return TRUE;

}


BOOL CKakuroGame::IsPencilMarkValid(int iX, int iY, int iValue)
{
	if(IsPencilMarkPossible(iX, iY, iValue)==FALSE)
		return FALSE;

	if(iValue < 1 || iValue > 9 || !IsTileValid(iX, iY))
		return FALSE;

	if(GetType(m_iTile[iX][iY]) == TP_Blank ||
		GetType(m_iTile[iX][iY]) == TP_Kakuro)
		return FALSE;

	//now we actually check the pencil mark
	if(!IsPencilValueValid(iValue, FindHorzTotal(iX, iY), FindHorzCWSize(iX, iY)))
		return FALSE;
	if(!IsPencilValueValid(iValue, FindVertTotal(iX, iY), FindVertCWSize(iX, iY)))
		return FALSE;

	int iValueBin = (int)(pow(2, iValue));

	int iHorz = GetHorzGuesses(iX, iY);
	int iVert = GetVertGuesses(iX, iY);
	if((iHorz & iValueBin) == iValueBin ||
		(iVert & iValueBin )== iValueBin)
		return FALSE;

	return TRUE;
}

int CKakuroGame::FindHorzTotal(int iX, int iY, BOOL bCheat)
{
	if(!IsTileValid(iX, iY))
		return 0;	

	if(GetType(m_iTile[iX][iY])== TP_Blank)
		return 0;

	int iTotal = 0;

	int iTempX = iX;

	while(GetType(m_iTile[iTempX][iY]) != TP_Kakuro)
		iTempX--;

	iTotal = GetHorzTotal(m_iTile[iTempX][iY]);
	iTempX++;

	//Now...if we're doing the cheater option....go back through the group and subtract off guesses
	if(bCheat)
	{
		while(GetType(m_iTile[iTempX][iY]) != TP_Kakuro &&
			GetType(m_iTile[iTempX][iY]) != TP_Blank)
		{
			if(iTempX >= m_iBoardSize)
				break;
			if(GetType(m_iTile[iTempX][iY]) == TP_Guess)
				iTotal -= GetGuess(m_iTile[iTempX][iY]);
			
			iTempX++;
		}
		//and finally...if we've selected a cell that already has a total......re-add it 
		if(GetType(m_iTile[iX][iY]) == TP_Guess)
			iTotal += GetGuess(m_iTile[iX][iY]);
	}

	return iTotal;
}


int CKakuroGame::FindVertTotal(int iX, int iY, BOOL bCheat)
{
	if(!IsTileValid(iX, iY))
		return 0;	

	if(GetType(m_iTile[iX][iY]) == TP_Blank)
		return 0;

	int iTotal = 0;

	int iTempY = iY;

	while(GetType(m_iTile[iX][iTempY]) != TP_Kakuro)
		iTempY--;

	iTotal = GetVertTotal(m_iTile[iX][iTempY]);

	//Now...if we're doing the cheater option....go back through the group and subtract off guesses
	if(bCheat)
	{
		iTempY++;

		while(GetType(m_iTile[iX][iTempY]) != TP_Kakuro &&
			GetType(m_iTile[iX][iTempY]) != TP_Blank)
		{
			if(iTempY >= m_iBoardSize)
				break;
			if(GetType(m_iTile[iX][iTempY]) == TP_Guess)
				iTotal -= GetGuess(m_iTile[iX][iTempY]);
			iTempY++;
		}

		//and finally...if we've selected a cell that already has a total...re-add it 
		if(GetType(m_iTile[iX][iY]) == TP_Guess)
			iTotal += GetGuess(m_iTile[iX][iY]);
	}

	return iTotal;

}

int CKakuroGame::FindHorzCWSize(int iX, int iY, BOOL bCheat)
{
	if(!IsTileValid(iX, iY))
		return 0;	

	int iTempX	= iX;
	int iSize	= 0;

	while(GetType(m_iTile[iTempX][iY]) != TP_Kakuro)
		iTempX--;

	iTempX++;

	if(bCheat)
	{
		while(GetType(m_iTile[iTempX][iY]) != TP_Kakuro &&
			GetType(m_iTile[iTempX][iY]) != TP_Blank)
		{
			if(iTempX >= m_iBoardSize)
				break;
			if(GetType(m_iTile[iTempX][iY]) == TP_PencilMark)
				iSize++;
			iTempX++;
		}

		//and finally...if the cell has a guess in it we still need to count it for the selection screen
		if(GetType(m_iTile[iX][iY]) == TP_Guess)
			iSize++;
	}
	else//count up the guess cells and the pencil mark cells
	{
		while(GetType(m_iTile[iTempX][iY]) != TP_Kakuro &&
			GetType(m_iTile[iTempX][iY]) != TP_Blank)
		{
			if(iTempX >= m_iBoardSize)
				break;
			iSize++;
			iTempX++;
		}
	}

	return iSize;
}

int CKakuroGame::FindVertCWSize(int iX, int iY, BOOL bCheat)
{
	if(!IsTileValid(iX, iY))
		return 0;	

	int iTempY	= iY;
	int iSize	= 0;

	while(GetType(m_iTile[iX][iTempY]) != TP_Kakuro)
		iTempY--;

	iTempY++;

	if(bCheat)
	{
		while(GetType(m_iTile[iX][iTempY]) != TP_Kakuro &&
			GetType(m_iTile[iX][iTempY]) != TP_Blank)
		{
			if(iTempY >= m_iBoardSize)
				break;
			if(GetType(m_iTile[iX][iTempY]) == TP_PencilMark)
				iSize++;
			iTempY++;
		}

		//and finally...if the cell has a guess in it we still need to count it for the selection screen
		if(GetType(m_iTile[iX][iY]) == TP_Guess)
			iSize++;
	}
	else//count up the guess cells and the pencil mark cells
	{
		while(GetType(m_iTile[iX][iTempY]) != TP_Kakuro &&
			GetType(m_iTile[iX][iTempY]) != TP_Blank)
		{
			if(iTempY >= m_iBoardSize)
				break;
			iSize++;
			iTempY++;
		}
	}

	return iSize;
}

int CKakuroGame::GetHorzGuesses(int iX, int iY)
{
	if(!IsTileValid(iX, iY))
		return 0;	

	int iTempX	= iX;
	int iGuesses= 0;

	while(GetType(m_iTile[iTempX][iY]) != TP_Kakuro)
		iTempX--;

	iTempX++;

	while(GetType(m_iTile[iTempX][iY]) != TP_Kakuro &&
		GetType(m_iTile[iTempX][iY]) != TP_Blank)
	{
		if(iTempX >= m_iBoardSize)
			break;
		else if(GetType(m_iTile[iTempX][iY]) == TP_Guess)
			iGuesses += (int)pow(2, GetGuess(m_iTile[iTempX][iY]));
		iTempX++;
	}
	
	//and finally...if its already been guessed
	if(GetType(m_iTile[iX][iY]) == TP_Guess)
		iGuesses -= (int)pow(2, GetGuess(m_iTile[iX][iY]));

	return iGuesses;
}

int CKakuroGame::GetVertGuesses(int iX, int iY)
{
	if(!IsTileValid(iX, iY))
		return 0;	

	int iTempY	= iY;
	int iGuesses= 0;

	while(GetType(m_iTile[iX][iTempY]) != TP_Kakuro)
		iTempY--;

	iTempY++;

	while(GetType(m_iTile[iX][iTempY]) != TP_Kakuro &&
		GetType(m_iTile[iX][iTempY]) != TP_Blank)
	{
		if(iTempY >= m_iBoardSize)
			break;
		else if(GetType(m_iTile[iX][iTempY]) == TP_Guess)
			iGuesses += (int)pow(2, GetGuess(m_iTile[iX][iTempY]));
		iTempY++;
	}

	//and finally...if its already been guessed
	if(GetType(m_iTile[iX][iY]) == TP_Guess)
		iGuesses -= (int)pow(2, GetGuess(m_iTile[iX][iY]));
	return iGuesses;
}

BOOL CKakuroGame::IsPencilValueValid(int iValue, int iTotal, int iGroupSize)
{
	if(iTotal < 1 || 
		iGroupSize < 1)
		return FALSE;

//	int iUpperLimit = 0;
//	int iLowerLimit = 0;

	switch(iGroupSize)
	{
	case 1:
		if(iValue != iTotal)
			return FALSE;
		return TRUE;
		break;
	default://lets use the table instead ... should be more accurate
		{
			int iPosValues = GetGuessPossibilities(iGroupSize, iTotal);
			return IsGuessValid(iValue, iPosValues);
		}
		break;
	}
//	return FALSE;
}

void CKakuroGame::IncrementTimer()
{
	if(m_bIsGameInPlay)
		m_iNumSeconds++;
}

void CKakuroGame::CalcFreeCells()
{
	if(!m_bIsGameInPlay)
		return;

	m_iNumFreeCells = 0;
	for(int x=0; x<m_iBoardSize; x++)
	{
		for(int y=0; y<m_iBoardSize; y++)
		{
			if(GetType(m_iTile[x][y]) == TP_PencilMark)
				m_iNumFreeCells++;
		}
	}
}

void CKakuroGame::AutoFillPencilMarks()
{	
	if(!m_bAutoPencilMarks)
		return;

	for(int x = 0; x < m_iBoardSize; x++)
	{
		for(int y = 0; y < m_iBoardSize; y++)
		{
			if(GetType(m_iTile[x][y]) == TP_PencilMark)
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

void CKakuroGame::RecalcPencilMarks(int iX, int iY)
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

	while(GetType(m_iTile[iTempX][iY]) != TP_Kakuro)
		iTempX--;

	iTempX++;

	while(GetType(m_iTile[iTempX][iY]) != TP_Kakuro &&
		GetType(m_iTile[iTempX][iY]) != TP_Blank)
	{
		if(iTempX >= m_iBoardSize)
			break;
		else if(GetType(m_iTile[iTempX][iY]) == TP_PencilMark)
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
                int iTemp = (int)(pow(2, iValue-1));
                int iPencil = GetPencil(m_iTile[iTempX][iY]);

                if((iPencil & iTemp) == iTemp)
                {
                    iPencil -= iTemp;
                    SetPencil(m_iTile[iTempX][iY], iPencil);
                }
            }
		}
		iTempX++;
	}

	while(GetType(m_iTile[iX][iTempY]) != TP_Kakuro)
		iTempY--;

	iTempY++;

	while(GetType(m_iTile[iX][iTempY]) != TP_Kakuro &&
		GetType(m_iTile[iX][iTempY]) != TP_Blank)
	{
		if(iTempY >= m_iBoardSize)
			break;
		else if(GetType(m_iTile[iX][iTempY]) == TP_PencilMark)
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
                int iTemp = (int)(pow(2, iValue-1));
                int iPencil = GetPencil(m_iTile[iX][iTempY]);

                if((iPencil & iTemp) == iTemp)
                {
                    iPencil -= iTemp;
                    SetPencil(m_iTile[iX][iTempY], iPencil);
                }
            }

		}
		iTempY++;
	}
}

double CKakuroGame::GetAccuracy()
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

int CKakuroGame::GetGuessPossibilities(int iNumCells, int iTotal)
{
	int iGuess = 511; // default to all possibilities

	for(int i=0; i< sizeof(g_sNumList)/sizeof(StructNumberList); i++)
	{
		if(iNumCells == g_sNumList[i].btNumCells && iTotal == g_sNumList[i].btTotal)
		{
			iGuess = g_sNumList[i].wIncluded;
			break;
		}
	}
	return iGuess;
}

//Guess... user guess
//NumberToCheck ... value from the array 
BOOL CKakuroGame::IsGuessValid(int iGuess, int iNumberToCheck)
{
	return (BOOL)(iNumberToCheck & (int)pow(2, 9-iGuess));
}

BOOL CKakuroGame::ClearAllPencilMarks()
{
	if(m_bAutoPencilMarks)
		return FALSE;

	//ok..this is a little worse...go through every cell, check if its a pencil mark or guess...and clear it
	for(int iX = 1; iX < m_iBoardSize; iX++)
	{
		for(int iY = 1; iY < m_iBoardSize; iY++)
		{
			if(GetType(m_iTile[iX][iY]) == TP_PencilMark ||
				GetType(m_iTile[iX][iY]) == TP_Guess)
			{
				SetPencil(m_iTile[iX][iY], 0);
			}
		}
	}

	return TRUE;
}


void CKakuroGame::SolveGame()
{
    for(int y = 0; y < m_iBoardSize; y++)
    {
        for(int x = 0; x < m_iBoardSize; x++)
        {
            if(GetType(m_iTile[x][y]) == TP_PencilMark ||
                (GetType(m_iTile[x][y]) == TP_Guess &&
                GetGuess(GetGuessPiece(x, y)) != GetSolution(GetGuessPiece(x, y)))    )
            {   //if its pencil marks or a wrong guess
                SetGuess(m_iTile[x][y], GetSolution(m_iTile[x][y]));
                SetType(m_iTile[x][y], TP_Guess);
                PostMessage(m_hWnd, WM_UPDATECELL, x, y);//animate them baby    
            }
        }
    }
    CalcFreeCells();
}


BOOL CKakuroGame::Hint()
{
	if(m_bIsGameInPlay == FALSE)
		return FALSE;
	
	BOOL	bFound	= FALSE;
	int		iLoop	= 0;
	int		iRandX	= -1;
	int		iRandY	= -1;

	while(bFound == FALSE)
	{
		if(iLoop > 250)
			return FALSE;//too many tries

		iRandX = rand()%m_iBoardSize;
		iRandY = rand()%m_iBoardSize;

		if(GetType(m_iTile[iRandX][iRandY]) == TP_PencilMark)
		{
			SetGuess(m_iTile[iRandX][iRandY], GetSolution(m_iTile[iRandX][iRandY]));
			SetType(m_iTile[iRandX][iRandY], TP_Guess);
			m_ptLastCorrectGuess.x	= iRandX;
			m_ptLastCorrectGuess.y	= iRandY;
            m_ptLastHint.x          = iRandX;
            m_ptLastHint.y          = iRandY;
            PostMessage(m_hWnd, WM_UPDATECELL, iRandX, iRandY);
			bFound = TRUE;
		}
	}

    CalcFreeCells();

	m_iMistakeCounter++; //you cheat..you pay the price

	return TRUE;
}

int	CKakuroGame::GetType(int d)
{
	return (d&3);
}

int	CKakuroGame::GetPencil(int d)
{
	return (d&2044)>>2;
}

int	CKakuroGame::GetGuess(int d)
{
	return (d&30720)>>11;
}

int	CKakuroGame::GetSolution(int d)
{
	return (d&491520)>>15;
}

int	CKakuroGame::GetHorzTotal(int d)
{
	return (d&252)>>2;
}

int	CKakuroGame::GetVertTotal(int d)
{
	return (d&16128)>>8;
}

void	CKakuroGame::SetType(int& iCurrent, int iType)
{
	// zero out
	iCurrent &= ~3;
	iCurrent |= (iType&3); 
}

void	CKakuroGame::SetPencil(int& iCurrent, int iPencil)
{
	// zero out
	iCurrent &= ~2044;
	iCurrent |= ((iPencil<<2)&2044);
}

void	CKakuroGame::SetGuess(int& iCurrent, int iGuess)
{
	// zero out
	iCurrent &= ~30720;
	iCurrent |= ((iGuess<<11)&30720);
}

void	CKakuroGame::SetSolution(int& iCurrent, int iSolution)
{
	// zero out
	iCurrent &= ~491520;
	iCurrent |= ((iSolution<<15)&491520);
}

void	CKakuroGame::SetHorzTotal(int& iCurrent, int iHorzTotal)
{
	// zero out
	iCurrent &= ~252;
	iCurrent |= ((iHorzTotal<<2)&252);
}

void	CKakuroGame::SetVertTotal(int& iCurrent, int iVertTotal)
{
	// zero out
	iCurrent &= ~16128;
	iCurrent |= ((iVertTotal<<8)&16128);
}

BOOL CKakuroGame::Undo()
{
    if(m_sLastMove.eLastMove == LASTMOVE_None)
        return FALSE;
 
    int iCell = m_iTile[m_sLastMove.iX][m_sLastMove.iY];

    m_iTile[m_sLastMove.iX][m_sLastMove.iY] = m_sLastMove.iTile;
    m_sLastMove.iTile = iCell;

    RecalcPencilMarks(m_sLastMove.iX, m_sLastMove.iY);
    CalcFreeCells();
    //lets just always post
    PostMessage(m_hWnd, WM_UNDOMOVE, m_sLastMove.iX, m_sLastMove.iY);
    return TRUE;
}