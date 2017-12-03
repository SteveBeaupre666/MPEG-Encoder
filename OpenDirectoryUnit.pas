unit OpenDirectoryUnit;

interface

uses
{$WARN UNIT_PLATFORM OFF}
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, FileCtrl;
{$WARN UNIT_PLATFORM ON}
type
  TOpenDirectoryForm = class(TForm)
    DirectoryListBox: TDirectoryListBox;
    DriveComboBox: TDriveComboBox;
    ButtonOk: TButton;
    ButtonCancel: TButton;
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  OpenDirectoryForm: TOpenDirectoryForm;

implementation

{$R *.dfm}

end.
