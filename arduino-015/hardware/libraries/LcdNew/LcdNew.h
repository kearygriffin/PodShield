#ifndef LcdNew_h
#define LcdNew_h

#include <inttypes.h>
#include "Print.h"

class LcdNew : public Print {
public:
  LcdNew(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
  LcdNew(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t,
    uint8_t, uint8_t, uint8_t, uint8_t);
  void clear();
  void home();
  void setCursor(int, int); 
  /*
  void shiftDisplayLeft();
  void shiftDisplayRight();
  */
  virtual void write(uint8_t);
  void command(uint8_t);
private:
  void sendraw4(uint8_t);
  void send(uint8_t, uint8_t);
  
  uint8_t _four_bit_mode;
  uint8_t _rs_pin; // LOW: command.  HIGH: character.
  uint8_t _rw_pin; // LOW: write to LCD.  HIGH: read from LCD.
  uint8_t _enable_pin; // activated by a HIGH pulse.
  uint8_t _data_pins[8];
};

#endif
