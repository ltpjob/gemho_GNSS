#include "gnss_config.h"
#include <stdio.h>
#include "SEE_i2c.h"

int save_config(gnss_config *config)
{
  int i=0;
  int ret = 0;
  uint8_t *pcfg = NULL;
  
  config->magicnumber = 0;
  pcfg = (uint8_t *)config;
  
  for(i=0; i<sizeof(*config); i++)
  {
    ret = SEE_i2c_write(pcfg[i], i);
    if(ret != 0)
      break;
  }
  
  config->magicnumber = MAGICNUMBER;
  for(i=0; i<sizeof(config->magicnumber); i++)
  {
    ret = SEE_i2c_write(pcfg[i], i);
    if(ret != 0)
      break;
  }
  
  return ret;
}


int read_config(gnss_config *config)
{
  int i=0;
  int ret = 0;
  uint8_t *pcfg = NULL;
  
  config->magicnumber = 0;
  pcfg = (uint8_t *)config;
  
  for(i=0; i<sizeof(*config); i++)
  {
    ret = SEE_i2c_read(&pcfg[i], i);
    if(ret != 0)
      break;
  }
  
  return ret;
}


