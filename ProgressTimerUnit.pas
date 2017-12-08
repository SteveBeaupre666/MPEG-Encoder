unit ProgressTimerUnit;
interface

uses
  Windows, SysUtils, MMSystem;

type
  TProgressTimer = class
  private
    Available:   LongBool;

    Frequency:   Int64;
    TimeScale:   Double;

    StartTime:   Int64;
    CurrentTime: Int64;

    procedure CalcTimeScale();
    procedure UpdateCurrentTime();
    function  GetElapsedTime(): Double;
  protected
    constructor Initialize();
  public
    procedure Reset();
    function  TimeLeft(i, n: Int64): Double;
  end;

implementation

constructor TProgressTimer.Initialize();
begin
Available := QueryPerformanceFrequency(Frequency);
CalcTimeScale();
Reset();
end;

procedure TProgressTimer.CalcTimeScale();
begin
case Available of
  True:  TimeScale := 1.0 / Frequency;
  False: TimeScale := 1.0 / 1000.0;
end;
end;

procedure TProgressTimer.UpdateCurrentTime();
begin
case Available of
  True:  QueryPerformanceCounter(CurrentTime);
  False: CurrentTime := timeGetTime();
end;
end;

procedure TProgressTimer.Reset();
begin
UpdateCurrentTime();
StartTime := CurrentTime;
end;

function TProgressTimer.GetElapsedTime(): Double;
begin
Result := (CurrentTime - StartTime) * TimeScale;
end;

function TProgressTimer.TimeLeft(i, n: Int64): Double;
var
 tl, dt: Double;
begin
UpdateCurrentTime();

dt := GetElapsedTime();

if((i <= 0) or (n <= 0) or (i > n)) then begin
  Result := -1.0;
  Exit;
end;

tl := (dt * (1.0 / (i / n))) - dt;

Result := tl;
end;

end.
