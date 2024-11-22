class CIssVPHand
{
	static private var NUMBER_OF_CARDS:Number = 5;
	private var m_oCards:Array;
	
	public function CIssVPHand()
	{
		m_oCards = new Array;
	}
	
	
	public function AddCard(oCard:CIssCard):Boolean
	{
		var i:Number = 0;
		
		if(m_oCards.length < NUMBER_OF_CARDS)
		{
			m_oCards.push(oCard);
			return true;
		}
		
		return false;
	}
	
	
	public function GetCard(iCardIndex:Number):CIssCard
	{
		if(iCardIndex < 0)
			return null;
		if(iCardIndex > NUMBER_OF_CARDS)
			return null;
		if(iCardIndex > m_oCards.length)
			return null;
			
		return m_oCards[iCardIndex];
		
	}
	
	public function RemoveCard(iCardIndex:Number):Boolean
	{
		if(iCardIndex < 0)
			return false;
		if(iCardIndex > NUMBER_OF_CARDS)
			return false;
		if(iCardIndex > m_oCards.length - 1)
			return false;
			
		for(var i:Number = iCardIndex; i < m_oCards.length - 1; i++)
		{
			m_oCards[i] = m_oCards[i+1];
		}
		
		m_oCards.pop();
	}
	
	public function ClearHand()
	{
		m_oCards = new Array;
	}
	
	
	
	
		
	
	
	
	
	
	
}

