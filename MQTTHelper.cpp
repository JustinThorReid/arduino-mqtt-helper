#include "MQTTHelper.h"

#define SerialPrintln(msg) Serial.println(msg)
#define SerialPrint(msg) Serial.print(msg)

void messageReceived(String &topic, String &payload)
{
    SerialPrintln("incoming: " + topic + " - " + payload);

    MQTTHelper::getInstance()->lastMsg.topic = new char[topic.length() + 1];
    strcpy(MQTTHelper::getInstance()->lastMsg.topic, topic.c_str());

    MQTTHelper::getInstance()->lastMsg.payload = new char[payload.length() + 1];
    strcpy(MQTTHelper::getInstance()->lastMsg.payload, payload.c_str());

    //payloadRecieved = payload.c_str();

    // Note: Do not use the client in the callback to publish, subscribe or
    // unsubscribe as it may cause deadlocks when other things arrive while
    // sending and receiving acknowledgments. Instead, change a global variable,
    // or push to a queue and handle it in the loop after calling `client.loop()`.
}

MQTTHelper::MQTTHelper()
{
}

const MQTTNotification *MQTTHelper::loop()
{
    if (lastMsg.topic != nullptr)
    {
        free(lastMsg.topic);
        lastMsg.topic = nullptr;
    }

    if (lastMsg.payload != nullptr)
    {
        free(lastMsg.payload);
        lastMsg.payload = nullptr;
    }

    if (millis() - lastMillis > 10)
    {
        lastMillis = millis();

        client.loop();
        //delay(10); // <- fixes some issues with WiFi stability

        if (!client.connected())
        {
            connect();
        }
    }

    if (lastMsg.topic != nullptr)
    {
        return &lastMsg;
    }
    return nullptr;
}

bool MQTTHelper::send(const char *topic, const char *payload, bool retained, MQTTQOS qos)
{
    client.publish(topic, payload, retained, qos);
}

void MQTTHelper::subscribe(const char *topic, MQTTQOS qos)
{
    client.subscribe(topic, qos);
}

void MQTTHelper::startConnection(char *ssid, const char *pass, const char *mqtt_host, const char *mqtt_user, const char *mqtt_pass, const char *mqtt_client)
{
    this->mqtt_client = mqtt_client;
    this->mqtt_pass = mqtt_pass;
    this->mqtt_user = mqtt_user;

    WiFi.begin(ssid, pass);
    WiFi.setAutoReconnect(true);
    client.begin(mqtt_host, net);
    client.onMessage(messageReceived);

    connect();
}

void MQTTHelper::connect()
{
    SerialPrint("checking wifi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        SerialPrint(".");
        delay(1000);
    }

    SerialPrint("\nconnecting to MQTT...");
    while (!client.connect(mqtt_client, mqtt_user, mqtt_pass))
    {
        SerialPrint(".");
        delay(1000);
    }

    SerialPrintln("\nconnected!");
}
