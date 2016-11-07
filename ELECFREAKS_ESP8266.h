#ifndef ELECFREAKS_ESP8266_h
#define ELECFREAKS_ESP8266_h

# include <SoftwareSerial.h>

 // UNO Serial �ɹ�ȣ ����
#define _RXPIN_ 5
#define _TXPIN_ 6

class ELEC_Wifi {

public :


		ELEC_Wifi(void) ;
		ELEC_Wifi(byte stx, byte srx) ;

		//  ESP8266 ��� �ʱ�ȭ
		void begin(void) ;

		// �߿��� ���� üũ
		String checkFirm() ;

		// ����� ��ī�� ���·� ����
		bool setStation() ;

		
 		// �����⿡ ���� �õ�
		bool joinAP(String ssid , String pwd) ;

		// ���Ӱ����� ���� ����
		String showJAP(void) ;

		/**
		 * ��� ���� üũ
		 * 
		 * return
		 *    2: Got IP
		 *    3: Connected
		 *    4: Disconnected
		 */
		String showStatus(void) ;


		//  ����� �ڵ����� ���� IP ����
		String showIP(void) ;

		// ���ݼ����� ����
		// type = TCP:1, UDP:0
		// addr = IP address
		// port = ���� ��Ʈ
		boolean connectServer(byte type, String addr, int port) ;
		
		// ���ݼ����� ����
		boolean Send(String str) ;

		// �޽��� ����
		int ReceiveMessage(char *buf);

private :
	// SoftwareSerial wSerial ;

};
#endif

