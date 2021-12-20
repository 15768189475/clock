#ifndef _t_discharge_h
#define _t_discharge_h
#include "conf.h"

void DischargeThreadStart(void);
void DischargeThreadStop(void);



void SmallMotorShortIsr(void);
void BigMotorCurrentCheck(void);
void SmallMotorCurrentCheck(void);






#endif












