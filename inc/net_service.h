#ifndef _NET_SERVICE_H_
#define _NET_SERVICE_H_

//#define _NETSERVICE_DEBUG_

void network_aliveTick();
void network_init(wiz_NetInfo *winfo);
int32_t net_service();
int32_t lookup_config();

#endif