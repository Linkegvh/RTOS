/*
    This code is meant to be used together with an ESP32-Pico-kit and utelises its ADC pins combining with voltage dividers to sense a 12S LiPo battery voltage levels. 
    Then, the ESP32 will communicate the voltage data through Wifi towards a beaglebone for further processing
    
    The hardware design of this code can be found in ...
*/

#include <WiFi.h> //ESP32 wifi library

// wifi communication credentials
const char * networkName = "Yonah";
const char * networkPwd = "123";

void setup(){
    Serial.begin(115200) //output to serial monitor for status info

    connectToWiFi(networkName, networkPwd); // establish wifi connection
}

void loop(){

}

/***********
 * Function: Wifi connection module
 * 
 * Used in: Setup
 * *********/
void connectToWiFi(const char *ssid, const char * pwd){

    Serial.println("Connecting to WiFi netowrk: " + String(ssid));
    WiFi.begin(ssid,pwd);

    // if wifi is not connected, the while loop will not be exited

    while (WiFi.status() != WL_CONNECTED){
        //Blink LED while we are Connecting
        digitalWrite (LED_PIN, ledState);
        ledState = (ledState++) % 2; // flip between 1 and 0
        delay (500); 
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi Connected Successfully!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_PIN, 1);
}

