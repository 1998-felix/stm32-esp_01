#include <Arduino.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include <PubSubClient.h>

#include "secrets.h"

int status = WL_IDLE_STATUS; // Wifi status

// function prototypes
void callback(char *topic, byte *payload, unsigned int length);
void reconnect();

// Initialize second UART on STM32 client object
HardwareSerial Serial2(USART2);

// Initailize MQTT client on device
WiFiEspClient espClient;
PubSubClient client(espClient);

void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial2.begin(115200);
  // initialize ESP module
  WiFi.init(&Serial2);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true)
      ;
  }

  // attempt to connect to WiFi network
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(wi_fi.wifi_ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(wi_fi.wifi_ssid, wi_fi.wifi_pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");

  // connect to MQTT server
  client.setServer(server, 1883);
  client.setCallback(callback);
}

// print any message received for subscribed topic
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  delay(1000);
}

void reconnect()
{
  // Loop until connection is made
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect, just a name to identify the client
    if (client.connect(mf.thing_name, mf.thing_id, mf.thing_key))
    {
      Serial.println("connected");
      // Once connected, publish announcement...
      client.publish(mf.topic_1, "hello world");
      // ... and resubscribe
      client.subscribe(mf.topic_2);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}