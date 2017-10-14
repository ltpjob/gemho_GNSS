#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include "socket.h"
#include "wizchip_conf.h"
#include "gemho_GNSS.h"
#include "net_service.h"
#include "crc32.h"
#include "gnss_config.h"

#define         GNSS_DATA               1
#define         GNSS_HEARTBEAT          2
#define         GNSS_ENUM               3
#define         GNSS_REPORTENUM         4
#define         GNSS_SETNETINFO         5

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

typedef struct tagupComDataHead_udp
{
  upComDataHead hdt;
  uint32_t crc32;
}upComDataHead_udp;


/**
 * @ingroup DATA_TYPE
 *  Network Information for WIZCHIP
 */
typedef struct NetInfo_t
{
   uint8_t mac[6];  ///< Source Mac Address
   uint8_t ip[4];   ///< Source IP Address
   uint8_t sn[4];   ///< Subnet Mask 
   uint8_t gw[4];   ///< Gateway IP Address
   uint8_t dns[4];  ///< DNS server IP Address
   uint8_t dhcp_mode;  ///< 1 - Static, 2 - DHCP
}NetInfo;

typedef struct tagdevInfo
{
  NetInfo netinfo;
  uint32_t cpuid[3];
  uint32_t crc32;
}devInfo;

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
  uint32_t cpuid[3];
  cpuid[0]=*(vu32*)(0x1ffff7e8);
  cpuid[1]=*(vu32*)(0x1ffff7ec);
  cpuid[2]=*(vu32*)(0x1ffff7f0);
  printf("CPUID: %x %x %x\r\n", cpuid[0], cpuid[1], cpuid[2]);
  printf("======================\r\n");
#endif
}

int32_t lookup_config()
{
   int32_t  ret;
   uint16_t size, sentsize;
   uint8_t  destip[4];
   uint8_t  broadcastip[4]={255, 255, 255, 255};
   uint16_t destport;
   uint32_t cpuid[3];
   uint8_t buffer[128] = "";
   upComDataHead_udp head;
   uint8_t sn = 3;
   uint16_t port = 5768;
   int32_t i;
   
   cpuid[0]=*(vu32*)(0x1ffff7e8);
   cpuid[1]=*(vu32*)(0x1ffff7ec);
   cpuid[2]=*(vu32*)(0x1ffff7f0);

   
   switch(getSn_SR(sn))
   {
      case SOCK_UDP :
        
        if((size = getSn_RX_RSR(sn)) >= sizeof(upComDataHead)) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
        {
          ret = recvfrom(sn, (uint8_t*)&head, sizeof(upComDataHead_udp), destip, (uint16_t*)&destport);
          if(ret <= 0)
          {
#ifdef _NETSERVICE_DEBUG_
            printf("%d: recvfrom error. %ld\r\n",sn,ret);
#endif
            return ret;
          }
          
          if(get_crc32(0, (uint8_t *)&head.hdt, sizeof(head.hdt)) == head.crc32)
          {
            if(head.hdt.start[0]==0x55&&head.hdt.start[1]==0xaa&&head.hdt.type==GNSS_SETNETINFO)
            {
              devInfo di; 
              
              ret = recvfrom(sn, (uint8_t*)&di, sizeof(devInfo), destip, (uint16_t*)&destport);
              if(ret <= 0)
              {
    #ifdef _NETSERVICE_DEBUG_
                printf("%d: recvfrom error. %ld\r\n",sn,ret);
    #endif
                return ret;
              }
              
              if(di.crc32 == get_crc32(0, (uint8_t *)&di, sizeof(di)-sizeof(di.crc32)))
              {
                if(di.cpuid[0] == cpuid[0] && di.cpuid[1] == cpuid[1] && di.cpuid[2] == cpuid[2])
                {
                  gnss_config cfg;
                
                  for(i=0; i<sizeof(di.netinfo.mac); i++)
                  {
                    cfg.netinfo.mac[i] = di.netinfo.mac[i];
                  }
                  
                  for(i=0; i<4; i++)
                  {
                    cfg.netinfo.ip[i] =  di.netinfo.ip[i]; 
                    cfg.netinfo.sn[i] =  di.netinfo.sn[i]; 
                    cfg.netinfo.gw[i] =  di.netinfo.gw[i]; 
                    cfg.netinfo.dns[i] = di.netinfo.dns[i];
                  }
                  
                  ret = save_config(&cfg);
                  if(ret == 0)
                  {
                    network_init(&cfg.netinfo);
                  }
                }
              }
            }
            else if(head.hdt.start[0]==0x55&&head.hdt.start[1]==0xaa&&head.hdt.type==GNSS_ENUM)
            {
              devInfo di;
              wiz_NetInfo wnf;
              
              
              ctlnetwork(CN_GET_NETINFO, &wnf);
              for(i=0; i<sizeof(di.netinfo.mac); i++)
              {
                di.netinfo.mac[i] = wnf.mac[i];
              }
              for(i=0; i<4; i++)
              {
                di.netinfo.ip[i] = wnf.ip[i];
                di.netinfo.sn[i] = wnf.sn[i];
                di.netinfo.gw[i] = wnf.gw[i];
                di.netinfo.dns[i] = wnf.dns[i];
              }
              di.netinfo.dhcp_mode = wnf.dhcp;
              
              di.cpuid[0] = cpuid[0];
              di.cpuid[1] = cpuid[1];
              di.cpuid[2] = cpuid[2];
              di.crc32 = get_crc32(0, (uint8_t *)&di, sizeof(di)-sizeof(di.crc32));
#ifdef _NETSERVICE_DEBUG_
              printf("%d %d %d %u\n", sizeof(di), sizeof(di.crc32), sizeof(di)-sizeof(di.crc32), di.crc32);
#endif              
              
              head.hdt.start[0] = 0x55;
              head.hdt.start[1] = 0xaa;
              head.hdt.type = GNSS_REPORTENUM;
              head.hdt.size = sizeof(devInfo);
              head.crc32 = get_crc32(0, (uint8_t *)&head.hdt, sizeof(head.hdt));
              
              
              memcpy(buffer, &head, sizeof(head));
              memcpy(buffer+sizeof(head), &di, sizeof(di));
              
              size = sizeof(head)+sizeof(di);
              sentsize = 0;
              while(sentsize != size)
              {
                ret = sendto(sn, buffer+sentsize, size-sentsize, broadcastip, destport);
                if(ret < 0)
                {
#ifdef _NETSERVICE_DEBUG_
                  printf("%d: sendto error. %ld\r\n",sn,ret);
#endif
                  return ret;
                }
                sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
              }
              
            }
          }
        }

          
        
//         if((size = getSn_RX_RSR(sn)) > 0)
//         {
//            if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
//            ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
//            if(ret <= 0)
//            {
//#ifdef _NETSERVICE_DEBUG_
//               printf("%d: recvfrom error. %ld\r\n",sn,ret);
//#endif
//               return ret;
//            }
//            size = (uint16_t) ret;
//            sentsize = 0;
//            while(sentsize != size)
//            {
//               ret = sendto(sn, buf+sentsize, size-sentsize, destip, destport);
//               if(ret < 0)
//               {
//#ifdef _NETSERVICE_DEBUG_
//                  printf("%d: sendto error. %ld\r\n",sn,ret);
//#endif
//                  return ret;
//               }
//               sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
//            }
//         }
         break;
      case SOCK_CLOSED:
#ifdef _NETSERVICE_DEBUG_
         printf("%d:UDP loopback start\r\n",sn);
#endif
         if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
            return ret;
#ifdef _NETSERVICE_DEBUG_
         printf("%d:Opened, UDP loopback, port [%d]\r\n", sn, port);
#endif
         break;
      default :
         break;
   }
   return 1;
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
      led_comm_set(1);
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
      led_comm_set(0);
      return -1;
    }
    
    if((size = getSn_RX_RSR(sn)) >= sizeof(upComDataHead)) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
    {
      ret = recv(sn, (uint8_t*)&head, sizeof(upComDataHead));
      
      if(ret <= 0) // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
        return ret;      

      if(head.start[0]==0x55&&head.start[1]==0xaa&&head.type==GNSS_HEARTBEAT)
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
      head.type = GNSS_HEARTBEAT;
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
      head.type = GNSS_DATA;
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
#ifdef _NETSERVICE_DEBUG_
        printf("bug happened\n");
#endif
      }
      
      g_recv_flag--;
    }
    
    break;
  case SOCK_CLOSE_WAIT :
#ifdef _NETSERVICE_DEBUG_
    printf("%d:CloseWait\r\n",sn);
#endif
    led_comm_set(0);
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
    led_comm_set(0);
    if((ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;
#ifdef _NETSERVICE_DEBUG_
    printf("%d:Socket opened\r\n",sn);
#endif
    break;
  default:
    break;
  }
  return 1;
}


