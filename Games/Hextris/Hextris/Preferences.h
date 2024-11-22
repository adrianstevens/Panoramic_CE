//preferences.h
#pragma once

#define TEXT_INDENT			GetSystemMetrics(SM_CXICON)/4
#define LIST_CELL_HEIGHT	GetSystemMetrics(SM_CXICON)/2
#define CHILD_WINDOW_TOP	GetSystemMetrics(SM_CXICON)*2/3
#define CHILD_WINDOW_BOTTOM	0
#define LIST_ITEM_HEIGHT	GetSystemMetrics(SM_CXICON)/2

enum EnumSorting
{
	SORT_Name,
	SORT_Size,
	SORT_NameDesc,
	SORT_SizeDesc,
};

enum EnumNetView
{
	NETVIEW_Normal,
	NETVIEW_NoGraph,
	NETVIEW_Graph,
};

extern HFONT g_hFont;