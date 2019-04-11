#include <HDC1000.h>
#include <SakuraIO.h>

SakuraIO_I2C sakuraio;
HDC1000 hdc1000;

uint32_t cnt = 0;

void setup() {
  hdc1000.begin();
  Serial.begin(9600);
  Serial.print("Waiting to come online");
  for(;;){
    if( (sakuraio.getConnectionStatus() & SAKURAIO_CONNECTION_STATUS_MASK) != 0x00 ) break;
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
}

void loop() {
  cnt++;
  Serial.println(cnt);

  float temp = hdc1000.getTemperature();

  if (temp == HDC1000_ERROR_CODE) {
    hdc1000.begin(); // retry once
    temp = hdc1000.getTemperature();
  }
  Serial.print("Temperature: ");
  Serial.println(temp);

  float humi = hdc1000.getHumidity();

  if (humi== HDC1000_ERROR_CODE) {
    hdc1000.begin(); // retry once
    humi = hdc1000.getHumidity();
  }

  Serial.print("Humidity: ");
  Serial.println(humi);

  if(sakuraio.enqueueTx(0,temp) != CMD_ERROR_NONE){
    Serial.println("[ERR] enqueue error");
  }
  if(sakuraio.enqueueTx(1,humi) != CMD_ERROR_NONE){
    Serial.println("[ERR] enqueue error");
  }
  if(sakuraio.enqueueTx(2,cnt) != CMD_ERROR_NONE){
    Serial.println("[ERR] enqueue error");
  }
  sakuraio.send();
  delay(5000);

  uint8_t available;
  uint8_t queued;
  if(sakuraio.getTxQueueLength(&available, &queued) != CMD_ERROR_NONE){
    Serial.println("[ERR] get tx queue length error");
  }

  Serial.print("Available :");
  Serial.print(available);
  Serial.print(" Queued :");
  Serial.println(queued);
}
