object RenderForm: TRenderForm
  Left = 559
  Top = 80
  Width = 503
  Height = 510
  Caption = 'RenderForm'
  Color = clGray
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  DesignSize = (
    487
    472)
  PixelsPerInch = 96
  TextHeight = 13
  object MainPanel: TPanel
    Left = 8
    Top = 8
    Width = 471
    Height = 438
    Anchors = [akLeft, akTop, akRight, akBottom]
    Color = 5263440
    TabOrder = 1
    DesignSize = (
      471
      438)
    object FileProgressGauge: TGauge
      Left = 8
      Top = 367
      Width = 455
      Height = 19
      Anchors = [akLeft, akRight, akBottom]
      Progress = 0
    end
    object TotalProgressGauge: TGauge
      Left = 8
      Top = 343
      Width = 455
      Height = 19
      Anchors = [akLeft, akRight, akBottom]
      Progress = 0
    end
    object RenderPanel: TPanel
      Left = 8
      Top = 8
      Width = 455
      Height = 329
      Anchors = [akLeft, akTop, akRight, akBottom]
      BevelOuter = bvLowered
      Color = 3355443
      TabOrder = 0
    end
    object ButtonCancel: TButton
      Left = 309
      Top = 395
      Width = 153
      Height = 33
      Anchors = [akRight, akBottom]
      Caption = 'Cancel'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -24
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ModalResult = 2
      ParentFont = False
      TabOrder = 1
      OnClick = ButtonCancelClick
    end
    object ButtonPause: TButton
      Left = 148
      Top = 395
      Width = 153
      Height = 33
      Anchors = [akRight, akBottom]
      Caption = 'Pause'
      Enabled = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -24
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 2
    end
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 453
    Width = 487
    Height = 19
    Color = 9474192
    Panels = <
      item
        Text = 'File'
        Width = 100
      end
      item
        Width = 100
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
end
