unit MainUnit;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Gauges, ExtCtrls, ComCtrls, ProgressTimerUnit;

  //{$define MULTI_THREADED}

const
  SUCESS = 0;
  
  WM_UPDATE_FILE_PROGRESS = WM_USER + 101;
  WM_THREAD_TERMINATED    = WM_USER + 102;

type

  TWMFileProgress = packed record
    Msg:       Cardinal;
    Frame:     Integer;
    NumFrames: Integer;
    Result:    Longint;
  end;

  TSetHandles       = procedure(hMainWnd, hRenderwnd: HWND; hRenderDC: HDC); stdcall;
  TConvertVideo     = function(input_fname, output_fname, error_msg: PCHAR): DWORD; stdcall;
  TConvertVideo2    = function(input_fname, output_fname, error_msg: PCHAR): DWORD; stdcall;
  TStartJob         = procedure(files_count: Integer; input_fname, output_fname: PCHAR); stdcall;
  TIsJobRunning     = function(): BOOL; stdcall;
  TCancelJob        = procedure(); stdcall;
  TInitializeOpenGL = function(h: HWND): BOOL; stdcall;
  TCleanupOpenGL    = procedure(); stdcall;

type
  TMainForm = class(TForm)
    FilesListBox: TListBox;
    ButtonAdd: TButton;
    ButtonRemove: TButton;
    ButtonClear: TButton;
    EditOutputFolder: TEdit;
    Label1: TLabel;
    ButtonBrowse: TButton;
    RenderPanel: TPanel;
    ButtonConvert: TButton;
    FileProgressGauge: TGauge;
    TotalProgressGauge: TGauge;
    CheckBoxRender: TCheckBox;
    StatusBar: TStatusBar;
    RenderWindow: TPanel;
    ButtonCancel: TButton;
    procedure FormCreate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure ButtonAddClick(Sender: TObject);
    procedure ButtonRemoveClick(Sender: TObject);
    procedure ButtonClearClick(Sender: TObject);
    procedure ButtonBrowseClick(Sender: TObject);
    procedure ButtonConvertClick(Sender: TObject);
    procedure FormCloseQuery(Sender: TObject; var CanClose: Boolean);
    procedure ButtonCancelClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
    hDll: THandle;

    SetHandles:       TSetHandles;
    ConvertVideo:     TConvertVideo;
    ConvertVideo2:    TConvertVideo2;
    StartJob:         TStartJob;
    IsJobRunning:     TIsJobRunning;
    CancelJob:        TCancelJob;
    InitializeOpenGL: TInitializeOpenGL;
    CleanupOpenGL:    TCleanupOpenGL;

    AppDir: String;
    SettingsFileName: String;

    //RenderWndDC: HDC;

    Timer: TProgressTimer;
    //StartTime: DWORD;

    function  GetAppDir(): String;
    procedure InitStatusBar();

    function  ChangeExtention(Name, Ext: String): String;
    function  FixPath(path: String): String;

    procedure EnableUI(IsJobDone: Boolean);

    procedure ConvertMultiThreaded();
    procedure ConvertSingleThreaded();

    procedure OnThreadTerminated(var Msg: TMessage); Message WM_THREAD_TERMINATED;
    procedure OnFileProgress(var Msg: TWMFileProgress); Message WM_UPDATE_FILE_PROGRESS;

    procedure UpdateFileProgress(Frame, NumFrames: Integer);
    procedure UpdateTotalProgress(i, NumFiles: Integer);

    procedure LoadSettings();
    procedure SaveSettings();
  end;

var
  MainForm: TMainForm;

implementation

uses OpenDirectoryUnit;

{$R *.dfm}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.FormCreate(Sender: TObject);
const
 DllName = 'Converter.dll';
begin
InitStatusBar();

AppDir := GetAppDir();
SettingsFileName := AppDir + 'Settings.txt';

hDll := LoadLibrary(PCHAR(DllName));
if(hDll <> 0) then begin
  @SetHandles       := GetProcAddress(hDll, '_SetHandles');
  @ConvertVideo     := GetProcAddress(hDll, '_ConvertVideo');
  @ConvertVideo2    := GetProcAddress(hDll, '_ConvertVideo2');
  @StartJob         := GetProcAddress(hDll, '_StartJob');
  @IsJobRunning     := GetProcAddress(hDll, '_IsJobRunning');
  @CancelJob        := GetProcAddress(hDll, '_CancelJob');
  @InitializeOpenGL := GetProcAddress(hDll, '_InitializeOpenGL');
  @CleanupOpenGL    := GetProcAddress(hDll, '_CleanupOpenGL');
end else begin
  ShowMessage(DllName + ' not found.');
  Application.Terminate;
end;

LoadSettings();

//RenderWndDC := GetDC(RenderWindow.Handle);
SetHandles(Self.Handle, RenderWindow.Handle, 0);
//InitializeOpenGL(RenderWindow.Handle);
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.FormCloseQuery(Sender: TObject; var CanClose: Boolean);
begin
CanClose := IsJobRunning() = FALSE;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.FormClose(Sender: TObject; var Action: TCloseAction);
begin
SaveSettings();

//if(RenderWndDC <> 0) then
//  ReleaseDC(RenderWindow.Handle, RenderWndDC);

if(hDll <> 0) then begin
  //CleanupOpenGL();
  FreeLibrary(hDll);
end;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.ButtonAddClick(Sender: TObject);
var
 OpnDlg: TOpenDialog;
begin
OpnDlg := TOpenDialog.Create(Self);

OpnDlg.Options := OpnDlg.Options + [ofAllowMultiSelect];
OpnDlg.Filter := 'Video files|*.avi;*.mp4;*.mkv;*.wmv;*.mov;*.flv;*.mpg';
OpnDlg.FilterIndex := 0;

if(OpnDlg.Execute) then begin
  FilesListBox.Items.AddStrings(OpnDlg.Files);
end;

OpnDlg.Free;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.ButtonRemoveClick(Sender: TObject);
begin
FilesListBox.DeleteSelected;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.ButtonClearClick(Sender: TObject);
begin
FilesListBox.Items.Clear;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.ButtonBrowseClick(Sender: TObject);
begin
OpenDirectoryForm.DirectoryListBox.Directory := EditOutputFolder.Text;
if(OpenDirectoryForm.ShowModal = mrOk) then begin
  EditOutputFolder.Text := OpenDirectoryForm.DirectoryListBox.Directory;
end;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.InitStatusBar();
begin
StatusBar.Panels[0].Text := 'File Progress: N\A';
StatusBar.Panels[1].Text := 'Total Progress: N\A';
StatusBar.Panels[2].Text := 'Elapsed Time: N\A';
StatusBar.Panels[3].Text := 'Remaining Time: N\A';
StatusBar.Panels[4].Text := 'Total remaining Time: N\A';
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.EnableUI(IsJobDone: Boolean);
begin
FilesListBox.Enabled     := IsJobDone;
ButtonAdd.Enabled        := IsJobDone;
ButtonRemove.Enabled     := IsJobDone;
ButtonClear.Enabled      := IsJobDone;
EditOutputFolder.Enabled := IsJobDone;
ButtonBrowse.Enabled     := IsJobDone;
CheckBoxRender.Enabled   := IsJobDone;
ButtonConvert.Enabled    := IsJobDone;
ButtonCancel.Enabled     := not IsJobDone;
Application.ProcessMessages;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.ConvertSingleThreaded();
const
 ErrorMsgSize = 1024;
var
 i, FilesCount:  Integer;
 InputFileName:  String;
 OutputFileName: String;
 OutputFilePath: String;
 ErrorMsg:  Array [0..ErrorMsgSize-1] of Char;
 pErrorMsg: PCHAR;
begin
pErrorMsg := @ErrorMsg[0];

EnableUI(False);

FilesCount := FilesListBox.Items.Count;

for i := 0 to FilesCount-1 do begin

  if(IsJobRunning()) then
    break;

  UpdateTotalProgress(i, FilesCount);

  Self.Caption := 'MPEG Converter' + ' - Encoding "' + InputFileName + '"';
  Application.ProcessMessages;
  
  InputFileName  := FilesListBox.Items[i];
  OutputFilePath := EditOutputFolder.Text;
  OutputFileName := FixPath(OutputFilePath) + ExtractFileName(ChangeFileExt(InputFileName, '.mpg'));

  ZeroMemory(pErrorMsg, ErrorMsgSize);
  if(ConvertVideo2(PCHAR(InputFileName), PCHAR(OutputFileName), pErrorMsg) <> SUCESS) then
    ShowMessage(pErrorMsg);
end;

Self.Caption := 'MPEG Converter';
UpdateTotalProgress(0, 0);
//RenderWindow.Refresh();
EnableUI(True);
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.ConvertMultiThreaded();
const
 ErrorMsgSize = 1024;
var
 //lkjlj: HWND;
 i, FilesCount:  Integer;
 InputFileName:  String;
 OutputFileName: String;
 OutputFilePath: String;
 InputFiles:     String;
 OutputFiles:    String;
begin
EnableUI(False);

InputFiles  := '';
OutputFiles := '';

FilesCount := FilesListBox.Items.Count;

for i := 0 to FilesCount-1 do begin

  InputFileName  := FilesListBox.Items[i];
  OutputFilePath := EditOutputFolder.Text;
  OutputFileName := FixPath(OutputFilePath) + ExtractFileName(ChangeFileExt(InputFileName, '.mpg'));

  InputFiles  := InputFiles  + '"' + InputFileName  + '"';
  OutputFiles := OutputFiles + '"' + OutputFileName + '"';

  if(i <> FilesCount-1) then begin
    InputFiles  := InputFiles  + ' ';
    OutputFiles := OutputFiles + ' ';
  end;

end;

SetHandles(Self.Handle, RenderWindow.Handle, 0);

StartJob(FilesCount, PCHAR(InputFiles), PCHAR(OutputFiles));
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.ButtonConvertClick(Sender: TObject);
begin
{$ifdef MULTI_THREADED}
ConvertMultiThreaded();
{$else}
ConvertSingleThreaded();
{$endif}
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.ButtonCancelClick(Sender: TObject);
begin
CancelJob();
end;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

function TMainForm.ChangeExtention(Name, Ext: String): String;
var
 NameLen, ExtLen, ExtPos: Integer;
 OldName, OldExt, NewName, NewExt: String;
begin
NewExt  := Ext;
NewName := Name;
OldName := ExtractFileName(Name);
NameLen := Length(OldName);
OldExt  := ExtractFileExt(OldName);
ExtLen  := Length(OldExt);

if(ExtLen > 0) then begin
  ExtPos := (NameLen - ExtLen) + 1;
  Delete(OldName, ExtPos, ExtLen);
  NewName := OldName + NewExt;
end;

Result := NewName;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

function TMainForm.FixPath(path: String): String;
var
 len: Integer;
begin
len := Length(path);
if(path[len] <> '\') then
  path := path + '\';

Result := path;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.OnThreadTerminated(var Msg: TMessage);
begin
EnableUI(True);
RenderWindow.Refresh();
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.OnFileProgress(var Msg: TWMFileProgress);
//var
// ElapsedTime, ReminingTime, Done, z: Single;
begin
//if(Msg.NumFrames > 0) then begin
  //ElapsedTime := (GetTickCount() - StartTime) / 1000.0;
  //Done := Msg.Frame / Msg.NumFrames;
  //ReminingTime := ElapsedTime * (1.0 - Done);
//end;

//z := ReminingTime;
UpdateFileProgress(Msg.Frame, Msg.NumFrames);
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.UpdateFileProgress(Frame, NumFrames: Integer);
begin
FileProgressGauge.Progress := Frame;
FileProgressGauge.MaxValue := NumFrames;
StatusBar.Panels[0].Text := 'File Progress: ' + IntToStr(Frame) + ' \ ' + IntToStr(NumFrames);
Application.ProcessMessages;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.UpdateTotalProgress(i, NumFiles: Integer);
begin
TotalProgressGauge.Progress := i;
TotalProgressGauge.MaxValue := NumFiles;
StatusBar.Panels[1].Text    := 'Total Progress: ' + IntToStr(i) + ' \ ' + IntToStr(NumFiles);
Application.ProcessMessages;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

function TMainForm.GetAppDir(): String;
var
 dir: String;
 Len: Integer;
begin
GetDir(0, dir);
Len := Length(dir);
if(dir[Len] <> '\') then
  dir := dir + '\';
Result := dir;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.LoadSettings();
var
 Lst: TStringList;
 NumItems, i: Integer;
begin
Lst := TStringList.Create;
try
  if(FileExists(SettingsFileName)) then begin
    Lst.LoadFromFile(SettingsFileName);

  EditOutputFolder.Text := Lst[0];
  NumItems := StrToIntDef(Lst[1], 0);

  FilesListBox.Items.Clear;
  for i := 0 to NumItems-1 do
    FilesListBox.Items.Add(Lst[i+2]);

  end;
finally
  Lst.Free;
end;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.SaveSettings();
var
 Lst: TStringList;
 NumItems, i: Integer;
begin
Lst := TStringList.Create;
try
  NumItems := FilesListBox.Count;

  Lst.Add(EditOutputFolder.Text);
  Lst.Add(IntToStr(NumItems));

  for i := 0 to NumItems-1 do
    Lst.Add(FilesListBox.Items.Strings[i]);

  Lst.SaveToFile(SettingsFileName);
finally
  Lst.Free;
end;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

end.
