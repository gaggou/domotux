/*
 * ESP8266 MQTT Client
 * It deals with 4 Channels
 * OUT_TOPIC_BUTTON: updated whenever button is changed with new button value
 * OUT_TOPIC_LED: updated when led controlled by button changes value. Update
 * mirrors the new value
 * IN_TOPIC: Switches on COMMAND_LED when '1' is sent to this channel. Switches 
 * it off on other values.
 *
 * Button is probed. When it is pressed, it toggles BUTTON_LED value.
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
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
const char* OUT_TOPIC_CODEUSE = "/esp/codeuse";
const char* IN_TOPIC = "/esp/led";
const char* IN_TOPIC_LINE0 = "/esp/message0";
const char* IN_TOPIC_LINE1 = "/esp/message1";
const char* ALIVE_TOPIC = "/alive";
const char* SENSOR_TOPIC = "/esp/sensor";
const char* SELF_NAME = "esp";

// Button debounce constants
const long debounceDelay = 50;    // the debounce time; increase if the output flickers

// IO constants, related to ESP8266 Board
const int I2C_INT = 16;
const int LED_RED = 15;
const int LED_BLUE = 13;
const int LED_GREEN = 12;
const int BUTTON_PIN = 4;
const int LED_LITTLE_BLUE = 2;
const int DEBUG_PIN1 = 3;

#define STATUS_LED LED_LITTLE_BLUE
#define ACTION_LED LED_BLUE
#define BUTTON_LED LED_GREEN

const unsigned long I2C_CLOCK = 100000;

// Network instances
WiFiClient espClient;
PubSubClient client(espClient);

// MQTT Messaging variables
long lastMsgTS = 0;
// To be removed soon: outgoing message
#if SEND_SENSOR_VALUE
char msg[10];
#endif // SEND_SENSOR_VALUE

#include <LiquidCrystal_I2C.h>
//LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

struct MessageHandle {
  const String*  message;
  unsigned int size;
  unsigned int index;
};

// Button debounce Variables
template <typename T> class ButtonHandle {
  public:
    int parameter;
    T state;             // the current reading from the input pin
    T lastState;   // the previous reading from the input pin
    long lastDebounceTime;  // the last time the output pin was toggled
    T (*getter)(int parameter);  // the last time the output pin was toggled
    ButtonHandle(int, T (*)(int));
    void handle(void (&commandAction)(T));
// public: void Button::handle(){
};
template <typename T> ButtonHandle<T>::ButtonHandle(int _parameter, T (*_getter)(int)){
  parameter = _parameter;
  getter = _getter;
  state = 0;
  lastState = 0;
  lastDebounceTime = 0;
}

template <typename T> void ButtonHandle<T>::handle(void (&commandAction)(T)){
  // read the state of the switch into a local variable:
  T reading = getter(parameter);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != state) {
      state = reading;
      commandAction(state);
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastState:
  lastState = reading;
}

bool pinGetter(int pin){
  return digitalRead(pin);
}

byte pcf8574Get(int address) {
  Wire.requestFrom(address, 1);
  if(Wire.available()){
    return Wire.read();
  }
}

//const char* toto[] = {"Message 1", "Message 2", "Message 3", "Message 4", "Message 5"};
MessageHandle msg;

bool ledState = LOW;

const String toto[] = {"Amandine", "Noemie", "Valentine", "Message 4", "Message 5"};

//void displayMH(LiquidCrystal_I2C& lcd, MessageHandle& m, int line){
//#if INCLUDE_SERIAL
//    Serial.print("LCD update: ");
//    Serial.println(m.message[m.index]);
//#endif // INCLUDE_SERIAL
//  lcd.setCursor(0,line);
//  lcd.print(m.message[m.index]);
//}
//
//void setupLCD() {
//  msg = {toto,5,0};
//  lcd.init();                      // initialize the lcd 
//  lcd.clear();
//  lcd.backlight();
//  displayMH(lcd, msg, 0);
//}

int updateValue(bool a, bool b, bool prevA, bool prevB, int value){
  // Front montant sur a
  if(a && !prevA){
    if(!b){
      value++;
    } else {
      value--;
    }
  } else if (!a&&prevA) {
    if(b){
      value++;
    } else {
      value--;
    }
  }else if(b && !prevB){
    if(a){
      value++;
    } else {
      value--;
    }
  } else if (!b&&prevB) {
    if(!a){
      value++;
    } else {
      value--;
    }
  }
  return value;
}

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

void setLED(bool value){
    Wire.beginTransmission(0x38);
    Wire.write(value?0x0:0xFF);
    Wire.endTransmission();
}

/**
 * Callback for MQTT messages
 **/
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
  char buffer[] = "                ";
  
  if(strcmp(topic, IN_TOPIC) == 0){
    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
      digitalWrite(ACTION_LED, HIGH);
      setLED(true);
    } else {
      digitalWrite(ACTION_LED, LOW);
      setLED(false);
    }
//  } else if(strcmp(topic, IN_TOPIC_LINE0) == 0){
//    for (int i=0; i<16 && i<length; i++){
//      buffer[i] = payload[i];
//    }
//    lcd.setCursor(0,0);
//    lcd.print(buffer);
//  } else if(strcmp(topic, IN_TOPIC_LINE1) == 0){
//    for (int i=0; i<16 && i<length; i++){
//      buffer[i] = payload[i];
//    }
//    lcd.setCursor(0,1);
//    lcd.print(buffer);
  }
}

/**
 * Reconnects to MQTT server
 **/
void reconnect() {
  // Loop until we're reconnected
  digitalWrite(STATUS_LED, LOW);
#if INCLUDE_SERIAL
  Serial.println("Attempting MQTT connection...");
#endif
  // Attempt to connect
  if (client.connect("ESP8266Client")) {
#if INCLUDE_SERIAL
    Serial.println("Connected");
#endif
    // Once connected, publish an announcement...
    client.publish(ALIVE_TOPIC, SELF_NAME);
    // ... and resubscribe
    client.subscribe(IN_TOPIC);
    client.subscribe(IN_TOPIC_LINE0);
    client.subscribe(IN_TOPIC_LINE1);
  } else {
#if INCLUDE_SERIAL
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");
#endif
  }
  if (client.connected()) {
    digitalWrite(STATUS_LED, LOW);
  }
}

void toggleGreen(bool state){
  // only toggle the LED if the new button state is HIGH
  if (state) {
       ledState = !ledState;
  }
  client.publish(OUT_TOPIC_BUTTON, state?"ON":"OFF");
  client.publish(OUT_TOPIC_LED, ledState?"ON":"OFF");
}

ButtonHandle<bool> button(BUTTON_PIN, &pinGetter);
ButtonHandle<byte> i2cProbe(0x38, &pcf8574Get);

void setupI2C(){
  Wire.begin();
  Wire.setClock(I2C_CLOCK);
}

void setup() {

//  pinMode(LED_RED, OUTPUT);
//  pinMode(LED_BLUE, OUTPUT);
//  pinMode(LED_GREEN, OUTPUT);
//  pinMode(LED_LITTLE_BLUE, OUTPUT);
//  pinMode(I2C_INT, INPUT_PULLUP);
//
//  pinMode(button.parameter, INPUT);
//  digitalWrite(STATUS_LED, HIGH);

#if INCLUDE_SERIAL
  //Serial.begin(115200);
#endif
  //setup_wifi();
  //client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
//  button.parameter = BUTTON_PIN;
//  button.state = LOW;             // the current reading from the input pin
//  button.lastState = LOW;   // the previous reading from the input pin
//  button.lastDebounceTime = 0;  // the last time the output pin was toggled
//  button.getter = &pinGetter;
//
//  i2cProbe.parameter = 0x38;
//  i2cProbe.state = 0;
//  i2cProbe.lastState = 0;
//  i2cProbe.lastDebounceTime = 0;
//  i2cProbe.getter = &pcf8574Get;

//  digitalWrite(STATUS_LED, LOW);
  //setupI2C();
  //setupLCD();
  setup_mrfc522();
}

volatile byte state = LOW;
int codeuse = 0;
//debounce
byte reading = 0;
byte lastButtonState = 0;
bool pinChanged = false;
bool stable = false;
byte stableCount = 0;
const byte STABILITY_CRITERIA = 3;
//const int analogOutPin = 5; // Analog output pin that the LED is attached to
int outputValue = 0;        // value output to the PWM (analog out)

byte buttonState = 0;
byte previousState = 0;
char buf[10];

void loop_i2c_probe(){
    // put your main code here, to run repeatedly
  if(!digitalRead(I2C_INT)) {
    stable = false;
    stableCount = 0;
  }
//  digitalWrite(DEBUG_PIN1, stable);
  if(!stable){
    Wire.requestFrom(0x38,1);
    if(Wire.available()){
      reading = ~Wire.read();
    }
    if(reading == lastButtonState){
      stableCount++;
    }
    if (stableCount >= STABILITY_CRITERIA){
      stable = true;
      buttonState = reading;
    }
    lastButtonState = reading;
    if(buttonState != previousState){
      int new_codeuse = updateValue(buttonState&0x8, buttonState&0x4, previousState&0x8, previousState&0x4, codeuse);
#if INCLUDE_SERIAL
      Serial.print("New Value: ");
      Serial.print(codeuse);
      Serial.print(" - ");
      Serial.print(buttonState, HEX);
      Serial.print(" - ");
      Serial.print(previousState, HEX);
      if(new_codeuse > codeuse){
        Serial.println(" +");
      }else{
        Serial.println(" -");
      }
#endif
      previousState = buttonState;
      codeuse = new_codeuse;
      //sprintf(buf,"%d", codeuse);
      //client.publish(OUT_TOPIC_CODEUSE, buf);
      sprintf(buf,"%x", buttonState);
      client.publish(OUT_TOPIC_CODEUSE, buf);

    }
  }

}

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN          5         // Configurable, see typical pin layout above
#define SS_PIN          16         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance


void setup_mrfc522() {
  Serial.begin(9600);   // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop_mrfc522() {
  ESP.wdtDisable();
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Dump debug info about the card; PICC_HaltA() is automatically called
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  ESP.wdtEnable(1000);
}

void loop() {

//  if (!client.connected()) {
//    reconnect();
//  } else {
//    digitalWrite(STATUS_LED, LOW);
//  }
  //client.loop();

//  button.handle(toggleGreen);
  long now = millis();
  if (now - lastMsgTS > 2000) {
    lastMsgTS = now;
#if SEND_SENSOR_VALUE
    snprintf (msg, 10, "%ld", analogRead(A0));
#if INCLUDE_SERIAL
    Serial.print("Publish message: ");
    Serial.println(msg);
#endif // INCLUDE_SERIAL
    //client.publish(SENSOR_TOPIC, msg);
#endif // SEND_SENSOR_VALUE
    //client.publish(ALIVE_TOPIC, SELF_NAME);

  }
  digitalWrite(BUTTON_LED, ledState);
  //loop_i2c_probe();
  loop_mrfc522();
}
