#include "ads1115.h"
#include <Arduino.h>
#include "MarlinCore.h"

ADS1115::ADS1115(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 * This device is ready to use automatically upon power-up. It defaults to
 * single-shot read mode, P0/N1 mux, 2.048v gain, 128 samples/sec, default
 * comparator with hysterysis, active-low polarity, non-latching comparator,
 * and comparater-disabled operation. 
 */
void ADS1115::initialize() {
    uint8_t i;
    Wire.begin();
    config_reg = 0x0583;


Serial.println("\r\n  !!!");
showRawConfigRegister();

// updateConfig(0x7F, 8, 0XFF);
// Serial.println(config_reg, HEX);
// showRawConfigRegister();

// updateConfig(0xFF, 0, 0XFF);
// Serial.println(config_reg, HEX);
// showRawConfigRegister();
// for(i = 0; i < 14; i++){
//     updateConfig(0, i, 1);
//     Serial.println(config_reg, HEX);
//     showRawConfigRegister();

// }

  setMode(ADS1115_MODE_SINGLESHOT);  
  setMultiplexer(ADS1115_MUX_P0_N1);
  setGain(ADS1115_PGA_0P256);
  setRate(ADS1115_RATE_128);
  setComparatorMode(ADS1115_COMP_MODE_HYSTERESIS);
  setComparatorPolarity(ADS1115_COMP_POL_ACTIVE_LOW);
  setComparatorLatchEnabled(ADS1115_COMP_LAT_NON_LATCHING);
  setComparatorQueueMode(ADS1115_COMP_QUE_DISABLE);

  showRawConfigRegister();
  Serial.println("Init adc done  !!!");
}

void ADS1115::debug_only(){
    uint8_t res, buf[2];
    Serial.print("\r\n Debug!!! ");
    res = readI2CW(ADS1115_RA_CONFIG);
    Serial.print(" Read ");
    Serial.print(res);    
    Serial.print("  ADS1115_RA_CONFIG = ");
    Serial.println(buffer[0], HEX);

    buf[0] = 0xFF;
    buf[1] = 0xFF;
    writeI2CW(ADS1115_RA_CONFIG, *(uint16_t*)buf);
    res = readI2CW(ADS1115_RA_CONFIG);
    Serial.print("  ADS1115_RA_CONFIG = ");
    Serial.println(buffer[0], HEX);   

    writeI2CW(ADS1115_RA_HI_THRESH, *(uint16_t*)buf);
    res = readI2CW(ADS1115_RA_HI_THRESH);
    Serial.print("  ADS1115_RA_HI_THRESH = ");
    Serial.println(buffer[0], HEX);  

    setMultiplexer(ADS1115_MUX_P0_NG);
    res = readI2CW(ADS1115_RA_CONFIG);
    Serial.print("  ADS1115_RA_CONFIG = ");
    Serial.println(buffer[0], HEX);   

    setMultiplexer(ADS1115_MUX_P1_NG);
    res = readI2CW(ADS1115_RA_CONFIG);
    Serial.print("  ADS1115_RA_CONFIG = ");
    Serial.println(buffer[0], HEX);       

}

bool ADS1115::pollConversion(uint16_t max_retries) {  
  for(uint16_t i = 0; i < max_retries; i++) {
    if (isConversionReady()) return true;
  }
  return false;
}

// uint8_t ADS1115::readI2C(uint8_t reg_addr, uint8_t num){
//     uint8_t i, *ptr = (uint8_t*)buffer, c;
//     Wire.beginTransmission(devAddr);
//     Wire.write(reg_addr);
//     Wire.endTransmission();
//     Wire.requestFrom(devAddr, num);

//     i = 0;
//     // Serial.print("\r\n  raw i2c read = ");
//     while(Wire.available()) { // slave may send less than requested
//         c = Wire.read();// receive a byte as character
//         ptr[i++] = c;   
//         // Serial.print(c, HEX);
//         // Serial.print(" ");
//     }
//     return i;
// }
//read 2 bytes
uint8_t ADS1115::readI2CW(uint8_t reg_addr){
    uint8_t i, i2c_buf[2], c;
    Wire.beginTransmission(devAddr);
    Wire.write(reg_addr);
    Wire.endTransmission();
    Wire.requestFrom(devAddr, 2);

    i = 0;
    // Serial.print("\r\n  raw i2c read = ");
    while(Wire.available()) { // slave may send less than requested
        c = Wire.read();// receive a byte as character
        if(i < 2){
            i2c_buf[i++] = c;   
        }      
        // Serial.print(c, HEX);
        // Serial.print(" ");
    }

    buffer[0] = (i2c_buf[0] << 8) | i2c_buf[1];
    return i;
}
/*
reg_addr	Register regAddr to read from
bitNum	Bit position to read (0-15) 
*/
bool ADS1115::readI2CBitW(uint8_t reg_addr, uint8_t bitNum){
    bool res;
    readI2CW(reg_addr);
    if(buffer[0] & (1 << bitNum)){
        res = true;
    }else
    {
        res = false;
    }
    return res;
}

uint16_t ADS1115::readI2CBitsW(uint8_t reg_addr, uint8_t bitStart, uint8_t bitMask){
    readI2CW(reg_addr);
    bitsResault = (buffer[0] >> bitStart) & bitMask;
    return bitsResault; 
}

uint8_t ADS1115::writeI2C(uint8_t reg_addr, uint8_t* buf, uint8_t num){
    uint8_t i, status;
    Wire.beginTransmission(devAddr);
    Wire.write(reg_addr);
    for(i = 0; i < num; i++){
        Wire.write(buf[i]);
    }

    status = Wire.endTransmission();
    return status;
}

uint8_t ADS1115::writeI2CW(uint8_t reg_addr, uint16_t value){
    uint8_t i2c_buf[2];
    i2c_buf[0] = value >> 8;
    i2c_buf[1] = value & 0xFF;

    return writeI2C(reg_addr, i2c_buf, 2);
}

void ADS1115::updateConfig(uint8_t value, uint8_t bitStart, uint8_t bitMask){
    config_reg &= ~(bitMask << bitStart);
    if(value){
        config_reg |= (value & bitMask) << bitStart;
    }
    writeI2CW(ADS1115_RA_CONFIG, config_reg);
}

int16_t ADS1115::getConversion(bool triggerAndPoll) {
    uint16_t tries = 0;
    bool ready;
    if (triggerAndPoll && devMode == ADS1115_MODE_SINGLESHOT) {
      triggerConversion();
      pollConversion(16);
    }

    ready = isConversionReady();
    while(ready == false){
        ready = isConversionReady();
        tries++;
        if(tries > 1000){
            Serial.println("\r\n1000 tries wait adc");
            break;
        }
    }

    readI2CW(ADS1115_RA_CONVERSION);
    if(tries > 0){
        Serial.print("\r\ntries wait adc = ");
        Serial.println(tries);
    }
    return buffer[0];
}

/** Get the current voltage reading
 * Read the current differential and return it multiplied
 * by the constant for the current gain.  mV is returned to
 * increase the precision of the voltage
 * @param triggerAndPoll If true (and only in singleshot mode) the conversion trigger 
 *        will be executed and the conversion results will be polled.
 */
float ADS1115::getMilliVolts(bool triggerAndPoll) {
  switch (pgaMode) { 
    case ADS1115_PGA_6P144:
      return (getConversion(triggerAndPoll) * ADS1115_MV_6P144);
      break;    
    case ADS1115_PGA_4P096:
      return (getConversion(triggerAndPoll) * ADS1115_MV_4P096);
      break;             
    case ADS1115_PGA_2P048:    
      return (getConversion(triggerAndPoll) * ADS1115_MV_2P048);
      break;       
    case ADS1115_PGA_1P024:     
      return (getConversion(triggerAndPoll) * ADS1115_MV_1P024);
      break;       
    case ADS1115_PGA_0P512:      
      return (getConversion(triggerAndPoll) * ADS1115_MV_0P512);
      break;       
    case ADS1115_PGA_0P256:           
    case ADS1115_PGA_0P256B:          
    case ADS1115_PGA_0P256C:      
      return (getConversion(triggerAndPoll) * ADS1115_MV_0P256);
      break;       
  }
}

// CONFIG register

/** Get operational status.
 * @return Current operational status (false for active conversion, true for inactive)
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_OS_BIT
 */
bool ADS1115::isConversionReady() {
    return readI2CBitW(ADS1115_RA_CONFIG, ADS1115_CFG_OS_BIT);
}
/** Trigger a new conversion.
 * Writing to this bit will only have effect while in power-down mode (no conversions active).
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_OS_BIT
 */
void ADS1115::triggerConversion() {
    updateConfig(1, ADS1115_CFG_OS_BIT, 1);
}

uint8_t ADS1115::getMultiplexer() {
    return readI2CBitsW(ADS1115_RA_CONFIG, ADS1115_CFG_MUX_BIT, ADS1115_CFG_MUX_MASK);
}
/** Set multiplexer connection.  Continous mode may fill the conversion register
 * with data before the MUX setting has taken effect.  A stop/start of the conversion
 * is done to reset the values.
 * @param mux New multiplexer connection setting
 * @see ADS1115_MUX_P0_N1
 * @see ADS1115_MUX_P0_N3
 * @see ADS1115_MUX_P1_N3
 * @see ADS1115_MUX_P2_N3
 * @see ADS1115_MUX_P0_NG
 * @see ADS1115_MUX_P1_NG
 * @see ADS1115_MUX_P2_NG
 * @see ADS1115_MUX_P3_NG
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_MUX_BIT
 * @see ADS1115_CFG_MUX_LENGTH
 */
void ADS1115::setMultiplexer(uint8_t mux) {
    updateConfig(mux, ADS1115_CFG_MUX_BIT, ADS1115_CFG_MUX_MASK);
    // Serial.print(" Set Multiplexer status = ");
    // Serial.println(status);
    // if (I2Cdev::writeBitsW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_MUX_BIT, ADS1115_CFG_MUX_LENGTH, mux)) {
        muxMode = mux;
        if (devMode == ADS1115_MODE_CONTINUOUS) {
          // Force a stop/start
          setMode(ADS1115_MODE_SINGLESHOT);
          getConversion();
          setMode(ADS1115_MODE_CONTINUOUS);
        }
    // }
    
}
/** Get programmable gain amplifier level.
 * @return Current programmable gain amplifier level
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_PGA_BIT
 * @see ADS1115_CFG_PGA_LENGTH
 */
uint8_t ADS1115::getGain() {
    pgaMode=readI2CBitsW(ADS1115_RA_CONFIG, ADS1115_CFG_PGA_BIT, ADS1115_CFG_PGA_MASK);
    return pgaMode;
}
/** Set programmable gain amplifier level.  
 * Continous mode may fill the conversion register
 * with data before the gain setting has taken effect.  A stop/start of the conversion
 * is done to reset the values.
 * @param gain New programmable gain amplifier level
 * @see ADS1115_PGA_6P144
 * @see ADS1115_PGA_4P096
 * @see ADS1115_PGA_2P048
 * @see ADS1115_PGA_1P024
 * @see ADS1115_PGA_0P512
 * @see ADS1115_PGA_0P256
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_PGA_BIT
 * @see ADS1115_CFG_PGA_LENGTH
 */
void ADS1115::setGain(uint8_t gain) {
    updateConfig(gain, ADS1115_CFG_PGA_BIT, ADS1115_CFG_PGA_MASK);

    // Serial.print(" Set mode status = ");
    // Serial.println(status);
    // if (I2Cdev::writeBitsW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_PGA_BIT, ADS1115_CFG_PGA_LENGTH, gain)) {
      pgaMode = gain;
         if (devMode == ADS1115_MODE_CONTINUOUS) {
            // Force a stop/start
            setMode(ADS1115_MODE_SINGLESHOT);
            getConversion();
            setMode(ADS1115_MODE_CONTINUOUS);
         }
    // }
}
/** Get device mode.
 * @return Current device mode
 * @see ADS1115_MODE_CONTINUOUS
 * @see ADS1115_MODE_SINGLESHOT
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_MODE_BIT
 */
bool ADS1115::getMode() {
    devMode = readI2CBitW(ADS1115_RA_CONFIG, ADS1115_CFG_MODE_BIT);
    return devMode;
}
/** Set device mode.
 * @param mode New device mode
 * @see ADS1115_MODE_CONTINUOUS
 * @see ADS1115_MODE_SINGLESHOT
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_MODE_BIT
 */
void ADS1115::setMode(bool mode) {
//    if (I2Cdev::writeBitW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_MODE_BIT, mode)) {
    updateConfig(mode, ADS1115_CFG_MODE_BIT, 1);
    devMode = mode;
 //   }
}
/** Get data rate.
 * @return Current data rate
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_DR_BIT
 * @see ADS1115_CFG_DR_LENGTH
 */
uint8_t ADS1115::getRate() {
    return readI2CBitsW(ADS1115_RA_CONFIG, ADS1115_CFG_DR_BIT, ADS1115_CFG_DR_MASK);
}
/** Set data rate.
 * @param rate New data rate
 * @see ADS1115_RATE_8
 * @see ADS1115_RATE_16
 * @see ADS1115_RATE_32
 * @see ADS1115_RATE_64
 * @see ADS1115_RATE_128
 * @see ADS1115_RATE_250
 * @see ADS1115_RATE_475
 * @see ADS1115_RATE_860
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_DR_BIT
 * @see ADS1115_CFG_DR_LENGTH
 */
void ADS1115::setRate(uint8_t rate) {
    updateConfig(rate, ADS1115_CFG_DR_BIT, ADS1115_CFG_DR_MASK);
    Serial.print("\r\ncongig_reg = ");
    Serial.println(config_reg);
}
/** Get comparator mode.
 * @return Current comparator mode
 * @see ADS1115_COMP_MODE_HYSTERESIS
 * @see ADS1115_COMP_MODE_WINDOW
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_MODE_BIT
 */
bool ADS1115::getComparatorMode() {
    return readI2CBitW(ADS1115_RA_CONFIG, ADS1115_CFG_COMP_MODE_BIT);
}
/** Set comparator mode.
 * @param mode New comparator mode
 * @see ADS1115_COMP_MODE_HYSTERESIS
 * @see ADS1115_COMP_MODE_WINDOW
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_MODE_BIT
 */
void ADS1115::setComparatorMode(bool mode) {
    updateConfig(mode, ADS1115_CFG_COMP_MODE_BIT, 1);
}
/** Get comparator polarity setting.
 * @return Current comparator polarity setting
 * @see ADS1115_COMP_POL_ACTIVE_LOW
 * @see ADS1115_COMP_POL_ACTIVE_HIGH
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_POL_BIT
 */
bool ADS1115::getComparatorPolarity() {
    return readI2CBitW(ADS1115_RA_CONFIG, ADS1115_CFG_COMP_POL_BIT);
}
/** Set comparator polarity setting.
 * @param polarity New comparator polarity setting
 * @see ADS1115_COMP_POL_ACTIVE_LOW
 * @see ADS1115_COMP_POL_ACTIVE_HIGH
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_POL_BIT
 */
void ADS1115::setComparatorPolarity(bool polarity) {
    updateConfig(polarity, ADS1115_CFG_COMP_POL_BIT, 1);
}
/** Get comparator latch enabled value.
 * @return Current comparator latch enabled value
 * @see ADS1115_COMP_LAT_NON_LATCHING
 * @see ADS1115_COMP_LAT_LATCHING
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_LAT_BIT
 */
bool ADS1115::getComparatorLatchEnabled() {
    return readI2CBitW(ADS1115_RA_CONFIG, ADS1115_CFG_COMP_LAT_BIT);
}
/** Set comparator latch enabled value.
 * @param enabled New comparator latch enabled value
 * @see ADS1115_COMP_LAT_NON_LATCHING
 * @see ADS1115_COMP_LAT_LATCHING
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_LAT_BIT
 */
void ADS1115::setComparatorLatchEnabled(bool enabled) {
    updateConfig(enabled, ADS1115_CFG_COMP_LAT_BIT, 1);
}
/** Get comparator queue mode.
 * @return Current comparator queue mode
 * @see ADS1115_COMP_QUE_ASSERT1
 * @see ADS1115_COMP_QUE_ASSERT2
 * @see ADS1115_COMP_QUE_ASSERT4
 * @see ADS1115_COMP_QUE_DISABLE
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_QUE_BIT
 * @see ADS1115_CFG_COMP_QUE_LENGTH
 */
uint8_t ADS1115::getComparatorQueueMode() {
    return readI2CBitsW(ADS1115_RA_CONFIG, ADS1115_CFG_COMP_QUE_BIT, ADS1115_CFG_COMP_QUE_MASK);
}
/** Set comparator queue mode.
 * @param mode New comparator queue mode
 * @see ADS1115_COMP_QUE_ASSERT1
 * @see ADS1115_COMP_QUE_ASSERT2
 * @see ADS1115_COMP_QUE_ASSERT4
 * @see ADS1115_COMP_QUE_DISABLE
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_QUE_BIT
 * @see ADS1115_CFG_COMP_QUE_LENGTH
 */
void ADS1115::setComparatorQueueMode(uint8_t mode) {
    updateConfig(mode, ADS1115_CFG_COMP_QUE_BIT, ADS1115_CFG_COMP_QUE_MASK);
}

// *_THRESH registers

/** Get low threshold value.
 * @return Current low threshold value
 * @see ADS1115_RA_LO_THRESH
 */
int16_t ADS1115::getLowThreshold() {
    readI2CW(ADS1115_RA_LO_THRESH);
    return buffer[0];
}
/** Set low threshold value.
 * @param threshold New low threshold value
 * @see ADS1115_RA_LO_THRESH
 */
void ADS1115::setLowThreshold(int16_t threshold) {
    writeI2CW(ADS1115_RA_LO_THRESH, threshold);    
}
/** Get high threshold value.
 * @return Current high threshold value
 * @see ADS1115_RA_HI_THRESH
 */
int16_t ADS1115::getHighThreshold() {
    readI2CW(ADS1115_RA_HI_THRESH);
    return buffer[0];
}
/** Set high threshold value.
 * @param threshold New high threshold value
 * @see ADS1115_RA_HI_THRESH
 */
void ADS1115::setHighThreshold(int16_t threshold) {
    writeI2CW(ADS1115_RA_HI_THRESH, threshold);  
}

/** Configures ALERT/RDY pin as a conversion ready pin.
 *  It does this by setting the MSB of the high threshold register to '1' and the MSB 
 *  of the low threshold register to '0'. COMP_POL and COMP_QUE bits will be set to '0'.
 *  Note: ALERT/RDY pin requires a pull up resistor.
 */
void ADS1115::setConversionReadyPinMode() {
    // uint16_t cfg = 1 << 15;
    // writeI2C(ADS1115_RA_HI_THRESH, (uint8_t*)&cfg, 2);
    // cfg = 0;
    // writeI2C(ADS1115_RA_LO_THRESH, (uint8_t*)&cfg, 2);
    setComparatorPolarity(0);
    setComparatorQueueMode(0);
}

/** Show all the config register settings
 */
void ADS1115::showConfigRegister() {
    readI2CW(ADS1115_RA_CONFIG);
    uint16_t configRegister =buffer[0];    
    
    Serial.print("\r\nRegister is:");
    Serial.println(configRegister,BIN);
    Serial.println(configRegister,HEX);

    Serial.print("OS:\t");
    Serial.println((configRegister >> ADS1115_CFG_OS_BIT) & 1);
    Serial.print("MUX:\t");
    Serial.println((configRegister >> ADS1115_CFG_MUX_BIT) & ADS1115_CFG_MUX_MASK);
    
    Serial.print("PGA:\t");
    Serial.println((configRegister >> ADS1115_CFG_PGA_BIT) & ADS1115_CFG_PGA_MASK);
    
    Serial.print("MODE:\t");
    Serial.println((configRegister >> ADS1115_CFG_MODE_BIT) & 1);
    
    Serial.print("DR:\t");
    Serial.println((configRegister >> ADS1115_CFG_DR_BIT) & ADS1115_CFG_DR_MASK);
    
    Serial.print("CMP_MODE:\t");
    Serial.println((configRegister >> ADS1115_CFG_COMP_MODE_BIT) & 1);
    
    Serial.print("CMP_POL:\t");
    Serial.println((configRegister >> ADS1115_CFG_COMP_POL_BIT) & 1);
    
    Serial.print("CMP_LAT:\t");
    Serial.println((configRegister >> ADS1115_CFG_COMP_LAT_BIT) & 1);
    
    Serial.print("CMP_QUE:\t");
    Serial.println((configRegister >> ADS1115_CFG_COMP_QUE_BIT) & ADS1115_CFG_COMP_QUE_MASK);
};


void ADS1115::showRawConfigRegister() {
    readI2CW(ADS1115_RA_CONFIG);
    uint16_t configRegister =buffer[0];    
    
    Serial.print("  Config is:");
    Serial.println(configRegister,HEX);
};