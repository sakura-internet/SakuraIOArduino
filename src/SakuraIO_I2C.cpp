#include <Wire.h>
#include <Arduino.h>
#include "SakuraIO.h"
#include "SakuraIO/debug.h"

#define SAKURAIO_SLAVE_ADDR 0x4F

#define MODE_IDLE  0x00
#define MODE_WRITE 0x01
#define MODE_READ  0x02

void SakuraIO_I2C::begin(){
  mode = MODE_IDLE;
}

void SakuraIO_I2C::end(){
  switch(mode){
    case MODE_WRITE:
      Wire.endTransmission();
      break;
    case MODE_READ:
      Wire.requestFrom(SAKURAIO_SLAVE_ADDR, 1, true);
      if( Wire.available() ) Wire.read();
      break;
  }

  mode = MODE_IDLE;
}

void SakuraIO_I2C::sendByte(uint8_t data){
  if(mode != MODE_WRITE){
    dbgln("beginTr");
    Wire.beginTransmission(SAKURAIO_SLAVE_ADDR);
    mode = MODE_WRITE;
  }
  dbg("Write=");
  dbgln(data, HEX);
  Wire.write(data);
}


uint8_t SakuraIO_I2C::startReceive(uint8_t length){
  dbgln("endTransmission");
  Wire.endTransmission();
  dbg("requestForm=");
  dbgln(length);
  Wire.requestFrom((uint8_t)SAKURAIO_SLAVE_ADDR, length, (uint8_t)true);
  mode = MODE_READ;
}

uint8_t SakuraIO_I2C::receiveByte(bool stop){
  uint8_t ret = 0;
  if( Wire.available() ) ret = Wire.read();
  if(stop){
    mode = MODE_IDLE;
  }
  dbg("Read=");
  dbgln(ret, HEX);
  return ret;
}

SakuraIO_I2C::SakuraIO_I2C(){
  Wire.begin();
  mode = MODE_IDLE;
}
