   M S B A M L   `   `   `  ����   	ServerWPF    ServerWPF.MainWindow   \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml       � Title$  Server��$	��750��$
��1610�� � 	Resources    DataGridCellStyle1 �� 
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
 �������  � ��# � /�P�  � {�	 .  . ���  � ��# � ��.��� $��0,0,0,0q� � ColumnDefinitions �� $��* ��� $��* � � RowDefinitions �� $��25 ��� $��53 ��� $��25 ��� $��25 ��� $��1* ��� $��25 ��� $��2* ��� $��25 ��� $��10 �.+��� $��6,0,6,0q�$��Server's state��$
��Bold"�  -   $��dgServer��$��
10,0,10,10q�   AutoGenerateColumns .  $��1��$��2��   
IsReadOnly .     RowHeaderWidth$ 0Q����� *
ServerData+   Columns 0  *System.Windows.Controls.DataGridTextColumn  ,  &System.Windows.Controls.DataGridColumn   Header$ State��   Width5  /System.Windows.Controls.DataGridLengthConverter$ *    
IsReadOnly . 1  +System.Windows.Controls.DataGridBoundColumn   Binding �� *ServerState+  $ 	Server IP��$ *  �� *
ServerIP+  $ Server port��$ *  �� *
ServerPort+�� $��Start server��$	��105��$��	0,0,125,0q�$��2��$��1��$��Right=����� *StartServer+�� $��Stop server��$	��105��$��0,0,10,0q�$��2��$��1��$��Right=����� *
StopServer+�� $��6,0,6,0q�$��Connected clients��$��3��$
��Bold"�  -   $��	dgClients��   	CellStyle# �    �� AlternationCount$ 2��   AlternatingRowBackground
 �����$��
10,0,10,10q�$��4�� .   . $ 0Q� �� SelectedItem �� *SelectedClient+���� *ClientsList+   $ 	Client ID��$ *  �� *
ClientID+  $ Client name��$ *  �� *
ClientName+�� $ ��Actions on selected client��$��1��$
��Left=�$��0q�$��4��$	��Top=�$	��200��$
��Bold"��� $��1��$
��Left=�$��20��$��5,24,0,0q�$��4�� �� TextWrapping$
 Wrap=�$	��Top=�$	��150������ *CurrentCommand+�� $��Send command��$
��Left=�$��
170,24,0,0q�$��4��$	��Top=�$	��105��$��20��$��1������ *SendCommandClient+�� $��Send RF type A��$
��Left=�$��
290,24,0,0q�$��4��$	��Top=�$	��105��$��20��$��1������ *	SendTypeA+�� $��Send RF type B��$
��Left=�$��
410,24,0,0q�$��4��$	��Top=�$	��105��$��20��$��1������ *	SendTypeB+�� $��Send RF type F��$
��Left=�$��
530,24,0,0q�$��4��$	��Top=�$	��105��$��20��$��1������ *	SendTypeF+�� $��
Is alive ?��$
��Left=�$��5,53,0,0q�$��4��$	��Top=�$	��105��$��1������ *
EchoClient+�� $��Diagnose��$
��Left=�$��5,83,0,0q�$��4��$	��Top=�$	��105��$��1������ *DiagnoseClient+�� $��Restart target��$
��Left=�$��	5,112,0,0q�$��4��$	��Top=�$	��105��$��1������ *RestartTarget+�� $��
Disconnect��$
��Left=�$��	5,142,0,0q�$��4��$	��Top=�$	��105��$��1������ *
StopClient+�� $��
Cold reset��$
��Left=�$��
170,53,0,0q�$��4��$	��Top=�$	��105��$��20��$��1������ *	ColdReset+�� $��
Warn reset��$
��Left=�$��
170,83,0,0q�$��4��$	��Top=�$	��105��$��20��$��1������ *	WarmReset+�� $��Power OFF field��$
��Left=�$��170,112,0,0q�$��4��$	��Top=�$	��105��$��20��$��1������ *PowerOFFField+�� $��Power ON field��$
��Left=�$��170,142,0,0q�$��4��$	��Top=�$	��105��$��20��$��1������ *PowerONField+�� $��6,0,6,0q�$��Resquests/responses��$��5��$
��Bold"�  -   $��dgLogs����. $
��Auto=�$
��Auto=�$��
10,0,10,10q�$��6��$��2�� .   . $ 0Q�$ 2��
 ��������� *
LogsList+   $ 	Client ID��$ 60  �� *
IdClient+  $ Client name��$	 100  �� *
NameClient+  $
 Time��$	 120  �� *	LogTime+  $ Request��$	 200  �� *	Request+  $ Response��$	 400  �� *
Response+  $ Server code��$
 Auto  �� *ErrServerCode+  $ Server description��$
 Auto  �� *ErrServerDescription+  $ Client code��$
 Auto  �� *ErrClientCode+  $ Client description��$
 Auto  �� *ErrClientDescription+  $ Terminal code��$
 Auto  �� *ErrTerminalCode+  $ Terminal description��$
 Auto  �� *ErrTerminalDescription+  $ 	Card code��$
 Auto  �� *ErrClientCode+  $ Card description��$
 Auto  �� *ErrClientDescription+�� $��
Clear logs��$	��105��$��0,0,10,0q�$��7��$��1��$��Right=����� *	ClearLogs+