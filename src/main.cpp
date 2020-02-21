#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>   
#include <Wire.h>
#include <Servo.h>
Servo servo;

#include <Adafruit_Sensor.h>
#include "Adafruit_BME280.h"
Adafruit_BME280 bme;

ESP8266WebServer server(80);

const char INDEX_HTML[] =
"<!DOCTYPE HTML>\n"
"<html>\n"
"\n"
"<head>\n"
"    <meta name=\"viewport\" content=\"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">\n"
"    <title>FingThing</title>\n"
"    <link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.4.1/css/bootstrap.min.css\">\n"
"    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js\"></script>\n"
"    <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.16.0/umd/popper.min.js\"></script>\n"
"    <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.4.1/js/bootstrap.min.js\"></script>\n"
"</head>\n"
"\n"
"<body>\n"
"    <nav class=\"navbar navbar-expand-lg navbar-light bg-light\">\n"
"        <a class=\"navbar-brand\" href=\"#\">FingThing - Dev</a>\n"
"        <button class=\"navbar-toggler\" type=\"button\" data-toggle=\"collapse\" data-target=\"#navbarSupportedContent\"\n"
"            aria-controls=\"navbarSupportedContent\" aria-expanded=\"false\" aria-label=\"Toggle navigation\">\n"
"            <span class=\"navbar-toggler-icon\"></span>\n"
"        </button>\n"
"\n"
"        <div class=\"collapse navbar-collapse\" id=\"navbarSupportedContent\">\n"
"            <ul class=\"navbar-nav mr-auto\">\n"
"                <li class=\"nav-item active\">\n"
"                    <a class=\"nav-link\" href=\"#\">Home <span class=\"sr-only\">(current)</span></a>\n"
"                </li>\n"
"                <li class=\"nav-item\">\n"
"                    <a class=\"nav-link\" href=\"reset\">Reset</a>\n"
"                </li>\n"
"            </ul>\n"
"            <form class=\"form-inline my-2 my-lg-0\" method=\"GET\" action=\"https://www.google.com/search\">\n"
"                <input class=\"form-control mr-sm-2\" type=\"search\" name=\"q\" placeholder=\"Search\" aria-label=\"Search\">\n"
"                <button class=\"btn btn-outline-success my-2 my-sm-0\" type=\"submit\">Search</button>\n"
"            </form>\n"
"        </div>\n"
"    </nav>\n"
"    <div class=\"container\">\n"
"        <div class=\"row mt-3\">\n"
"            <div class=\"card mx-auto\" style=\"width: 25rem;\">\n"
"                <div class=\"card-header\">\n"
"                    Light 1\n"
"                </div>\n"
"                <div class=\"card-body\">\n"
"                    <h5 class=\"card-title\">Control Panel</h5>\n"
"                    <p class=\"card-text\">\n"
"                        <FORM action=\"/\" method=\"post\">\n"
"                            <div class=\"form-group\">\n"
"                                <div class=\"custom-control custom-radio\">\n"
"                                    <input type=\"radio\" id=\"customRadio1\" name=\"LED\" value=\"1\"\n"
"                                        class=\"custom-control-input\">\n"
"                                    <label class=\"custom-control-label\" for=\"customRadio1\">On</label>\n"
"                                </div>\n"
"                                <div class=\"custom-control custom-radio\">\n"
"                                    <input type=\"radio\" id=\"customRadio2\" name=\"LED\" value=\"0\"\n"
"                                        class=\"custom-control-input\">\n"
"                                    <label class=\"custom-control-label\" for=\"customRadio2\">Off</label>\n"
"                                </div>\n"
"                            </div>\n"
"                            <div class=\"form-group\">\n"
"                                <div class=\"form-group row\">\n"
"                                    <label for=\"example-number-input\" class=\"col-4 col-form-label\">Rotate</label>\n"
"                                    <div class=\"col-10\">\n"
"                                        <input class=\"form-control width: 4rem;\" type=\"number\" name=\"rotate\" value=\"42\"\n"
"                                            id=\"example-number-input\">\n"
"                                    </div>\n"
"                                </div>\n"
"                            </div><button type=\"submit\" class=\"btn btn-primary\">Submit</button>\n"
"                            <button type=\"reset\" class=\"btn btn-primary\">Reset</button>\n"
"                        </FORM>\n"
"                    </p>\n"
"                </div>\n"
"            </div>\n"
"        </div>\n"
"    </div>\n"
"</body>\n"
"\n"
"</html>";

void returnFail(String msg)
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(500, "text/plain", msg + "\r\n");
}

void handleSubmit()
{
// The certificate is stored in PMEM


// And so is the key.  These could also be in DRAM


  String LEDvalue;
  int16_t rotate;
  if (server.hasArg("LED")){
    LEDvalue = server.arg("LED");
    if (LEDvalue == "1") {
      digitalWrite(LED_BUILTIN, 0);
      server.send(200, "text/html", INDEX_HTML);
    }
    else if (LEDvalue == "0") {
      digitalWrite(LED_BUILTIN, 1);
      server.send(200, "text/html", INDEX_HTML);
    }
    else {
      server.send(200, "text/html", INDEX_HTML);
    }
  } 
  else if(server.hasArg("rotate"))
  {
    rotate = server.arg("rotate").toInt();
    if(rotate > 50)
    {
      digitalWrite(LED_BUILTIN, 0);
      server.send(200, "text/html", INDEX_HTML);
    }
    else{
      digitalWrite(LED_BUILTIN,1);
      server.send(200, "text/html", INDEX_HTML);
    }
  } else {
    server.send(200, "text/html", INDEX_HTML);
  } 
}
 

void handleRoot() {
  handleSubmit();
  // if (server.hasArg("rotate")) {
   // handleSubmit();
  //}
  //else if (server.hasArg("LED")) {
  //  handleSubmit();
 // }
 // else {
 //   server.send(200, "text/html", INDEX_HTML);
 // }
}

void handleReset(){

    WiFiManager wifiManager;
  wifiManager.resetSettings();
  ESP.reset();
  
}

void handleNotFound() {
  digitalWrite(LED_BUILTIN, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message); 
  digitalWrite(LED_BUILTIN, 0);
}

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 1);
  Serial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.autoConnect("FingThing");
  //WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (MDNS.begin("fingerbox", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  server.on("/", handleRoot);

  server.on("/reset", handleReset);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  MDNS.addService("http", "tcp", 80);
  //MDNS.update();
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}