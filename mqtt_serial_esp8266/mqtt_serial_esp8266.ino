/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

#define DOMOTICZ_IN_TOPIC      "domoticz/in"   // [DomoticzInTopic]
#define DOMOTICZ_OUT_TOPIC     "domoticz/out"  // [DomoticzOutTopic]
  
// Update these with values suitable for your network.
// moved to secrets.h
//const char* ssid = "........";
//const char* password = "........";
//const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      IPAddress ip = WiFi.localIP();
      String ipstr = ip.toString();
      snprintf (msg, 75, "hello world from %s", ipstr.c_str() );
      client.publish("tele", msg);
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  String buf;
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (Serial.available()) {
     char waste = Serial.peek();
     if (waste < 0) {
        Serial.println("empty serial buffer");
     } else if (waste == 0x0A || waste == 0x0D) {
        waste = Serial.read();
        Serial.print(String(waste, HEX));
     }

     Serial.print("Serial input: ");
   
     buf = Serial.readStringUntil(0x0D);
     if (buf.length() > 0) {
       waste = Serial.peek();
       if (waste >= 0 && waste == 0x0A) {
          waste = Serial.read();
       }
       buf.replace(',',';');
       String newBuf = "{\"idx\":24,\"nvalue\":0,\"svalue\":\"" + buf + "\"}";

      client.publish(DOMOTICZ_IN_TOPIC, newBuf.c_str());
      lastMsg = now;
     }
  } else {

//  if (now - lastMsg > 10000) {
//    lastMsg = now;
//    ++value;
//    snprintf (msg, 75, "hello world #%ld", value);
//    Serial.print("Publish message: ");
//    Serial.println(msg);
//    client.publish("tele", msg);
//  }
  }
}
