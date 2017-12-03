object OpenDirectoryForm: TOpenDirectoryForm
  Left = 192
  Top = 124
  BorderStyle = bsDialog
  Caption = 'Select Directory'
  ClientHeight = 360
  ClientWidth = 416
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
  object DirectoryListBox: TDirectoryListBox
    Left = 8
    Top = 8
    Width = 400
    Height = 289
    ItemHeight = 16
    TabOrder = 0
  end
  object DriveComboBox: TDriveComboBox
    Left = 8
    Top = 304
    Width = 400
    Height = 19
    DirList = DirectoryListBox
    TabOrder = 1
  end
  object ButtonOk: TButton
    Left = 224
    Top = 328
    Width = 88
    Height = 25
    Caption = 'Ok'
    Default = True
    ModalResult = 1
    TabOrder = 2
  end
  object ButtonCancel: TButton
    Left = 320
    Top = 328
    Width = 88
    Height = 25
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 3
  end
end
