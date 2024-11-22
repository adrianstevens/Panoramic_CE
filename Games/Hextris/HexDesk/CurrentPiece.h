#define NUMBER_OF_PIECES	10

#include "stdafx.h"

enum EnumHexPeice
{
	HEX_Wall,
	HEX_GamePiece,
	HEX_Erased,
	HEX_Blank, 
};

struct GameHexagon
{	
	EnumHexPeice	ePiece;
	int				iColorIndex;
};


class CurrentPiece
{
public:
	CurrentPiece();
	void		Reset(int iPiece = -1);
	void		Rotate();//we'll want to rotate both directions eventually but good enough for now
	void		MovePieceDown(){m_ptLocation.y--;};
	void		MovePieceUp(){m_ptLocation.y++;};
	void		MovePieceRight(){m_ptLocation.x++;};
	void		MovePieceLeft(){m_ptLocation.x--;};
	
	void		SetPosition(int iX, int iY){m_ptLocation.x = iX; m_ptLocation.y = iY;};
	POINT*		GetLocation(){return &m_ptLocation;};
	GameHexagon* GetHex(int iX, int iY){return &data[iX][iY];};

	int			GetPieceType(){return m_iPieceType;};
	
	BOOL		CopyPiece(CurrentPiece* oPiece);

private:
	GameHexagon		data[5][5];
	COLORREF		m_crColor;

	POINT			m_ptLocation;

	int				m_iPieceType;
};
