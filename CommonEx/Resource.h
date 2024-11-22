#pragma once

extern HINSTANCE g_hStringResource;
extern int		 g_iDaysLeft;

#ifdef WIN32_PLATFORM_WFSP
#include "resourcesp.h"

#elif WIN32_PLATFORM_PSPC
#include "resourceppc.h"
#elif SOLEUS
#include "resourcesol.h"
#include "fx.h"

#include "phonedlg.h"
#pragma comment( lib,"phoneDlg.lib")
#include "softkeysapi.h"
#pragma comment( lib, "phonecontrols.lib")
#pragma comment( lib, "fx.lib")
#undef IDYES
#define IDYES 0
#undef	MB_OK
#define MB_OK CMB_OK
#undef  MB_YESNO
#define MB_YESNO CMB_YESNO
#undef MessageBox
#define MessageBox(a,b,c,d) CommonMessageBox(a,b,c,d)

#ifndef SOLEUS
#define SOLEUS 1
#endif

#elif UNDER_CE //for pure CE ... ie my car GPS
#include "resourcece.h"

#else 
//hackin for desktop for now
#include "resourcepc.h"
#endif
