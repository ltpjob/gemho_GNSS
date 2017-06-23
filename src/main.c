#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "spi.h"
#include "wizchip_conf.h"
#include "socket.h"
#include "loopback.h"

#define RXBUFFERSIZE 2048

uint8_t RXBuffer0[RXBUFFERSIZE];
uint8_t RXBuffer1[RXBUFFERSIZE];
uint8_t netBuffer[RXBUFFERSIZE];

uint8_t using_buf0 = 1;
uint8_t recv_flag = 0;

wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
                             .ip = {192, 168, 88, 5},
                             .sn = {255,255,255,0},
                             .gw = {192, 168, 88, 1},
                             .dns = {0,0,0,0},
                             .dhcp = NETINFO_STATIC };

int fputc(int ch, FILE *f)
{
  return ch;
}


void RCC_Configuration(void)
{
    /* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 | RCC_APB1Periph_I2C2, ENABLE);
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
    
  //usart1
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //spi2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* Confugure MISO pin as Input Floating  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB, GPIO_Pin_12);
  
  //i2c2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void NVIC_Configuration(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;   
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void DMA_Configuration(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  /* USARTy TX DMA1 Channel (triggered by USARTy Tx event) Config */
  DMA_DeInit(DMA1_Channel5);  
  DMA_Cmd(DMA1_Channel5, DISABLE); 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RXBuffer0;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = sizeof(RXBuffer0);
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  
  DMA_ClearFlag(DMA1_FLAG_TC5);                                 
  DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);          
}

void USART_GPS_init()
{
  USART_InitTypeDef USART_InitStructure;
  
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  USART_Init(USART1, &USART_InitStructure);
}

void USART_GSP_start()
{
  USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
  DMA_Cmd(DMA1_Channel5, ENABLE);
  USART_Cmd(USART1, ENABLE);
}

void W5500_config()
{
  SPI_Configuration();
  
  // First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP 
  /* Critical section callback */
  reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);   //×¢²áÁÙ½çÇøº¯Êý
  /* Chip selection call back */
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
  reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);//×¢²áSPIÆ¬Ñ¡ÐÅºÅº¯Êý
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
  reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);  // CS must be tried with LOW.
#else
#if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
#error "Unknown _WIZCHIP_IO_MODE_"
#else
  reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
#endif
#endif
  /* SPI Read & Write callback function */
  reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);    //×¢²á¶ÁÐ´º¯Êý
  
  SPI_start();
}

void network_init(void)
{
  uint8_t tmpstr[6];
  ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
  ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);
  
  // Display Network Information
  ctlwizchip(CW_GET_ID,(void*)tmpstr);
  printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
  printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
         gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
  printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
  printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
  printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
  printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
  printf("======================\r\n");
}

void W5500_start()
{
//  while(1);
}

int main(void)
{
  SystemInit();
  
  RCC_Configuration();
  NVIC_Configuration();
  GPIO_Configuration();
  DMA_Configuration();
  USART_GPS_init();
  USART_GSP_start();
  
  W5500_config();
  
  network_init();
  
  while(1)
  {
    loopback_tcps(0, netBuffer, 5566);
    
      
    
//    static uint8_t flagChange = 0;
//    
//    if(flagChange != using_buf0)
//    {
//      uint32_t count = sizeof(RXBuffer0);
//      int i = 0;
//      uint8_t *buf = NULL;
//      
//      if(using_buf0 == 1)
//        buf = RXBuffer1;
//      else
//        buf = RXBuffer0;
//        
//      for(i=0; i<count; i++)
//      {
//        printf("%c", buf[i]);
//      }
//      
//      flagChange = using_buf0;
//    }
    
//    if(recv_flag == 1)
//    {
//      uint8_t *buf = NULL;
//      uint32_t i = 0;
//      
//      recv_flag = 0;
//      if(using_buf0 == 1)
//        buf = RXBuffer1;
//      else
//        buf = RXBuffer0;
//      
//      for(i=0; i<RXBUFFERSIZE; i++)
//      {
//        USART_SendData(USART1,buf[i]);
//        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
//      }
//      
//    }
    
  }
}

void DMA1_Channel5_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_FLAG_TC5))
  {
    if(using_buf0 ==0)
    {
      DMA1_Channel5->CMAR = (uint32_t)RXBuffer0;
      using_buf0 = 1;
    }
    else
    {
      DMA1_Channel5->CMAR = (uint32_t)RXBuffer1;
      using_buf0 = 0;
    }
    
    recv_flag = 1;
    
    DMA_ClearITPendingBit(DMA1_IT_TC5);
  }
}