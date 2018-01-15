/*
 * Below The Line - Light
 * 
 * Create a WiFi Access Point with a Web Server
 * Any traffic to the web page will change the light
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>    // https://github.com/adafruit/Adafruit_NeoPixel
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

#define NEO_PIN            D2     // Pin the RGB LED (WS2812B) is attached

// Access Point Credentials
const char *ap_ssid     = "below";
const char *ap_password = "belowtheline";

ESP8266WebServer server(80);

// Reference the RGB LED
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, NEO_PIN);
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, NEO_PIN, NEO_GRB + NEO_KHZ800);

// State of the lights
bool light_on = false;

// Rotate pizel color
uint32_t colors[] = {
  pixels.Color(128,   0,   0),
  pixels.Color(  0, 128,   0),
  pixels.Color(  0,   0, 128),
  pixels.Color(128, 128,   0),
  pixels.Color(  0, 128, 128),
  pixels.Color(128,   0, 128),
  pixels.Color(128, 128, 128),
};
int max_colors = sizeof(colors)/sizeof(uint32_t);
int current_color = max_colors;   // Set to max so getColor flips over to 0 on first call


void setup() {
  Serial.begin(115200);

  // Setup Built In LED as well
  pinMode(BUILTIN_LED, OUTPUT);

  Serial.print("Configuring access point \"");
  Serial.print(ap_ssid);
  Serial.println("\" ...");

  // Start the Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // Setup the RGB LED
  pixels.begin();     // This initializes the NeoPixel library.
  light_on = false;   // Turn LED off to start
  display_light();
  
  // Set up the Server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

/*
 * Every time the web page is accessed turn the light on or off.
 */
void handleRoot() {
  light_on = !light_on;
  display_light();

  // Send back current state of Light
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["light_state"] = light_on;
  String payload;
  json.printTo(payload);
  server.send(200, "application/json", payload);
}

/*
 * Set the LED to ON or OFF based on light_on value
 */
void display_light() {
  digitalWrite(BUILTIN_LED, !light_on);
  if (light_on) {
    Serial.println("Light is ON");
    
    // Standard White
    //pixels.setPixelColor(0, pixels.Color(255, 255, 255));

    // Rotate Colors
    pixels.setPixelColor(0, getColor());
  }
  else {
    Serial.println("Light is OFF");
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));  
  }
  pixels.show();
  
}

/*
 * Get a different color each time called by rotating through the color array
 */
uint32_t getColor() {
  current_color++;
  if (current_color >= max_colors) {
    current_color = 0;
  }
  Serial.print("Current Color = ");
  Serial.println(current_color);
  Serial.print("Maximum Color = ");
  Serial.println(max_colors);
  return colors[current_color];
}

