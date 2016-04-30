/*
 * beerctrl.h
 *
 *  Created on: Jul 12, 2015
 *      Author: roosen
 */
#include <stdint.h>

#ifndef USER_BEERCTRL_H_
#define USER_BEERCTRL_H_

#define BCTRL_CTRL_MANUAL    0
#define BCTRL_CTRL_AUTOMATIC 1

#define BCTRL_FRIDGE_OFF     0
#define BCTRL_FRIDGE_COOL    1
#define BCTRL_FRIDGE_HEAT    2

#define BCTRL_EVENT_FRIDGE   0
#define BCTRL_EVENT_KP       1
#define BCTRL_EVENT_TEMP     2
#define BCTRL_EVENT_CTRL     3
#define BCTRL_EVENT_KI       4
#define BCTRL_EVENT_KD       5

// event = base + sensor index
#define BCTRL_EVENT_MAX_INDEX    10

#define BCTRL_EVENT_READING_BASE 10
#define BCTRL_EVENT_OUT_BASE     (BCTRL_EVENT_READING_BASE + BCTRL_EVENT_MAX_INDEX)
#define BCTRL_EVENT_ERR_BASE     (BCTRL_EVENT_OUT_BASE + BCTRL_EVENT_MAX_INDEX)


void BCTRL_Init(void (*eventCallback)(int event, int state));

void BCTRL_SetKP(int kp);
void BCTRL_SetKI(int ki);
void BCTRL_SetKD(int kd);
void BCTRL_SetTemp(int16_t temp);
int16_t BCTRL_GetTemp(void);
void BCTRL_SetCtrl(int ctrl);
void BCTRL_SetFridge(int state);

void BCTRL_ReportNewReading(int idx, int16_t temp);
void BCTRL_Trigger(void);

#endif /* USER_BEERCTRL_H_ */
