#include <SakuraIO.h>

//SakuraIO_SPI sakuraio(10);
SakuraIO_I2C sakuraio;


uint8_t updateFirmware();

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  delay(3000);
  if(updateFirmware() == 0){
    Serial.println("Update successfull");
  }else{
    Serial.println("Update failure");
  }
}

uint8_t updateFirmware(){
  uint8_t ret;
  char version[33] = {0};

  // Check module
  uint16_t productId = sakuraio.getProductID();
  switch(productId){
    case 0x0001:
      Serial.println("SCM-LTE-Beta");
      break;
    case 0x0002:
      Serial.println("SCM-LTE-01");
      break;
    default:
      Serial.println("Please check connection");
      return 1;
  }

  delay(1000);


  // Waiting for online
  Serial.println("Waiting for online");
  while(1){
    Serial.print('.');
    Serial.flush();
    delay(2000);
    if((sakuraio.getConnectionStatus() & SAKURAIO_CONNECTION_STATUS_MASK) != 0x00){
      break;
    }
  }
  Serial.println("");
  Serial.println("Online");


  Serial.println("Get current version");
  ret = sakuraio.getFirmwareVersion(version);
  if((ret = sakuraio.getFirmwareVersion(version)) != CMD_ERROR_NONE){
    Serial.print("Error code=");
    Serial.println(ret);
    return 1;
  }
  Serial.print("Current: ");
  Serial.println(version);


  delay(1000);


  // Request unlock
  Serial.println("Unlock");
  if((ret = sakuraio.unlock()) != CMD_ERROR_NONE){
    Serial.print("Error code=");
    Serial.println(ret);
    return 1;
  }

  delay(1000);

  // Request firmware update
  Serial.println("Starting update");
  if((ret = sakuraio.updateFirmware()) != CMD_ERROR_NONE){
    Serial.print("Error code=");
    Serial.println(ret);
    return 1;
  }

  delay(1000);

  // Check update status
  uint8_t errCode = 0x00;
  Serial.println("Waiting for update");
  while(1){

    delay(1000);
    Serial.print('.');
    Serial.flush();

    uint8_t updateStatus = sakuraio.getFirmwareUpdateStatus();
    if(updateStatus == 0xff || (updateStatus & 0x80) != 0x00){
      continue;
    }else if(updateStatus == 0x00){
      // Success
      break;
    }else{
      // Error
      errCode = updateStatus & 0x3f;
      break;
    }
  }

  Serial.println("");
  switch(errCode){
    case 0x00:
      return 0;
      break;
    case 0x01:
      Serial.println("Already updated");
      return 0;
      break;
    default:
      Serial.print("Error code=");
      Serial.println(errCode);
      break;
  }

  return 1;
}



void loop() {
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
}
