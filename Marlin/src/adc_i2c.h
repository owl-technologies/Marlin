#include "ads1115.h"

enum
{
    ADC_I2C_HEATER0 = 0,
    // ADC_I2C_HEATER1,
    ADC_I2C_HEATER_BED,
    ADC_I2C_NUM_SENSORS
};

void ads1115_i2c_init();
void ads1115_loop();
float ADCgetTemperature(uint8_t num_ch);
void ADCupdateValues();