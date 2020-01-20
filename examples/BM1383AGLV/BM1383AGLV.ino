/*
  BM1383AGLV

  このスケッチはArduino Uno、sakura.ioおよびsakura.ioシールド 
  for Arduino、ロームセンサ評価キットおよびBM1383AGLVの組み合わせで
  動作確認を行っています。

  利用の際には以下のライブラリをインストールする必要があります。

  This sketch has been verified to work with the combination 
  of Arduino Uno, sakura.io (and Shield for Arduino), 
  ROHM Sensor Shield (and BM1383AGLV).

  You need to install the following library to use it.

  SakuraIO(Available from Library Manager)
  https://github.com/sakuraio/SakuraIOArduino

  BM1383AGLV
  https://www.rohm.co.jp/sensor-shield-support/pressure-sensor
 */

// ヘッダファイル指定　Including header files
#include <Wire.h>
#include <BM1383AGLV.h>
#include <SakuraIO.h>

// 変数の定義　Definition of variables
SakuraIO_I2C sakuraio;
BM1383AGLV bm1383aglv;
uint32_t cnt = 0;
byte rc;
float press = 0;
float temp = 0;

// 起動時に1回だけ実行　Run once at startup
void setup() {

  Serial.begin(9600);
  while (!Serial);
  Wire.begin();

  rc = bm1383aglv.init();

  if (rc != 0) {
    Serial.println(F("BM1383AGLV initialization failed"));
    Serial.flush();
  }

  // sakura.ioモジュールの通信確立待機　Wait for module communication to be established
  Serial.print("Waiting to come online");
  for (;;) {
    if ( (sakuraio.getConnectionStatus() & 0x80) == 0x80 ) break;
    Serial.print(".");
    delay(1000);
  }

  Serial.println("");

}

void loop() {

  // cnt値のカウントアップ　Count up cnt value
  cnt++;
  Serial.println(cnt);

  // BM1383AGLVの値取得　Get the value of BM1383AGLV
  rc = bm1383aglv.get_val(&press, &temp);
  if (rc == 0) {
    Serial.print("Atmospheric pressure = ");
    Serial.print(press);
    Serial.println(" [hPa]");
    Serial.print("Temperature =  ");
    Serial.print(temp);
    Serial.println(" [degrees Celsius]");
    Serial.println("");
  }

  // sakura.ioモジュールへの各値のキューイング　Queuing each value to module
  if (sakuraio.enqueueTx(0, cnt) != CMD_ERROR_NONE) {
    Serial.println("[ERR] enqueue error");
  }
  if (sakuraio.enqueueTx(1, press) != CMD_ERROR_NONE) {
    Serial.println("[ERR] enqueue error");
  }
  if (sakuraio.enqueueTx(2, temp) != CMD_ERROR_NONE) {
    Serial.println("[ERR] enqueue error");
  }

  // キューイングされた値の送信　Sending queued values
  sakuraio.send();

  // 待機時間の調整　Adjust wait time
  delay(5000);
}