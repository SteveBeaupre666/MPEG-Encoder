object MainForm: TMainForm
  Left = 78
  Top = 157
  Width = 1155
  Height = 481
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
  DesignSize = (
    1139
    443)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 332
    Width = 64
    Height = 13
    Caption = 'Output Folder'
  end
  object FileProgressGauge: TGauge
    Left = 488
    Top = 376
    Width = 642
    Height = 19
    Anchors = [akLeft, akRight, akBottom]
    Progress = 0
  end
  object TotalProgressGauge: TGauge
    Left = 488
    Top = 398
    Width = 642
    Height = 19
    Anchors = [akLeft, akRight, akBottom]
    Progress = 0
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
  object RenderPanel: TPanel
    Left = 488
    Top = 8
    Width = 642
    Height = 362
    Anchors = [akLeft, akTop, akRight, akBottom]
    BevelOuter = bvLowered
    TabOrder = 6
    object RenderWindow: TPanel
      Left = 1
      Top = 1
      Width = 640
      Height = 360
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 0
    end
  end
  object ButtonConvert: TButton
    Left = 8
    Top = 376
    Width = 257
    Height = 41
    Anchors = [akLeft, akBottom]
    Caption = 'Convert'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -24
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 7
    OnClick = ButtonConvertClick
  end
  object CheckBoxRender: TCheckBox
    Left = 376
    Top = 304
    Width = 97
    Height = 17
    Caption = 'Render Frames'
    Checked = True
    State = cbChecked
    TabOrder = 8
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 424
    Width = 1139
    Height = 19
    Panels = <
      item
        Width = 150
      end
      item
        Width = 150
      end
      item
        Width = 150
      end
      item
        Width = 150
      end
      item
        Width = 150
      end
      item
        Width = 150
      end>
    SimplePanel = False
  end
  object ButtonCancel: TButton
    Left = 272
    Top = 376
    Width = 209
    Height = 41
    Caption = 'Cancel'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -24
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 10
    OnClick = ButtonCancelClick
  end
  object RenderTimer: TTimer
    Interval = 50
    OnTimer = RenderTimerTimer
    Left = 24
    Top = 24
  end
end
