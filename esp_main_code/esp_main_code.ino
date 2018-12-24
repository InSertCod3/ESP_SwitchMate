#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

// openweathermap Api Key
const String OWM_apikey = "0000000000000000000000000000";
 
// Wifi Network Creds
const char* ssid = "autoconnectap";
const char* password = "password";

WiFiManager wifiManager;

// Http Client
HTTPClient http;

// HTTP Server Init
ESP8266WebServer server(80);

// Rocker Pins Variables
const int rockerpin_zero = 4;     // The number of the pushbutton pin || GND --> (NC [2a125vac]) +  GPIO4 (D2) (C [2a125vac])
int rockerpin_zero_buttonState = 0;
const int rockerpin_two = 5;     // The number of the pushbutton pin || GND --> (NC [2a125vac]) +  GPIO5 (D1) (C [2a125vac])
int rockerpin_two_buttonState = 0;

void rgb_led_set_color(int rgb_pins[], int rgb_values[]) {
  /*
  - Set RGB led to == yellow
      rgb_led_set_color([11, 10, 9], [255, 255, 0]) 
  */
  analogWrite(rgb_pins[0], rgb_values[0]);
  analogWrite(rgb_pins[1], rgb_values[1]);
  analogWrite(rgb_pins[2], rgb_values[2]);  
}

void http_index() {
  server.send(200, "text/plain", "Seem Like Everthing is Working, :0");
}

void http_status(){
    server.send(200, "text/plain", "This is the Status Page");
}

class Weather_API {
    public: 
    const String city_weather_endpoint = "http://api.openweathermap.org/data/2.5/weather?q=";
    const String default_metric = "imperial";
    
    void get_weather_of_city(String city_loc) {
        /*
        get_weather_of_city("new york,us") 
         */
         String b_host = city_weather_endpoint + city_loc + "&units=" + default_metric;
         Serial.println("Sending HTTP GET (request) ... " + b_host);
          http.begin(b_host + "&APPID=" + OWM_apikey); //Specify the URL
          int httpCode = http.GET();  //Make the request
          if (httpCode > 0) { //Check for the returning code
              String payload = http.getString();
              Serial.println(httpCode);
              Serial.println(payload);
          } else {
              Serial.println("Error on HTTP request");
          }
          http.end(); //Free the resources
        }
};

void rocker_logic(){
  // read the state of the pushbutton value:
  rockerpin_zero_buttonState = digitalRead(rockerpin_zero);
  rockerpin_two_buttonState = digitalRead(rockerpin_two);
  
  if (rockerpin_zero_buttonState == HIGH) {
    
  } else {
    Serial.print("\nrockerpin_zero : Pressed");
  }

  if (rockerpin_two_buttonState == HIGH) {
    
  } else {
    Serial.print("\nrockerpin_two : Pressed");
  }
}

void handleNotFound(){
  String message = "Invaild Request\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void){
  // Init rocker pins for use.
  pinMode(rockerpin_zero, INPUT_PULLUP);
  pinMode(rockerpin_two, INPUT_PULLUP);
  
  // Weather API class Init
  Weather_API WAPI;
  
  // Serial Start at {115200}
  Serial.begin(115200);
  
  //  WiFi.begin(ssid, password);
  wifiManager.autoConnect(ssid);
  Serial.println("");

  // Wait for Esp the Establish Network Connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("ESP --> Connected to ");
  Serial.println(ssid);
  Serial.print("ESP --> IP address: ");
  Serial.println(WiFi.localIP());
  if (MDNS.begin("esp8266")) {
    Serial.println("ESP --> MDNS responder started");
  }

  server.on("/", http_index);
  server.on("/status", http_status);
  server.onNotFound(handleNotFound);
  server.begin();
  // WAPI.get_weather_of_city("new york,us");
  Serial.println("ESP --> HTTP server started");
}

void loop(void){
  server.handleClient();
  /// Handle Rocker Button Pin Logic
  rocker_logic();
}
