program MPEGEncoder;

uses
  Forms,
  MainUnit in 'MainUnit.pas' {MainForm},
  OpenDirectoryUnit in 'OpenDirectoryUnit.pas' {OpenDirectoryForm},
  RenderUnit in 'RenderUnit.pas' {RenderForm},
  SettingsUnit in 'SettingsUnit.pas' {SettingsForm};

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TMainForm, MainForm);
  Application.CreateForm(TOpenDirectoryForm, OpenDirectoryForm);
  Application.CreateForm(TSettingsForm, SettingsForm);
  Application.Run;
end.
