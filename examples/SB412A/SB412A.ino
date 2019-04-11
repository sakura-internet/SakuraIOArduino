#include <SakuraIO.h>
SakuraIO_I2C sakuraio;

const int ledPin = 13;         // LED接続ピン（定数）
const int analogInPin = A0;    // アナログ入力ピン（定数）
uint32_t  ad;                  // AD値（変数）
uint32_t cnt = 0 ;

void setup()
{
  pinMode(ledPin, OUTPUT);     // LEDのピンを出力に設定
  Serial.print("Waiting to come online");
  Serial.begin(9600);
  for (;;) {
    if ( (sakuraio.getConnectionStatus() & SAKURAIO_CONNECTION_STATUS_MASK) != 0x00 ) break;
    Serial.print(".");
    delay(100);
  }
}

void loop()
{
  cnt++;
  Serial.println(cnt);
  ad = analogRead(analogInPin);         // AD値を取得
  Serial.println(ad);
  sakuraio.enqueueTx(0, ad);
  if ( ad == 0 ) {
    digitalWrite(ledPin, LOW);          // AD値が0ならLEDを消灯
  } else {
    digitalWrite(ledPin, HIGH);         // AD値が0以外ならLEDを点灯
  }
  sakuraio.enqueueTx(1, cnt);
  sakuraio.send();
  delay(1000);
}
