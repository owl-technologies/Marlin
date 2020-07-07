#include <Arduino.h>
#include <oled.h>
#include "MarlinCore.h"

///it needs to compile FlexCAN_T4 well
#undef min
#undef max

#include <FlexCAN_T4.h>
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can0;

void canSniff(const CAN_message_t &msg) {
  Serial.print("MB "); Serial.print(msg.mb);
  Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
  Serial.print("  LEN: "); Serial.print(msg.len);
  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
  Serial.print(" TS: "); Serial.print(msg.timestamp);
  Serial.print(" ID: "); Serial.print(msg.id, HEX);
  Serial.print(" Buffer: ");
  for ( uint8_t i = 0; i < msg.len; i++ ) {
    Serial.print(msg.buf[i], HEX); Serial.print(" ");
  } Serial.println();  
//   SERIAL_ECHO_MSG("MB "); SERIAL_ECHO_MSG(msg.mb);
//   SERIAL_ECHO_MSG("  OVERRUN: "); SERIAL_ECHO_MSG(msg.flags.overrun);
//   SERIAL_ECHO_MSG("  LEN: "); SERIAL_ECHO_MSG(msg.len);
//   SERIAL_ECHO_MSG(" EXT: "); SERIAL_ECHO_MSG(msg.flags.extended);
//   SERIAL_ECHO_MSG(" TS: "); SERIAL_ECHO_MSG(msg.timestamp);
//   SERIAL_ECHO_MSG(" ID: "); SERIAL_ECHO_MSG(msg.id);
//   SERIAL_ECHO_MSG(" Buffer: ");
//   for ( uint8_t i = 0; i < msg.len; i++ ) {
//     SERIAL_ECHO_MSG(msg.buf[i]); SERIAL_ECHO_MSG(" ");
//   }
}

void can_setup(void) {
//   Serial.begin(115200); delay(400);
//   pinMode(6, OUTPUT); digitalWrite(6, LOW); /* optional tranceiver enable pin */
  Can0.begin();
  Can0.setBaudRate(125000);
  Can0.setMaxMB(16);
  Can0.enableFIFO();
  Can0.enableFIFOInterrupt();
  Can0.onReceive(canSniff);
  Can0.mailboxStatus();
}

void can_proc_in_loop() {
  Can0.events();

  static uint32_t timeout = millis();
  if ( millis() - timeout > 2000 ) {
    CAN_message_t msg;
    msg.id = random(0x1,0x7FE);
    for ( uint8_t i = 0; i < 8; i++ ) msg.buf[i] = i + 1;
    Can0.write(msg);
    timeout = millis();
    Serial.print("CAN sent ");
  }

}


// There are 2 different versions of the board
// OLED display=OLED(2,14,4); 
OLED display=OLED(18,19,16);

static const uint8_t bitmap[] =
{ 
    1,2,4,8,16,32,64,128,128,128,192,192,128,128,128,64,32,16,8,4,2,1, // first page (8 vertical bits, 22 columns)
    255,255,255,255,15,15,15,15,15,15,15,15,15,15,15,15,15,15,255,255,255,255 // second page (8 vertical bits, 22 columns)
};


void setup_oled()
{
    delay(1000);
    display.begin();   
}

void contrast(int value)
{
    char buffer[4];
    display.clear();  
    display.draw_string(0,0,"Contrast:");
    itoa(value,buffer,10);
    display.draw_string(64,0,buffer);
    display.draw_rectangle(0,20,value/2,31,OLED::SOLID);
    display.display();
    display.set_contrast(value);
    delay(500);
}

void loop_oled(){
    // Draw pixels in the outer edges  
    display.draw_pixel(0,0);
    display.draw_pixel(127,0);
    display.draw_pixel(127,31);
    display.draw_pixel(0,31);    
    display.display();
    
    // // Draw hollow circles
    // for (uint_least8_t radius=3; radius<62; radius+=3)
    // {
    //     delay(50);
    //     display.draw_circle(64,16,radius);
    //     if (radius>15)
    //     {
    //         display.draw_circle(64,16,radius-15,OLED::SOLID,OLED::BLACK);
    //     }
    //     display.display();
    // }
    
    // // Draw solid circles
    // delay(500);
    // display.draw_circle(36,16,14,OLED::SOLID);
    // display.display();
    // delay(500);
    // display.draw_circle(36,16,7,OLED::SOLID,OLED::BLACK);
    // display.display();
    
    // Draw rectangles
    delay(500);
    display.draw_rectangle(64,0,98,31);
    display.display();
    delay(500);
    display.draw_rectangle(69,5,93,26,OLED::SOLID);
    display.display();

    // scroll up
    delay(1000);
    display.scroll_up(32,20);
    
    // Draw text with normal size
    display.draw_string(4,2,"Hello");
    display.display();
    
    // // Draw a line
    // delay(1000);
    // display.draw_line(4,10,34,10);
    // display.display();    
    
    // // Draw text from program memory with double size
    // delay(1000);
    // display.draw_string_P(16,15,PSTR("World!"),OLED::DOUBLE_SIZE);
    // display.display();
    
    // // Draw a cross
    // delay(1000);
    // display.draw_line(16,31,88,15);
    // display.draw_line(16,15,88,31);
    // display.display();
    
    // // Draw a raw bitmap
    // delay(1000);    
    // display.draw_bitmap_P(100,8,22,16,bitmap);
    // display.display();
    
    // Demonstrate scrolling
    //delay(1000);
    display.set_scrolling(OLED::HORIZONTAL_RIGHT);
    delay(3000);
    display.set_scrolling(OLED::HORIZONTAL_LEFT);
    delay(3000);
    display.set_scrolling(OLED::NO_SCROLLING);
    
    // Redraw after scrolling to get the original picture
    display.display();
    
    // // Flash the display
    // delay(1000);
    // for (int i=0; i<10; i++)
    // {
    //     display.set_invert(true);
    //     delay(200);
    //     display.set_invert(false);
    //     delay(200);
    // }
    
    // // Show contrast values
    // contrast(128);
    // contrast(64);
    // contrast(32);
    // contrast(16);
    // contrast(8);
    // contrast(4);
    // contrast(2);
    // contrast(1);
    // contrast(2);
    // contrast(4);
    // contrast(8);
    // contrast(16);
    // contrast(32);
    // contrast(64);
    // contrast(128);
    // contrast(255);
    // contrast(128);
    // delay(3000);
          
    // display.clear();
}

