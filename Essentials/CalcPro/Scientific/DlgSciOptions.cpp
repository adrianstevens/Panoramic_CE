#include "DlgSciOptions.h"
#include "resource.h"
#include "IssLocalisation.h"

void CDlgSciOptions::PopulateOptions()
{
    m_oMenu.ResetContent();

    m_oMenu.AddCategory(m_oStr->GetText(ID(IDS_MENU_General), m_hInst));

    EnumOption* eOption = new EnumOption;
    *eOption = OPTION_Skin;
    m_oMenu.AddItem(eOption, IDMENU_Select);

#ifdef WIN32_PLATFORM_PSPC
    eOption = new EnumOption;
    *eOption = OPTION_FullScreen;
    m_oMenu.AddItem(eOption, IDMENU_Select);
#endif

    eOption = new EnumOption;
    *eOption = OPTION_PlaySounds;
    m_oMenu.AddItem(eOption, IDMENU_Select);

	eOption = new EnumOption;
	*eOption = OPTION_Language;
	m_oMenu.AddItem(eOption, IDMENU_Select);
    eOption = new EnumOption;
    *eOption = OPTION_UseRegionalSettings;
    m_oMenu.AddItem(eOption, IDMENU_Select);   

    m_oMenu.AddCategory(m_oStr->GetText(ID(IDS_MENU_Calculator), m_hInst));

    eOption = new EnumOption;
    *eOption = OPTION_CalculatorMode;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_Notation;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_RecordTape;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_AdvancedMemory;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_FixedDigits;
    m_oMenu.AddItem(eOption, IDMENU_Select);
    
    eOption = new EnumOption;
    *eOption = OPTION_ConversionBuffer;
    m_oMenu.AddItem(eOption, IDMENU_Select);
    
    eOption = new EnumOption;
    *eOption = OPTION_RPNStackSize;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    m_oMenu.AddCategory(m_oStr->GetText(ID(IDS_MENU_Graphing), m_hInst));

    eOption = new EnumOption;
    *eOption = OPTION_GraphAccuracy;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_GraphUsePoints;
    m_oMenu.AddItem(eOption, IDMENU_Select);


    m_oMenu.SetItemHeights(BUTTON_Height, BUTTON_Height);
    m_oMenu.SetSelectedItemIndex(0, TRUE);
}
