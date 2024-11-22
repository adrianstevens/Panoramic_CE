#include "Fractal.h"
#include <stdlib.h>


CFractal::CFractal(void)
{
    m_iWidth    = GetSystemMetrics(SM_CXSCREEN);
    m_iHeight   = GetSystemMetrics(SM_CYSCREEN);

    m_iWidthCalc = 32;
    m_iHeightCalc = 32;
}

CFractal::~CFractal(void)
{
}

BOOL CFractal::Render(CIssGDIEx& gdi, EnumFractalType eType)
{
    switch(eType)
    {
    case FRAC_Normal:
        {
            // myfractal
            myfractal<double> m(m_iWidth, m_iHeight);
            m.render(-1.250, 0.048, 0.005, 0.005);

            //std::ofstream os0("myfractal.tga", std::ios_base::binary);
            //os0 << m;
        }
        break;
    case FRAC_MandelBrot:
        {
            // mandelbrot set
            mandelbrot<double> m0(m_iWidth, m_iHeight);
            mandelbrot<double> m1 = m0;			// test copy constructor
            mandelbrot<double> m(m_iWidthCalc, m_iHeightCalc);
            m = m1;								// test assignment operator

            m.render(-1.5, 1.5, 3, 3);			// 400x400

            //std::ofstream os("mandelbrot.tga", std::ios_base::binary);
           // os << m;
        }
        break;
    case FRAC_SeaHorse:
        {
            // sea horse valley
            mandelbrot<double> m(m_iWidth, m_iHeight);
            m.render(-1.250, 0.048, 0.005, 0.005);

            //std::ofstream os("seahorsevalley.tga", std::ios_base::binary);
            //os << m;

            unsigned short _W = m.width(), _H = m.height();

            unsigned char *_P = m.data(), _C[3];

            unsigned iTemp = 0;

            for(int i = 0; i < m.width(); i++)
            {
                for(int j = 0; j < m.height(); j++)
                {
                    iTemp = i * j * 3;
                    _C[0] = _P[iTemp + 2], _C[1] = _P[iTemp + 1], _C[2] = _P[iTemp];
                    gdi.SetPixelColor(i, j, RGB(_C[2], _C[1], _C[0]));
                }
            }

         /*   for (unsigned _I = 0; _I < _W * _H * 3U; _I += 3)
            {
                _C[0] = _P[_I + 2], _C[1] = _P[_I + 1], _C[2] = _P[_I];
                //m.write((const char *)_C, 3 * sizeof(unsigned char));
                
            }*/


        }
        break;
    case FRAC_Macro:
        {
            // san marco fractal
            julia<float> j0(m_iWidth, m_iHeight);
            julia<float> j1 = j0;				// test copy constructor
            julia<float> j(m_iWidthCalc, m_iHeight);
            j = j1;								// test assignment operator

            j.c = std::complex<float>(-3.0f / 4.0f, 0);
            j.render(-1.5, 1.5, 3, 3);			// 400x400

            //std::ofstream os("sanmarco.tga", std::ios_base::binary);
            //os << j;
        }
        break;
    case FRAC_Cactus:
        {
            // cactus fractal
            cactus<float> c0(m_iWidth, m_iHeight);
            cactus<float> c1 = c0;				// test copy constructor
            cactus<float> c(m_iWidthCalc, m_iHeight);
            c = c1;								// test assignment operator

            c.render(-1.5, 1.5, 3, 3);			

            //std::ofstream os("cactus.tga", std::ios_base::binary);
            //os << c;
        }
        break;
    }

    if(gdi == NULL)
        return TRUE;





    return TRUE;
}










