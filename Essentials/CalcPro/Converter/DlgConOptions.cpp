#include "DlgConOptions.h"
#include "resource.h"
#include "IssLocalisation.h"

void CDlgConOptions::PopulateOptions()
{
    m_oMenu.ResetContent();

    m_oMenu.AddCategory(m_oStr->GetText(ID(IDS_MENU_General), m_hInst));

    EnumOption* eOption = new EnumOption;
    *eOption = OPTION_Skin;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_PlaySounds;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_Language;
    m_oMenu.AddItem(eOption, IDMENU_Select);

    eOption = new EnumOption;
    *eOption = OPTION_UseRegionalSettings;
    m_oMenu.AddItem(eOption, IDMENU_Select);   

    if(IsSPLand())
    {
        eOption = new EnumOption;
        *eOption = OPTION_Keyboard;
        m_oMenu.AddItem(eOption, IDMENU_Select);  

        eOption = new EnumOption;
        *eOption = OPTION_KeyboardOffset;
        m_oMenu.AddItem(eOption, IDMENU_Select);  

        eOption = new EnumOption;
        *eOption = OPTION_KeyboardLetters;
        m_oMenu.AddItem(eOption, IDMENU_Select);  
    }

    m_oMenu.SetItemHeights(BUTTON_Height, BUTTON_Height);
    m_oMenu.SetSelectedItemIndex(0, TRUE);
}