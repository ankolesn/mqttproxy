//
// Created by ankolesn on 16.05.22.
//

#include "mqttproxy.hpp"

using namespace std;

namespace settings {
    std::map<std::string, std::string> topics{
            {"zigbee2mqtt/temp",    "room1"},
            {"zigbee2mqtt/light1",  "room1"},
            {"zigbee2mqtt/light2",  "room2"},
            {"zigbee2mqtt/contact", "room2"}
    };
    const std::string host = "192.168.24.179";
}

/* This function pretends to read some data from a sensor and publish it.*/
void publish_sensor_data(struct mosquitto *mosq, const string &topic, const string &data) {

    /* Publish the message
     * mosq - our client instance
     * *mid = nullptr - we don't want to know what the message id for this message is
     * topic = "example/temperature" - the topic on which this message will be published
     * payloadlen = strlen(payload) - the length of our payload in bytes
     * payload - the actual payload
     * qos = 2 - publish with QoS 2 for this example
     * retain = false - do not use the retained message feature for this message
     */
    auto rc = mosquitto_publish(mosq, nullptr, topic.c_str(), data.size(), data.c_str(), 2, false);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error publishing: %s\n", mosquitto_strerror(rc));
    }
}

/* Callback called when the client receives a CONNACK message from the broker. */
void on_connect(struct mosquitto *mosq, void *obj, int reason_code) {
    int rc;
    /* Print out the connection result. mosquitto_connack_string() produces an
     * appropriate string for MQTT v3.x clients, the equivalent for MQTT v5.0
     * clients is mosquitto_reason_string().
     */
    printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
    if (reason_code != 0) {
        /* If the connection fails for any reason, we don't want to keep on
         * retrying in this example, so disconnect. Without this, the client
         * will attempt to reconnect. */
        mosquitto_disconnect(mosq);
    }

    /* Making subscriptions in the on_connect() callback means that if the
     * connection drops and is automatically resumed by the client, then the
     * subscriptions will be recreated when the client reconnects. */
    rc = mosquitto_subscribe(mosq, nullptr, "zigbee2mqtt/#", 1);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
        /* We might as well disconnect if we were unable to subscribe */
        mosquitto_disconnect(mosq);
    }
}


/* Callback called when the broker sends a SUBACK in response to a SUBSCRIBE. */
void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos) {
    int i;
    bool have_subscription = false;

    /* In this example we only subscribe to a single topic at once, but a
     * SUBSCRIBE can contain many topics at once, so this is one way to check
     * them all. */
    for (i = 0; i < qos_count; i++) {
        printf("on_subscribe: %d:granted qos = %d\n", i, granted_qos[i]);
        if (granted_qos[i] <= 2) {
            have_subscription = true;
        }
    }
    if (!have_subscription) {
        /* The broker rejected all of our subscriptions, we know we only sent
         * the one SUBSCRIBE, so there is no point remaining connected. */
        fprintf(stderr, "Error: All subscriptions rejected.\n");
        mosquitto_disconnect(mosq);
    }
}


/* Callback called when the client receives a message. */
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    using nlohmann::json;
    json j;
    string m = (char *) msg->payload;
    try {
        j = json::parse(m);
    }
    catch (...) {
        cerr << "Error\n";
    }
    for (auto &el: j.items()) {
        if (settings::topics.find(msg->topic) != settings::topics.end()) {
            auto v = el.value().get<float>();
            string topic = "home/" + settings::topics[msg->topic] + '/' + el.key();
            publish_sensor_data(mosq, topic, to_string(v));
        }
    }
}
