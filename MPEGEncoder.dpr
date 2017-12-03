program MPEGEncoder;

uses
  Forms,
  MainUnit in 'MainUnit.pas' {MainForm},
  OpenDirectoryUnit in 'OpenDirectoryUnit.pas' {OpenDirectoryForm};

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TMainForm, MainForm);
  Application.CreateForm(TOpenDirectoryForm, OpenDirectoryForm);
  Application.Run;
end.
