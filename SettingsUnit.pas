unit SettingsUnit;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Spin, ExtCtrls;

type
  TSettingsForm = class(TForm)
    Button1: TButton;
    Button2: TButton;
    GroupBox2: TGroupBox;
    GroupBox3: TGroupBox;
    RadioButton1: TRadioButton;
    RadioButton2: TRadioButton;
    RadioButton3: TRadioButton;
    GroupBox4: TGroupBox;
    ComboBox3: TComboBox;
    RadioButton5: TRadioButton;
    RadioButton6: TRadioButton;
    GroupBox1: TGroupBox;
    CheckBox1: TCheckBox;
    RadioButton7: TRadioButton;
    ComboBox2: TComboBox;
    RadioButton4: TRadioButton;
    Label4: TLabel;
    SpinEdit1: TSpinEdit;
    Label3: TLabel;
    SpinEdit2: TSpinEdit;
    Label5: TLabel;
    ComboBox1: TComboBox;
    CheckBox2: TCheckBox;
    CheckBox3: TCheckBox;
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  SettingsForm: TSettingsForm;

implementation

{$R *.dfm}

end.
