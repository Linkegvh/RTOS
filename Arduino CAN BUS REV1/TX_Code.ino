#include <SPI.h>
#include <mcp_can.h>

const int spiCSPin = 10;
int ledHIGH    = 1;
int ledLOW     = 0;

MCP_CAN CAN(spiCSPin);

void setup()
{
    Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS))
    {
        Serial.println("CAN BUS init Failed");
        delay(100);
    }
    Serial.println("CAN BUS Shield Init OK!");
}

unsigned char stmp[8] = {ledHIGH, 1, 2, 3, ledLOW, 5, 6, 7};
unsigned char testing[8] = {'h'};
    
void loop()
{   
  Serial.println("In loop");
  CAN.sendMsgBuf(0x43, 0, 8, stmp);
  CAN.sendMsgBuf(0x00, 0 , 8, testing);
  delay(1000);
}
