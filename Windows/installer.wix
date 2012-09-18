!include "LogicLib.nsh"

!define VERSION "2.2.4"
Name "Salor POS"
OutFile "salor_installer.exe"
InstallDir $PROGRAMFILES\SalorPOS
InstallDirRegKey HKLM "Software\NSIS_SalorPOS" "Install_Dir"
RequestExecutionLevel admin

Section "SalorPOS (required)"
	SetOutPath $INSTDIR
	File "salor.exe"
	WriteRegStr HKLM "Software\NSIS_SalorPOS" "Install_Dir" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SalorPOS" "DisplayName" "SalorPOS"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SalorPOS" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SalorPOS" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SalorPOS" "NoRepair" 1
	WriteUninstaller "uninstall.exe"
SectionEnd

Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\Salor POS"
  CreateShortCut "$SMPROGRAMS\Salor POS\Salor (MakeNSISW).lnk" "$INSTDIR\salor.exe" "" "$INSTDIR\salor.exe" 0
SectionEnd

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SalorPOS"
  DeleteRegKey HKLM "Software\NSIS_SalorPOS"

  ; Remove files and uninstaller
  Delete $INSTDIR\salor.exe
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Salor POS\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Salor POS"
  RMDir "$INSTDIR"

SectionEnd