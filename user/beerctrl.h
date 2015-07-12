/*
 * beerctrl.h
 *
 *  Created on: Jul 12, 2015
 *      Author: roosen
 */

#ifndef USER_BEERCTRL_H_
#define USER_BEERCTRL_H_

#define BCTRL_CTRL_MANUAL    0
#define BCTRL_CTRL_AUTOMATIC 1

#define BCTRL_FRIDGE_OFF     0
#define BCTRL_FRIDGE_COOL    1
#define BCTRL_FRIDGE_HEAT    2

void BCTRL_SetTemp(int t);
void BCTRL_SetCtrl(int ctrl);
void BCTRL_SetFridge(int state);

#endif /* USER_BEERCTRL_H_ */
