
//---------------------------------------------------------
// Includes

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "config.h"

//TODO: Separar en 2 programas -> Receptor y transmisor.

//TODO: Probar parsing de JSON para requests POST/PUT
//TODO: Mostrar errores cuando las peticiones son incorrectas
//TODO: Configurar como RELAY (si/no) -> reenvia la request a otra IP

//TODO: Separar codigo en funciones y cambiar a CONSTANTES, documentar -> emprolijar
//TODO: Set DEBUG variable para omitir comunicacion serial (?)

//---------------------------------------------------------
// Defines
#define WIFI_OUTPUT_PIN 2 // GPIO2
#define INITIAL_DEVICE_STATE LOW
#define JSON_CONTENT_TYPE "application/json"

//---------------------------------------------------------
// Variables init

const char* ssid      = WIFI_SSID;
const char* password  = WIFI_PWD;

bool deviceState = INITIAL_DEVICE_STATE;
const bool relayMode = DEVICE_RELAY ? HIGH : LOW;

// Create an instance of the server
// specify the port to listen on as an argument
ESP8266WebServer server(SERVER_PORT);

//---------------------------------------------------------

// Setup function
void setup() {
  
  // prepare GPIO2
    pinMode(WIFI_OUTPUT_PIN, OUTPUT);
    digitalWrite(WIFI_OUTPUT_PIN, deviceState);

  // start Serial communication
  Serial.begin(115200);
  delay(1500);

  setupWebServer();
}

// Connect and start WiFi server
void setupWebServer() {
  
  // Connect to WiFi network
  Serial.println();
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

  // Print the IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", [](){
    if(server.method() == HTTP_GET) {
      handleReadState();
    }
    else {
      handleUpdateState();
    }
  });

  // Start the server
  server.begin();
  Serial.println("HTTP Server started");
}

void handleReadState() {
  
  String currentState = (deviceState == HIGH) ? "on" : "off"; 
  server.send(200, JSON_CONTENT_TYPE, "{\"state\":\"" + currentState + "\"}");
}

void handleUpdateState(){
  
  String newState = "off";

  if(server.method() != HTTP_DELETE) {
    newState = server.arg("state");
  }

  deviceState = (newState == "on") ? HIGH : LOW;

  Serial.println("Updating PIN to state " + deviceState);
  digitalWrite(WIFI_OUTPUT_PIN, deviceState);

  handleReadState();
}

//---------------------------------------------------------
// Loop function

void loop() {
  server.handleClient();
}
