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

#define HEATER_MUX PERIPHS_IO_MUX_GPIO2_U
#define HEATER_FUNC FUNC_GPIO2
#define HEATER_PIN 2


void ICACHE_FLASH_ATTR HEATER_init(void)
{
	PIN_FUNC_SELECT(HEATER_MUX, HEATER_FUNC);
}

void ICACHE_FLASH_ATTR HEATER_setState(int state)
{
	if (state == HEATER_STATE_HEATING) {
		gpio_output_set(0, (1 << HEATER_PIN), (1 << HEATER_PIN), 0);
	} else {
		gpio_output_set((1 << HEATER_PIN), 0, (1 << HEATER_PIN), 0);
	}
}
