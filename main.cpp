#include "mqttproxy.hpp"


using namespace std;


int main(int argc, char *argv[]) {
    struct mosquitto *mosq;
    int rc;

    /* Required before calling other mosquitto functions */
    mosquitto_lib_init();

    /* Create a new client instance.
     * id = nullptr -> ask the broker to generate a client id for us
     * clean session = true -> the broker should remove old sessions when we connect
     * obj = nullptr -> we aren't passing any of our private data for callbacks
     */
    mosq = mosquitto_new(nullptr, true, nullptr);
    if (mosq == nullptr) {
        fprintf(stderr, "Error: Out of memory.\n");
        return 1;
    }

    /* Configure callbacks. This should be done before connecting ideally. */
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_subscribe_callback_set(mosq, on_subscribe);
    mosquitto_message_callback_set(mosq, on_message);

    /* Connect to test.mosquitto.org on port 1883, with a keepalive of 60 seconds.
     * This call makes the socket connection only, it does not complete the MQTT
     * CONNECT/CONNACK flow, you should use mosquitto_loop_start() or
     * mosquitto_loop_forever() for processing net traffic. */
    rc = mosquitto_connect(mosq, "192.168.24.179", 1883, 60);
    if (rc != MOSQ_ERR_SUCCESS) {
        mosquitto_destroy(mosq);
        fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
        return 1;
    }

    /* Run the network loop in a blocking call. The only thing we do in this
     * example is to print incoming messages, so a blocking call here is fine.
     *
     * This call will continue forever, carrying automatic reconnections if
     * necessary, until the user calls mosquitto_disconnect().
     */
    mosquitto_loop_forever(mosq, -1, 1);

    mosquitto_lib_cleanup();
    return 0;
}