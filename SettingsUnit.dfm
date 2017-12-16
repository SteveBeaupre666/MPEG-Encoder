object SettingsForm: TSettingsForm
  Left = 268
  Top = 166
  Width = 369
  Height = 312
  Caption = 'Encoder Settings'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 256
    Top = 239
    Width = 89
    Height = 25
    Caption = 'Cancel'
    TabOrder = 0
  end
  object Button2: TButton
    Left = 160
    Top = 239
    Width = 89
    Height = 25
    Caption = 'Ok'
    TabOrder = 1
  end
  object GroupBox2: TGroupBox
    Left = 8
    Top = 8
    Width = 145
    Height = 97
    Caption = 'Framerate'
    TabOrder = 2
    object RadioButton3: TRadioButton
      Left = 8
      Top = 24
      Width = 73
      Height = 17
      Caption = 'Automatic'
      TabOrder = 0
    end
    object RadioButton7: TRadioButton
      Left = 8
      Top = 48
      Width = 73
      Height = 17
      Caption = 'Custom'
      Checked = True
      TabOrder = 1
      TabStop = True
    end
    object ComboBox2: TComboBox
      Left = 24
      Top = 64
      Width = 105
      Height = 21
      ItemHeight = 13
      TabOrder = 2
    end
  end
  object GroupBox3: TGroupBox
    Left = 160
    Top = 8
    Width = 185
    Height = 225
    Caption = 'Resolution'
    TabOrder = 3
    object Label4: TLabel
      Left = 24
      Top = 124
      Width = 28
      Height = 13
      Caption = 'Width'
    end
    object Label3: TLabel
      Left = 92
      Top = 146
      Width = 7
      Height = 13
      Caption = 'X'
    end
    object Label5: TLabel
      Left = 104
      Top = 124
      Width = 31
      Height = 13
      Caption = 'Height'
    end
    object RadioButton1: TRadioButton
      Left = 8
      Top = 24
      Width = 89
      Height = 17
      Caption = 'Keep Original'
      Checked = True
      TabOrder = 0
      TabStop = True
    end
    object RadioButton2: TRadioButton
      Left = 8
      Top = 100
      Width = 73
      Height = 17
      Caption = 'Custom'
      TabOrder = 1
    end
    object RadioButton4: TRadioButton
      Left = 8
      Top = 52
      Width = 57
      Height = 17
      Caption = 'Preset'
      TabOrder = 2
    end
    object SpinEdit1: TSpinEdit
      Left = 24
      Top = 140
      Width = 65
      Height = 22
      MaxValue = 0
      MinValue = 0
      TabOrder = 3
      Value = 0
    end
    object SpinEdit2: TSpinEdit
      Left = 104
      Top = 140
      Width = 65
      Height = 22
      MaxValue = 0
      MinValue = 0
      TabOrder = 4
      Value = 0
    end
    object ComboBox1: TComboBox
      Left = 24
      Top = 72
      Width = 105
      Height = 21
      ItemHeight = 13
      TabOrder = 5
    end
    object CheckBox2: TCheckBox
      Left = 24
      Top = 168
      Width = 121
      Height = 17
      Caption = 'Align to 16 bytes Fix'
      TabOrder = 6
    end
    object CheckBox3: TCheckBox
      Left = 24
      Top = 192
      Width = 105
      Height = 17
      Caption = 'Adjust Size Fix'
      TabOrder = 7
    end
  end
  object GroupBox4: TGroupBox
    Left = 8
    Top = 112
    Width = 145
    Height = 97
    Caption = 'Bitrate'
    TabOrder = 4
    object ComboBox3: TComboBox
      Left = 24
      Top = 64
      Width = 105
      Height = 21
      ItemHeight = 13
      TabOrder = 0
    end
    object RadioButton5: TRadioButton
      Left = 8
      Top = 24
      Width = 73
      Height = 17
      Caption = 'Automatic'
      TabOrder = 1
    end
    object RadioButton6: TRadioButton
      Left = 8
      Top = 44
      Width = 73
      Height = 17
      Caption = 'Custom'
      Checked = True
      TabOrder = 2
      TabStop = True
    end
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 216
    Width = 145
    Height = 49
    Caption = 'Misc'
    TabOrder = 5
    object CheckBox1: TCheckBox
      Left = 24
      Top = 20
      Width = 89
      Height = 17
      Caption = 'Multithreaded'
      TabOrder = 0
    end
  end
end
