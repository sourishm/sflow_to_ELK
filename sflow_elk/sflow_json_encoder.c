/* Copyright (c) 2015, Rack Research (rackresearch.com) */

#include <jansson.h>
#include <string.h>
#include "sflow_flow_sample.h"
#include "json_data.h"
#include "sflow_json_encoder.h"

enum json_value_type {
    j_integer,
    j_string
};

struct json_format {
    char                    json_name[NAME_SIZE];
    enum json_value_type    type;
};

#define     JSON_FORMAT_SIZE    14
struct json_format frmt[JSON_FORMAT_SIZE] = {  {"sflow_agent_address", j_string },
                                        {"sflow_sampling_rate", j_integer },
                                        {"sflow_i_iface_value", j_integer },
                                        {"sflow_o_iface_value", j_integer },
                                        {"sflow_vlan_src", j_integer },
                                        {"sflow_vlan_dst", j_integer },
                                        {"sflow_ipv4_src", j_string },
                                        {"sflow_ipv4_dst", j_string },
                                        {"sflow_frame_length", j_integer },
                                        {"sflow_frame_length_multiplied", j_integer },
                                        {"sflow_tcp_src_port", j_integer },
                                        {"sflow_tcp_dst_port", j_integer },
                                        {"sflow_i_iface_name", j_string },
                                        {"sflow_o_iface_name", j_string } };

struct json_data* sflow_flow_encode(struct sflow_samples *sflow_flows)
{
    json_t              *root;
    struct json_data    *jdata;
    int                 i,j,num_flows;

    if (sflow_flows == NULL)
        return (NULL);

    num_flows = sflow_flows->num_samples;
    if (num_flows == 0)
        return (NULL);

    root = json_object();
    jdata = malloc(sizeof(struct json_data));
    memset(jdata, 0, sizeof(struct json_data));

    for (i = 0; i <= JSON_FORMAT_SIZE; i++) {
        for (j = 0; j < num_flows; j++) {
            if (strcmp(frmt[i].json_name, sflow_flows->sample[j].sample_name) == 0) {
                if ((frmt[i].type == j_integer) && (*sflow_flows->sample[j].sample_type == INTEGER)) {
                    json_object_set_new(root, frmt[i].json_name,
                            json_integer(sflow_flows->sample[j].sample_value.value_i));
                } else if ((frmt[i].type == j_string) && (*sflow_flows->sample[j].sample_type == STRING)) {
                    json_object_set_new(root, frmt[i].json_name,
                            json_string(sflow_flows->sample[j].sample_value.value_s));
                }
            }
        }
    }

    jdata->data = (char *)json_dumps(root, JSON_COMPACT | JSON_PRESERVE_ORDER);
    jdata->data_len = strlen(jdata->data);

    json_object_clear(root);

    return (jdata);
}
