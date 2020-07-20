#include "adc_i2c.h"

#define ADC0_ADDR ADS1115_ADDRESS_ADDR_GND
#define ADC1_ADDR ADS1115_ADDRESS_ADDR_VDD
ADS1115 adc0(ADC1_ADDR);

float adc_mv[4] = {10, 10, 10, 10};

////////////ads1115
void ads1115_i2c_init(){
  adc0.initialize();
}

void ads1115_loop(){
    Serial.println("\r\nSensor DIFF 0_1 ************************");
    adc0.setMultiplexer(ADS1115_MUX_P0_N1);
  //  adc0.showRawConfigRegister();   
    // Serial.println(adc0.getConversion());
    // Serial.println(adc0.getConversion());
    Serial.println(adc0.getMilliVolts());

    Serial.println("Sensor P2 ************************");
    adc0.setMultiplexer(ADS1115_MUX_P2_NG);
  //  adc0.showRawConfigRegister();   
    // Serial.println(adc0.getConversion());
    // Serial.println(adc0.getConversion());
    Serial.println(adc0.getMilliVolts());


    Serial.println("Sensor P3 ************************");
    adc0.setMultiplexer(ADS1115_MUX_P3_NG);
  //  adc0.showRawConfigRegister();   
    // Serial.println(adc0.getConversion());
    // Serial.println(adc0.getConversion());
    Serial.println(adc0.getMilliVolts());

    Serial.println("Sensor P2_N3 ************************");
    adc0.setMultiplexer(ADS1115_MUX_P2_N3);
  //  adc0.showRawConfigRegister();   
    // Serial.println(adc0.getConversion());
    // Serial.println(adc0.getConversion());
    Serial.println(adc0.getMilliVolts());
}

float ADCgetTemperature(uint8_t num_ch){
    return(adc_mv[num_ch]);
}

void ADCupdateValues(){
    uint8_t i, mux;

    for(i = 0; i < ADC_I2C_HEATER1; i++){
        switch(i){
        case ADC_I2C_HEATER0:
            mux = ADS1115_MUX_P0_N1;
            break;
        case ADC_I2C_HEATER1:
            mux = ADS1115_MUX_P2_NG;
            break;
//        case ADC_I2C_HEATER_BED:
        default:
            mux = ADS1115_MUX_P3_NG;
        }
        adc0.setMultiplexer(mux);
        adc_mv[i] = adc0.getMilliVolts();
    }
}

