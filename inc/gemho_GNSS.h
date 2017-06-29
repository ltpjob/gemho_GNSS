#include "wizchip_conf.h"

#define RXBUFFERSIZE 256

extern uint8_t RXBuffer0[RXBUFFERSIZE];
extern uint8_t RXBuffer1[RXBUFFERSIZE];
extern __IO uint8_t g_using_buf0;
extern __IO uint8_t g_recv_flag;
extern wiz_NetInfo gWIZNETINFO;


