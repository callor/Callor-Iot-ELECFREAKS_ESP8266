#include "Arduino.h"
#include "ELECFREAKS_ESP8266.h"


SoftwareSerial wSerial(_TXPIN_,_RXPIN_);

// 디버그 메시지 시리얼 모니터로 출력
#define DBG(message)    Serial.println(message)
#define DBGW(message)   Serial.write(message)


// 기본값은 TX 5, RX 6 번
ELEC_Wifi::ELEC_Wifi(void) {

}

// 포트를 바꾸어 초기화 하고 싶을때
ELEC_Wifi::ELEC_Wifi(byte xTX, byte xRX) {
	wSerial = SoftwareSerial(xTX, xRX) ;
}


void ELEC_Wifi::begin(void) {
      boolean result = false;
      
      wSerial.begin(9600); // wSerial 포트 속도, ELECFREAKS 8266은 기본 속도가 9600으로 세팅 되어 있다.
      Serial.begin(9600);   // 시리얼 모니터 속도 지정
      
      wSerial.flush();
      wSerial.setTimeout(3000);

      // wSerial Reboot
      wSerial.println("AT+RST");
      Serial.println("AT+RST\r\n");
      result = wSerial.find((char*)"OK");
      if(result) Serial.println("Module is ready\r\n");
      else {
            Serial.println("Module have no response\r\n");
            while(1);
      }
}

String ELEC_Wifi::checkFirm(){
    String data = "";
    wSerial.println(F("AT+GMR"));

    unsigned long start;
    start = millis();
    while (millis()-start<2000) {
      if(wSerial.available()>0) {
          char a = wSerial.read();
          data=data+a;
      }
    }

    return data;
}



/**
 *  
 */
bool ELEC_Wifi::setStation()
{
    String data;

    // 모듈을 Station Mode로 설정
    wSerial.println("AT+CWMODE=1");  
    
    unsigned long start;
    start = millis();
    while (millis()-start<2000) {
      if(wSerial.available()>0) {
          char a = wSerial.read();
          data=data+a;
      }
      // 모드 변경이 성공하거나, 이미 Station 모드일경우 true, 아니면 false
      if (data.indexOf("OK")!=-1 || data.indexOf("no change")!=-1) return true;
      if (data.indexOf("ERROR")!=-1 || data.indexOf("busy")!=-1) return false;
   }
  return true;
}

/**
 * 공유기에 접속 시도
 */

 boolean ELEC_Wifi::joinAP(String ssid , String pwd) {
 
    wSerial.print("AT+CWJAP=");

    //"ssid"
    wSerial.print("\"");    
    wSerial.print(ssid);  
    wSerial.print("\"");

    wSerial.print(",");
    
    //"pwd"
    wSerial.print("\"");      
    wSerial.print(pwd); 
    wSerial.println("\"");

	String data;
    unsigned long start;
    start = millis();
    while (millis()-start<3000) {
       if(wSerial.available()>0) {
             char a =wSerial.read();
             data=data+a;
       }
	   
       if (data.indexOf("OK")!=-1) return true;
    }
	DBG(data);


//    while (millis()-start<3000) {                            
//        if(wSerial.find("OK")==true) return true;
//    }
    return false;
}


String ELEC_Wifi::showJAP(void)
{
    wSerial.flush();
    wSerial.println("AT+CWJAP?");  
    
    String data;
    unsigned long start;
    start = millis();
    while (millis()-start<3000) {
       if(wSerial.available()>0) {
             char a =wSerial.read();
             data=data+a;
       }
       if (data.indexOf("OK")!=-1 || data.indexOf("ERROR")!=-1 ) break;
       
    }
    
    char head[4] = {0x0D,0x0A};   
    char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
    data.replace("AT+CWJAP?","");
    data.replace("+CWJAP","AP");
    data.replace("OK","");
    data.replace(tail,"");
    data.replace(head,"");
      
    return data;
}


/**
 * 모듈 상태 체크
 * 
 * return
 *    2: Got IP
 *    3: Connected
 *    4: Disconnected
 */

String ELEC_Wifi::showStatus(void) {
    wSerial.println("AT+CIPSTATUS");  

    String data;
    unsigned long start;
    start = millis();
    while (millis()-start<3000) {
       if(wSerial.available()>0) {
          char tc = wSerial.read();
          data=data+tc;
       }
       if (data.indexOf("OK")!=-1) break;
    }

    char head[4] = {0x0D,0x0A};   
    char tail[7] = {0x0D,0x0A,0x0D,0x0A};        
    data.replace("AT+CIPSTATUS","");
    data.replace("OK","");
    data.replace(tail,"");
    data.replace(head,"");
    data.replace("STATUS:2","Got IP");
    data.replace("STATUS:3","Connect");
    data.replace("STATUS:4","DisConnect");
    
              
    return data;
}

/**
 * 모듈이 자동으로 받은 IP 리턴
 */
String ELEC_Wifi::showIP(void) {
    String data;
    unsigned long start;

    for(int a=0; a<3;a++) {
        wSerial.println("AT+CIFSR");  
        start = millis();
        while (millis()-start<3000) {
            while(wSerial.available()>0) {
              char ct = wSerial.read();
              data=data+ct;
            }
            if(data.indexOf("AT+CIFSR")!=-1) break;
        }
        if(data.indexOf(".") != -1) break;
        data = "";
    }

    char head[4] = {0x0D,0x0A};   
    char tail[7] = {0x0D,0x0D,0x0A};        
    data.replace("AT+CIFSR","");
    data.replace("OK","");
    data.replace(tail,"");
    data.replace(head,"");
  
    return data;
}

boolean ELEC_Wifi::connectServer(byte type, String addr, int port) {
    String data;
    wSerial.print("AT+CIPSTART=");
    if(type>0) {
        wSerial.print("\"TCP\"");
    }else {
        wSerial.print("\"UDP\"");
    }
    wSerial.print(",");
    wSerial.print("\"");
    wSerial.print(addr);
    wSerial.print("\"");
    wSerial.print(",");
//    wSerial.print("\"");
    wSerial.println(String(port));
//    wSerial.println("\"");
    unsigned long start;
  start = millis();
  while (millis()-start<3000) { 
     if(wSerial.available()>0)
     {
     char a =wSerial.read();
     data=data+a;
     }
     if (data.indexOf("OK")!=-1 || data.indexOf("ALREAY CONNECT")!=-1 || data.indexOf("ERROR")!=-1)
     {
         return true;
     }
  }
  return false;
}




boolean ELEC_Wifi::Send(String str) {
    wSerial.print("AT+CIPSEND=");
    wSerial.println(str.length());

    unsigned long start;
    start = millis();
    bool found;
    while (millis()-start<5000) {                            
        if(wSerial.find((char*)">")==true ) {
            found = true;
            break;
        }
     }
    if(found) wSerial.println(str);
    else {
      
      return false;
    }


    String data;
    start = millis();
    while (millis()-start<5000) {
       if(wSerial.available()>0) {
            char a =wSerial.read();
            data=data+a;
        }
        
        if (data.indexOf("SEND OK")!=-1) {
            return true;
        }
     }
      return false;
}

int ELEC_Wifi::ReceiveMessage(char *buf)
{
	//+IPD,<len>:<data>
	//+IPD,<id>,<len>:<data>
	String data = "";
	if (wSerial.available()>0)
	{
		unsigned long start;
		start = millis();
		char c0 = wSerial.read();
		if (c0 == '+')
		{
			while (millis()-start<5000) 
			{
				if (wSerial.available()>0)
				{
					char c = wSerial.read();
					data += c;
				}
				if (data.indexOf("\nOK")!=-1)
				{
					break;
				}
			}
			
			int sLen = strlen(data.c_str());
			int i,j;
			for (i = 0; i <= sLen; i++)
			{
				if (data[i] == ':')
				{
					break;
				}
			}

			boolean found = false;
			for (j = 4; j <= i; j++)
			{
				if (data[j] == ',')
				{
					found = true;
					break;
				}
				
			}
			int iSize;

			if(found ==true)
			{


			}
			else
			{			
					String _size = data.substring(4, i);
					iSize = _size.toInt();
					String str = data.substring(i+1, i+1+iSize);
					strcpy(buf, str.c_str());
			}
			return iSize;
		}
	}
	return 0;
}