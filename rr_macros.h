/* Copyright (c) 2015, Rack Research (rackresearch.com) */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define ETH_ALEN 6
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"

#define MAC_ARG(x) ((uin8_t*)(&x))[0], \
    ((uin8_t*)(&x))[1], \
((uin8_t*)(&x))[2], \
((uin8_t*)(&x))[3], \
((uin8_t*)(&x))[4], \
((uin8_t*)(&x))[5]

#define NIPQUAD(addr) \
    ((unsigned char *)&addr)[0], \
((unsigned char *)&addr)[1], \
((unsigned char *)&addr)[2], \
((unsigned char *)&addr)[3]

#define INET_ADDRSTRLEN 16
#define MAC_LEN 18

inline char *rr_ip4_to_string(uint32_t ip, char *str)
{
   snprintf(str, INET_ADDRSTRLEN, "%u.%u.%u.%u", NIPQUAD(ip));
   return str;
}

inline char *rr_mac_to_string(uint8_t *mac, char *str)
{
   snprintf(str, MAC_LEN, MAC_FMT, mac[0], 
         mac[1],
         mac[2],
         mac[3],
         mac[4],
         mac[5]);
   return str;
}
