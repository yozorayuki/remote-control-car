#ifndef __WIFICONTROL_H
#define __WIFICONTROL_H

#include <stm32f10x.h>

#define _HOST "10.18.196.202"
#define _PORT 8080


extern u8 bConnected;

void WIFI_Config(void);
void WIFI_ClientConfig(void);
void WIFI_ConnectToServer(void);
void WIFI_WaitForConnect(void);
void WIFI_Send(char _str[]);
void WIFI_Check(void);
void onExternData(u8 _s, u8 _e);

#endif
