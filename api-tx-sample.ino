#include <SoftwareSerial.h>

#define RXPIN 10
#define TXPIN 11

#define DESTADDR 0x0013a20040e8754d

SoftwareSerial SSerial = SoftwareSerial(RXPIN,TXPIN);

uint8_t buf[64];
uint8_t id=0;
void sendMsg(uint64_t addr64,uint8_t* msg,uint8_t len){
  buf[0]=0x7e;
  buf[1]=((len+14) & 0xff00)>>8;
  buf[2]=(len+14) & 0xff;
  buf[3]=0x10;
  buf[4]=id;
  for(int i=0;i<8;i++){
    buf[i+5]=(addr64 >> (7 - i) *8) & 0xff;
  }
  buf[13]=0xff;
  buf[14]=0xfe;
  buf[15]=0x00;
  buf[16]=0x00;
  for(int i=0;i<len;i++){
    buf[i+17]=msg[i];
  }
  uint16_t sum = 0;
  for(int i=3;i<17+len;i++){
    sum += buf[i];
  }
  sum = sum & 0xff;
  sum = 0xff - sum;
  buf[17+len]=sum;
  for(int i=0;i<17+len+1;i++){
    Serial.println(buf[i],HEX);
    SSerial.write(buf[i]);
  }
  id++;
}

void setup() {
  // put your setup code here, to run once:
  SSerial.begin(9600);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  sendMsg(DESTADDR,"test",4);
  delay(1000);
}
