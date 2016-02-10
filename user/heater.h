/*
 * heater.h
 *
 * Copyright (C) 2016 Henri Roosen <henri.roosen@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef USER_HEATER_H_
#define USER_HEATER_H_


#define HEATER_STATE_OFF     0
#define HEATER_STATE_HEATING 1

void HEATER_init(void);
void HEATER_setState(int state);

#endif /* USER_HEATER_H_ */
