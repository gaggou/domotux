/*
 *
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
#define INCLUDE_SERIAL 1
#define SEND_SENSOR_VALUE 0

// This file will not be part of the released software
// It defines the PERSO_DEFINE_* referrenced below
#include "perso_network.h"

// Update these with values suitable for your network.

// Network constants
const char* MY_SSID = PERSO_DEFINE_SSID;
const char* MY_PASSWORD = PERSO_DEFINE_PASS;
const char* mqtt_server = PERSO_DEFINE_SERVER_URL;
const char* OUT_TOPIC_BUTTON = "/esp/button";
const char* OUT_TOPIC_LED = "/esp/command";
const char* IN_TOPIC = "/esp/led";

// Button debounce constants
const long debounceDelay = 50;    // the debounce time; increase if the output flickers

// IO constants, related to ESP8266 Board
const int LED_RED = 15;
const int LED_BLUE = 13;
const int LED_GREEN = 12;
const int BUTTON_PIN = 4;

#define STATUS_LED LED_RED
#define ACTION_LED LED_BLUE
#define BUTTON_LED LED_GREEN

// Network instances
WiFiClient espClient;
PubSubClient client(espClient);

// MQTT Messaging variables
long lastMsgTS = 0;
// To be removed soon: outgoing message
#if SEND_SENSOR_VALUE
char msg[10];
#endif // SEND_SENSOR_VALUE

// Button debounce Variables
struct ButtonHandle {
  int buttonPin;
  int buttonState;             // the current reading from the input pin
  int lastButtonState;   // the previous reading from the input pin
  long lastDebounceTime;  // the last time the output pin was toggled
};

bool ledState = LOW;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
#if INCLUDE_SERIAL
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(MY_SSID);
#endif

  // Wifi client only, no AP.
  WiFi.mode(WIFI_STA);
  WiFi.begin(MY_SSID, MY_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#if INCLUDE_SERIAL
    Serial.print(".");
#endif
  }

#if INCLUDE_SERIAL
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

void callback(char* topic, byte* payload, unsigned int length) {
#if INCLUDE_SERIAL
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
#endif

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(ACTION_LED, HIGH);
  } else {
    digitalWrite(ACTION_LED, LOW);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
#if INCLUDE_SERIAL
    Serial.print("Attempting MQTT connection...");
#endif
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
#if INCLUDE_SERIAL
      Serial.println("connected");
#endif
      // Once connected, publish an announcement...
      client.publish("alive", "esp");
      // ... and resubscribe
      client.subscribe(IN_TOPIC);
    } else {
#if INCLUDE_SERIAL
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
#endif
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


bool handleButton(ButtonHandle &button){
  // read the state of the switch into a local variable:
  int reading = digitalRead(button.buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != button.lastButtonState) {
    // reset the debouncing timer
    button.lastDebounceTime = millis();
  }

  if ((millis() - button.lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != button.buttonState) {
      button.buttonState = reading;
      commandAction(button.buttonState);
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  button.lastButtonState = reading;
}

void commandAction(bool state){
  // only toggle the LED if the new button state is HIGH
  if (state) {
       ledState = !ledState;
  }
  client.publish(OUT_TOPIC_BUTTON, state?"ON":"OFF");
  client.publish(OUT_TOPIC_LED, ledState?"ON":"OFF");
}

ButtonHandle button;

void setup() {

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  pinMode(button.buttonPin, INPUT);
  digitalWrite(STATUS_LED, HIGH);

#if INCLUDE_SERIAL
  Serial.begin(115200);
#endif
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  button.buttonPin = BUTTON_PIN;
  button.buttonState = LOW;             // the current reading from the input pin
  button.lastButtonState = LOW;   // the previous reading from the input pin
  button.lastDebounceTime = 0;  // the last time the output pin was toggled
  digitalWrite(STATUS_LED, LOW);

}

void loop() {

  if (!client.connected()) {
    digitalWrite(STATUS_LED, HIGH);
    // Warning : This is a blocking call
    // TODO: remove the blocking call
    reconnect();
    digitalWrite(STATUS_LED, LOW);
  }
  client.loop();

  handleButton(button);
  long now = millis();
  if (now - lastMsgTS > 2000) {
    lastMsgTS = now;
#if SEND_SENSOR_VALUE
    snprintf (msg, 10, "%ld", analogRead(A0));
#if INCLUDE_SERIAL
    Serial.print("Publish message: ");
    Serial.println(msg);
#endif // INCLUDE_SERIAL
    client.publish("/esp/sensor", msg);
#endif // SEND_SENSOR_VALUE
    client.publish("alive", "esp");
  }
  digitalWrite(BUTTON_LED, ledState);
}
