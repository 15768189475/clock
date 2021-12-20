#ifndef _main_h
#define _main_h
#include "common.h"
 



void ServoInit(void);
void Write(uint8);
void SetLeftServo(uint8 role);
void SetRightServo(uint8 role);

extern uint32 tick;
#endif
