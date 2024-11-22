class CIssCardSet
{
	private var m_oCards:Array;
	private var m_bUseJokers:Boolean;
	private var m_iCardIndex:Number;
	private var m_iNumCards:Number;
	private var m_iNumDecks:Number;
	
	public function CIssCardSet()
	{
		
	}
	
	public function Initialize():Void
	{
		m_bUseJokers = false;
		Reset();
	}
	
	private function Reset():Void
	{
		m_iCardIndex		= 0;
		m_iNumCards			= -1;
		m_iNumDecks			= 0;
	}
	
	public function Shuffle():Void
	{
		m_iCardIndex	 = 0; //need this....
	
		var oTempCard;
		
		var 	iNumSwaps = m_iNumDecks * 100;//100 is the number of shuffles per deck in use
		var 	iSwap1;
		var		iSwap2;
		
		for (var i = 0; i < iNumSwaps; i++)
		{
			iSwap1 = random(m_iNumCards);
			iSwap2 = random(m_iNumCards);
	
			//simple swap
			oTempCard.m_iRank			= m_oCards[iSwap1].m_iRank;
			oTempCard.m_iSuit			= m_oCards[iSwap1].m_iSuit;
	
			m_oCards[iSwap1].m_iRank	= m_oCards[iSwap2].m_iRank;
			m_oCards[iSwap1].m_iSuit	= m_oCards[iSwap2].m_iSuit;
	
			m_oCards[iSwap2].m_iRank	= oTempCard.m_iRank;
			m_oCards[iSwap2].m_iSuit	= oTempCard.m_iSuit;
		}
	}
	
	public function ChangeNumDecks(iDecks:Number):Void
	{
		if(iDecks > 0 && iDecks < 16)
			m_iNumDecks = iDecks;
		else
			return;
			
		//set the number of cards
		m_iNumCards = m_iNumDecks*52;
		if(m_bUseJokers == true)
			m_iNumCards += m_iNumDecks*2;
			
		//Set the Array Size
		m_oCards = new Array;
		for(var i = 0; i < m_iNumCards; i++)
			m_oCards[i] = new CIssCard;
			
		//int the decks and shuffle
		InitializeDeck();
		Shuffle();
	}
	
	public function GetNumDecks():Number
	{
		return m_iNumDecks;
	}
	
	public function GetNextCard():CIssCard
	{
		if(m_iCardIndex >= m_iNumCards + 1)
			return;
		m_iCardIndex++;
		return m_oCards[m_iCardIndex-1];
	}
	
	
	public function GetCardsLeft():Number
	{
		return m_iNumCards - m_iCardIndex;
	}
	
	private function InitializeDeck():Void
	{
		var iCardsInDeck = 52;
		if(m_bUseJokers == true)
			iCardsInDeck += 2;
			
		if(m_iNumDecks < 1)
			m_iNumDecks = 1;
			
		for(var i = 0; i < m_iNumDecks; i++)
		{
			for(var j = 0; j < 4; j++) //suits
			{
				for(var k = 0; k < 13; k++)
				{
					m_oCards[k+j*13+i*iCardsInDeck].m_iSuit = j;
					m_oCards[k+j*13+i*iCardsInDeck].m_iRank = k+1;
				}
			}
			m_oCards[(i+1)*iCardsInDeck - 2].m_iRank = 0;
			m_oCards[(i+1)*iCardsInDeck - 1].m_iRank = 0;//zero for Joker G
		}
	}
	
	
	
		
}