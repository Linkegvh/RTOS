const int ADC_Pin_num = 3;
const int ADC_Pin1 [ADC_Pin_num] = {12, 14, 27};
float calcualted_per_cell_voltage[ADC_Pin_num];
float calculated_accumulative_voltage [ADC_Pin_num];
int test_NUM = 0;

void setup(){
    Serial.begin(115200);
    delay (100);
    int i = 0;
    while (i < ADC_Pin_num){
        calculated_accumulative_voltage[i] = 0;
        calcualted_per_cell_voltage[i] = 0;
        i++;
    }

    Serial.println("Start Voltage Checker");    
}

void loop(){
    Serial.println("H1");
    long Voltage_reading_raw [ADC_Pin_num]; // ADC pins is 12 bit for ESP32 based board

    Serial.println("H2");
    ADC_read(Voltage_reading_raw);

    Serial.println("H3");
    calculation(Voltage_reading_raw, calcualted_per_cell_voltage, calculated_accumulative_voltage);
    
    Serial.println("Voltage reading:");
    int i;
    for (i = 0; i < ADC_Pin_num; i ++){
        Serial.print(i); Serial.print("th cell: "); Serial.print(calcualted_per_cell_voltage[i]);
        Serial.print("V, "); Serial.print("Accumulated voltage: "); Serial.print(calculated_accumulative_voltage[i]);
        Serial.println("V");
    }

    delay (1000);
}

void ADC_read(long *Voltage_levels){
    int i;
    for (i = 0; i < ADC_Pin_num ; i++){
        Voltage_levels[i] = analogRead(ADC_Pin1[i]);
    }
    return;
}

void calculation(long * voltage_raw, float * per_cell_vol, float * accum_vol){
    Serial.println("H4");
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
        
        if (i = 0){
            per_cell_vol[i] = accum_vol[i];
        }else{
            per_cell_vol[i] = accum_vol[i] - accum_vol[i - 1];
        } 
        
        i ++;
    }
    test_NUM ++;
    per_cell_vol[0] = test_NUM;
    return;
}