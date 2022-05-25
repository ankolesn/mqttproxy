//
// Created by ankolesn on 16.05.22.
//

#ifndef MQTTPROXY_MQTTPROXY_HPP
#define MQTTPROXY_MQTTPROXY_HPP


#include <mosquitto.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <iostream>


void publish_sensor_data(struct mosquitto *mosq, const std::string &topic, const std::string &data);

void on_connect(struct mosquitto *mosq, void *obj, int reason_code);

void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos);

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);

#endif //MQTTPROXY_MQTTPROXY_HPP


