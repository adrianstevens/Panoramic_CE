"c:\program files (x86)\microsoft visual studio 8\common7\ide\devenv" /rebuild "release|Win32" "..\..\calcproPC.sln" 

copy ..\..\release\calcpropc.exe .\

"c:\program files (x86)\nsis\makensis.exe" CalcPro-Desktop.nsi
"c:\program files (x86)\nsis\makensis.exe" CalcPro-Desktop-Demo.nsi