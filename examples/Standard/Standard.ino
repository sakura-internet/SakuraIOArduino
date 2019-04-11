#include <SakuraIO.h>

//SakuraIO_SPI sakuraio(10);
SakuraIO_I2C sakuraio;

void setup() {
  Serial.begin(9600);
  Serial.print("Waiting to come online");
  for(;;){
    if( (sakuraio.getConnectionStatus() & SAKURAIO_CONNECTION_STATUS_MASK) != 0x00 ) break;
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
}

uint8_t counter = 0;

void loop() {
  counter++;

  Serial.println("");
  Serial.println(counter);

  uint8_t request[33] = {};
  uint8_t response[33] = {};

  // Echoback
  request[0] = counter;
  sakuraio.echoback(1, request, response);
  Serial.print(request[0]);
  Serial.print(" -> ");
  Serial.println(response[0]);

  // Unixtime
  uint32_t unixtime = (uint32_t)(sakuraio.getUnixtime()/1000UL);
  Serial.print("Unixtime ");
  Serial.println(unixtime);

  // ProductID
  uint16_t pid = sakuraio.getProductID();
  Serial.print("PID ");
  Serial.println(pid);

  // UniqueID
  sakuraio.getUniqueID((char *)response);
  Serial.print("UID ");
  Serial.println((char *)response);

  // Version
  sakuraio.getFirmwareVersion((char *)response);
  Serial.print("Ver ");
  Serial.println((char *)response);

  // Connection Status
  uint8_t connectionStatus = sakuraio.getConnectionStatus();
  Serial.print("Status ");
  Serial.println(connectionStatus);

  //getSignalQuality
  uint8_t signalQuality = sakuraio.getSignalQuality();
  Serial.print("Quality ");
  Serial.println(signalQuality);


  // Tx Queue
  uint8_t ret;
  ret = sakuraio.enqueueTx((uint8_t)0, (int32_t)counter);
  Serial.print("Enqueue ");
  Serial.println(ret);

  uint8_t avail;
  uint8_t queued;
  sakuraio.getTxQueueLength(&avail, &queued);
  Serial.print("Tx Available=");
  Serial.print(avail);
  Serial.print(" Queued=");
  Serial.println(queued);

  if(queued >= 30){
    ret = sakuraio.clearTx();
    Serial.print("Clear ");
    Serial.println(ret);
  }else if(queued >= 5){
    ret = sakuraio.send();
    Serial.print("Send ");
    Serial.println(ret);
  }

  // Rx Queue
  sakuraio.getRxQueueLength(&avail, &queued);
  Serial.print("Rx Available=");
  Serial.print(avail);
  Serial.print(" Queued=");
  Serial.println(queued);

  for(uint8_t i=0; i<queued; i++){
    uint8_t channel;
    uint8_t type;
    uint8_t values[8];
    int64_t offset;
    ret = sakuraio.dequeueRx(&channel, &type, values, &offset);
    Serial.print("Dequeue ");
    Serial.print(ret);
    if(ret == 0x01){
      Serial.print(" ch="); Serial.print(channel);
      Serial.print(" type="); Serial.print((char)type);
      Serial.print(" values=[");
      for(uint8_t b=0; b<7; b++){
        Serial.print(values[b]);
        Serial.print(" ");
      }
      Serial.print(values[7]);
      Serial.print("] offset="); Serial.println((int32_t)offset);
    }else{
      Serial.println(" ERROR");
    }
  }

  delay(5000);
}
