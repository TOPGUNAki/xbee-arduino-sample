#include <SoftwareSerial.h>

#define RXPIN 10
#define TXPIN 11

#define FRM_ID_TX_REQ 0x10
#define FRM_ID_RECEIVE_FRM 0x90

#define TGTADDR 0x7766554433221100

SoftwareSerial SSerial = SoftwareSerial(RXPIN,TXPIN);

uint8_t buf[64];
uint8_t cnt;
uint16_t len;


void setup() {
  // put your setup code here, to run once:
  SSerial.begin(9600);
  Serial.begin(9600);

  while(!Serial);
  sendMsg(TGTADDR,"start test",10);
}

void sendMsg(uint64_t addr,uint8_t* str,int len){
  static uint8_t buf[64];
  buf[0] = 0x7e;
  buf[1] = ((len + 14) & 0xff00) << 8;
  buf[2] = ((len + 14)) & 0xff;
  buf[3] = FRM_ID_TX_REQ;
  buf[4] = 0x01;
  for (int i = 0;i < 8;i++){
    buf[5+i]=(addr >> (8 * (7 - i))) & 0xff;
  }
  buf[13] = 0xff;
  buf[14] = 0xfe;
  buf[15] = 0x00;
  buf[16] = 0x00;
  for (int i = 0;i < len;i++){
    buf[17+i]=str[i];
  }
  int sum = 0;
  for (int i = 0;i < len + 14;i++){
    sum = sum + buf[3+i];
  }
  sum = sum & 0xff;
  sum = 0xff - sum;
  buf[17+len] = sum;

  for (int i = 0;i<len+18;i++){
    SSerial.write(buf[i]);
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  buf[0]=SSerial.read();
  if(buf[0]==0x7e){
    while(SSerial.available()<2);
    buf[1]=SSerial.read();
    buf[2]=SSerial.read();
    len=buf[1]*256+buf[2];
    while(SSerial.available()<len+1);
    for(cnt=3;cnt<3+len+1;cnt++){
      buf[cnt]=SSerial.read();
    }

    uint16_t sum = 0;
    for(cnt=3;cnt<3+len;cnt++){
      sum += buf[cnt];
    }
    sum &= 0xff;
    sum = 0xff - sum;
    if(sum == buf[3+len]){
      sendMsg(TGTADDR,&buf[15],len-12);
      for(cnt=15;cnt<3+len;cnt++){
        Serial.write(buf[cnt]);
      }
    }
  }
}
