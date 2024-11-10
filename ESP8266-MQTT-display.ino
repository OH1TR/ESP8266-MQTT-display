#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1

//const int ledPin = 2;

// Replace the next variables with your SSID/Password combination
const char* ssid = "YOUR_WLAN_SSID";
const char* password = "YOUR_WLAN_PASSWORD";


const char* mqtt_server = "YOUR_MQTT_SERVER_IP_OR_NAME";
const char* mqtt_user = "YOUR_MQTT_USERNAME";
const char* mqtt_password = "YOUR_MQTT_PASSWORD";
const char* mqtt_sub_subject = "espdisplay/display";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long LastMsg=0;

void SysMsg(char* msg)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(msg);
  display.display();
}


void setup() {
//  pinMode(ledPin, OUTPUT);
//  digitalWrite(ledPin, HIGH);

  Serial.begin(115200);
  Wire.begin(5, 4);    
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  SysMsg("Init...");

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
  WiFi.mode(WIFI_STA);
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

void callback(char* topic, byte* message, unsigned int length) {  
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println();

  JsonDocument doc;
  deserializeJson(doc, message,length);

  JsonArray array = doc.as<JsonArray>();
  for(JsonVariant v : array) {
     const auto& kv = v.as<JsonObject>();  
     const char* cmd = kv["c"];
     if(cmd[0]=='C')
     {
        display.clearDisplay();
        Serial.println("Clear display");
     }
     else if (cmd[0]=='F')
     {
      const int v = kv["v"];
      display.setTextSize(v);
      Serial.print("setTextSize");
      Serial.print(v);
      Serial.println();
     }
    else if (cmd[0]=='G')
     {
      const int x = kv["x"];
      const int y = kv["y"];
      display.setCursor(x, y);
      Serial.print("setCursor");
      Serial.print(x);
      Serial.print(y);
      Serial.println();
     }
     else if (cmd[0]=='P')
     {
      const char* v = kv["v"];
      display.println(v);
      Serial.print("println");
      Serial.print(v);
      Serial.println();
     }
     else if (cmd[0]=='D')
     {
      display.display();
      Serial.println("display");
     }
      Serial.println(cmd);
  }
  LastMsg=millis();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client",mqtt_user,mqtt_password)) {
      Serial.println("connected");
      client.subscribe(mqtt_sub_subject);
      SysMsg("MQTT connected");
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
  if(WiFi.status() != WL_CONNECTED)
  {
    SysMsg("No WiFi");
  }
  else if(!client.connected())
  {
    SysMsg("No MQTT");
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //overflow?
  if(LastMsg<millis() && (millis()-LastMsg)>60*60*1000)
  {
    SysMsg("No messages");
    delay(5000);
    ESP.restart();
  }
}