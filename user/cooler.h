/*
 * cooler.h
 *
 *  Created on: Sep 26, 2015
 *      Author: roosen
 */

#ifndef USER_COOLER_H_
#define USER_COOLER_H_


#define COOLER_STATE_OFF     0
#define COOLER_STATE_COOLING 1

void COOLER_init(void);
void COOLER_setState(int state);

#endif /* USER_COOLER_H_ */
