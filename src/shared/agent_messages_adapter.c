/*
 * Utils Agent Messages Adapter
 * Copyright (C) 2015, Wazuh Inc.
 * Dec 29, 2022.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#include "agent_messages_adapter.h"
#include "cJSON.h"
#include <stdbool.h>

char* adapt_delta_message(const char* data, const char* name, const char* id, const char* ip, const char* node_name) {
    cJSON* j_msg_to_send = NULL;
    cJSON* j_agent_info = NULL;
    cJSON* j_msg = NULL;
    char* msg_to_send = NULL;

    j_msg = cJSON_Parse(data);
    if(!j_msg) {
        return NULL;
    }

    j_msg_to_send = cJSON_CreateObject();

    j_agent_info = cJSON_CreateObject();
    cJSON_AddStringToObject(j_agent_info, "agent_id", id);
    cJSON_AddStringToObject(j_agent_info, "agent_ip", ip);
    cJSON_AddStringToObject(j_agent_info, "agent_name", name);
    cJSON_AddStringToObject(j_agent_info, "node_name", node_name);
    cJSON_AddItemToObject(j_msg_to_send, "agent_info", j_agent_info);

    cJSON_AddItemToObject(j_msg_to_send, "data_type", cJSON_DetachItemFromObject(j_msg, "type"));

    cJSON_AddItemToObject(j_msg_to_send, "data", cJSON_DetachItemFromObject(j_msg, "data"));
    cJSON_AddItemToObject(j_msg_to_send, "operation", cJSON_DetachItemFromObject(j_msg, "operation"));

    msg_to_send = cJSON_PrintUnformatted(j_msg_to_send);

    cJSON_Delete(j_msg_to_send);
    cJSON_Delete(j_msg);

    return msg_to_send;
}

char* adapt_sync_message(const char* data, const char* name, const char* id, const char* ip, const char* node_name) {
    cJSON* j_msg_to_send = NULL;
    cJSON* j_agent_info = NULL;
    cJSON* j_msg = NULL;
    cJSON* j_data = NULL;
    char* msg_to_send = NULL;

    j_msg = cJSON_Parse(data);
    if(!j_msg) {
        return NULL;
    }

    j_msg_to_send = cJSON_CreateObject();

    j_agent_info = cJSON_CreateObject();
    cJSON_AddStringToObject(j_agent_info, "agent_id", id);
    cJSON_AddStringToObject(j_agent_info, "agent_ip", ip);
    cJSON_AddStringToObject(j_agent_info, "agent_name", name);
    cJSON_AddStringToObject(j_agent_info, "node_name", node_name);
    cJSON_AddItemToObject(j_msg_to_send, "agent_info", j_agent_info);

    cJSON_AddItemToObject(j_msg_to_send, "data_type", cJSON_DetachItemFromObject(j_msg, "type"));

    cJSON* j_data_msg = cJSON_GetObjectItem(j_msg, "data");
    if (j_data_msg) {
        j_data = cJSON_CreateObject();
        cJSON_AddItemToObject(j_data, "attributes_type", cJSON_DetachItemFromObject(j_msg, "component"));
        for (cJSON* j_item = j_data_msg->child; j_item; j_item = j_item->next) {
            cJSON_AddItemToObject(j_data, j_item->string, cJSON_Duplicate(cJSON_GetObjectItem(j_data_msg, j_item->string), true));
        }
        cJSON_AddItemToObject(j_msg_to_send, "data", j_data);
    }

    msg_to_send = cJSON_PrintUnformatted(j_msg_to_send);

    cJSON_Delete(j_msg_to_send);
    cJSON_Delete(j_msg);

    return msg_to_send;
}
