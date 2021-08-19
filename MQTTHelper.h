#ifndef _c_mqtt_helper
#define _c_mqtt_helper

#include <Arduino.h>
#include <WiFi.h>
#include <MQTT.h> //https://github.com/256dpi/arduino-mqtt

enum MQTTQOS
{
    AT_MOST_ONCE = 0,
    AT_LEAST_ONCE = 1,
    EXACTLY_ONCE = 2
};

struct MQTTNotification
{
    char *topic;
    char *payload;
};

/** Combination MQTT and Wifi Helper */
class MQTTHelper
{
public:
    static MQTTHelper *getInstance()
    {
        static MQTTHelper instance;
        return &instance;
    };

    MQTTNotification lastMsg;

    /** Must be called before use */
    void startConnection(char *ssid, const char *pass, const char *mqtt_host, const char *mqtt_user, const char *mqtt_pass, const char *mqtt_client);
    /** Start recieving notifications for a topic */
    void subscribe(const char *topic, MQTTQOS qos);
    /** Call every loop, do not alter the response object */
    const MQTTNotification *loop();
    /** Post a message to a topic
     * @return true if successful
     */
    bool send(const char *topic, const char *payload, bool retained, MQTTQOS qos);

private:
    WiFiClient net;
    MQTTClient client;

    const char *mqtt_user;
    const char *mqtt_pass;
    const char *mqtt_client;

    unsigned long lastMillis = 0;

    MQTTHelper();
    void connect();
};

#endif
