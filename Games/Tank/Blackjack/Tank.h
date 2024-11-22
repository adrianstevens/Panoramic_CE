#pragma once

#include "stdafx.h"

// A structure to store the co-ords for each point
struct tdpoint
{
    float x;
    float y;
    float z;
};

class CTank
{
public:
    CTank(void);
    ~CTank(void);

    void    Initialize(int object_type);

    float   GetX();
    float   GetZ();
    void    SetPosition(float x, float y, float z);
    void    Spin(float angle);
    void    Rotate(float angle);
    void    Move(float distance);
    void    Draw(HDC hdc, RECT& rcClient);

public:
    // Member variables

    int     ObjectType;
    int     HALFWIDTH; 
    int     HALFHEIGHT;

    float   position_x;
    float   position_y;
    float   position_z;

private:
    int     m_iArraySize;



    // Declare an array (tdshape) of points (tdpoint)
    tdpoint  *tdshape;       


};
