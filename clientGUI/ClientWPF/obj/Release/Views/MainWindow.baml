   M S B A M L   `   `   `  ����   	ClientWPF    ClientWPF.MainWindow   \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml       � Title$  Client��$	��700��$
��1000�� � 	Resources    DataGridCellStyle1 �� 
TargetType*  $System.Windows.Controls.DataGridCell �� Property �� Value �� 
TargetType J� SnapsToDevicePixels �� Triggers P� Property	 P� Value&   System.Windows.Controls.DataGrid
   FocusBorderBrushKey ]� 
Conditions �� Property �� Value��%  &	        �� "  .���  � ��
 ���� ��  � ��
 ���� ��  � ��$ 1q���  � �� �� "  .R��� #��z��#��z��#��z�� . ._��� # z  P�  �@ 
IsSelected	 . . ���  � ��
 ��  ���  � ��
 �������  � ��# � ��P�  �@J�IsKeyboardFocusWithin	 . . ���  � ��# � 
 ]�  ��  �@ 
IsSelected . ��  �@��IsSelectionActive .  ./���  � ��
 ��  ���  � ��
 �������  � ��# � /�P�  � {�	 .  . ���  � ��# � ��.��� $��0,0,0,0q� � ColumnDefinitions �� $��* � � RowDefinitions �� $��25 ��� $��53 ��� $��25 ��� $��25 ��� $��1* ��� $��25 ��� $��25 ��� $��2* ��� $��25 ��� $��10 �.+��� $��6,0,6,0q�$��Client's state��$
��Bold"�  -   $��dgClient��$��
10,0,10,10q�   AutoGenerateColumns .  $��1��   
IsReadOnly .     RowHeaderWidth$ 0Q����� *
ClientData+   Columns 0  *System.Windows.Controls.DataGridTextColumn  ,  &System.Windows.Controls.DataGridColumn   Header$ State��   Width5  /System.Windows.Controls.DataGridLengthConverter$ *    
IsReadOnly . 1  +System.Windows.Controls.DataGridBoundColumn   Binding �� *ClientState+   ElementStyle �� " �� P�  � ��$	 	CONNECTED��. ���  � ��
 ����P�  � ��$	 INITIALIZED��. ���  � ��
 ��ح�  $ 	Server IP��$ *  �� *IpClientConnected+  $ Server port��$ *  �� *PortClientConnected+�� $��
Disconnect��$	��105��$��20��$��0,0,10,0q�$��2��$��Right=����� *DisconnectClient+�� $��Connect��$	��105��$��20��$��	0,0,125,0q�$��2��$��Right=����� *ConnectClient+�� $��6,0,6,0q�$��Available readers��$��3��$
��Bold"�  -   $��	dgReaders��$��
10,0,10,10q�$��4�� .   . $ 0Q� �� AlternationCount$ 2��   AlternatingRowBackground
 �����   	CellStyle# �    �� SelectedItem �� *SelectedReader+���� *ReadersList+   $ 	Reader ID��$ *  �� *
ReaderID+  $ Reader name��$ *  �� *
ReaderName+�� $��Reload readers��$	��105��$��20��$��0,0,10,0q�$��5��$��Right=����� *ReloadReaders+�� $��6,21,6,4q�$��Resquests/responses��$��5��$
��Bold"�$��2��  -   $��dgLogs����. $
��Auto=�$
��Auto=�$��
10,0,10,10q�$��7�� .   . $ 0Q�$ 2��
 ��������� *
LogsList+   $
 Type��$	 100  �� *	LogType+  $
 Time��$	 150  �� *	LogTime+  $
 Data��$
 Auto  �� *	LogData+�� $��
Clear logs��$	��105��$��20��$��0,0,10,0q�$��8��$��Right=����� *	ClearLogs+