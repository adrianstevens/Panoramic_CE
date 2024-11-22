#include "SEngine.h"
#include "IssRegistry.h"

CSEngine::CSEngine(void)
:m_eGameType(STYPE_Kakuro)
,m_bShowHints(FALSE)
,m_eDiff(DIFF_UnKnown)
{
    for(int i = 0; i < STYPE_Count; i++)
    {
        for(int j = 0; j < DIFF_UnKnown; j++)
        {
            m_iBestTimes[i][j] = DEFAULT_TIME; //59:59
            m_iAvgTimes[i][j] = DEFAULT_TIME;
            m_iGameCounts[i][j] = 0;
        }
    }

    TCHAR szRegKey[STRING_MAX];
    TCHAR szValue[STRING_MAX];
    DWORD dwTemp;

    CIssString* oStr = CIssString::Instance();

    oStr->StringCopy(szRegKey, REG_KEY);
    oStr->Concatenate(szRegKey, _T("\\Scores"));

    for(int i=0; i< STYPE_Count; i++)
    {
        for(int j = 0; j < DIFF_UnKnown; j++)
        {
            oStr->Format(szValue, _T("%i%i"),i,j);

            if(S_OK == GetKey(szRegKey, szValue, dwTemp))
            {
                m_iBestTimes[i][j] = dwTemp;
            }
        }
    }

    oStr->StringCopy(szRegKey, REG_KEY);
    oStr->Concatenate(szRegKey, _T("\\Count"));

    for(int i=0; i< STYPE_Count; i++)
    {
        for(int j = 0; j < DIFF_UnKnown; j++)
        {
            oStr->Format(szValue, _T("%i%i"),i,j);

            if(S_OK == GetKey(szRegKey, szValue, dwTemp))
            {
                m_iGameCounts[i][j] = dwTemp;
            }
        }
    }

    oStr->StringCopy(szRegKey, REG_KEY);
    oStr->Concatenate(szRegKey, _T("\\Avg"));

    for(int i=0; i< STYPE_Count; i++)
    {
        for(int j = 0; j < DIFF_UnKnown; j++)
        {
            oStr->Format(szValue, _T("%i%i"),i,j);

            if(S_OK == GetKey(szRegKey, szValue, dwTemp))
            {
                m_iAvgTimes[i][j] = dwTemp;
            }
        }
    }
}

CSEngine::~CSEngine(void)
{
    TCHAR szRegKey[STRING_MAX];
    TCHAR szValue[STRING_MAX];
    DWORD dwTemp;

    CIssString* oStr = CIssString::Instance();

    oStr->StringCopy(szRegKey, REG_KEY);
    oStr->Concatenate(szRegKey, _T("\\Scores"));

    for(int i=0; i< STYPE_Count; i++)
    {
        for(int j = 0; j < DIFF_UnKnown; j++)
        {
            oStr->Format(szValue, _T("%i%i"),i,j);
            dwTemp = m_iBestTimes[i][j];

            SetKey(szRegKey, szValue, dwTemp);
        }
    }

    oStr->StringCopy(szRegKey, REG_KEY);
    oStr->Concatenate(szRegKey, _T("\\Count"));

    for(int i=0; i< STYPE_Count; i++)
    {
        for(int j = 0; j < DIFF_UnKnown; j++)
        {
            oStr->Format(szValue, _T("%i%i"),i,j);
            dwTemp = m_iGameCounts[i][j];

            SetKey(szRegKey, szValue, dwTemp);
        }
    }

    oStr->StringCopy(szRegKey, REG_KEY);
    oStr->Concatenate(szRegKey, _T("\\Avg"));

    for(int i=0; i< STYPE_Count; i++)
    {
        for(int j = 0; j < DIFF_UnKnown; j++)
        {
            oStr->Format(szValue, _T("%i%i"),i,j);
            dwTemp = m_iAvgTimes[i][j];

            SetKey(szRegKey, szValue, dwTemp);
        }
    }
}

void CSEngine::Initialize(HINSTANCE hInst, HWND hWnd)
{
    m_oKakuro.Initialize(hInst, hWnd);
    m_oKiller.Init(hInst, hWnd);
    m_oSudoku.Init(hInst, hWnd);
    m_oKenKen.Init(hInst, hWnd);
    m_oGreater.Init(hInst, hWnd);
}


int CSEngine::GetCellsFree()
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        return m_oSudoku.GetNumFreeCells();
    	break;
    case STYPE_Killer:
        return m_oKiller.GetNumFreeCells();
    	break;
    case STYPE_Kakuro:
        return m_oKakuro.GetNumFreeCells();
        break;
    case STYPE_KenKen:
        return m_oKenKen.GetNumFreeCells();
        break;
    case STYPE_Greater:
        return m_oGreater.GetNumFreeCells();
        break;
    default:
        return -1;
        break;
    }
}

int CSEngine::GetGridSize()
{
    if(m_eGameType == STYPE_Kakuro)
        return m_oKakuro.GetBoardSize();
    return 9;
}

BOOL CSEngine::NewGame()
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        return m_oSudoku.NewGame();
        break;
    case STYPE_Killer:
        m_oKiller.NewGame(m_oKiller.GetDifficulty());
        return TRUE;
        break;
    case STYPE_Kakuro:
        return m_oKakuro.NewGame();
        break;
    case STYPE_KenKen:
        m_oKenKen.NewGame(m_oKenKen.GetDifficulty());
        return TRUE;
        break;
    case STYPE_Greater:
        m_oGreater.NewGame();
        return TRUE;
        break;
    default:
        return FALSE;
        break;
    }
}

BOOL CSEngine::IsGameComplete(int iGameTime)
{
    BOOL bRet = FALSE;
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        bRet = m_oSudoku.IsGameComplete();
        break;
    case STYPE_Killer:
        bRet =  m_oKiller.IsGameComplete();
        break;
    case STYPE_Kakuro:
        bRet = m_oKakuro.IsGameComplete();
        break;
    case STYPE_KenKen:
        bRet = m_oKenKen.IsGameComplete();
        break;
    case STYPE_Greater:
        bRet = m_oGreater.IsGameComplete();
        break;
    default:
        return -1;
        break;
    }

    if(bRet == TRUE)
        CheckTime(iGameTime);

    return bRet;
}

void CSEngine::CheckTime(int iGameTime)
{
    if(iGameTime < m_iBestTimes[GetGameType()][GetDifficulty()])
        m_iBestTimes[GetGameType()][GetDifficulty()] = iGameTime;

    //increase the game count
    m_iGameCounts[GetGameType()][GetDifficulty()]++;

    //and calc the average time
    if(m_iGameCounts[GetGameType()][GetDifficulty()] == 1)
    {
        m_iAvgTimes[GetGameType()][GetDifficulty()] = iGameTime;
    }
    else
    {
        int iTemp = m_iGameCounts[GetGameType()][GetDifficulty()];
        iTemp--;
        iTemp = iTemp * m_iAvgTimes[GetGameType()][GetDifficulty()] + iGameTime;

        m_iAvgTimes[GetGameType()][GetDifficulty()] = iTemp / m_iGameCounts[GetGameType()][GetDifficulty()];
    }
}

void CSEngine::RestartGame()
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        m_oSudoku.RestartGame();
        break;
    case STYPE_Killer:
        m_oKiller.RestartGame();
        break;
    case STYPE_Kakuro:
        m_oKakuro.RestartGame();
        break;
    case STYPE_KenKen:
        m_oKenKen.RestartGame();
        break;
    case STYPE_Greater:
        m_oGreater.RestartGame();
        break;
    default:
        break;
    }
}

void CSEngine::OnSetGuess(int iX, int iY, int iGuess)
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        m_oSudoku.OnSetGuess(iX, iY, iGuess);
        break;
    case STYPE_Killer:
        m_oKiller.OnSetGuess(iX, iY, iGuess);
        break;
    case STYPE_Kakuro:
        if(m_oKakuro.GetTileType(iX, iY) == TP_Guess ||
            m_oKakuro.GetTileType(iX, iY) == TP_PencilMark)
            m_oKakuro.SetGuessPiece(iGuess, iX, iY);
        break;
    case STYPE_KenKen:
        m_oKenKen.OnSetGuess(iX, iY, iGuess);
        break;
    case STYPE_Greater:
        m_oGreater.OnSetGuess(iX, iY, iGuess);
        break;
    default:
        break;
    }
}

void CSEngine::OnSetPencilMark(int iX, int iY, int iPencil)
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        m_oSudoku.TogglePencilMarks(iX, iY, iPencil);
        break;
    case STYPE_Killer:
        m_oKiller.TogglePencilMarks(iX, iY, iPencil);
        break;
    case STYPE_Kakuro:
        if(m_oKakuro.GetTileType(iX, iY) == TP_Guess ||
            m_oKakuro.GetTileType(iX, iY) == TP_PencilMark)
            m_oKakuro.TogglePencil(iPencil, iX, iY);
        break;
    case STYPE_KenKen:
        m_oKenKen.TogglePencilMarks(iX, iY  , iPencil);
        break;
    case STYPE_Greater:
        m_oGreater.TogglePencilMarks(iX, iY, iPencil);
        break;
    default:
        break;
    }
}


POINT CSEngine::GetLastCorrectGuess()
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        return m_oSudoku.GetLastCorrectGuess();
        break;
    case STYPE_Killer:
        return m_oKiller.GetLastCorrectGuess();
        break;
    case STYPE_Kakuro:
        return m_oKakuro.GetLastCorrectGuess();
        break;
    case STYPE_KenKen:
        return m_oKenKen.GetLastCorrectGuess();
        break;
    case STYPE_Greater:
        return m_oGreater.GetLastCorrectGuess();
        break;
    default:
        POINT pt;
        pt.x=-1; pt.y = -1;
        return pt;
        break;
    }
}

POINT CSEngine::GetLastHint()
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        return m_oSudoku.GetLastHint();
        break;
    case STYPE_Killer:
        return m_oKiller.GetLastHint();
        break;
    case STYPE_Kakuro:
        return m_oKakuro.GetLastHint();
        break;
    case STYPE_KenKen:
        return m_oKenKen.GetLastHint();
        break;
    case STYPE_Greater:
        return m_oGreater.GetLastHint();
        break;
    default:
        POINT pt;
        pt.x=-1; pt.y = -1;
        return pt;
        break;
    }
}

POINT CSEngine::GetLastWrongGuess()
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        return m_oSudoku.GetLastWrongGuess();
        break;
    case STYPE_Killer:
        return m_oKiller.GetLastWrongGuess();
        break;
    case STYPE_KenKen:
        return m_oKenKen.GetLastWrongGuess();
        break;
    case STYPE_Greater:
        return m_oGreater.GetLastWrongGuess();
        break;
    case STYPE_Kakuro:
         //BUGBUG return m_oKakuro.GetLastWrongGuess();
        //break;
    default:
        POINT pt;
        pt.x=-1; pt.y = -1;
        return pt;
        break;
    }
}

BOOL CSEngine::IsLocked(int iX, int iY)
{
    if(m_eGameType == STYPE_Sudoku)
        return m_oSudoku.IsLocked(iX, iY);
    else if(m_eGameType == STYPE_Killer)
        return m_oKiller.IsLocked(iX, iY);
    else if(m_eGameType == STYPE_KenKen)
        return m_oKenKen.IsLocked(iX, iY);
    else if(m_eGameType == STYPE_Greater)
        return m_oGreater.IsLocked(iX, iY);
    return FALSE;//no locked cells for kakuro ... 
}

BOOL CSEngine::IsGiven(int iX, int iY)
{
    if(m_eGameType == STYPE_Sudoku)
        return m_oSudoku.IsGiven(iX, iY);
    else if(m_eGameType == STYPE_Killer)
        return m_oKiller.IsGiven(iX, iY);
    else if(m_eGameType == STYPE_KenKen)
        return m_oKenKen.IsGiven(iX, iY);
    else if(m_eGameType == STYPE_Greater)
        return m_oGreater.IsGiven(iX, iY);//might do this for difficulty ... hmmm
    return FALSE;//no given cells for kakuro ... 
}

int CSEngine::GetSolution(int iX, int iY)
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        return m_oSudoku.GetSolution(iX, iY);
        break;
    case STYPE_Killer:
        return m_oKiller.GetSolution(iX, iY);
        break;
    case STYPE_Kakuro:
        if(m_oKakuro.GetTileType(iX, iY) == TP_Guess)
            return m_oKakuro.GetSolution(m_oKakuro.GetGuessPiece(iX, iY));
        return 0;
        break;
    case STYPE_KenKen:
        return m_oKenKen.GetSolution(iX, iY);
        break;
    case STYPE_Greater:
        return m_oGreater.GetSolution(iX, iY);
        break;
    default:
        return 0;
        break;
    }
}

int CSEngine::GetGuess(int iX, int iY)
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        return m_oSudoku.GetGuess(iX, iY);
        break;
    case STYPE_Killer:
        return m_oKiller.GetGuess(iX, iY);
        break;
    case STYPE_Kakuro:
        if(m_oKakuro.GetTileType(iX, iY) == TP_Guess)
            return m_oKakuro.GetGuess(m_oKakuro.GetGuessPiece(iX, iY));
        return 0;
        break;
    case STYPE_KenKen:
        return m_oKenKen.GetGuess(iX, iY);
        break;
    case STYPE_Greater:
        return m_oGreater.GetGuess(iX, iY);
        break;
    default:
        return 0;
        break;
    }
}

DWORD CSEngine::GetOutline(int iX, int iY)
{
    if(m_eGameType == STYPE_Killer)
        return m_oKiller.GetOutline(iX, iY); //engine is backwards ... thank you Kev ...
    if(m_eGameType == STYPE_KenKen)
        return m_oKenKen.GetOutline(iX, iY);

    return KOUTLINE_None;
}

int CSEngine::GetTotal(int iX, int iY)
{
    if(m_eGameType == STYPE_Killer)
        return m_oKiller.GetTotal(iX, iY);

    if(m_eGameType == STYPE_KenKen)
        return m_oKenKen.GetTotal(iX, iY);

    return -1;
}

int CSEngine::GetKenKenAnswer(int iX, int iY)
{
    if(m_eGameType == STYPE_KenKen)
        return m_oKenKen.GetMathAnswer(iX, iY);
    else 
        return -1;
}

EnumOperator CSEngine::GetKenKenOp(int iX, int iY)
{
    if(m_eGameType == STYPE_KenKen)
        return m_oKenKen.GetOp(iX, iY);
    else 
        return OP_None;
}

int CSEngine::GetSamuIndex(int iX, int iY)
{
    if(iX < 0 || iY < 0 || iX >= 9 || iY >= 9)
        return -1;
    if(m_eGameType == STYPE_Killer)
        return m_oKiller.GetSamuIndex(iX, iY);
    if(m_eGameType == STYPE_KenKen)
        return m_oKenKen.GetSamuIndex(iX, iY);

    return -1;
}

int CSEngine::GetSamuValue(int iX, int iY)
{
    if(iX < 0 || iY < 0 || iX >= 9 || iY >= 9)
        return -1;
    if(m_eGameType == STYPE_Killer)
        return m_oKiller.GetSamuValue(iX, iY);
    if(m_eGameType == STYPE_KenKen)
        return m_oKenKen.GetSamuValue(iX, iY);

    return -1;
}

int CSEngine::GetHorzTotal(int iX, int iY)
{
    if(m_eGameType != STYPE_Kakuro)
        return 0;
    if(m_oKakuro.GetTileType(iX, iY) != TP_Kakuro)
        return 0;
    return m_oKakuro.GetHorzTotal(m_oKakuro.GetKakuroPiece(iX, iY));
}

int CSEngine::GetVertTotal(int iX, int iY)
{
    if(m_eGameType != STYPE_Kakuro)
        return 0;
    if(m_oKakuro.GetTileType(iX, iY) != TP_Kakuro)
        return 0;
    return m_oKakuro.GetVertTotal(m_oKakuro.GetKakuroPiece(iX, iY));
}

BOOL CSEngine::IsBlank(int iX, int iY)
{
    if(m_eGameType != STYPE_Kakuro)
        return FALSE;
    if(m_oKakuro.GetTileType(iX, iY) == TP_Blank)
        return TRUE;
    return FALSE;
}

BOOL CSEngine::IsKakuro(int iX, int iY)
{
    if(m_eGameType != STYPE_Kakuro)
        return FALSE;
    if(m_oKakuro.GetTileType(iX, iY) == TP_Kakuro)
        return TRUE;
    return FALSE;
}

BOOL CSEngine::SetBoardSize(EnumSize eSize)
{
    if(m_eGameType != STYPE_Kakuro)
        return FALSE;

    m_oKakuro.SetSize(eSize);
    return TRUE;
}

BOOL CSEngine::SetDifficulty(EnumDifficulty eDiff)
{

    m_oSudoku.SetDifficulty(eDiff);
    m_oKiller.SetDifficulty(eDiff);
    m_oKenKen.SetDifficulty(eDiff);
    m_oGreater.SetDifficulty(eDiff);

    switch(eDiff)
    {
    case DIFF_Medium:
        m_oKakuro.SetLevel(LVL_Medium);
    	break;
    case DIFF_Hard:
    case DIFF_VeryHard:
    case DIFF_Hardcore:
        m_oKakuro.SetLevel(LVL_Hard);
        break;
    default:
        m_oKakuro.SetLevel(LVL_Easy);
        break;
    }

    m_eDiff = eDiff;
    return TRUE;
}

BOOL CSEngine::Solve()
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        m_oSudoku.SolveGame();
        break;
    case STYPE_Killer:
        m_oKiller.SolveGame();
        break;
    case STYPE_Kakuro:
        m_oKakuro.SolveGame();
        break;
    case STYPE_KenKen:
        m_oKenKen.SolveGame();
        break;
    case STYPE_Greater:
        m_oGreater.SolveGame();
        break;
    }
    return TRUE;
}

BOOL CSEngine::Hint()
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        return m_oSudoku.Hint();
        break;
    case STYPE_Killer:
        return m_oKiller.Hint();
        break;
    case STYPE_Kakuro:
        return m_oKakuro.Hint();
        break;
    case STYPE_KenKen:
        return m_oKenKen.Hint();
        break;
    case STYPE_Greater:
        return m_oGreater.Hint();
        break;
    }
    return FALSE;
}

BOOL CSEngine::Undo()
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        return m_oSudoku.Undo();
        break;
    case STYPE_Killer:
        return m_oKiller.Undo();
        break;
    case STYPE_Kakuro:
        return m_oKakuro.Undo();
        break;
    case STYPE_KenKen:
        return m_oKenKen.Undo();
        break;
    case STYPE_Greater:
        return m_oGreater.Undo();
        break;
    }
    return FALSE;
}

int CSEngine::GetPencilMarks(int iX, int iY)
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        return m_oSudoku.GetPencilMarks(iX, iY);
        break;
    case STYPE_Killer:
        return m_oKiller.GetPencilMarks(iX, iY);
        break;
    case STYPE_Kakuro:
        return m_oKakuro.GetPencil(m_oKakuro.GetGuessPiece(iX, iY));
        break;
    case STYPE_KenKen:
        return m_oKenKen.GetPencilMarks(iX, iY);
        break;
    case STYPE_Greater:
        return m_oGreater.GetPencilMarks(iX, iY);
        break;
    }
    return 0;
}

void CSEngine::ClearPencilMarks()
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        m_oSudoku.ClearAllPencilMarks();
        break;
    case STYPE_Killer:
        m_oKiller.ClearAllPencilMarks();
        break;
    case STYPE_Kakuro:
        m_oKakuro.ClearAllPencilMarks();
        break;
    case STYPE_KenKen:
        m_oKenKen.ClearAllPencilMarks();
        break;
    case STYPE_Greater:
        m_oGreater.ClearAllPencilMarks();
        break;
    }
}

BOOL CSEngine::GetAutoPencilMarks()
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        return m_oSudoku.GetAutoPencil();
        break;
    case STYPE_Killer:
        return m_oKiller.GetAutoPencil();
        break;
    case STYPE_Kakuro:
        return m_oKakuro.GetAutoPencil();
        break;
    case STYPE_KenKen:
        return m_oKenKen.GetAutoPencil();
        break;
    case STYPE_Greater:
        return m_oGreater.GetAutoPencil();
        break;
    }
    return FALSE;
}

BOOL CSEngine::GetAutoRemovePencil()
{
    //they're all set so this is fine
    return m_oSudoku.GetAutoRemovePencil();
}

void CSEngine::SetAutoPencilMarks(BOOL bAuto)
{
    m_oSudoku.SetAutoPencilMarks(bAuto);
    m_oKiller.SetAutoPencilMarks(bAuto);
    m_oKakuro.SetAutoPencilMarks(bAuto);
    m_oKenKen.SetAutoPencilMarks(bAuto);
    m_oGreater.SetAutoPencilMarks(bAuto);
}

void CSEngine::SetAutoRemovePencil(BOOL bAuto)
{
    m_oSudoku.SetAutoRemovePencilMarks(bAuto);
    m_oKiller.SetAutoRemovePencilMarks(bAuto);
    m_oKakuro.SetAutoRemovePencilMarks(bAuto);
    m_oKenKen.SetAutoRemovePencilMarks(bAuto);
    m_oGreater.SetAutoRemovePencilMarks(bAuto);
}

void CSEngine::SetShowLocked(BOOL bShowLocked)
{
   m_oSudoku.SetLocked(bShowLocked);
   m_oKiller.SetLocked(bShowLocked);
   m_oKenKen.SetLocked(bShowLocked);
   m_oGreater.SetLocked(bShowLocked);
}

void CSEngine::SetShowErrors(BOOL bShow)
{
    m_oSudoku.SetShowErrors(bShow);
    m_oKiller.SetShowErrors(bShow);
    m_oKakuro.SetShowErrors(bShow);
    m_oKenKen.SetShowErrors(bShow);
    m_oGreater.SetShowErrors(bShow);
}

BOOL CSEngine::DrawTotal(int iX, int iY)
{
    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        break;
    case STYPE_Killer:
        return m_oKiller.DrawTotal(iX, iY);    
        break;
    case STYPE_Kakuro:
        break;
    case STYPE_KenKen:
        return m_oKenKen.DrawTotal(iX, iY);
        break;
    case STYPE_Greater:
        break;
    }
    return FALSE;
}

BOOL CSEngine::IsPencilMarkValid(int iValue, int iX, int iY)
{
    if(IsGiven(iX, iY) ||
        IsLocked(iX, iY) ||
        IsKakuro(iX, iY) || 
        IsBlank(iX, iY) )
        return FALSE;

    if(m_bShowHints == FALSE)
        return TRUE;//all good then

    switch(m_eGameType)
    {
    case STYPE_Sudoku:
        return m_oSudoku.IsPencilMarkValid(iX, iY, iValue);
        break;
    case STYPE_Killer:
        return m_oKiller.IsPencilMarkValid(iX, iY, iValue);
        break;
    case STYPE_Kakuro:
        return m_oKakuro.IsPencilMarkPossible(iX, iY, iValue);
        break;
    case STYPE_KenKen:
        return m_oKenKen.IsPencilMarkValid(iX, iY, iValue);
        break;
    case STYPE_Greater:
        return m_oGreater.IsPencilMarkValid(iX, iY, iValue);
        break;
    }
    return FALSE;
}

EnumGreaterDir CSEngine::GetGVertical(int iX, int iY)
{
    if(m_eGameType != STYPE_Greater)
        return GDIR_Unknown;
    return m_oGreater.GetGVertical(iX, iY);
}

EnumGreaterDir CSEngine::GetGHorizontal(int iX, int iY)
{
    if(m_eGameType != STYPE_Greater)
        return GDIR_Unknown;
    return m_oGreater.GetGHorizontal(iX, iY);
}