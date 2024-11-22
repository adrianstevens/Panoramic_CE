#pragma once

#include "fractallib.h"
#include <fstream>

#include "IssGDIEx.h"

using namespace fractals;

template<class _Ty>
class myfractal : public mandelbrot<_Ty> 
{
public:
    myfractal(unsigned short _W, unsigned short _H)
        : mandelbrot<_Ty>(_W, _H) {}
protected:
    virtual void color(unsigned _N, unsigned char& _R,
        unsigned char& _G, unsigned char& _B)
    {_R = _N, _G = _N * 2, _B = _N * 3; }
};

enum EnumFractalType
{
    FRAC_Normal,
    FRAC_MandelBrot,
    FRAC_SeaHorse,
    FRAC_Macro,
    FRAC_Cactus,
};

class CFractal
{
public:
    CFractal(void);
    ~CFractal(void);

    BOOL        Render(CIssGDIEx& gdi, EnumFractalType eType);


private:
    int         m_iWidth;//actual size
    int         m_iHeight;//actual size
    int         m_iWidthCalc;//accuracy
    int         m_iHeightCalc;//accuracy



};
