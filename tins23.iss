#include "include\version.inc"

[Setup]
AppName={#APPLICATION_NAME}
AppVerName={#APPLICATION_NAME} {#APPLICATION_VERSION}
AppPublisher=Helixsoft.nl
AppPublisherURL=http://www.helixsoft.nl
DefaultDirName={pf}\Helixsoft.nl\Tins2022
DefaultGroupName=Tins2022
UninstallDisplayIcon={app}\tins22.exe
Compression=lzma
SolidCompression=yes
#ifdef DEBUG
OutputBaseFilename=Install_Tins2022-Debug-{#APPLICATION_SHORT_VERSION}
#else
OutputBaseFilename=Install_Tins2022-{#APPLICATION_SHORT_VERSION}
#endif
OutputDir=dist

[Files]

#ifdef DEBUG
Source: "build/debug_win/tins22.exe"; DestDir: "{app}"
Source: "build/debug_win/*.dll"; DestDir: "{app}"; Flags: ignoreversion;
#else
Source: "build/release_win/tins22.exe"; DestDir: "{app}"
Source: "build/release_win/*.dll"; DestDir: "{app}"; Flags: ignoreversion;
#endif

Source: "README.md"; DestDir: "{app}"; Flags: isreadme
Source: "LICENSE.txt"; DestDir: "{app}";
Source: "data\*"; DestDir: "{app}\data";

[Icons]
Name: "{group}\TINS2022"; Filename: "{app}\tins22.exe";
Name: "{commondesktop}\TINS2022"; Filename: "{app}\tins22.exe";

[Run]
Filename: "{app}\tins22.exe"; Description: "Launch application"; Flags: postinstall nowait skipifsilent

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"
