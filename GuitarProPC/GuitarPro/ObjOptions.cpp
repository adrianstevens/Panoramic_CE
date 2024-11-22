#include "ObjOptions.h"
#include "IssRegistry.h"

CObjOptions::CObjOptions(void)
:m_hInst(NULL)
{
    SetDefaults();
#ifdef DEBUG
    m_bLeftHanded = TRUE;
#endif
}

CObjOptions::~CObjOptions(void)
{
}

void CObjOptions::SetDefaults()
{
    m_bLeftHanded   = (FALSE);
    m_eFretmarkers  = (FMARKERS_Numbered);

    m_eMetroSound   = METSND_Standard;
    m_iMetroBPM     = 120;
    m_iMetroMeter   = 4;

    m_eSampleFreq   = FQ_SAMP_Med;
    m_iTrigger      = 3;

    m_eStrum        = STRUM_Slow;
}

CMyGenerator* CObjOptions::GetGen()
{
    return &m_oChords;
}

void CObjOptions::SetInstrument(EnumInstrument eInst)
{
    m_oChords.SetInstrument(eInst, FALSE);
    m_oChords.SetTuning(GetGen()->GetTuning(), TRUE);
    
}

