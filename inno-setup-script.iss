; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "OkMSG"
#define MyAppVersion "24.06.22"
#define MyAppPublisher "OkStar"
#define MyAppURL "https://github.com/okstar-org/ok-msg-desktop"
#define MyAppExeName "ok-msg-desktop.exe"
#define MyAppExeFile "ok-msg-desktop_windows-latest_x64_setup"
#define MyAppExePATH "ok-msg-desktop"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{A8FE8EAC-3A4F-44E4-9B03-D41D5B20F749}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName=C:\Program Files\OkStar\{#MyAppName}
DisableProgramGroupPage=yes
LicenseFile={#MyAppExePATH}\LICENSE
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
OutputDir=.\deploy-setup
OutputBaseFilename={#MyAppExeFile}
Compression=lzma
SolidCompression=yes
WizardStyle=modern

; Ĭ��ֵyes(����[Setup]�β�����UseSetupLdr�������ʱ, Inno Setup����Ϊ����ֵ��yes).
; ��Ҫ�����Ǹ��߰�װ��������Ҫ�����������͵İ�װ�ļ������ֵ��yes, ��װ�������Ὣ���е�setup���ݱ�������һ��������EXE�ļ�; ���ֵΪno, ��װ����������Ѱ�װ�����ļ����ٱ��뵽�����ļ���: setup.exe, setup-0.bin, setup-1.bin��
; ����Ҫʹ��no��Ψһԭ��Ӧ���ǳ��ڵ��Ե�Ŀ�ġ�
; ע��: ��һ����װ��Ҫ����̵�ʱ��(DiskSpanning=yes)��Ҫ���ò�����ֵ����Ϊno. �����ֵΪyes��ʱ��, ��װ����ᱻ�������û���ʱĿ¼���������ʱĿ¼������, �����������Ϊnoʱ��Щ�������ᷢ��, ���Windows��ͼ�ڴ������ҵ�setup.exe
; �ļ����Ҳ������ļ�, ����ܵ��´���, ��Ϊ�������д��������Ĵ��̡�
; ע��: ��Ҫʹ��UseSetupLdr=no���Ա�����Ͱ�װ�����ϵ�����ǩ����֤�����ӳ�, ��Ӧʹ�ô��̿�ӡ����Բ鿴SignTool��ȡ����ϸ����Ϣ������Ҫע��Ի���UseSetupLdr=no�İ�װ�����������ǩ����������Ч������ǩ������ж�ء�
UseSetupLdr=yes
; VersionInfoVesion����[Setup]����������ָʾ�������ļ��İ汾��, ������'.'(���)��'.'(���)�ֿ���4��������ֵ��ɵ�,�����д����ֶεĻ�, Ĭ����: 0.0.0.0
; ���ֻд���ݵİ汾����ֵҲ�ǿ��Ե�, ȱʧ����ֵ���ݻ��Զ�׷��0, ��������������1.2����Զ�������Ϊ1.2.0.0
; ��UseSetupLdr��ֵΪnoʱ, ��������Ͳ�������ɵĶ������ļ����κε�Ӱ����
VersionInfoVersion={#MyAppVersion}

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "chinese"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: ".\{#MyAppExePATH}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
