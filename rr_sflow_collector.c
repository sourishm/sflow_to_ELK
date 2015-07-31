/* Copyright (c) 2015, Rack Research (rackresearch.com) */

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <endian.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlog.h"

#include "rr_sflow_defs.h"
#include "sflow_elk/sflow_flow_sample.h"
#include "sflow_elk/json_data.h"
#include "sflow_elk/sflow_json_encoder.h"
#include "sflow_elk/json_logstash.h"
#define IFACE_NAME  "TestIn"
#define OFACE_NAME  "TestOut"

static const int integer = INTEGER;
static const int string = STRING;
static zlog_category_t *c;

void rr_transmit_sflow_data(struct sflow_samples *sf_samp)
{
   struct json_data *jdata = NULL;

   jdata = (struct json_data *)sflow_flow_encode(sf_samp);
   if (jdata == NULL)
      return;

   (void)udp_logstash((void *)jdata->data, jdata->data_len);
}

void incr_nsamples(int *nsamples)
{
   if (*nsamples > MAX_SAMPLES)
      return;
   (*nsamples)++;
}

void rr_parse_sflow_record_type(struct sflow_pkt *data,
      uint8_t *pkt,
      uint16_t sample_type,
      uint32_t sample_length)
{
    struct sflow_samples sf_samp = {0};
    struct Sflow5rawpacket *rpkt = (struct Sflow5rawpacket *)pkt;
    struct ethhdr *ethh = (struct ethhdr *)(rpkt->data);
    struct iphdr *iph;

    //vlan support
    if(ethh->h_proto == htons(ETH_P_8021Q))
    {
       iph = (struct iphdr *)( rpkt->data + sizeof(struct ethhdr) + 4 );
    }
    else
    {
       iph = (struct iphdr *)( rpkt->data + sizeof(struct ethhdr) );
    }

    uint16_t *port;
    int nsamples = 0;
    uint32_t frame_len_multi;
    uint32_t srcPort, dstPort;
    char srcMac[MAC_LEN], dstMac[MAC_LEN];

    rr_ip4_to_string(iph->saddr, data->flow_src_ip_str);
    rr_ip4_to_string(iph->daddr, data->flow_dst_ip_str);

    rr_mac_to_string((uint8_t*) &ethh->h_source, srcMac);
    rr_mac_to_string((uint8_t*) &ethh->h_dest, dstMac);

    port = (uint16_t *)((uint8_t*)iph + sizeof(struct iphdr));

    if(iph->protocol == IPPROTO_UDP || 
            iph->protocol == IPPROTO_TCP)
    {
        srcPort = ntohs(port[0]);
        dstPort = ntohs(port[1]);
    }
    else
        srcPort = dstPort = 0;

    zlog_debug(c, "AgentIP %s SourceMac: %s DstMac: %s SrcIP %s DstIP %s"\
            " SrcPort %u DstPort %u\n",
            data->agent_ip_str,
            srcMac, dstMac,
            data->flow_src_ip_str,
            data->flow_dst_ip_str,
            srcPort,
            dstPort);

    sf_samp.sample[nsamples].sample_name = (char *)&SFLOW_AGENT_ADDRESS;
    sf_samp.sample[nsamples].sample_type = (int *)&string;
    sf_samp.sample[nsamples].sample_value.value_s = data->agent_ip_str;

    incr_nsamples(&nsamples);
    sf_samp.sample[nsamples].sample_name = (char *)&SFLOW_SAMPLING_RATE;
    sf_samp.sample[nsamples].sample_type = (int *)&integer;
    if(data->sh_type == 1)
        sf_samp.sample[nsamples].sample_value.value_i = ntohl(data->h.sh1->sampling_rate);
    else
        sf_samp.sample[nsamples].sample_value.value_i = ntohl(data->h.sh3->sampling_rate);

    incr_nsamples(&nsamples);
    sf_samp.sample[nsamples].sample_name = (char *)&SFLOW_I_IFACE_VALUE;
    sf_samp.sample[nsamples].sample_type = (int *)&integer;
    if(data->sh_type == 1)
        sf_samp.sample[nsamples].sample_value.value_i = ntohl(data->h.sh1->i_iface_value);
    else
        sf_samp.sample[nsamples].sample_value.value_i = ntohl(data->h.sh3->i_iface_value);

    incr_nsamples(&nsamples);
    sf_samp.sample[nsamples].sample_name = (char *)&SFLOW_O_IFACE_VALUE;
    sf_samp.sample[nsamples].sample_type = (int *)&integer;
    if(data->sh_type == 1)
        sf_samp.sample[nsamples].sample_value.value_i = ntohl(data->h.sh1->o_iface_value);
    else
        sf_samp.sample[nsamples].sample_value.value_i = ntohl(data->h.sh3->o_iface_value);

    incr_nsamples(&nsamples);
    sf_samp.sample[nsamples].sample_name = (char *)&SFLOW_IPV4_SRC;
    sf_samp.sample[nsamples].sample_type = (int *)&string;
    sf_samp.sample[nsamples].sample_value.value_s = data->flow_src_ip_str;

    incr_nsamples(&nsamples);
    sf_samp.sample[nsamples].sample_name = (char *)&SFLOW_IPV4_DST;
    sf_samp.sample[nsamples].sample_type = (int *)&string;
    sf_samp.sample[nsamples].sample_value.value_s = data->flow_dst_ip_str;

    incr_nsamples(&nsamples);
    sf_samp.sample[nsamples].sample_name = (char *)&SFLOW_FRAME_LENGTH;
    sf_samp.sample[nsamples].sample_type = (int *)&integer;
    sf_samp.sample[nsamples].sample_value.value_i = ntohl(rpkt->frame_length);

    incr_nsamples(&nsamples);
    sf_samp.sample[nsamples].sample_name = (char *)&SFLOW_FRAME_LENGTH_MULTIPLIED;
    sf_samp.sample[nsamples].sample_type = (int *)&integer;
    if(data->sh_type == 1)
        frame_len_multi = ntohl(data->h.sh1->sampling_rate) * ntohl(rpkt->frame_length);
    else
        frame_len_multi = ntohl(data->h.sh3->sampling_rate) * ntohl(rpkt->frame_length);
    sf_samp.sample[nsamples].sample_value.value_i = frame_len_multi;

    incr_nsamples(&nsamples);
    sf_samp.sample[nsamples].sample_name = (char *)&SFLOW_TCP_SRC_PORT;
    sf_samp.sample[nsamples].sample_type = (int *)&integer;
    sf_samp.sample[nsamples].sample_value.value_i = srcPort;

    incr_nsamples(&nsamples);
    sf_samp.sample[nsamples].sample_name = (char *)&SFLOW_TCP_DST_PORT;
    sf_samp.sample[nsamples].sample_type = (int *)&integer;
    sf_samp.sample[nsamples].sample_value.value_i = dstPort;

    incr_nsamples(&nsamples);
    sf_samp.sample[nsamples].sample_name = (char *)&SFLOW_I_IFACE_NAME;
    sf_samp.sample[nsamples].sample_type = (int *)&string;
    sf_samp.sample[nsamples].sample_value.value_s = (char *)&IFACE_NAME;

    incr_nsamples(&nsamples);
    sf_samp.sample[nsamples].sample_name = (char *)&SFLOW_O_IFACE_NAME;
    sf_samp.sample[nsamples].sample_type = (int *)&string;
    sf_samp.sample[nsamples].sample_value.value_s = (char *)&OFACE_NAME;
    incr_nsamples(&nsamples);

    sf_samp.num_samples = nsamples;

    rr_transmit_sflow_data(&sf_samp);
}

int rr_sflow_get_records(struct sflow_pkt *data)
{
   switch (data->sh_type)
   {
      case 1:
         return ntohl(data->h.sh1->num_records);
      case 2:
         return ntohl(data->h.sh2->num_records);
      case 3:
         return ntohl(data->h.sh3->num_records);
      case 4:
         return ntohl(data->h.sh4->num_records);
   }

   assert(0);
   return 0;
}

uint8_t *rr_sflow_get_record_ptr(struct sflow_pkt *data)
{
   switch (data->sh_type)
   {
      case 1:
         return data->h.sh1->data;
      case 2:
         return data->h.sh2->data;
      case 3:
         return data->h.sh3->data;
      case 4:
         return data->h.sh4->data;
   }

   assert(0);
   return 0;
}

void rr_parse_counters(struct sflow_pkt *data)
{
   uint32_t num_records = rr_sflow_get_records(data); 

   zlog_debug(c, "Flow sample agent address %u.%u.%u.%u num_records %u\n",
         NIPQUAD(data->agent_address),
         num_records);

   uint8_t *pkt = rr_sflow_get_record_ptr(data);

   for(;num_records > 0; --num_records)
   {
      struct sflow_sample *fs = (struct sflow_sample *)pkt;
      uint16_t sample_type = ntohs(fs->sflow_sample_type);
      uint32_t sample_length = ntohl(fs->sample_length);

      switch(sample_type)
      {
         case 1: //Generic counter
            {
               struct Sflow5genericcounter *gc = (struct Sflow5genericcounter *)fs->data;

               zlog_debug(c, "rr_parse_counters generic counters \n"
                     "int_index: %u \n"\
                     "int_type: %u \n"\
                     "int_speed: %"PRIu64" \n"\
                     "int_direction: %u \n"\
                     "int_admin_status: %u \n"\
                     "int_oper_status: %u \n"\
                     "input_octets: %"PRIu64" \n"\
                     "input_packets: %u \n"\
                     "input_packets_multi: %u \n"\
                     "input_packets_broad: %u \n"\
                     "input_packets_discard: %u \n"\
                     "input_packets_error: %u \n"\
                     "unknown_proto: %u \n"\
                     "output_octets: %"PRIu64" \n"\
                     "output_packets: %u \n"\
                     "output_packets_multi: %u \n"\
                     "output_packets_broad: %u \n"\
                     "output_packets_discard: %u \n"\
                     "output_packets_error: %u \n"\
                     "prom_mode: %u \n",
                  ntohl(gc->int_index),
                  ntohl(gc->int_type),
                  be64toh(gc->int_speed),
                  ntohl(gc->int_direction),
                  ntohs(gc->int_admin_status),
                  ntohs(gc->int_oper_status),
                  be64toh(gc->input_octets),
                  ntohl(gc->input_packets),
                  ntohl(gc->input_packets_multi),
                  ntohl(gc->input_packets_broad),
                  ntohl(gc->input_packets_discard),
                  ntohl(gc->input_packets_error),
                  ntohl(gc->unknown_proto),
                  be64toh(gc->output_octets),
                  ntohl(gc->output_packets),
                  ntohl(gc->output_packets_multi),
                  ntohl(gc->output_packets_broad),
                  ntohl(gc->output_packets_discard),
                  ntohl(gc->output_packets_error),
                  ntohl(gc->prom_mode));


               break;
            }
         case 2: //Eth counter
            {
               struct Sflow5ethcounter *ethc = (struct Sflow5ethcounter *)fs->data;
               zlog_debug(c, "rr_parse_counters eth counter\n"\
                     "alignment_errors: %u \n"\
                     "fcs_errors: %u \n"\
                     "single_collision_frames: %u \n"\
                     "multi_collision_frames: %u \n"\
                     "sqe_test_errors: %u \n"\
                     "deffered_transmission: %u \n"\
                     "late_collision: %u \n"\
                     "excessive_collision: %u \n"\
                     "internal_mac_transmit_errors: %u \n"\
                     "carrier_sense_errors: %u \n"\
                     "frame_too_long: %u \n"\
                     "internal_mac_receive_errors: %u \n"\
                     "symbol_errors: %u \n",
                     ntohl(ethc->alignment_errors),
                     ntohl(ethc->fcs_errors),
                     ntohl(ethc->single_collision_frames),
                     ntohl(ethc->multi_collision_frames),
                     ntohl(ethc->sqe_test_errors),
                     ntohl(ethc->deffered_transmission),
                     ntohl(ethc->late_collision),
                     ntohl(ethc->excessive_collision),
                     ntohl(ethc->internal_mac_transmit_errors),
                     ntohl(ethc->carrier_sense_errors),
                     ntohl(ethc->frame_too_long),
                     ntohl(ethc->internal_mac_receive_errors),
                     ntohl(ethc->symbol_errors));
               break;
            }

         default:
            {
               zlog_info(c, "rr_parse_counters Unhandled sample  %u\n", sample_type);
               break;
            }
      }

      pkt += sizeof(struct sflow_sample);
      pkt += sample_length;
   }
}

void rr_parse_sflow_sample(struct sflow_pkt *data)
{
   uint32_t num_records = rr_sflow_get_records(data); 

   zlog_debug(c, "Flow sample agent address %u.%u.%u.%u num_records %u\n",
         NIPQUAD(data->agent_address),
         num_records);

   uint8_t *pkt = rr_sflow_get_record_ptr(data);

   for(;num_records > 0; --num_records)
   {
      struct sflow_sample *fs = (struct sflow_sample *)pkt;
      uint16_t sample_type = ntohs(fs->sflow_sample_type);
      uint32_t sample_length = ntohl(fs->sample_length);

      switch(sample_type)
      {
         case 1:
            rr_parse_sflow_record_type(data, fs->data, sample_type, sample_length);
            break;
         case 1001:
            {
               struct Sflow5extswitch *esw = (struct Sflow5extswitch *)fs->data;
               uint32_t src_vlan, dst_vlan;
              
               src_vlan = ntohl(esw->src_vlan);
               dst_vlan = ntohl(esw->dst_vlan);
              
               zlog_debug(c, "Sflow5extswitch: src_vlan %u pri %u dst_vlan %u pri %u\n",
                     src_vlan, ntohl(esw->src_priority), 
                     dst_vlan, ntohl(esw->dst_priority));
               break;
            }
         default:
            break;
      }
      pkt += sizeof(struct sflow_sample);
      pkt += sample_length;
   }
}

void rr_handle_sflow_packet(struct sflow_pkt *data)
{
   uint8_t *pkt = data->pkt;
   struct sflowHeader *sf = (struct sflowHeader *)pkt;

   pkt += sizeof(struct sflowHeader);

   uint32_t num_samples = ntohl(sf->num_samples);
   data->agent_address = sf->agent_address;

   rr_ip4_to_string(data->agent_address, data->agent_ip_str);

   for (;num_samples > 0; --num_samples)
   {
      struct sflow_sample *fs = (struct sflow_sample *)pkt;

      uint16_t sample_type = ntohs(fs->sflow_sample_type);
      uint32_t sample_length = ntohl(fs->sample_length);

      zlog_debug(c, "Sflow sample version: %u Agent Address %s num_sample %u sample length %u sample_type %u\n",
            ntohl(sf->version), data->agent_ip_str, num_samples, sample_length, sample_type);

      data->h.sh = (void *)fs->data;
      data->sh_type = sample_type;
      switch(sample_type)
      {
         case 1: //flow
         case 3: //flow
            rr_parse_sflow_sample(data);
            break;
         case 2: //counter
         case 4: //counter
            rr_parse_counters(data);
            break;
         default:
            zlog_error(c, "Unhandled type %u\n", sample_type);
      }

      pkt += sizeof(struct sflow_sample);
      pkt += sample_length;
   }
}

int main(int argc, char**argv)
{
   int sockfd;
   struct sockaddr_in servaddr,cliaddr;
   socklen_t len;

   int rc;
   rc = zlog_init("zlog.conf");
   if (rc) {
      printf("init failed\n");
      return -1;
   }

   c = zlog_get_category("collector");
   if (!c) {
      printf("get cat fail\n");
      zlog_fini();
      return -2;
   }

   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   memset(&servaddr, 0, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
   servaddr.sin_port=htons(6343);
   bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

   for (;;)
   {
      int readlen = 0;
      char mesg[4096];
      struct sflow_pkt pkt;
      memset(mesg, 0, 2048);
      len = sizeof(cliaddr);
      readlen = recvfrom(sockfd,mesg,2048,0,(struct sockaddr *)&cliaddr,&len);
      if(readlen <= 0)
         continue;

      pkt.pkt = (uint8_t *)mesg;
      pkt.agent_address = 0;
      pkt.h.sh = NULL;

      zlog_debug(c, "Rcvd bytes %u\n", readlen);
      rr_handle_sflow_packet(&pkt);
   }

   zlog_fini();
   return 0;
}
