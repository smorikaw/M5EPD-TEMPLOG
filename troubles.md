内蔵I2Cで"Wire"を使っているのでPORT AのI2CはWire1.begin(25,32)<BR>
I2Cが5Vなので3.3Vものに接続するときはレベル変換が必要<BR>
Ardiino IDEの"ボード”設定の切り替えを忘れる<BR>
M5.shutdownが電源が接続されていると無視される？<BR>
deepsleepも電源が接続されているときとバッテリー駆動時で動作が違う<BR>
FILE_WRITEでは追記されずFILE_APPENDが必要<BR>
