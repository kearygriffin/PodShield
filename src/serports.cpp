#include "podshield.h"
#include "serports.h"
#include "ArduinoSerial.h"
#include "PCSerial.h"

#ifdef STANDALONE_PC
PCSerial PSerial0("/dev/ttyUSB0");
PCSerial PSerial1("/dev/ttyUSB1");
PCSerial PSerial2("/dev/ttyUSB2");
PCSerial PSerial3("/dev/ttyUSB3");
#elif defined(SHIELD_REV_1) || defined(SHIELD_1280)
ArduinoSerial PSerial0(&Serial);
ArduinoSerial PSerial1(&Serial1);
ArduinoSerial PSerial2(&Serial2);
ArduinoSerial PSerial3(&Serial3);
#elif defined(STANDALONE_644P)
ArduinoSerial PSerial0(&Serial);
ArduinoSerial PSerial1(&Serial1);
// Plus extra
#else
ArduinoSerial PSerial0(&Serial);
#endif

#if defined(STANDALONE_PC)
PodSerial *DebugSerial = NULL;
PodSerial *HeadSerial = &PSerial0;
PodSerial *SerialAux1 = &PSerial1;
#elif defined(SHIELD_REV_1)
PodSerial *DebugSerial = &PSerial0;
PodSerial *SerialAux1 = &PSerial1;
PodSerial *HeadSerial = &PSerial2;
PodSerial *IPodSerial = &PSerial3;
PodSerial *SerialAux2 = NULL;

#endif
