#include "WifiControl.h"

#include "UsartConfig.h"
#include "delay.h"
#include <stdio.h>

u8 bATOk = 0, bConnected = 0, bSending;

void WIFI_Config(void) {
	RcvBufClear();
	while(!bATOk)
	{
		printf("AT\r\n");
		delay_ms(5);
		WIFI_Check();
	}
	bATOk = 0;
	
	delay_ms(50);
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	printf("AT+RST\r\n");
	delay_ms(1000);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	RcvBufClear();
	while(!bATOk)
	{
		printf("AT\r\n");
		delay_ms(5);
		WIFI_Check();
	}
	bATOk = 0;
	
	RcvBufClear();
	while(!bATOk)
	{
		printf("AT+CIPMUX=1\r\n");
		delay_ms(5);
		WIFI_Check();
	}
	bATOk = 0;
	
	RcvBufClear();
	while(!bATOk)
	{
		printf("AT+CIPSERVER=1\r\n");
		delay_ms(5);
		WIFI_Check();
	}
}


void WIFI_ClientConfig(void) {
	RcvBufClear();
	while(!bATOk)
	{
		printf("AT\r\n");
		delay_ms(100);
		WIFI_Check();
	}
	bATOk = 0;
	
	RcvBufClear();
	while(!bATOk)
	{
		printf("AT+CWJAP=\"BUAA-WiFi\",\"\"\r\n");
		for(int i = 0; !bATOk && i < 100; i++) {
			delay_ms(10);
			WIFI_Check();
		}
	}
	bATOk = 0;
}


void WIFI_ConnectToServer(void) {
	RcvBufClear();
	bATOk = 0;
	while(!bATOk)
	{
		//printf("AT+CIPSTART=\"TCP\",\"172.16.9.18\",5432\r\n");
		printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", _HOST, _PORT);
		for(int i = 0; !bATOk && i < 100; i++) {
			delay_ms(10);
			WIFI_Check();
		}
	}
	
	RcvBufClear();
	bATOk = 0;
	while(!bATOk)
	{
		printf("AT+CIPMODE=1\r\n");
		for(int i = 0; !bATOk && i < 50; i++) {
			delay_ms(10);
			WIFI_Check();
		}
	}
	
	RcvBufClear();
	bATOk = 0;
	while(!bATOk)
	{
		printf("AT+CIPSEND\r\n");
		for(int i = 0; !bATOk && i < 50; i++) {
			delay_ms(10);
			WIFI_Check();
		}
	}
}


void WIFI_WaitForConnect(void) {
	RcvBufClear();
	while(!bConnected)
	{
		for(int i = 0; i < 70; i++) {
			delay_ms(10);
			WIFI_Check();
		}
	}
}


void WIFI_Send(char _str[]) {
	if(!bConnected || bSending) return;
	
	int lenth = 0;
	while(_str[lenth]) lenth++;
	if(lenth == 0) return;
	
	bSending = 1;
	
	printf("AT+CIPSEND=0,%d\r\n", lenth);
	
	delay_ms(1);
	printf("%s", _str);
	
	bSending = 0;
}


void WIFI_Check(void) {
	while(RcvBuf[RcvStart] != RcvStart) {
		u8 start = RcvStart, end = RcvBuf[RcvStart];
		u8 i = start;
		switch(RcvBuf[start+1]) {
			case '+':
				//+IPD,0,18:这是一条测试消息
				while(RcvBuf[i++] != ':');
				onExternData(i, end);
				break;
			
			case '0':
				//0,CONNECT
				//0,CLOSED
				if(RcvBuf[start+4] == 'O') {
					bConnected = 1;
				}
				if(RcvBuf[start+4] == 'L') {
					bConnected = 0;
				}
				break;
			
			case 'O':
				//OK
				bATOk = 1;
				break;
		}
		RcvStart = end;
		for(i = start; i != end; i++) RcvBuf[i] = i;
	}
}


void onExternData(u8 _s, u8 _e) {
	
	return;
}

