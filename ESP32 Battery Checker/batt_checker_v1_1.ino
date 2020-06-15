#include <WiFi.h>
#include <WebServer.h>

/**********************************************
 * 
 * Version 1.1
 * 
 * Updates includes 
 * - a ADC voltage level math calculation
 * - establish a esp32 based wifi because connecting to my own home wifi seems to have some problem
 * 
 * ********************************************/


/*Put your SSID & Password*/
const char* ssid = "ESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */
// This could be removed if we are connecting to another wifi.
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

/****
 * Creating an instance of the WebServer Class, with the HTTP port being 80
 * ***/
WebServer server(80);

/********************************
 * ADC pin setup
 * As far fetched as the number look like, they are actually adjacent ADC pins
 * ******************************/

const uint8_t ADC_Pin_num = 3;
const uint8_t ADC_Pin1 [ADC_Pin_num] = {12, 14, 27};
float calcualted_per_cell_voltage[ADC_Pin_num];
float calculated_accumulative_voltage [ADC_Pin_num];

uint8_t NUM = 0;
uint8_t test_NUM = 0;

void setup() {
    Serial.begin(115200);
    delay(100);
    int i = 0;
    while (i < ADC_Pin_num){
        calculated_accumulative_voltage[i] = 0;
        calcualted_per_cell_voltage[i] = 0;
        i++;
    }

    // wifi WiFi set up
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);

    // server set up
    server.on("/", handle_OnConnect);
    server.onNotFound(handle_NotFound);

    server.begin();
    Serial.println("HTTP server started");   
}

void loop() {
    server.handleClient();
    uint16_t Voltage_reading_raw [ADC_Pin_num]; // ADC pins is 12 bit for ESP32 based board

    ADC_read(Voltage_reading_raw);

    calculation(Voltage_reading_raw, calcualted_per_cell_voltage, calculated_accumulative_voltage);

    //server.send(200, "text/html", SendHTML()); 
    delay(1000);
}
/***********************************************************************
 *
 * The below block are set-up overheads that has to do with WIFI and HTTP server
 *
/**********************************************************************/


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

void handle_OnConnect() {
  NUM ++;
  Serial.print("Connection Established for the ");
  Serial.print(NUM);
  Serial.println("th time");

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
        ptr += i + 1; // starting with 1 instead of 0
        ptr += "th Cell: ";
        ptr += calcualted_per_cell_voltage[i];
        ptr += "V, ";
        
        ptr += "Accumulative voltage: ";
        ptr += calculated_accumulative_voltage[i];
        ptr += "V ";
        ptr += "</p>\n";
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

void ADC_read(uint16_t *Voltage_levels){
    int i;
    for (i = 0; i < ADC_Pin_num ; i++){
        Voltage_levels[i] = analogRead(ADC_Pin1[i]);
    }
    return;
}


/*************************
 * 
 * Calculation will update the true per cell voltage of the LiPo battery and also update the true accumulation voltage
 * of x number cells voltage combined together. 
 * 
 * The switch statement handles the different cases of multipler for different LiPo cells as they are all different
 * 
 * ***********************/ 
void calculation(uint16_t * voltage_raw, float * per_cell_vol, float * accum_vol){
    int i = 0;
    float temp = 0;
    while (i < ADC_Pin_num){
        // the max value for 12 bit is 4096 and for ESP32 ADC pins that means 3.3V
        temp = ((float) voltage_raw[i] / 4096) * 3.3;
        
        switch (i){
            case 0:
                accum_vol[i] = (temp * 10) / 7; break;
            case 1:
                accum_vol[i] = (temp * 20) / 7; break;
            case 2:
                accum_vol[i] = temp * 4; break;
        }
        
        if (i = 0) 
            per_cell_vol[i] = accum_vol[i];
        else 
            per_cell_vol[i] = accum_vol[i] - accum_vol[i - 1];
        
        i ++;
    }
    test_NUM ++;
    per_cell_vol[0] = test_NUM;
    return;
}