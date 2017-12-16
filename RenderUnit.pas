unit RenderUnit;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ExtCtrls, Gauges, ComCtrls, StdCtrls;

const
  WM_UPDATE_PROGRESS   = WM_USER + 101;
  WM_THREAD_TERMINATED = WM_USER + 102;

type
  TProgressInfo = record
    FileNumber:       Integer;
    FilesCount:       Integer;
    FrameNumber:      Integer;
    FramesCount:      Integer;
    FramesPerSecs   : Single;
    ElapsedTime:      Single;
    RemainingTime:    Single;
  end;
  ProgressInfoPtr = ^TProgressInfo;

  TWMFileProgress = packed record
    Msg:       Cardinal;
    Info:      ProgressInfoPtr;
    dummy:     Integer;
    Result:    Longint;
  end;

type
  TRenderForm = class(TForm)
    StatusBar: TStatusBar;
    MainPanel: TPanel;
    RenderPanel: TPanel;
    FileProgressGauge: TGauge;
    TotalProgressGauge: TGauge;
    ButtonCancel: TButton;
    ButtonPause: TButton;
    procedure FormCreate(Sender: TObject);
    procedure ButtonCancelClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
    procedure ResetProgressBars();
    procedure UpdateStatusBar(ProgressInfo: ProgressInfoPtr);

    procedure OnProgress(var Msg: TWMFileProgress);  Message WM_UPDATE_PROGRESS;
    procedure OnThreadTerminated(var Msg: TMessage);     Message WM_THREAD_TERMINATED;
  end;

var
  RenderForm: TRenderForm;

implementation

uses MainUnit;

{$R *.dfm}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TRenderForm.FormCreate(Sender: TObject);
begin
MainForm.SetHandle(Self.Handle);
MainForm.InitializeOpenGL(RenderPanel.Handle);
MainForm.SetBgColor(0.2, 0.2, 0.2);
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TRenderForm.ButtonCancelClick(Sender: TObject);
begin
MainForm.CancelJob();
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TRenderForm.OnThreadTerminated(var Msg: TMessage);
begin
if(Msg.LParam = 0) then
  ShowMessage('Conversion done.');

MainForm.EnableUI(True);
MainForm.Enabled := True;
Self.Hide;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TRenderForm.ResetProgressBars();
begin
FileProgressGauge.Progress  := 0;
TotalProgressGauge.Progress := 0;
FileProgressGauge.MaxValue  := 100;
TotalProgressGauge.MaxValue := 100;
Application.ProcessMessages;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TRenderForm.UpdateStatusBar(ProgressInfo: ProgressInfoPtr);
const
 NA = 'N\A';
var
 FrameNumber:   String;
 FramesCount:   String;
 FileNumber:    String;
 FilesCount:    String;
 FramesPerSecs: String;
 ElapsedTime:   String;
 RemainingTime: String;
begin
FrameNumber   := NA;
FramesCount   := NA;
FileNumber    := NA;
FilesCount    := NA;
FramesPerSecs := NA;
ElapsedTime   := NA;
RemainingTime := NA;

if(ProgressInfo <> nil) then begin
  if(ProgressInfo.FrameNumber   >= 0)   then FrameNumber   := IntToStr(ProgressInfo.FrameNumber);
  if(ProgressInfo.FramesCount   >= 0)   then FramesCount   := IntToStr(ProgressInfo.FramesCount);
  if(ProgressInfo.FileNumber    >= 0)   then FileNumber    := IntToStr(ProgressInfo.FileNumber);
  if(ProgressInfo.FilesCount    >= 0)   then FilesCount    := IntToStr(ProgressInfo.FilesCount);
  if(ProgressInfo.FramesPerSecs >= 0)   then FramesPerSecs := FloatToStr(ProgressInfo.FramesPerSecs);
  if(ProgressInfo.ElapsedTime   >= 0)   then ElapsedTime   := FloatToStr(ProgressInfo.ElapsedTime);
  if(ProgressInfo.RemainingTime >= 0.0) then RemainingTime := FloatToStr(ProgressInfo.RemainingTime);
end;

StatusBar.Panels[0].Text := 'Frame: ' + FrameNumber;
StatusBar.Panels[1].Text := 'Total: ' + FramesCount;

StatusBar.Panels[2].Text := 'Files: ' + FileNumber;
StatusBar.Panels[3].Text := 'Total: ' + FilesCount;

StatusBar.Panels[4].Text := 'FPS: '            + FramesPerSecs;
StatusBar.Panels[5].Text := 'Elapsed Time: '   + ElapsedTime;
StatusBar.Panels[6].Text := 'Remaining Time: ' + RemainingTime;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////

procedure TRenderForm.OnProgress(var Msg: TWMFileProgress);
begin
FileProgressGauge.Progress := Msg.Info.FrameNumber;
FileProgressGauge.MaxValue := Msg.Info.FramesCount;

UpdateStatusBar(Msg.Info);
Application.ProcessMessages;
end;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

end.
