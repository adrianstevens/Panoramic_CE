class CIssCard
{
	private var m_iSuit;
	private var m_iRank; //rank being the value of 1 to 13....or Ace to King
	
	public function GetCardValue():Number
	{
		if(m_iRank < 10)
			return m_iRank;
		else 
			return 10;
	}
	
	public function GetCardRank():Number {return m_iRank;};
		
	public function GetCardSuit():Number {return m_iSuit;};
	
	public function SetCardRank(iRank:Number):Boolean
	{
		if(iRank < 14 && iRank > 0)
		{
			m_iRank = iRank;
			return true;
		}
		else
		{
			return false;
		}
	}
	
	public function SetCardSuit(iSuit:Number):Boolean
	{	
		if(iSuit < 4 && iSuit > 0)
		{
			m_iSuit = iSuit;
			return true;
		}
		else
		{
			return false;
		}
	}


}
