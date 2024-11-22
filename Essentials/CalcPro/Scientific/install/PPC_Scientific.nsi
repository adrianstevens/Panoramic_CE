; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "Panoramic Scientific Calc"
!define PRODUCT_VERSION "1.0.0"
!define PRODUCT_PUBLISHER "Panoramic Software Inc."
!define PRODUCT_WEB_SITE "http://www.panoramicsoft.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Scientific.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

Name "${PRODUCT_NAME}"
OutFile "Pano_Scientific_PPC.exe"
InstallDir "$PROGRAMFILES\Panoramic\ScientificCalc"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails hide
ShowUnInstDetails hide

BrandingText "www.panoramicsoft.com"

; if you want to use the Gradient background
BGGradient 0 587FA4 FFFFFF

; check box
!define MUI_COMPONENTSPAGE_CHECKBITMAP "check.bmp"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "clc-logo.bmp"
!define MUI_ABORTWARNING
!define MUI_ICON "YourApp.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "install_sidebar.bmp"
!define MUI_COMPONENTSPAGE_SMALLDESC

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!define MUI_LICENSEPAGE_RADIOBUTTONS
!insertmacro MUI_PAGE_LICENSE "eula.rtf"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Section "Scientific Calc" SEC01

  SetOutPath "$INSTDIR"
  File "PPC_Scientific.ARM720.CAB"
  FILE "PPC_Install.ini"

  StrCpy $0 "$INSTDIR\PPC_Install.ini"
  Call InstallCAB
  
SectionEnd


;Section -AdditionalIcons
;  SetOutPath $INSTDIR
;  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
;  CreateShortCut "$SMPROGRAMS\Scientific\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
;  CreateShortCut "$SMPROGRAMS\Scientific\Uninstall.lnk" "$INSTDIR\uninst.exe"
;SectionEnd

;Section -Post
;  WriteUninstaller "$INSTDIR\uninst.exe"
;  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Scientific.exe"
;  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
;  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
;  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Scientific.exe"
;  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
;  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
;  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
;SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "Install Scientific Calc for Windows Mobile Professional."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function .onInit
  ; check for another instance of this installer
  System::Call 'kernel32::CreateMutexA(i 0, i 0, t "Scientific Calc Installer") i .r1 ?e'
  Pop $R0
  StrCmp $R0 0 +3
    MessageBox MB_OK|MB_ICONEXCLAMATION "The installer is already running."
    Abort
   
  ; Launch the Splashscreen
  SetOutPath $TEMP
  File /oname=spltmp.bmp "splash.bmp"
  advsplash::show 1000 600 400 0xFF00FF $TEMP\spltmp
  Pop $0 ; $0 has '1' if the user closed the splash screen early,
         ; '0' if everything closed normal, and '-1' if some error occured.
  Delete $TEMP\spltmp.bmp
  
  ; one-time initialization needed for InstallCAB subroutine
  ReadRegStr $1 HKEY_LOCAL_MACHINE "software\Microsoft\Windows\CurrentVersion\App Paths\CEAppMgr.exe" ""
  IfErrors Error
  Goto End
  Error:
  MessageBox MB_OK|MB_ICONEXCLAMATION \
  "Unable to find Application Manager for PocketPC applications. \
  Please install ActiveSync and reinstall"
  End:
  
FunctionEnd

; Installs a PocketPC cab-application
; It expects $0 to contain the absolute location of the ini file
; to be installed.
Function InstallCAB
  ExecWait '"$1" "$0"'
FunctionEnd


