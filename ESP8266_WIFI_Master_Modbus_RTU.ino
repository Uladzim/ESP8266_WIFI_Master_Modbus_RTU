#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

WiFiUDP Udp;
const char* ssid = "_VLADIMIR";			//
const char* password = "+375445970587";	//

unsigned int MODBUS_SPEED = 57600;
unsigned int UDP_PORT = 4210;
char incomingPacket[255] = {0};

char crc_16H = 0x00;
char crc_16L = 0x00;
int one = 0;
int two = 0;
int counter = 0;
int counterSymbol = 0;
unsigned char clear[128] = {0};
void toCheckAndSend(char _a[]);
void toClearData(void);
char toHex(char _a, char _b);
void toCRC_16HL(unsigned char*, int);
void toSend(void);

void setup(){
  Serial.begin(MODBUS_SPEED);
  Serial.println();
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  Udp.begin(UDP_PORT);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), UDP_PORT);
}
void loop(){
  int packetSize = Udp.parsePacket();
  if (packetSize){
    int len = Udp.read(incomingPacket, 255);
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    if(len > 0){
		incomingPacket[len] = 0;
		toCheckAndSend(incomingPacket); 
    }
    Udp.endPacket();
  }
}
void toCheckAndSend(char _symbol[]){
  toClearData();
  for(int i = 0; _symbol[i] != 0; i++){
    if((counter == 0) && (_symbol[i-1] == ' ') && (_symbol[i+1] == 0)){
        clear[counterSymbol] = toHex('0', _symbol[i]);
        counterSymbol++;
        counter = 0;
        one = 0;
        two = 0;
    }
    if((counter == 0) && (_symbol[i] != ' ')){
      one = _symbol[i];
      counter = 1;
    }else if((counter == 1) && (_symbol[i] != ' ')){
      clear[counterSymbol] = toHex(one, _symbol[i]);
      counterSymbol++;
      counter = 0;
      one = 0;
      two = 0;
    }else if((counter == 1) && (_symbol[i] == ' ')){
      clear[counterSymbol] = toHex('0', one);
      counterSymbol++;
      counter = 0;
      one = 0;
      two = 0;
    }else if((counter == 1) && (_symbol[i] == 0)){
      clear[counterSymbol] = toHex('0', one);
      counterSymbol++;
      counter = 0;
      one = 0;
      two = 0;
    }else{
      //printf("Show_Space_between\n");
    }
  }
  toCRC_16HL(clear, counterSymbol);
  toSend();
}
void toClearData(void){
  one = 0;
  two = 0;
  counter = 0;
  counterSymbol = 0;
  crc_16H = 0x00;
  crc_16L = 0x00;
  for(int i=0; i<128; i++){
    clear[i] = {0};
  }
}
char toHex(char _a, char _b){
  int hight = 0;
  int low = 0;
  if(_a >= 'a' && _a <= 'f'){_a -= 32;}
  if(_b >= 'a' && _b <= 'f'){_b -= 32;}
  if(_a >= 'A' && _a <= 'F'){
    //'f' == 70; 70-55 = 15; F==15;
    hight = ((_a - 55) * 16);
  }else if(_a >= '0' && _a <= '9'){
    hight = ((_a - 48) * 16);
  }else{
    //printf("Error: Symbol hight register\n");
    //return -1;
  }
  if(_b >= 'A' && _b <= 'F'){
    low = ((_b - 64) + 9);
  }else if(_b >= '0' && _b <= '9'){
    low = (_b - 48);
  }else{
    //printf("Error: Symbol low register\n");
    //return -1;
  }
  //printf("H:%d, L:%d\n", hight, low);
  return hight+low;
}
void toCRC_16HL(unsigned char *_data, int _length){
  int j = 0;
  unsigned int reg_crc = 0xffff;
  while(_length--){
    reg_crc ^= *_data++;
    for(j = 0; j < 8; j++){
      if(reg_crc & 0x01){
        reg_crc = (reg_crc >> 1) ^ 0xa001;
      }else{
        reg_crc = reg_crc >> 1;
      }
    }
  }
  clear[counterSymbol++] = crc_16H = reg_crc;
  clear[counterSymbol++] = crc_16L = reg_crc >> 8;
}
void toSend(void){
	for(int i=0; i<counterSymbol; i++){
        Serial.printf("%c", clear[i]);
	}
}