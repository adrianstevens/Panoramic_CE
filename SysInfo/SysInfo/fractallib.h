// fractal header
#ifndef _FRACTAL_
#define _FRACTAL_
#include <complex>
#include <ostream>

namespace fractals 
{
    // TEMPLATE CLASS fractal
    template<class _Ty>
    class fractal {
    public:
        fractal(unsigned short _W, unsigned short _H) :
          _Width(_W), _Height(_H), iterations(256)
          {_M = new unsigned char[_W * _H * 3];
          ::memset(_M, 0, sizeof(unsigned char) * _W * _H * 3); }
          fractal(const fractal& _X) : _M(0)
          {_Assign(_X); }
          ~fractal()
          {delete[] _M; }
          unsigned short width() const
          {return _Width; }
          unsigned short height() const
          {return _Height; }
          unsigned char *data() const
          {return _M; }
          unsigned iterations;
          void render(_Ty _X0, _Ty _Y0, _Ty _W, _Ty _H)
          {for (unsigned short _Y = 0; _Y < _Height; _Y++)
          for (unsigned short _X = 0; _X < _Width; _X++)
          {_Ty _Cx = _X0 + (_X * _W / _Width);
          _Ty _Cy = _Y0 - (_Y * _H / _Height);
          init(_Cx, _Cy);
          unsigned _N = (iterations <= 256) ?
iterations : 256;
          unsigned _I = 0;
          for (; _I < _N; _I++)
          {if (test())
          break;
          next(); }
          unsigned char _R = 0, _G = 0, _B = 0;
          color(_I, _R, _G, _B);
          unsigned char *_P = _M + (_Y *
              (_Width * 3) + (_X * 3));
          *_P++ = _R, *_P++ = _G, *_P = _B; }}
    protected:
        void _Assign(const fractal& _X)
        {delete[] _M;
        _Width = _X._Width;
        _Height = _X._Height;
        _M = new unsigned char[_Width * _Height * 3];
        ::memset(_M, 0, sizeof(unsigned char) * _Width * _Height * 3); }
        _Ty distance(const std::complex<_Ty>& _C)
        {       //seems to make it happy ... not sure
            return (float)::sqrt(_C.real() * _C.real() + 	_C.imag() * _C.imag()); 
        }
        virtual void init(_Ty _X, _Ty _Y) = 0;
        virtual bool test() = 0;
        virtual void color(unsigned _N, unsigned char& _R,
            unsigned char& _G, unsigned char& _B) = 0;
        virtual void next() = 0;
        unsigned short _Width, _Height;
        unsigned char *_M;
    };
    // TEMPLATE CLASS mandelbrot
    template<class _Ty>
    class mandelbrot : public fractal<_Ty> 
    {
    public:
        mandelbrot(unsigned short _W, unsigned short _H) :
          fractal<_Ty>(_W, _H) {}
          mandelbrot(const mandelbrot& _X) : fractal<_Ty>(_X)
          {_Z = _X._Z, _C = _X._C; }
          mandelbrot& operator=(const mandelbrot& _X)
          {if (this == &_X) return (*this);
          _Assign(_X);
          _Z = _X._Z, _C = _X._C;
          return (*this); }
    protected:
        virtual void init(_Ty _X, _Ty _Y)
        {_C = std::complex<_Ty>(_X, _Y);
        _Z = std::complex<_Ty>(0.0, 0.0); }
        virtual bool test()
        {return (distance(_Z) > 2.0); }
        virtual void color(unsigned _N, unsigned char& _R,
            unsigned char& _G, unsigned char& _B)
        {_R = _G = _B = _N; }
        virtual void next()
        {std::complex<_Ty> _T = _Z * _Z;			// keep borland happy
        _T += _C; _Z = _T; }
        std::complex<_Ty> _Z, _C;
    };
    // TEMPLATE CLASS julia
    template<class _Ty>
    class julia : public fractal<_Ty> 
    {   
    public:
        std::complex<_Ty> c;
        julia(unsigned short _W, unsigned short _H) :
        fractal<_Ty>(_W, _H), c(0.5, 0.5) {}
        julia(const julia& _X) : fractal<_Ty>(_X)
        {_Z = _X._Z, c = _X.c; }
        julia& operator=(const julia& _X)
        {if (this == &_X) return (*this);
        _Assign(_X);
        _Z = _X._Z, c = _X.c;
        return (*this); }
    protected:
        virtual void init(_Ty _X, _Ty _Y)
        {_Z = std::complex<_Ty>(_X, _Y); }
        virtual bool test()
        {return (distance(_Z) > 2.0); }
        virtual void color(unsigned _N, unsigned char& _R,
            unsigned char& _G, unsigned char& _B)
        {_R = _G = _B = _N; }
        virtual void next()
        {std::complex<_Ty> _T = _Z * _Z;			// keep borland happy
        _T += c; _Z = _T; }
        std::complex<_Ty> _Z;
    };
    // TEMPLATE CLASS cactus
    template<class _Ty>
    class cactus : public fractal<_Ty> {
    public:
        cactus(unsigned short _W, unsigned short _H) :
          fractal<_Ty>(_W, _H) {}
          cactus(const cactus& _X) : fractal<_Ty>(_X)
          {
              _Z = _X._Z, _Z0 = _X._Z0; }
          cactus& operator=(const cactus& _X)
          {if (this == &_X) return (*this);
          _Assign(_X);
          _Z = _X._Z, _Z0 = _X._Z0;
          return (*this); }
    protected:
        virtual void init(_Ty _X, _Ty _Y)
        {
            _Z = _Z0 = std::complex<_Ty>(_X, _Y); 
        }
        virtual bool test()
        {
            return (distance(_Z) > 2.0); 
        }
        
        virtual void color(unsigned _N, unsigned char& _R,
            unsigned char& _G, unsigned char& _B)
        {
            _R = _G = _B = _N; 
        }
        virtual void next()
        {
            std::complex<_Ty> _T = _Z * _Z * _Z;		// keep borland happy
            _T += (_Z0 - std::complex<_Ty>(1, 0)) * _Z;
            _Z = _T - _Z0; 
        }
        std::complex<_Ty> _Z, _Z0;
    };
    // TEMPLATE FUNCTION operator<<
    template<class _Ty>
    std::ostream& operator<<(std::ostream& _O, const fractal<_Ty>& _F)
    {
        unsigned short _W = _F.width(), _H = _F.height();

        _O.write("\0\0\2\0\0\0\0\0\0\0\0\0", sizeof(char) * 12);
        _O.write((const char *)&_W, sizeof(unsigned short));
        _O.write((const char *)&_H, sizeof(unsigned short));
        _O.write("\x18\x20", sizeof(char) * 2);
        
        unsigned char *_P = _F.data(), _C[3];
        
        for (unsigned _I = 0; _I < _W * _H * 3U; _I += 3)
        {
            _C[0] = _P[_I + 2], _C[1] = _P[_I + 1], _C[2] = _P[_I];
            _O.write((const char *)_C, 3 * sizeof(unsigned char));
        }
        return _O; 
    }

}	// namespace

#endif /* _FRACTAL_ */

/*
* http://home.tiscali.be/zoetrope
*/
