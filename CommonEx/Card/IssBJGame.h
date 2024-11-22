// IssBJGame.h: interface for the CIssBJGame class.
//
//////////////////////////////////////////////////////////////////////
#pragma once


#include "IssBJHand.h"
#include "IssCardSet.h"
#include "IssString.h"

#define WM_BJ_ENDGAME   WM_USER+300
#define WM_DEALER_FLIP  WM_USER+301
#define WM_SHUFFLE      WM_USER+302
#define WM_BJRESULTS    WM_USER+303


#define MAX_CHIPS_PER_STACK 15

#define BANK_DEFAULT 10000

enum EnumGameDifficulty
{
	BJDIFF_Easy,
	BJDIFF_Normal,
	BJDIFF_Expert,
    BJDIFF_Learn,
	BJDIFF_Custom,
};

enum EnumGameState
{
	GSTATE_Bet,
	GSTATE_Deal,
	GSTATE_PlayerFirstMove,
	GSTATE_PlayerMove,
	GSTATE_SplitMove,
	GSTATE_DealerMove,
	GSTATE_EndOfGame,
};

enum EnumGameResult
{
	GRESULT_BlackJack,
    GRESULT_Win,
	GRESULT_Draw,
	GRESULT_Surrender,
    GRESULT_Lose,
	GRESULT_NULL, //for the ol split hand
};

enum EnumCardCount
{
    COUNT_WizardAce5,
    COUNT_KO,
    COUNT_HiLow,
    COUNT_HiOpt1,
    COUNT_HiOpt2,
    COUNT_Zen,
    COUNT_OmegaII,
    COUNT_Count,
};

enum EnumNextMove
{   //small for my arrays
    NM_S,//stand
    NM_H,//hit
    NM_D,
    NM_DH,//double or hit
    NM_DS,//double or stand
    NM_Sp,//split
    NM_Su,//surrender ... otherwise hit
    NM_In,//insurance ... never without card counting
    NM_Nothing,
};

enum EnumCardCountView
{
    CC_On,
    CC_Off,
    CC_SemiHidden,
};

struct TypeOptions
{
    EnumCardCountView   eCardCountView;
    EnumCardCount       eCardCount;
    EnumGameDifficulty  eGameDifficulty;
    BOOL                bPlaySounds;
    DWORD               dwLang;
};


class CIssBJGame  
{
public:
	CIssBJGame();
	virtual ~CIssBJGame();

	void			Init(HWND hWnd){m_hWnd = hWnd;};
	void			Reset();

    EnumGameState	GetGameState(){return m_eGameState;};

	//Basically we make a function for every possible Button
	BOOL			AddBetAmount(int iAmount);
    BOOL            Add25ChipBet();
    BOOL            Add50ChipBet();
    BOOL            Add100ChipBet();
    BOOL            Add500ChipBet();

    BOOL            DealerCardFaceUp();

    int             Get25ChipCount(){return m_iNum25Chips;};
    int             Get50ChipCount(){return m_iNum50Chips;};
    int             Get100ChipCount(){return m_iNum100Chips;};


	BOOL			ResetBetAmount();
	BOOL			Deal();
	BOOL			PlayDealerHand();
	BOOL			CheckGameStatus();//ie...check the scores for blackjacks, busts, etc
	BOOL			EvaluateScores();
	BOOL			StartNewHand();
	BOOL			IsNewGame(){return m_bNewGame;};
    BOOL            HasGameStarted();


	//Get some strings for the interface
	void			GetBank(TCHAR* szBank);
	int				GetBankAmount(){return m_iBank;};
    int             GetPrevBankAmount(){return m_iPreviousBank;};
    int             GetHiBankAmount(){return m_iBankHi;};
	void			SetBankAmount(int iBank){m_iBank = iBank;};
	void			GetBet(TCHAR* szBet);
	int				GetBetAmount(){return m_iBetAmount;};
	void			SetBetAmount(int iBet);
	void			GetPlayerScore(TCHAR* szPlayerScore){if(m_bShowScores == FALSE)	m_oStr->Empty(szPlayerScore);else GetScore(szPlayerScore, &m_oPlayerHand);};
	void			GetDealerScore(TCHAR* szDealerScore);//{GetScore(szDealerScore, &m_oDealerHand);};
	void			GetSplitScore(TCHAR* szSplitScore){if(m_bShowScores == FALSE)	m_oStr->Empty(szSplitScore);else GetScore(szSplitScore, &m_oSplitHand);};
    int             GetCardCount();

    void            SetCountSystem(EnumCardCount eCount){m_eCountSystem = eCount;};
    EnumCardCount   GetCountSystem(){return m_eCountSystem;};
    void            SetCardCountView(EnumCardCountView eView){m_eCardCountView = eView;};
    EnumCardCountView GetCardCountView(){return m_eCardCountView;};

	//Get Pointers to the Hands to make drawing Easier
	CIssBJHand*		GetDealerHand(){return &m_oDealerHand;};
	CIssBJHand*		GetPlayerHand(){return &m_oPlayerHand;};
	CIssBJHand*		GetSplitHand(){return &m_oSplitHand;};

	BOOL			Hit();
	BOOL			Stand();
	BOOL			Split();
	BOOL			Insurance();
	BOOL			DoubleDown();//sideways card
	BOOL			Surrender(); //pussy
    BOOL            Bet();

	//Functions to check states of certain BlackJack Events
	BOOL			ShowInsuranceButton();
	BOOL			ShowSplitButton();
	BOOL			ShowDoubleButton();
	BOOL			ShowSurrenderButton();
    BOOL            ShowDealButton(){if(m_iBetAmount) return TRUE; return FALSE;};
	BOOL			ShowDealerCard(){return m_bShowDealerCard;};

    //bet buttons
    BOOL            Show25();
    BOOL            Show50();
    BOOL            Show100();
    BOOL            Show500();

	BOOL			ShowDealerScore();
	BOOL			ShowPlayerScore();
	BOOL			ShowSplitScore();
	BOOL			ShowResults();
    
	BOOL			DealAnotherCard();

    BOOL            DealerBusts();



	BOOL			SetDifficulty(EnumGameDifficulty eDiff);
	EnumGameDifficulty GetDifficulty();
		
	int				GetNumDealerCards(){return m_iNumDealerCardsDealt;};
	int				GetNumPlayerCards(){return m_iNumPlayerCardsDealt;};
	int				GetNumSplitCards(){return m_iNumSplitCardsDealt;};
	BOOL			IsSplitting(){return m_bSplit;};
    BOOL            IsDouble(){return m_bDoubleDown;};
    BOOL            IsInsurance(){return m_bInsurance;};//if its been selected

    BOOL            IsFinishedDealing();

    
	
	//add functions for advanced options if time permits
		
    EnumGameResult  GetPlayerResult(){return m_eResultPlayer;};
    EnumGameResult  GetSplitResult(){return m_eResultSplit;};

    EnumNextMove    GetNextMove();

private:
	void			GetScore(TCHAR* szScore, CIssBJHand* oHand);
	
	void			NewHandReset();

    BOOL            AddCardToCount(CIssCard* oCard);
    int             GetCardCountValue(CIssCard* oCard);


    void            MatchChipsToBet();

    

    

public://saving time for now
	//Options
	BOOL			m_bBurnFirstCard;//toss the first card away on a shuffle
	BOOL			m_bDealerHitsOnSoft17;
	BOOL			m_bBlackJack2to1; //normal is 3 to 2
	BOOL			m_b5CardsScores21;
	BOOL			m_bAllowSurrender;
	BOOL			m_bShowScores;
	BOOL			m_bShowDealerCard;
	BOOL			m_bPlayEndSound;



	BOOL			m_bDoubleDown;
	BOOL			m_bCalcDoubleDown;
	BOOL			m_bNewGame;

private:
	CIssString*		m_oStr;

	CIssBJHand		m_oDealerHand;
	CIssBJHand		m_oPlayerHand;
	CIssBJHand		m_oSplitHand;
	CIssCardSet		*m_oCardSet;

	EnumGameState	m_eGameState;
    EnumGameResult  m_eResultPlayer;
    EnumGameResult  m_eResultSplit;
    EnumCardCount   m_eCountSystem;
    EnumCardCountView m_eCardCountView;

	HWND			m_hWnd;	//dlg window handle



	int				m_iBetAmount;
	int				m_iBank;
    int             m_iPreviousBank;
	int				m_iLow, m_iHi; //temps but used ALL of the time
	int				m_iNumDecks;
    int             m_iBankHi; //we'll keep track for the hiscore table
	
	BOOL			m_bSplit;
	BOOL			m_bSurrender;
	BOOL			m_bInsurance;

	int				m_iNumDealerCardsDealt;
	int				m_iNumPlayerCardsDealt;
	int				m_iNumSplitCardsDealt;

    int             m_iNum25Chips;
    int             m_iNum50Chips;
    int             m_iNum100Chips;

    int             m_iCardCount; //http://en.wikipedia.org/wiki/Card_counting

	EnumGameDifficulty	m_eGameDiff;
};

