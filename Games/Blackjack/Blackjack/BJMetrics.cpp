#include "BJMetrics.h"

CBJMetrics::CBJMetrics(void)
{
}

CBJMetrics::~CBJMetrics(void)
{
}

int CBJMetrics::GetTextHeight()
{
#ifdef WIN32_PLATFORM_WFSP
    return GetSystemMetrics(SM_CXICON)*2/5;
#else
    return GetSystemMetrics(SM_CXICON)/2;
#endif
}

int CBJMetrics::GetCountTextHeight()
{
#ifdef WIN32_PLATFORM_WFSP
    return GetSystemMetrics(SM_CXICON)/2;
#else
    return GetSystemMetrics(SM_CXICON)*2/3;
#endif
}

int CBJMetrics::GetTextIndent()
{
#ifdef WIN32_PLATFORM_WFSP
    return GetSystemMetrics(SM_CXICON)/8;
#else
    return GetSystemMetrics(SM_CXICON)/7;
#endif


}

int CBJMetrics::GetCardHeight()
{
        //90 * 3 /4 = 67
    if(GetSystemMetrics(SM_CXSCREEN) > GetSystemMetrics(SM_CYSCREEN))
        return GetSystemMetrics(SM_CYSCREEN)/4;
    else
        return GetSystemMetrics(SM_CYSCREEN)/5;
}


int CBJMetrics::GetCountSize()
{
#ifdef WIN32_PLATFORM_WFSP
    return GetSystemMetrics(SM_CXICON);
#else
    return GetSystemMetrics(SM_CXICON)*3/2;
#endif
}

int CBJMetrics::GetCountWidth()
{
#ifdef WIN32_PLATFORM_WFSP
    return GetSystemMetrics(SM_CXICON)*4/3;
#else
    return GetSystemMetrics(SM_CXICON)*5/3;
#endif
}

int CBJMetrics::GetChipSize()
{
#ifdef WIN32_PLATFORM_WFSP
    return GetSystemMetrics(SM_CXICON)*3/4;
#else
    return GetSystemMetrics(SM_CXICON);
#endif
}
