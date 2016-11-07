/**
   Edit By Callor 2016. 11. 1

   ElECREAK 사의 esp8266 Serial Wifi Module을
   아두이노 UNO에서 사용하기 위한 프로젝트
      1. UNO의 SoftwareSerial을 통해 Wifi 통신
      2. Station Mode 만을 테스트
      3. 서버에 PUT method 방식으로 값을 넘기고
      4. 결과를 시리얼 모니터로 확인

   ELECREAK사의 esp8266 Serial Wifi Module은
      1. 기본 속도가 9600으로 세팅되어 있어 UNO의 SofrwareSerial을 통해 제어 할 수 있다.
      2. 아두이노 UNO에 Vcc(3.3v)와 GND, TX, RX 핀만을 연결하고 Wifi를 사용할 수 있다.

*/




#include <ELECFREAKS_ESP8266.h>


#define Debug Serial // UNO의 0, 1의 Serial의 이름을 Debug로 설정

#define DBT(message)      Debug.print(message)
#define DBG(message)      Debug.println(message)
#define DBGW(message)     Debug.write(message)

#define SSID "MyAP"
#define PASSWD "MyPasswd"

#define UDP 0
#define TCP 1

#define wSTX 5
#define wSRX 6

ELEC_Wifi wifi(wSTX, wSRX) ;

void setup() {

  delay(100);
  DBG("Start esp8266...");
  wifi.begin() ;
  wifi.setStation() ;

  Serial.print("AP : ");
  wifi.joinAP(SSID, PASSWD) ;
  DBG(wifi.showJAP());

  Serial.print("Status : ");
  DBG(wifi.showStatus());

  Serial.print("IP : ");
  DBG(wifi.showIP());


}
int last = millis();
long interval = 0 ;
void loop() {

  String ip = "192.168.0.110";
  int port = 3000;
  wifi.connectServer(TCP, ip, port);


  if (millis() > (last + interval)) {
    DBG("Send..") ;

    String strValue = String(millis());
    DBG(strValue);

    //    while(wifi.Send("PUT /insert/HT001/50/20/" + strValue + " HTTP/1.0\r\n\r\n")) {
    //    while(!wifi.Send("HEAD /time HTTP/1.1\r\nHOST: 192.168.0.87:3000\r\nConnection: close\r\n\r\n")) {
    while (!wifi.Send("HEAD /time/" + strValue + " HTTP/1.1\r\n\r\n")) {

      DBG("Re Connect..") ;
      wifi.connectServer(TCP, ip, port);
      DBG("Re Send..") ;
    }
    last = millis();
    interval = 100000;
  }

  char message[320];

  if (wifi.ReceiveMessage(message))
  {
    DBG(message);
  }

}