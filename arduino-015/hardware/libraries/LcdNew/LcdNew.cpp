#include "LcdNew.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "WProgram.h"

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1 
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LcdNew constructor is called).

LcdNew::LcdNew(uint8_t rs, uint8_t rw, uint8_t enable,
  uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
  uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) :
  _four_bit_mode(0), _rs_pin(rs), _rw_pin(rw), _enable_pin(enable)
{
  _data_pins[0] = d0;
  _data_pins[1] = d1;
  _data_pins[2] = d2;
  _data_pins[3] = d3; 
  _data_pins[4] = d4;
  _data_pins[5] = d5;
  _data_pins[6] = d6;
  _data_pins[7] = d7; 
  
  pinMode(_rs_pin, OUTPUT);
  pinMode(_rw_pin, OUTPUT);
  pinMode(_enable_pin, OUTPUT);
  
  for (int i = 0; i < 8; i++)
    pinMode(_data_pins[i], OUTPUT);
 
  command(0x38);  // function set: 8 bits, 1 line, 5x8 dots
  command(0x0C);  // display control: turn display on, cursor off, no blinking
  command(0x06);  // entry mode set: increment automatically, display shift, right shift
  clear();
}

LcdNew::LcdNew(uint8_t rs, uint8_t rw, uint8_t enable,
  uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3) :
  _four_bit_mode(1), _rs_pin(rs), _rw_pin(rw), _enable_pin(enable)
{
  _data_pins[0] = d0;
  _data_pins[1] = d1;
  _data_pins[2] = d2;
  _data_pins[3] = d3; 
  
  pinMode(_rs_pin, OUTPUT);
  pinMode(_rw_pin, OUTPUT);
  pinMode(_enable_pin, OUTPUT);
  
  for (int i = 0; i < 4; i++)
    pinMode(_data_pins[i], OUTPUT);
 
  //command(0x28);  // function set: 4 bits, 1 line, 5x8 dots
  //command(0x0C);  // display control: turn display on, cursor off, no blinking
  //command(0x06);  // entry mode set: increment automatically, display shift, right shift
delayMicroseconds(16000);        // mandatory delay for Vcc stabilization
 sendraw4(0x30);                 // set 8-bit mode (yes, it's true!)
 delayMicroseconds(5000);        // mandatory delay
 sendraw4(0x30);
 delayMicroseconds(200);
 sendraw4(0x30);
 delayMicroseconds(40);          // command delay
 sendraw4(0x20);                 // finally set 4-bit mode
 delayMicroseconds(40);          // command delay

 command(0x28);            // 4-bit, 2-line, 5x7 char set
 command(0x08);            // display off
 command(0x01);            // clear display
 delayMicroseconds(16000);
 command(0x06);            // increment, no shift
 command(0x0c);            // display on, no cursor, no blink
  clear();
}

void LcdNew::clear()
{
  command(0x01);  // clear display, set cursor position to zero
  delayMicroseconds(16000);
}

void LcdNew::home()
{
  command(0x02);  // set cursor position to zero
  delayMicroseconds(2000);
}

void LcdNew::setCursor(int col, int row)
{
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  command(0x80 | (col + row_offsets[row]));
}

void LcdNew::command(uint8_t value) {
  send(value, LOW);
}

void LcdNew::write(uint8_t value) {
  send(value, HIGH);
}

void LcdNew::sendraw4(uint8_t value) {
  digitalWrite(_rs_pin, LOW);
  digitalWrite(_rw_pin, LOW);

  for (int i = 0; i < 4; i++) {
    digitalWrite(_data_pins[i], (value >> (i + 4)) & 0x01);
  }

  digitalWrite(_enable_pin, HIGH);
  digitalWrite(_enable_pin, LOW);

}
void LcdNew::send(uint8_t value, uint8_t mode) {
  digitalWrite(_rs_pin, mode);
  digitalWrite(_rw_pin, LOW);

  if (_four_bit_mode) {
    for (int i = 0; i < 4; i++) {
      digitalWrite(_data_pins[i], (value >> (i + 4)) & 0x01);
    }
    
    digitalWrite(_enable_pin, HIGH);
    digitalWrite(_enable_pin, LOW);
    
    for (int i = 0; i < 4; i++) {
      digitalWrite(_data_pins[i], (value >> i) & 0x01);
    }

    digitalWrite(_enable_pin, HIGH);
    digitalWrite(_enable_pin, LOW);
  } else {
    for (int i = 0; i < 8; i++) {
      digitalWrite(_data_pins[i], (value >> i) & 0x01);
    }

    digitalWrite(_enable_pin, HIGH);
    digitalWrite(_enable_pin, LOW);
  }
}
