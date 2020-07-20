#include <Arduino.h>
#include "MarlinCore.h"
#include "math.h"
///it needs to compile FlexCAN_T4 well
#undef min
#undef max

//supported axes bits
//0 - reserved(always 0)
//1 - 1 << 1 = 2 exstruder
//2 - 1 << 2 = 4 Z axe
#define SUPPORTED_AXES 2

#define MARLIN_CAN_ID  0x711
#define MASTER_CAN_ID  0x710
#define SIZE_CAN_BUF  256 //use only multiply of 2

#include <FlexCAN_T4.h>
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can0;

SerialToCAN SerialCAN;

typedef struct{
  uint16_t index_w;
  uint16_t index_r;
  uint8_t buf[SIZE_CAN_BUF];
}can_buf_t;

can_buf_t can_rx, can_tx;

uint8_t serial_wait_data = 0; //it should contein number of expected data len
const uint8_t can_header[4] = {0xAA, 0xBB, 0xCC, 0xDD};
uint8_t wait_init_can = 1;

//SerialToCAN SerialCAN;

void canSniff(const CAN_message_t &msg) {
  // Serial.print("MB "); Serial.print(msg.mb);
  // Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
  // Serial.print("  LEN: "); Serial.print(msg.len);
  // Serial.print(" EXT: "); Serial.print(msg.flags.extended);
  // Serial.print(" TS: "); Serial.print(msg.timestamp);
  // Serial.print(" ID: "); Serial.print(msg.id, HEX);
  // Serial.print(" Buffer: ");
  // for ( uint8_t i = 0; i < msg.len; i++ ) {
  //   Serial.print(msg.buf[i], HEX); Serial.print(" ");
  // } Serial.println(); 
  uint8_t i; 
  uint16_t free_bytes_in_buf;
  uint8_t str[16];
  if(msg.id == MARLIN_CAN_ID){
    if(serial_wait_data > 0){
      if(serial_wait_data >= msg.len){
        for(i = 0; i < msg.len; i++){
          can_rx.buf[can_rx.index_w] = msg.buf[i];
          can_rx.index_w = (can_rx.index_w + 1) & (SIZE_CAN_BUF - 1);
          str[i] = msg.buf[i];
        }
        Serial.write(str, i);
        serial_wait_data -= msg.len;
      }else{
        Serial.print(" wrong len, expected "); Serial.print(serial_wait_data);
        Serial.print(" received "); Serial.print(msg.len);
        serial_wait_data = 0;
      }
    }else if (msg.len == 5){
      if(memcmp(can_header, msg.buf, 4) == 0){
        free_bytes_in_buf = (SIZE_CAN_BUF - can_rx.index_w + can_rx.index_r) & (SIZE_CAN_BUF - 1);
        if(free_bytes_in_buf >= msg.buf[4] ){
          serial_wait_data = msg.buf[4];
          Serial.print(" wait msg ");
          Serial.println(serial_wait_data);
        }else{
          Serial.print(" No enough space for message len = "); Serial.print(msg.buf[4]);
        }
      }else{
        Serial.print(" Wrong Header received "); 
          for (i = 0; i < 4; i++ ) {
            Serial.print(msg.buf[i], HEX); Serial.print(" ");
          } Serial.println();
      }
    }else{
      Serial.print("Unknown message: ");
      Serial.print(" ID: "); Serial.print(msg.id, HEX);
      for (i = 0; i < msg.len; i++ ) {
        Serial.print(msg.buf[i], HEX); Serial.print(" ");
      } Serial.println();  
    }
  }
}

void can_write_less_8bytes(const char *buffer, size_t size){
    CAN_message_t msg;
    msg.id = MASTER_CAN_ID;
    memcpy(msg.buf, buffer, size);
    msg.len = size;
    if(wait_init_can){
      Serial.print("waiting CAN init ");
      Serial.println(size);
    }else{
      Can0.write(msg);
    }
    
}

void can_init_send_msg(){
  char buf[] = {0xDD, SUPPORTED_AXES};
  can_write_less_8bytes(buf, sizeof(buf));
  Serial.println("CAN inited ");
}

size_t can_write(const char *buffer, size_t size){
  char buf[8];
  uint16_t i = 0, len;

  memcpy(buf, can_header, 4);
  buf[4] = size;
  can_write_less_8bytes(buf, 5); //send length

  while(i < size){
    len = size - i;
    if(len > 8){
      len = 8;
    }
    can_write_less_8bytes(buffer + i, len);
    i += len;
  }
  return i;
}

void can_setup(void) {
  Can0.begin();
  Can0.setBaudRate(500000);
  Can0.setMaxMB(64);
  Can0.enableFIFO();
  Can0.enableFIFOInterrupt();
  Can0.onReceive(canSniff);
  Can0.mailboxStatus();
  
  can_rx.index_r = SIZE_CAN_BUF - 1;
  can_rx.index_w = 0;
  can_tx.index_r = SIZE_CAN_BUF - 1;
  can_tx.index_w = 0;
  wait_init_can = 0;  
  can_init_send_msg();
}

void can_proc_in_loop() {
  Can0.events();
  // static uint32_t timeout = millis();
  // if ( millis() - timeout > 2000 ) {
  //   CAN_message_t msg;
  //   msg.id = random(0x1,0x7FE);
  //   for ( uint8_t i = 0; i < 8; i++ ) msg.buf[i] = i + 1;
  //   Can0.write(msg);
  //   timeout = millis();
  //   Serial.print("CAN sent ");
  // }
}
 
bool can_data_available(){
  uint16_t available_bytes = (can_rx.index_w - can_rx.index_r - 1) & (SIZE_CAN_BUF - 1);
  bool res;
  if(available_bytes == 0){
    res = false;
  }else{
    res = true;
  }
  return res;
}

int can_read_serial(){
  uint8_t c;
  can_rx.index_r = ( can_rx.index_r + 1 ) & (SIZE_CAN_BUF - 1);
  c = can_rx.buf[can_rx.index_r];
  return c;
}

void can_serial_send(){
  char buf[8];
  size_t len, i;
  uint16_t available_bytes = (can_tx.index_w - can_tx.index_r - 1) & (SIZE_CAN_BUF - 1);
  // Serial.print("can_serial_send ");
  // Serial.print(available_bytes);
  // Serial.print(" ");
  // Serial.print(can_tx.index_w);
  // Serial.print(" ");
  // Serial.println(can_tx.index_r);

  while(available_bytes != 0){
    if(available_bytes > 8){
      len = 8;
    }else{
      len = available_bytes;
    }

    for(i = 0; i < len; i++){
      can_tx.index_r = ( can_tx.index_r + 1 ) & (SIZE_CAN_BUF - 1);
      buf[i] = can_tx.buf[can_tx.index_r];
    }
    can_write(buf, len);
    available_bytes = (can_tx.index_w - can_tx.index_r - 1) & (SIZE_CAN_BUF - 1);
  }
}

size_t SerialToCAN::write(uint8_t b){
	size_t count = 1;
  size_t free_bytes_in_buf;
  free_bytes_in_buf = (SIZE_CAN_BUF - can_tx.index_w + can_tx.index_r) & (SIZE_CAN_BUF - 1);
  if(free_bytes_in_buf >= 1 ){
    can_tx.buf[can_tx.index_w] = b;
    can_tx.index_w = (can_tx.index_w + 1) & (SIZE_CAN_BUF - 1);
    if(b == 0x0A){
      can_serial_send();
    }
  }else{
    Serial.println(" No enough space for message len = ");
  }
	return count;
}


size_t SerialToCAN::write(const char *buffer, size_t size)
{
	if (buffer == nullptr) return 0;
	size_t i;
  for(i = 0; i < size; i++){
    write(buffer[i]);
  }
	return i;
}

size_t SerialToCAN::printNumber(unsigned long n, uint8_t base, uint8_t sign)
{
	uint8_t buf[34];
	uint8_t digit, i;

	// TODO: make these checks as inline, since base is
	// almost always a constant.  base = 0 (BYTE) should
	// inline as a call directly to write()
	if (base == 0) {
		return write((uint8_t)n);
	} else if (base == 1) {
		base = 10;
	}


	if (n == 0) {
		buf[sizeof(buf) - 1] = '0';
		i = sizeof(buf) - 1;
	} else {
		i = sizeof(buf) - 1;
		while (1) {
			digit = n % base;
			buf[i] = ((digit < 10) ? '0' + digit : 'A' + digit - 10);
			n /= base;
			if (n == 0) break;
			i--;
		}
	}
	if (sign) {
		i--;
		buf[i] = '-';
	}
	return write((char*)buf + i, sizeof(buf) - i);
}

size_t SerialToCAN::print(const String &s)
{
	char buffer[9];
	size_t count = 0;
	unsigned int index = 0;
	unsigned int len = s.length();
	while (len > 0) {
		s.getBytes((unsigned char*)buffer, sizeof(buffer), index);
		unsigned int nbytes = len;
		if (nbytes > sizeof(buffer)-1) nbytes = sizeof(buffer)-1;
		index += nbytes;
		len -= nbytes;
		count += can_write(buffer, nbytes);
	}
	return count;
}

size_t SerialToCAN::print(long n)
{
	uint8_t sign=0;

	if (n < 0) {
		sign = '-';
		n = -n;
	}
	return printNumber(n, 10, sign);
}

size_t SerialToCAN::println(void)
{
	char buf[2]={'\r', '\n'};
	return write(buf, 2);
}

size_t SerialToCAN::printFloat(double number, uint8_t digits){
	uint8_t sign=0;
	size_t count=0;

	if (isnan(number)) return print("nan");
    	if (isinf(number)) return print("inf");
    	if (number > 4294967040.0f) return print("ovf");  // constant determined empirically
    	if (number <-4294967040.0f) return print("ovf");  // constant determined empirically
	
	// Handle negative numbers
	if (number < 0.0) {
		sign = 1;
		number = -number;
	}

	// Round correctly so that print(1.999, 2) prints as "2.00"
	double rounding = 0.5;
	for (uint8_t i=0; i<digits; ++i) {
		rounding *= 0.1;
	}
	number += rounding;

	// Extract the integer part of the number and print it
	unsigned long int_part = (unsigned long)number;
	double remainder = number - (double)int_part;
	count += printNumber(int_part, 10, sign);

	// Print the decimal point, but only if there are digits beyond
	if (digits > 0) {
    char buf[16];
		uint8_t n, count=1;
		buf[0] = '.';

		// Extract digits from the remainder one at a time
		if (digits > sizeof(buf) - 1) digits = sizeof(buf) - 1;

		while (digits-- > 0) {
			remainder *= 10.0;
			n = (uint8_t)(remainder);
			buf[count++] = '0' + n;
			remainder -= n; 
		}
		count += write(buf, count);
	}
	return count;
}

size_t SerialToCAN::print(double d){
  Serial.println("!!! to do print(double d) ");
  return 0;
}

#define ADC0_ADDR ADS1115_ADDRESS_ADDR_GND
#define ADC1_ADDR ADS1115_ADDRESS_ADDR_VDD
ADS1115 adc0(ADC1_ADDR);

////////////ads1118
void ads1118_i2c_init(){
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
}
