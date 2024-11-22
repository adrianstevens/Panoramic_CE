#include "CurrentPiece.h"


CurrentPiece::CurrentPiece()
{
	Reset();
	m_crColor = 0;
	m_iPieceType = 0;
	m_bHasOrb = FALSE;
}

void CurrentPiece::Reset(int iPiece, BOOL bBonus)
{	
	m_bHasOrb = bBonus;

	static byte piece[5*5*10]={0,0,0,0,0,		//Line
						0,0,0,0,0,
						1,1,1,1,0,
						0,0,0,0,0,
						0,0,0,0,0,

						0,0,0,0,0,		//Square
						0,1,1,0,0,
						0,1,1,0,0,
						0,0,0,0,0,
						0,0,0,0,0,

						0,0,0,0,0,		//'L' 1
						0,0,0,1,0,
						1,1,1,0,0,
						0,0,0,0,0,
						0,0,0,0,0,

						0,0,0,0,0,		//'L' 2
						0,1,0,0,0,
						0,1,1,1,0,
						0,0,0,0,0,
						0,0,0,0,0,

						0,0,0,0,0,		//Club 1
						0,0,1,0,0,
						0,1,1,1,0,
						0,0,0,0,0,
						0,0,0,0,0,

						0,0,0,0,0,		//Club 2
						0,0,0,1,0,
						0,1,1,1,0,
						0,0,0,0,0,
						0,0,0,0,0,

						0,0,0,0,0,		//Zig-zag 1
						0,1,1,0,0,
						0,0,1,1,0,
						0,0,0,0,0,
						0,0,0,0,0,

						0,0,0,0,0,		//Zig-zag 2
						0,0,1,1,0,
						1,1,0,0,0,
						0,0,0,0,0,
						0,0,0,0,0,

						0,0,0,0,0,		//'U'
						0,1,0,1,0,
						0,1,1,0,0,
						0,0,0,0,0,
						0,0,0,0,0,

								//Radiation
						0,0,0,0,0,
						0,0,0,1,0,
						0,1,1,0,0,
						0,0,1,0,0,
						0,0,0,0,0};

	if(iPiece == -1)
	{
		static int iPrev  = -1;
		static int iPrev2 = -1;

		m_iPieceType=rand()%10;

		while(iPrev == iPrev2 &&
			iPrev2 == m_iPieceType)
			m_iPieceType=rand()%10;

		iPrev  = iPrev2;
		iPrev2 = m_iPieceType;
	}
	else
		m_iPieceType = iPiece;

	for(int y=0; y < 5; ++y)
	{
		for(int x=0; x < 5; ++x)
		{
			if(piece[x+y*5+m_iPieceType*25] == 1)
			{
				if(bBonus)
				{
					data[x][y].ePiece = HEX_BonusOrb;
					bBonus = FALSE;
				}
				else
					data[x][y].ePiece = HEX_GamePiece;
			}
			else
			{
				data[x][y].ePiece = HEX_Blank;
			}
		}
	}
}

void CurrentPiece::Rotate()
{
	//Rotation centered on (2,2)

	GameHexagon temp;

	temp=data[1][3];			//Inner loop
	data[1][3]=data[1][2];
	data[1][2]=data[2][1];
	data[2][1]=data[3][1];
	data[3][1]=data[3][2];
	data[3][2]=data[2][3];
	data[2][3]=temp;

	temp=data[0][4];			//One outer loop
	data[0][4]=data[0][2];
	data[0][2]=data[2][0];
	data[2][0]=data[4][0];
	data[4][0]=data[4][2];
	data[4][2]=data[2][4];
	data[2][4]=temp;

	temp=data[1][4];			//The other one
	data[1][4]=data[0][3];		
	data[0][3]=data[1][1];
	data[1][1]=data[3][0];
	data[3][0]=data[4][1];
	data[4][1]=data[3][3];
	data[3][3]=temp;

	int k = 0;

}

BOOL CurrentPiece::CopyPiece(CurrentPiece* oPiece)
{
	if(oPiece == NULL)
		return FALSE;

	for(int i = 0; i < 5; i++)
	{
		for(int j = 0; j < 5; j++)
		{
			data[i][j] = *oPiece->GetHex(i,j);
			
		}
	}
	m_ptLocation = *oPiece->GetLocation();
	m_iPieceType = oPiece->GetPieceType();
	
	return TRUE;
}