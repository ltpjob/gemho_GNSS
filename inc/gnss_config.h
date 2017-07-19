#ifndef _GNSS_CONFIG_H_
#define _GNSS_CONFIG_H_


#include "wizchip_conf.h"


#define MAGICNUMBER 0x56585759

typedef struct tag_gnss_config
{
  uint32_t magicnumber;
  wiz_NetInfo netinfo;
}gnss_config; 


int save_config(gnss_config *config);

int read_config(gnss_config *config);


#endif