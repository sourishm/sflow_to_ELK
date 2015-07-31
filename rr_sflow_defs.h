/* Copyright (c) 2015, Rack Research (rackresearch.com) */

#ifndef _RR_SFLOW_DEF_HPP_
#define _RR_SFLOW_DEF_HPP_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include "rr_macros.h"

struct sflow_pkt {
    uint8_t *pkt;
    uint8_t *data;
    uint32_t agent_address;
    union {
        struct Sflow5sampleheader1 *sh1;
        struct Sflow5counterheader2 *sh2;
        struct Sflow5sampleheader3 *sh3;
        struct Sflow5counterheader4 *sh4;
        void *sh;
    }h;
    uint8_t sh_type;
    char agent_ip_str[INET_ADDRSTRLEN];
    char flow_src_ip_str[INET_ADDRSTRLEN];
    char flow_dst_ip_str[INET_ADDRSTRLEN];

};

struct sflowHeader {
    uint32_t version;
    uint32_t address_type;
    uint32_t agent_address;
    uint32_t sub_agent_id;
    uint32_t seq_number;
    uint32_t sys_uptime;
    uint32_t num_samples;
};

struct sflow_sample {
    uint16_t enterprise_std;
    uint16_t sflow_sample_type;
    uint32_t sample_length;
    uint8_t data[0];
};

struct Sflow5sampleheader1 {
    uint32_t seq_number;
    uint32_t source_id_type;
    uint32_t sampling_rate;
    uint32_t sample_pool;
    uint32_t dropped_packets;
    uint32_t i_iface_value;
    uint32_t o_iface_value;
    uint32_t num_records;
    uint8_t data[0];
};

struct Sflow5sampleheader3 {
    uint32_t seq_number;
    uint32_t source_id_type;
    uint32_t source_id_index;
    uint32_t sampling_rate;
    uint32_t sample_pool;
    uint32_t dropped_packets;
    uint32_t i_iface_format;
    uint32_t i_iface_value;
    uint32_t o_iface_format;
    uint32_t o_iface_value;
    uint32_t num_records;
    uint8_t data[0];
};

struct Sflow5counterheader4 {
    uint32_t seq_number;
    uint32_t source_id_type;
    uint32_t source_id_index;
    uint32_t num_records;
    uint8_t data[0];
};

struct Sflow5counterheader2 {
    uint32_t seq_number;
    uint32_t source_id_type;
    uint32_t num_records;
    uint8_t data[0];
};

struct Sflow5rawpacket {
    uint32_t header_protocol;
    uint32_t frame_length;
    uint32_t payload;
    uint32_t xy;
    uint8_t data[0];
};

struct Sflow5extswitch {
    uint32_t src_vlan;
    uint32_t src_priority;
    uint32_t dst_vlan;
    uint32_t dst_priority;
};

struct Sflow5genericcounter {
    uint32_t int_index;
    uint32_t int_type;
    uint64_t int_speed;
    uint32_t int_direction;
    uint16_t int_admin_status;
    uint16_t int_oper_status;
    uint64_t input_octets;
    uint32_t input_packets;
    uint32_t input_packets_multi;
    uint32_t input_packets_broad;
    uint32_t input_packets_discard;
    uint32_t input_packets_error;
    uint32_t unknown_proto;
    uint64_t output_octets;
    uint32_t output_packets;
    uint32_t output_packets_multi;
    uint32_t output_packets_broad;
    uint32_t output_packets_discard;
    uint32_t output_packets_error;
    uint32_t prom_mode;
};

struct Sflow5ethcounter {
    uint32_t alignment_errors;
    uint32_t fcs_errors;
    uint32_t single_collision_frames;
    uint32_t multi_collision_frames;
    uint32_t sqe_test_errors;
    uint32_t deffered_transmission;
    uint32_t late_collision;
    uint32_t excessive_collision;
    uint32_t internal_mac_transmit_errors;
    uint32_t carrier_sense_errors;
    uint32_t frame_too_long;
    uint32_t internal_mac_receive_errors;
    uint32_t symbol_errors;
};

#endif
