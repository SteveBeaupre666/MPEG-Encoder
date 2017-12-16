object MainForm: TMainForm
  Left = 197
  Top = 140
  Width = 510
  Height = 465
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  Caption = 'MPEG Converter'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 332
    Width = 64
    Height = 13
    Caption = 'Output Folder'
  end
  object FilesListBox: TListBox
    Left = 8
    Top = 8
    Width = 473
    Height = 281
    ItemHeight = 13
    TabOrder = 0
  end
  object ButtonAdd: TButton
    Left = 8
    Top = 300
    Width = 105
    Height = 25
    Caption = 'Add'
    TabOrder = 1
    OnClick = ButtonAddClick
  end
  object ButtonRemove: TButton
    Left = 120
    Top = 300
    Width = 105
    Height = 25
    Caption = 'Remove'
    TabOrder = 2
    OnClick = ButtonRemoveClick
  end
  object ButtonClear: TButton
    Left = 232
    Top = 300
    Width = 137
    Height = 25
    Caption = 'Clear All'
    TabOrder = 3
    OnClick = ButtonClearClick
  end
  object EditOutputFolder: TEdit
    Left = 8
    Top = 348
    Width = 361
    Height = 21
    ReadOnly = True
    TabOrder = 4
    Text = 'C:\New Programming Folder\Video Processing\Video'
  end
  object ButtonBrowse: TButton
    Left = 376
    Top = 348
    Width = 105
    Height = 21
    Caption = 'Browse...'
    TabOrder = 5
    OnClick = ButtonBrowseClick
  end
  object ButtonConvert: TButton
    Left = 8
    Top = 380
    Width = 473
    Height = 41
    Caption = 'Convert'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -24
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 6
    OnClick = ButtonConvertClick
  end
end
