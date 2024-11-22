<!-- 
/******************************************************************************
//Copyright (c) 2005 by Intrinsyc Software International, Inc.  All 
//rights reserved.
******************************************************************************/
 -->

<View 
    id="IDFX_OPTIONS_VIEW" 
    TextID="IDS_APP_TITLE" 
    class="IDS_MAIN_CLASS" 
    style="WS_POPUP" 
    >
  <SingleLineList
        id="IDC_LST_PLAYSOUNDS"
        style="LBS_STANDARD|WS_CHILD|WS_VISIBLE|WS_TABSTOP"
        exstyle="SLS_EX_SELECTBLOCK|SLS_EX_NOSELECTWRAP"
        top="50"
        left="10"
        width="170"
        height="26"
        visible="ATTR_TRUE" />
  
  <SingleLineList
        id="IDC_LST_DIFFICULTY"
        style="LBS_STANDARD|WS_CHILD|WS_VISIBLE|WS_TABSTOP"
        exstyle="SLS_EX_SELECTBLOCK|SLS_EX_NOSELECTWRAP"
        top="30"
        left="10"
        width="170"
        height="26"
        visible="ATTR_TRUE" />

  <SingleLineList
       id="IDC_LST_BACKGROUND"
       style="LBS_STANDARD|WS_CHILD|WS_VISIBLE|WS_TABSTOP"
       exstyle="SLS_EX_SELECTBLOCK|SLS_EX_NOSELECTWRAP"
       top="10"
       left="10"
       width="170"
       height="26"
       visible="ATTR_TRUE" />
  
	<!--<SoftKeys id="IDC_SOFTKEYS_1">

    <SoftKey 
          id="IDC_SOFTKEY_LEFT" 
          commandId="IDMENU_Save" 
          textId="IDS_MENU_Save" 
          />

    <SoftKey 
          id="IDC_SOFTKEY_RIGHT" 
          commandId="IDMENU_Cancel" 
          textId="IDS_MENU_Cancel" 
          />

  </SoftKeys>-->


</View>