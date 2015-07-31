/* Copyright (c) 2015, Rack Research (rackresearch.com) */

#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "json_logstash.h"
#include <stdio.h>

/* logstash server and port */
#define SERVER  "192.168.2.200"
#define PORT    6543

int udp_logstash(void *data, int data_len)
{
    static int s = -1;
    int     error = 0;
    int     slen;
    struct  sockaddr_in si_other;

    printf("%s\n", (char *)data);

    if (data == NULL)
        return (1);
    slen = sizeof(si_other);

    if(s == -1)
       if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
          return (1);

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);

    if (inet_aton(SERVER , &si_other.sin_addr) == 0)
        return (1);

    if (sendto(s, data, data_len, 0, (struct sockaddr *) &si_other, slen)==-1)
        return (1);

    return error;
}
