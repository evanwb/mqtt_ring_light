#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define PIXEL_PIN 15
#define PIXEL_COUNT 24

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

#define MQTT_VERSION MQTT_VERSION_3_1_1

const char *WIFI_SSID = "";
const char *WIFI_PASSWORD = "";

const PROGMEM char *MQTT_SERVER_IP = "";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char *MQTT_CLIENT_ID = "ring_light";

// MQTT: topic
const PROGMEM char *MQTT_SENSOR_TOPIC = "ring/#";

int red = 255, green = 255, blue = 255;
int brightness = 0;
int storedbrightness = 0;
boolean lightOn = false;
WiFiClient wifiClient;
PubSubClient client(wifiClient);

void publishData(char *topic, String message)
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();

  root.prettyPrintTo(Serial);
  char data[200];
  root.printTo(data, root.measureLength() + 1);
  client.publish(topic, data, true);
  yield();
}

void callback(char *p_topic, byte *p_payload, unsigned int p_length)
{
  Serial.println("Message recieved ");
  Serial.println(p_topic);
  String message;
  for (int i = 0; i < p_length; i++)
  {
    (message += (char)p_payload[i]);
  }
  int str_len = message.length() + 1;
  char char_array[str_len];
  message.toCharArray(char_array, str_len);
  Serial.println(message);

  if (strcmp(p_topic, "ring/color") == 0)
  {
    int r, g, b;
    if (sscanf(char_array, "%d,%d,%d", &r, &g, &b) == 3)
    {
      changeColor(r, g, b);
      char buffer[40];
      sprintf(buffer, "%d,%d,%d", r, g, b);
      Serial.print(buffer);
      client.publish("ring/color/status", buffer);
    }
  }
  else if (strcmp(p_topic, "ring/brightness") == 0)
  {
    int b = message.toInt();
    Serial.print("Brightness ");
    Serial.println(b);
    brightness = b;
    char buffer[40];
    sprintf(buffer, "%d", b);
    client.publish("ring/brightness/status", buffer);
  }
  else if (strcmp(p_topic, "ring/switch") == 0)
  {
    if (strcmp(char_array, "on") == 0)
    {
      brightness = lightOn ? brightness : storedbrightness != 0 ? storedbrightness : 200;
      client.publish("ring/status", "on");
      lightOn = true;
    }
    else
    {
      storedbrightness = brightness;
      brightness = 0;
      client.publish("ring/status", "off");
      lightOn = false;
    }
  }

  else if (strcmp(p_topic, "ring/temp") == 0)
  {
    int temp = message.toInt();
    int r = map(temp, 153, 500, 255, 255);
    int g = map(temp, 153, 500, 255, 160);
    int b = map(temp, 153, 500, 255, 3);
    changeColor(r, g, b);
    char buffer[40];
    sprintf(buffer, "%d", temp);
    char rgb[40];
    sprintf(rgb, "%d,%d,%d", r, g, b);
    client.publish("ring/temp/status", buffer);
    client.publish("ring/color/status", rgb);
  }
}

void reconnect()
{

  while (!client.connected())
  {
    Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, "", ""))
    {
      Serial.println("INFO: connected");
      client.subscribe(MQTT_SENSOR_TOPIC);
      break;
    }
    else
    {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void colorWipe(uint32_t color)
{
  for (int i = 0; i < strip.numPixels(); i++)
  { // For each pixel in strip...
    strip.setPixelColor(i, color); //  Set pixel's color (in RAM)
    strip.show();                  //  Update strip to match
  }
}

void changeColor(int r, int g, int b)
{
  red = r;
  green = g;
  blue = b;
}
void setup()
{
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  strip.show();  // Initialize all pixels to 'off'
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("INFO: WiFi connected");
  Serial.println("INFO: IP address: ");
  Serial.println(WiFi.localIP());

  // init the MQTT connection
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
  client.publish("ring/status", "off");
  client.publish("ring/brightness/status", "0");
  client.publish("ring/color/status", "0,0,0");
  strip.setBrightness(brightness);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  colorWipe(strip.Color(red, green, blue));
  strip.setBrightness(brightness);
  delay(200);
}
