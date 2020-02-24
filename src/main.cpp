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
#define SEALEVELPRESSURE_HPA (1016.20)

ESP8266WebServer server(80);
int16_t start = 90;
int16_t stop = 180;
int16_t delay_t = 100;

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
"    <script>\n"
"        $(document).ready(function () {\n"
"\n"
"            (function worker() {\n"
"                $.ajax({\n"
"                    //window.origin \n"
"                    url: window.origin + '/data',\n"
"                    success: function (data) {\n"
"                        $('#dgc').text(data[\"dgc\"]);\n"
"                        $('#prh').text(data[\"prh\"]);\n"
"                        $('#hpa').text(data[\"hpa\"]);\n"
"                    },\n"
"                    complete: function () {\n"
"                        // Schedule the next request when the current one's complete\n"
"                        setTimeout(worker, 10000);\n"
"                    }\n"
"                });\n"
"            })();\n"
"        });\n"
"    </script>\n"
"</head>\n"
"\n"
"<body>\n"
"\n"
"    <nav class=\"navbar navbar-expand-lg navbar-light bg-light\">\n"
"        <a class=\"navbar-brand\" style=\"font-family: 'Comic Sans MS', cursive, sans-serif\" href=\"#\">FingThing - Dev</a>\n"
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
"                    <a class=\"nav-link\" href=\"/reset\">Reset</a>\n"
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
"                    <div class=\"row\">\n"
"                        <div class=\"col-md-7\">Switch 1</div>\n"
"                        <div class=\"col-md-5 float-right\"><i id=\"dgc\"></i><i>C </i><i id=\"prh\"></i><i>% </i><i\n"
"                                id=\"hpa\"></i><i>hPa</i></div>\n"
"                    </div>\n"
"                </div>\n"
"                <div class=\"card-body\">\n"
"                    <h5 class=\"card-title\">Control Panel</h5>\n"
"                    <p class=\"card-text\">\n"
"                        <div class=\"row\">\n"
"                            <form action=\"/\" method=\"post\">\n"
"                                <input type=\"hidden\" name=\"switch\">\n"
"                                <button type=\"submit\" class=\"btn btn-primary\">Give it a finger</button>\n"
"                            </form>\n"
"                        </div>\n"
"                        <p></p>\n"
"                        <div class=\"row\">\n"
"                            <p>\n"
"                                <button class=\"btn btn-primary\" type=\"button\" data-toggle=\"collapse\"\n"
"                                    data-target=\"#collapseExample\" aria-expanded=\"false\"\n"
"                                    aria-controls=\"collapseExample\">\n"
"                                    Settings\n"
"                                </button>\n"
"                            </p>\n"
"                            <div class=\"collapse\" id=\"collapseExample\">\n"
"                                <FORM action=\"/\" method=\"post\">\n"
"                                    <div class=\"form-group\">\n"
"                                        <div class=\"custom-control custom-radio\">\n"
"                                            <input type=\"radio\" id=\"customRadio1\" name=\"LED\" value=\"1\"\n"
"                                                class=\"custom-control-input\">\n"
"                                            <label class=\"custom-control-label\" for=\"customRadio1\">On</label>\n"
"                                        </div>\n"
"                                        <div class=\"custom-control custom-radio\">\n"
"                                            <input type=\"radio\" id=\"customRadio2\" name=\"LED\" value=\"0\"\n"
"                                                class=\"custom-control-input\">\n"
"                                            <label class=\"custom-control-label\" for=\"customRadio2\">Off</label>\n"
"                                        </div>\n"
"                                    </div>\n"
"                                    <div class=\"form-group\">\n"
"                                        <div class=\"form-group row\">\n"
"                                            <label for=\"example-number-input\" class=\"col-4 col-form-label\">Start</label>\n"
"                                            <div class=\"col-10\">\n"
"                                                <input class=\"form-control width: 4rem;\" type=\"number\" name=\"start\"\n"
"                                                    value=\"90\" id=\"example-number-input\">\n"
"                                            </div>\n"
"                                            <label for=\"example-number-input1\" class=\"col-4 col-form-label\">Stop</label>\n"
"                                            <div class=\"col-10\">\n"
"                                                <input class=\"form-control width: 4rem;\" type=\"number\" name=\"stop\"\n"
"                                                    value=\"180\" id=\"example-number-input1\">\n"
"                                            </div>\n"
"                                            <label for=\"example-number-input2\"\n"
"                                                class=\"col-4 col-form-label\">Delay</label>\n"
"                                            <div class=\"col-10\">\n"
"                                                <input class=\"form-control width: 4rem;\" type=\"number\" name=\"delay\"\n"
"                                                    value=\"100\" id=\"example-number-input2\">\n"
"                                            </div>\n"
"                                        </div>\n"
"                                    </div><button type=\"submit\" class=\"btn btn-primary\">Submit</button>\n"
"                                    <button type=\"reset\" class=\"btn btn-primary\">Reset</button>\n"
"                                </FORM>\n"
"                            </div>\n"
"                        </div>\n"
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

void move(int from, int to) {
  int slow = 20;
  int step = 1;

  if(from == to)
    return;

  if(from < to) {
    for(int pos = from; pos < to; pos += step) {
      servo.write(pos);
      delay(slow);
    }
  } else {
    for(int pos = from; pos > to; pos -= step) {
      servo.write(pos);
      delay(slow);
    }
  }
}

void handleTemp()
{
  String res = "";
  int dgc = bme.readTemperature();
  int hpa = bme.readPressure() / 100;
  int prh = bme.readHumidity();
  res = "{\"dgc\": "+String(dgc)+", \"prh\": "+String(prh)+", \"hpa\": "+String(hpa)+"}";
  server.sendHeader("Access-Control-Allow-Origin","*");
  server.send(200, "text/json", res);
}
 

void handleRoot() {
  String LEDvalue;
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
  else if(server.hasArg("switch"))
  {
    move(start,stop);
    delay(delay_t);
    move(stop,start);
    server.send(200, "text/html", INDEX_HTML);
  }
  else if(server.hasArg("start"))
  {
    start = server.arg("start").toInt();
    stop = server.arg("stop").toInt();
    delay_t = server.arg("delay").toInt();
    digitalWrite(LED_BUILTIN, 0);
    move(start, stop);
    delay(delay_t);
    move(stop,start);
    server.send(200, "text/html", INDEX_HTML);
  } else {
    server.send(200, "text/html", INDEX_HTML);
  } 
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
  //Pin config
  pinMode(LED_BUILTIN, OUTPUT);


  digitalWrite(LED_BUILTIN, 1);
  //Serial setup
  Serial.begin(115200);

  //Wifi
  WiFiManager wifiManager;
  wifiManager.autoConnect("FingThing");
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
  
  if (MDNS.begin("fingthing", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  
  //Servo setup
  servo.attach(D8);

  //bme setup
  bme.begin(0x76);

  bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                  Adafruit_BME280::SAMPLING_X1,
                  Adafruit_BME280::SAMPLING_X1,
                  Adafruit_BME280::SAMPLING_X1,
                  Adafruit_BME280::FILTER_X4,
                  Adafruit_BME280::STANDBY_MS_0_5
                  );

  //Server setup
  server.on("/", handleRoot);

  server.on("/reset", handleReset);
  server.on("/data", handleTemp);
  server.on("/data", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin","*");
    server.send(204);
 });

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