#include "stm32f10x.h"
#include <stdio.h>
#include "loopback.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "gemho_GNSS.h"

//#define _NETSERVICE_DEBUG_

static uint32_t aliveSendMs;
static uint32_t aliveRcvMs;

void network_aliveTick()
{
  aliveSendMs++;
  aliveRcvMs++;
}

#pragma pack(1)
typedef struct tagupComDataHead
{
  uint8_t start[2];
  uint16_t type;
  uint32_t size;
}upComDataHead;
#pragma pack()


void network_init(wiz_NetInfo *winfo)
{
#ifdef _NETSERVICE_DEBUG_
  uint8_t tmpstr[6];
#endif
  ctlnetwork(CN_SET_NETINFO, winfo);
  ctlnetwork(CN_GET_NETINFO, winfo);
  
  // Display Network Information
  
#ifdef _NETSERVICE_DEBUG_
  ctlwizchip(CW_GET_ID,(void*)tmpstr);
  printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
  printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",winfo->mac[0],winfo->mac[1],winfo->mac[2],
         winfo->mac[3],winfo->mac[4],winfo->mac[5]);
  printf("SIP: %d.%d.%d.%d\r\n", winfo->ip[0],winfo->ip[1],winfo->ip[2],winfo->ip[3]);
  printf("GAR: %d.%d.%d.%d\r\n", winfo->gw[0],winfo->gw[1],winfo->gw[2],winfo->gw[3]);
  printf("SUB: %d.%d.%d.%d\r\n", winfo->sn[0],winfo->sn[1],winfo->sn[2],winfo->sn[3]);
  printf("DNS: %d.%d.%d.%d\r\n", winfo->dns[0],winfo->dns[1],winfo->dns[2],winfo->dns[3]);
  printf("======================\r\n");
#endif
}

int32_t net_service()
{
  int32_t ret;
  uint16_t size = 0, sentsize=0;
  uint8_t sn = 0;
  uint16_t port = 5566;
  uint8_t *pData = NULL;
  upComDataHead head;
  uint32_t cpuid[3];
  uint8_t buffer[128] = "";
  
  
  cpuid[0]=*(vu32*)(0x1ffff7e8);
  cpuid[1]=*(vu32*)(0x1ffff7ec);
  cpuid[2]=*(vu32*)(0x1ffff7f0);
  
#ifdef _NETSERVICE_DEBUG_
  uint8_t destip[4];
  uint16_t destport;
#endif
  
  switch(getSn_SR(sn))
  {
  case SOCK_ESTABLISHED :
    if(getSn_IR(sn) & Sn_IR_CON)
    {
#ifdef _NETSERVICE_DEBUG_
      getSn_DIPR(sn, destip);
      destport = getSn_DPORT(sn);
      
      printf("%d:Connected - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
      aliveSendMs = 0;
      aliveRcvMs = 0;
      setSn_IR(sn, Sn_IR_CON);
    }
    
    if(aliveRcvMs >= 10*1000)
    {
      close(sn);
      return -1;
    }
    
    if((size = getSn_RX_RSR(sn)) >= sizeof(upComDataHead)) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
    {
      ret = recv(sn, (uint8_t*)&head, sizeof(upComDataHead));
      
      if(ret <= 0) // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
        return ret;      

      if(head.start[0]==0x55&&head.start[1]==0xaa&&head.type==2)
      {
        aliveRcvMs = 0;
      }
      else
      {
        uint32_t rcvCount = 10;
        while((size = getSn_RX_RSR(sn)) > 0 && rcvCount--)
        {
          if(size > sizeof(buffer)) 
            size = sizeof(buffer);
          
          ret = recv(sn, buffer, size);
          if(ret <= 0) 
            return ret; 
        }
      }
    }
    
    if(aliveSendMs >= 2*1000)
    {
      head.start[0] = 0x55;
      head.start[1] = 0xaa;
      head.type = 2;
      head.size = 0;
      
      size = sizeof(upComDataHead);
      sentsize = 0;
      pData = (void *)&head;
      while(size != sentsize)
      {
        ret = send(sn, pData+sentsize, size-sentsize);
        if(ret < 0)
        {
          close(sn);
          return ret;
        }
        sentsize += ret;
      }
      
      aliveSendMs = 0;
    }
    
    if(g_recv_flag>1)
    {
#ifdef _NETSERVICE_DEBUG_
      printf("bad recv value!!!  %d\n", g_recv_flag);
#endif
      g_recv_flag=0;
    }
    else if(g_recv_flag == 1)
    {
      size = sizeof(upComDataHead);
      head.start[0] = 0x55;
      head.start[1] = 0xaa;
      head.type = 1;
      head.size = RXBUFFERSIZE + sizeof(cpuid);
      
      size = sizeof(upComDataHead);
      sentsize = 0;
      pData = (void *)&head;
      while(size != sentsize)
      {
        ret = send(sn, pData+sentsize, size-sentsize);
        if(ret < 0)
        {
          close(sn);
          return ret;
        }
        sentsize += ret;
      }
      
      size = sizeof(cpuid);
      sentsize = 0;
      pData = (void *)&cpuid;
      while(size != sentsize)
      {
        ret = send(sn, pData+sentsize, size-sentsize);
        if(ret < 0)
        {
          close(sn);
          return ret;
        }
        sentsize += ret;
      }
      
      size = RXBUFFERSIZE;
      sentsize = 0;
      if(g_using_buf0 == 0)
        pData = RXBuffer0;
      else
        pData = RXBuffer1;
      while(size != sentsize)
      {
        ret = send(sn, pData+sentsize, size-sentsize);
        if(ret < 0)
        {
          close(sn);
          return ret;
        }
        sentsize += ret;
      }
      
      if(g_recv_flag>1)
      {
        printf("bug happened\n");
      }
      
      g_recv_flag--;
    }
    
    break;
  case SOCK_CLOSE_WAIT :
#ifdef _NETSERVICE_DEBUG_
    printf("%d:CloseWait\r\n",sn);
#endif
    if((ret = disconnect(sn)) != SOCK_OK) return ret;
#ifdef _NETSERVICE_DEBUG_
    printf("%d:Socket Closed\r\n", sn);
#endif
    break;
  case SOCK_INIT :
#ifdef _NETSERVICE_DEBUG_
    printf("%d:Listen, TCP server loopback, port [%d]\r\n", sn, port);
#endif
    if( (ret = listen(sn)) != SOCK_OK) return ret;
    break;
  case SOCK_CLOSED:
#ifdef _NETSERVICE_DEBUG_
    printf("%d:TCP server loopback start\r\n",sn);
#endif
    if((ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
#ifdef _NETSERVICE_DEBUG_
    printf("%d:Socket opened\r\n",sn);
#endif
    break;
  default:
    break;
  }
  return 0;
}