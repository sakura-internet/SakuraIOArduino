#include <SPI.h>
#include "SakuraIO.h"
#include "SakuraIO/debug.h"

#if SPI_HAS_TRANSACTION
static SPISettings settings;
#endif

void SakuraIO_SPI::begin(){
#if SPI_HAS_TRANSACTION
  SPI.beginTransaction(settings);
#endif
  dbgln("CS=0");
  digitalWrite(cs, LOW);
}

void SakuraIO_SPI::end(){
  dbgln("CS=1");
  digitalWrite(cs, HIGH);
#if SPI_HAS_TRANSACTION
  SPI.endTransaction();
#endif
  delayMicroseconds(20);
}

void SakuraIO_SPI::sendByte(uint8_t data){
  delayMicroseconds(20);
  dbg("Send=");
  dbgln(data, HEX);
  SPI.transfer(data);
}


uint8_t SakuraIO_SPI::receiveByte(bool stop){
  uint8_t ret;
  delayMicroseconds(20);
  ret = SPI.transfer(0x00);
  dbg("Recv=");
  dbgln(ret, HEX);
  return ret;
}

SakuraIO_SPI::SakuraIO_SPI(int _cs){
  cs = _cs;
  SPI.begin();
#if SPI_HAS_TRANSACTION
  settings = SPISettings(4000000, MSBFIRST, SPI_MODE0); // 4MHz, MSB First, SPI mode 0
#endif
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);
}
