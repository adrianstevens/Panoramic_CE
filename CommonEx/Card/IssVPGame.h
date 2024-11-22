#pragma once

#include "IssVPHand.h"
#include "IssCardSet.h"
#include "IssString.h"
#include "IssPokerHandEval.h"
#include "IssPokerScoreCalc.h"

#define MAX_MUTLIPLAY 5 //why not?

#define MAX_BANK 9999999

enum EnumVPGameState
{
	VPSTATE_Bet,
	VPSTATE_Deal,
	VPSTATE_Hold,
	VPSTATE_Deal2,
	VPSTATE_EndGame,
};

enum EnumMultiPlay
{
    MPLAY_Off,
    MPLAY_3,
    MPLAY_5,
};

enum EnumBetAmounts
{
    BA_1,
    BA_5,
    BA_10,
    BA_20,
    BA_50,
    BA_100,
 //   BA_500,
 //   BA_1000,
    BA_Count,
};

class CIssVPGame
{
public:
	CIssVPGame(void);
public:
	~CIssVPGame(void);

	// betting actions
    EnumBetAmounts      GetBetAmount();
    void                SetBetAmount(EnumBetAmounts eBet);
	BOOL				Deal();

    BOOL                OnBetOne();
    BOOL                OnBetMax();
    BOOL                OnDeal();
    BOOL                OnDouble();//for stud poker
    BOOL                IsDouble(){if(m_eGameType == PTYPE_StudVP && m_bDouble) return TRUE; return FALSE;};

	BOOL				ClearBet();
	BOOL				AddBet(int iBet);

    BOOL                IsGameInProgress();

	// holding actions
	BOOL				Hold(int iButton);
    BOOL                IsHeld(int iCard);

	// dealing actions
	BOOL				DealNextCard();

	CIssCard*			GetCard(int iIndex, int iSet = 0);
	BOOL				GetHold(int iIndex){return m_bHold[iIndex];};
	void				Reset();
	void				NewHand();
	EnumVPGameState		GetGameState(){return m_eGameState;};
	int					GetNumDecks(){return m_iNumDecks;};
	BOOL				SetNumDecks(int iNumDecks){m_iNumDecks = iNumDecks;m_oCardSet[0].ChangeNumDecks(iNumDecks);return TRUE;};
	int					GetBank(TCHAR* szBank = NULL);
	int					GetPreviousBank(){return m_iOldBank;};
	int					GetBet(TCHAR* szBet = NULL);
	BOOL				SetBank(int iBank){m_iBank = iBank;return TRUE;};
	BOOL				SetBet(int iBet){m_iBetAmount = iBet;return TRUE;};
    BOOL                IsWinningResult(EnumPokerResult eResult);

    BOOL                IsBonusAval();

	EnumPokerResult		EvaluateHand(int iHand = 0);
    EnumMultiPlay       GetMultiplay(){return m_eMultiplay;};
    BOOL                SetMultiplay(EnumMultiPlay eMulti);

    CIssPokerScoreCalc* GetScoreCalc(){return &m_oScoreCalc;}; //its useful for other stuff

    void                SetNumHands(int iMulti);
    int                 GetNumHands(){return m_iNumMultiplay;};
    BOOL                IsWinner(int iCard, int iHand = 0);
    int                 GetPayoutCount();//number of payout types
    int                 GetAmountWon(){return m_iAmountWon;};

    BOOL                IsMaxBet(){if(m_iNumCredits == 5) return TRUE; return FALSE;};
    BOOL                IsBroke();

    void                ResetStats();

    EnumPokerType		GetGameType(){return m_eGameType;};
    void                SetGameType(EnumPokerType eType);
private:

private:
	virtual void		UpdateBank();
    BOOL                DealStud();
	BOOL				DealBonusGame();

    void                SetMultiplayHands();    //we'll initialize the hands and decks after the user has held their chosen cards
    void                SetMultiplayStud();    //we'll initialize the hands and decks after the user has held their chosen cards
	int					GetAmount(EnumBetAmounts eAmt);

    void                CheckBet(BOOL bUpdateBank = TRUE);
    void                AutoHold();

    void                UpdateStats();
    

public:
	EnumPokerResult     m_eResult[MAX_MUTLIPLAY];   // we'll save it so we don't have to calc every time
    EnumPokerResult     m_eBestResult;
    EnumBetAmounts  m_eBetAmounts;

    BOOL                m_bBonusGame;
	BOOL				m_bDouble;					// if we doubled the stud bet
    BOOL                m_bAutoHold;
    BOOL                m_bBankBroken;


    //stats
    int                 m_iBiggestWin;
    int                 m_iHighestBank;
    double              m_dbPayout[6]; //for each game
    int                 m_iNumHandsPlayed[6];

protected:
    EnumPokerType		m_eGameType;	// What type of Video Poker we're playing
	EnumMultiPlay       m_eMultiplay;
	CIssString*			m_oStr;						// string class
	CIssPokerHandEval	m_oHandEval;				// Hand evaluation class
	CIssPokerScoreCalc	m_oScoreCalc;				// Get the Scores
	CIssCardSet			m_oCardSet[MAX_MUTLIPLAY];	// our deck of cards
	CIssVPHand			m_oHand[MAX_MUTLIPLAY];	    // our current hand
	EnumVPGameState		m_eGameState;				// game state machine

    

    BOOL				m_bHold[DEFAULT_NUM_CARDS];	// which card is being held
	BOOL				m_bShowing[MAX_MUTLIPLAY][DEFAULT_NUM_CARDS];// which card is currently being shown
	    
    int             m_iAmountWon;
    int             m_iNumCredits;
    int					m_iBetAmount;				// bet amount you are going to lose
	int					m_iBank;					// the bank cha ching
	int					m_iOldBank;					// bank before the last payoff
	int					m_iNumDecks;				// number of decks to use
    int                 m_iNumMultiplay;
};
