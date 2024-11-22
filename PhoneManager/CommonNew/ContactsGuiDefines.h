#pragma once


#define WM_CHANGE_Gui		            WM_USER + 100
#define WM_FadeIn			            WM_USER + 200 
#define WM_PicLoader                    WM_USER + 300
#define WM_AniChange					WM_USER + 301

#define REG_Save                            _T("SOFTWARE\\Pano\\PhoneGenius\\Contacts")
#define REG_Save_LocationPersonal			_T("SOFTWARE\\Pano\\PhoneGenius\\Favs\\Personal")
#define REG_Save_LocationBusiness			_T("SOFTWARE\\Pano\\PhoneGenius\\Favs\\Business")
#define REG_MRU			                    _T("SOFTWARE\\Pano\\PhoneGenius\\MRU")

////////////////// obselete soon /////////////////////////
#define BTN_ACTIVE_GRAD1	            (COLORREF)0xD3BD84
#define BTN_ACTIVE_GRAD2	            (COLORREF)0x9E6331
#define BTN_ACTIVE_GRAD3	            (COLORREF)0x7D3108
#define BTN_ACTIVE_GRAD4	            (COLORREF)0xDF425A

#define BTN_INACTIVE_GRAD1	            (COLORREF)0x93BD84
#define BTN_INACTIVE_GRAD2	            (COLORREF)0x4E6331
#define BTN_INACTIVE_GRAD3	            (COLORREF)0x2D3108
#define BTN_INACTIVE_GRAD4	            (COLORREF)0x0F425A
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// COLORS
#define COLOR_TOOLBAR_BASE              (COLORREF)0x000000
#define COLOR_TOOLBAR_GRAD1_ST          (COLORREF)0x2B2B2B
#define COLOR_TOOLBAR_GRAD1_END         (COLORREF)0x646464
#define COLOR_TOOLBAR_GRAD2_ST          (COLORREF)0x787878
#define COLOR_TOOLBAR_GRAD2_END         (COLORREF)0x989898
#define COLOR_TOOLBAR_SEP_DARK          (COLORREF)0x373737
#define COLOR_TOOLBAR_SEP_LIGHT         (COLORREF)0x808080
#define COLOR_SLIDER_GRAD1_ST           RGB(237,245,248)
#define COLOR_SLIDER_GRAD1_END          RGB(193,197,199)
#define COLOR_SLIDER_GRAD2_ST           RGB(181,183,185)
#define COLOR_SLIDER_GRAD2_END          RGB(144,144,144)

#define COLOR_TEXT_NORMAL               0xFFFFFF//(COLORREF)0xCFCFCF

#define COLOR_FAVORITES_BG              (COLORREF)0x2A2A2A
#define COLOR_FAVORITES_BORDER          (COLORREF)0x343434//0x3A3A3A
#define COLOR_FAVORITES_PLACEHOLDER_BG  (COLORREF)0x7A7A7A
#define COLOR_FAVORITES_BG_TEXT         (COLORREF)0x9FA3A4

#define COLOR_CONTACTS_BG_GRAD1_ST      RGB(102,102,102)
#define COLOR_CONTACTS_BG_GRAD1_END     RGB(0,0,0)
#define COLOR_CONTACTS_BG_GRAD2_ST      RGB(106,106,106)
#define COLOR_CONTACTS_BG_GRAD2_END     RGB(12,12,12)
#define COLOR_CONTACTS_ALPHA_BG         RGB(180,180,180)
#define COLOR_HISTORY_DROPBAR           RGB(148,150,148)
#define COLOR_HISTORY_BG                RGB(51,51,51)
#define COLOR_HISTORY_SMS_UNREAD        RGB(255,255,0)

//////////////////////////////////////////////////////////
// MEASUREMENTS
#define FAVORITES_GRID_BORDER           2
#define FAVORITES_GRID_MARGIN           2
#define FAVORITES_TEXT_MARGIN           4
#define FAVORITES_SHADOW_OFFSET         6
#define FAVORITES_SLIDER_HEIGHT         38
#define FAVORITES_SLIDER_KNOB_HEIGHT    28
#define FAVORITES_SLIDER_TRACK_HEIGHT   31

#define CONTACTS_ALPHA_WIDTH            16
#define CONTACTS_LIST_INSET             6
#define HISTORY_DROPTAB_HEIGHT          10
#define HISTORY_DROPTAB_HEIGHT_FULL     80

//////////////////////////////////////////////////////////
// MISC
#define TOOLBAR_ANIMATE_TIME            700
#define FAVORITES_SLIDER_ANIMATE_TIME   300
#define FAVORITES_PAGE_ANIMATE_TIME     400
#define FAVORITES_MAX_TapLaunchTime     700
#define FAVORITES_MIN_DragMoveThresh    16
#define TOOLBAR_MIN_DragMoveThresh      16
#define INDENT                          (GetSystemMetrics(SM_CXSMICON)/4)
#define CALL_HISTORY_MAX_COUNT          500

//favorites block size
#define MIN_BLOCK_SIZE (10)
#define MAX_BLOCK_SIZE (200) 

