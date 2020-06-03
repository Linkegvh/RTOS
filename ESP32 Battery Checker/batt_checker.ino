#include <WiFi.h>
#include <WebServer.h>

/*Put your SSID & Password*/
const char* ssid = "Lingke @05-163";  // Enter SSID here
const char* password = "84931801";  //Enter Password here

/****
 * Creating an instance of the WebServer Class, with the HTTP port being 80
 * ***/
WebServer server(80);


/********************************
 * ADC pin setup
 * As far fetched as the number look like, they are actually adjacent ADC pins
 * ******************************/

uint8_t ADC_Pin_num = 6;
const uint8_t ADC_Pin1 [ADC_Pin_num] = {12, 14, 27, 26, 25, 23};
uint8_t Voltage_levels[ADC_Pin_num];

const uint8_t NUM = 0;

void setup() {
    std_setup();
    WiFi_setup();
    server_setup();
}

void loop() {
    server.handleClient();
    ADC_read(Voltage_levels);
    server.send(200, "text/html", SendHTML()); 
    delay(1000);
}
/***********************************************************************
 *
 * The below block are set-up overheads that has to do with WIFI and HTTP server
 *
/**********************************************************************/

// standard set up takes care of setting up serial communication
// and pin mode for each GPIO and ADC pins. 
void std_setup(){
    Serial.begin(115200);
    delay(100);
}

void WiFi_setup(){
    Serial.println("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid,password);
    
    //check wi-fi is connected to wi-fi network
    while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected..!");
    Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
}

void server_setup(){
    server.on("/", handle_OnConnect);
    server.onNotFound(handle_NotFound);

    server.begin();
    Serial.println("HTTP server started");   
}

void handle_OnConnect() {
  NUM ++;
  Serial.print("Connection Established for the ");
  Serial.print(NUM);
  Serial.println("th time");

  for (int i = 0; i < ADC_Pin_num; i ++) Voltage_levels[i] = 0;

  server.send(200, "text/html", SendHTML()); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(){
    String ptr = "<!DOCTYPE html> <html>\n";
    ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
    ptr +="<title>Voltage Checker (Remote Version)</title>\n";
    ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
    ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
    ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
    ptr +=".button-on {background-color: #3498db;}\n";
    ptr +=".button-on:active {background-color: #2980b9;}\n";
    ptr +=".button-off {background-color: #34495e;}\n";
    ptr +=".button-off:active {background-color: #2c3e50;}\n";
    ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
    ptr +="</style>\n";
    ptr +="</head>\n";
    ptr +="<body>\n";
    ptr +="<h1>ESP32 Web Server</h1>\n";
    ptr +="<h3>Using Station(STA) Mode</h3>\n";


    int i = 0;

    while(i < ADC_Pin_num){
        ptr += "<p>";
        ptr += i;
        ptr += "th Cell: ";
        ptr += Voltage_levels[i];
        ptr += "V </p>\n";
        i++;
    }

    ptr +="</body>\n";
    ptr +="</html>\n";
    return ptr;
}


/************************************************************************
 * 
 * ADC read function
 * 
 * Returns ADC value of pins defined in teh ADC_Pin1[6] in front. 
 * 
 * ADC resolution: 12 bits (0 ~ 4095) (0V ~ 3.3V)
 * 
 * **********************************************************************/

void ADC_read(uint8_t *Voltage_levels){
    int i;
    for (i = 0; i < ADC_Pin_num ; i++){
        Voltage_levels[i] = analogRead(ADC_Pin1[i]);
    }
    return;
}