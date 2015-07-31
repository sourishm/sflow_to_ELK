/* Copyright (c) 2015, Rack Research (rackresearch.com) */

#ifndef SFLOW_FLOW_SAMPLE_H
#define SFLOW_FLOW_SAMPLE_H

#define NAME_SIZE   256

#define MAX_SAMPLES 15

/* sflow flow sample names */
#define SFLOW_AGENT_ADDRESS         "sflow_agent_address"
#define SFLOW_SAMPLING_RATE         "sflow_sampling_rate"
#define SFLOW_I_IFACE_VALUE         "sflow_i_iface_value"
#define SFLOW_O_IFACE_VALUE         "sflow_o_iface_value"
#define SFLOW_VLAN_SRC              "sflow_vlan_src"
#define SFLOW_VLAN_DST              "sflow_vlan_dst"
#define SFLOW_FRAME_LENGTH          "sflow_frame_length"
#define SFLOW_FRAME_LENGTH_MULTIPLIED   "sflow_frame_length_multiplied"
#define SFLOW_TCP_SRC_PORT          "sflow_tcp_src_port"
#define SFLOW_TCP_DST_PORT          "sflow_tcp_dst_port"
#define SFLOW_IPV4_SRC              "sflow_ipv4_src"
#define SFLOW_IPV4_DST              "sflow_ipv4_dst"
#define SFLOW_I_IFACE_NAME          "sflow_i_iface_name"
#define SFLOW_O_IFACE_NAME          "sflow_o_iface_name"

#define INTEGER     0
#define STRING      1

union flow_sample_value {
    char                   *value_s;
    unsigned int           value_i;
};

struct flow_sample {
    char                        *sample_name;
    union flow_sample_value     sample_value;
    int                         *sample_type;
};

struct sflow_samples {
    struct flow_sample  sample[MAX_SAMPLES];
    int                 num_samples;
};

#endif
