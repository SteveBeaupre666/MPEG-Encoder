unit MainUnit;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Gauges, ExtCtrls, ComCtrls;

const
  SUCESS = 0;
  WM_UPDATE_FILE_PROGRESS = WM_USER + 101;

type
  TWMFileProgress = packed record
    Msg:       Cardinal;
    Frame:     Integer;
    NumFrames: Integer;
    Result:    Longint;
  end;

  TSetHandles   = procedure(hMainWnd, hRenderwnd: HWND); stdcall;
  TConvertVideo = function(input_fname, output_fname, error_msg: PCHAR): DWORD; stdcall;
  TAbortJob     = procedure(); stdcall;

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
    CheckBoxRenderFrames: TCheckBox;
    StatusBar: TStatusBar;
    RenderWindow: TPanel;
    procedure FormCreate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure ButtonAddClick(Sender: TObject);
    procedure ButtonRemoveClick(Sender: TObject);
    procedure ButtonClearClick(Sender: TObject);
    procedure ButtonBrowseClick(Sender: TObject);
    procedure ButtonConvertClick(Sender: TObject);
    procedure FormCloseQuery(Sender: TObject; var CanClose: Boolean);
  private
    { Private declarations }
  public
    { Public declarations }
    hDll: THandle;

    SetHandles:   TSetHandles;
    ConvertVideo: TConvertVideo;
    AbortJob:     TAbortJob;

    IsJobRunning:  Boolean;
    IsJobCanceled: Boolean;
    //procedure CancelJob();

    StartTime, ElapsedTime, RemainingTime: DWORD;

    function  ChangeExtention(Name, Ext: String): String;
    procedure OnFileProgress(var Msg: TWMFileProgress); Message WM_UPDATE_FILE_PROGRESS;
    procedure UpdateFileProgress(Frame, NumFrames: Integer);
    procedure UpdateTotalProgress(i, NumFiles: Integer);
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
FilesListBox.Items.Add('C:\New Programming Folder\Video Processing\Video\Titanic.mkv');

StatusBar.Panels[0].Text := 'File Progress: N\A';
StatusBar.Panels[1].Text := 'Total Progress: N\A';
StatusBar.Panels[2].Text := 'Remaining Time: N\A';
StatusBar.Panels[3].Text := 'Total remaining Time: N\A';
StatusBar.Panels[4].Text := '';

hDll := LoadLibrary(PCHAR(DllName));
if(hDll <> 0) then begin
  @SetHandles   := GetProcAddress(hDll, '_SetHandles');
  @ConvertVideo := GetProcAddress(hDll, '_ConvertVideo');
  @AbortJob     := GetProcAddress(hDll, '_AbortJob');
end else begin
  ShowMessage(DllName + ' not found.');
  Application.Terminate;
end;

SetHandles(Self.Handle, RenderWindow.Handle);
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.FormCloseQuery(Sender: TObject; var CanClose: Boolean);
begin
CanClose := not IsJobRunning;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.FormClose(Sender: TObject; var Action: TCloseAction);
begin
//CancelJob();
if(hDll <> 0) then begin
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

{procedure TMainForm.CancelJob();
begin
if(IsJobRunning) then begin
  AbortJob();
  IsJobCanceled := True;
  while(IsJobRunning) do begin
    Sleep(50);
    Application.ProcessMessages;
  end;
end;
end;}

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.ButtonConvertClick(Sender: TObject);
const
 ErrorMsgSize   = 1024;
var
 i, FilesCount:  Integer;
 InputFileName:  String;
 OutputFileName: String;
 OutputFilePath: String;
 ErrorMsg:  Array[0..ErrorMsgSize-1] of char;
 pErrorMsg: PCHAR;
begin
pErrorMsg := @ErrorMsg[0];

if(ButtonConvert.Caption = 'Convert') then begin

  FilesCount := FilesListBox.Items.Count;

  ButtonConvert.Caption := 'Cancel';

  IsJobRunning := True;
  IsJobCanceled := False;

  for i := 0 to FilesCount-1 do begin

    if(IsJobCanceled) then
      break;

    UpdateTotalProgress(i, FilesCount);

    InputFileName  := FilesListBox.Items[i];
    OutputFilePath := EditOutputFolder.Text;
    OutputFileName := ExtractFilePath(OutputFilePath) + ExtractFileName(ChangeFileExt(InputFileName, '.mpg'));

    Self.Caption := 'MPEG Converter' + ' - Encoding "' + InputFileName + '"';
    Application.ProcessMessages;

    ElapsedTime := 0;
    RemainingTime := 0;
    StartTime := GetTickCount();

    ZeroMemory(pErrorMsg, ErrorMsgSize);
    if(ConvertVideo(PCHAR(InputFileName), PCHAR(OutputFileName), pErrorMsg) <> SUCESS) then
      ShowMessage(pErrorMsg);

  end;

  IsJobRunning := False;

end else begin
  IsJobCanceled := True;
  AbortJob();
  Exit;
end;

Self.Caption := 'MPEG Converter';
ButtonConvert.Caption := 'Convert';
UpdateTotalProgress(0, 0);
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
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TMainForm.OnFileProgress(var Msg: TWMFileProgress);
var
 done: Single;
begin
if(Msg.NumFrames > 0) then begin
  ElapsedTime := GetTickCount() - StartTime;
  done := Msg.Frame / Msg.NumFrames;
  RemainingTime := 0;
end;

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

end.
