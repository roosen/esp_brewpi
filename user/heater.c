/*
 * heater.c
 *
 * Copyright (C) 2016 Henri Roosen <henri.roosen@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */
#include <ets_sys.h>

#include "heater.h"


void ICACHE_FLASH_ATTR HEATER_init(void)
{
}

void ICACHE_FLASH_ATTR HEATER_setState(int state)
{
	if (state == HEATER_STATE_HEATING) {
	} else {
	}
}
