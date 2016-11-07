#include "Arduino.h"
#include "ELECFREAKS_ESP8266.h"


SoftwareSerial wSerial(_TXPIN_,_RXPIN_);

// ����� �޽��� �ø��� ����ͷ� ���
#define DBG(message)    Serial.println(message)
#define DBGW(message)   Serial.write(message)


// �⺻���� TX 5, RX 6 ��
ELEC_Wifi::ELEC_Wifi(void) {

}

// ��Ʈ�� �ٲپ� �ʱ�ȭ �ϰ� ������
ELEC_Wifi::ELEC_Wifi(byte xTX, byte xRX) {
	wSerial = SoftwareSerial(xTX, xRX) ;
}


void ELEC_Wifi::begin(void) {
      boolean result = false;
      
      wSerial.begin(9600); // wSerial ��Ʈ �ӵ�, ELECFREAKS 8266�� �⺻ �ӵ��� 9600���� ���� �Ǿ� �ִ�.
      Serial.begin(9600);   // �ø��� ����� �ӵ� ����
      
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

    // ����� Station Mode�� ����
    wSerial.println("AT+CWMODE=1");  
    
    unsigned long start;
    start = millis();
    while (millis()-start<2000) {
      if(wSerial.available()>0) {
          char a = wSerial.read();
          data=data+a;
      }
      // ��� ������ �����ϰų�, �̹� Station ����ϰ�� true, �ƴϸ� false
      if (data.indexOf("OK")!=-1 || data.indexOf("no change")!=-1) return true;
      if (data.indexOf("ERROR")!=-1 || data.indexOf("busy")!=-1) return false;
   }
  return true;
}

/**
 * �����⿡ ���� �õ�
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
 * ��� ���� üũ
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
 * ����� �ڵ����� ���� IP ����
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