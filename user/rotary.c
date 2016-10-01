/*
 * Driver for rotary encoder
 *
 * Copyright (C) 2016 Henri Roosen <henri.roosen@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */
#include <ets_sys.h>
#include <gpio.h>

#include "rotary.h"


static void (*btnCallback)(void);
static void (*rotCallback)(int dir);


static void ICACHE_FLASH_ATTR rotary_handler(void *arg)
{
	uint32 status;

	// read interrupt status
	status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	// clear interrupt status
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, status);

	if (status & BIT(ROTARY_SW_PIN)) {
		// button SW
		if (btnCallback)
			btnCallback();
	} else {
		// rotary
		static int armed;
		static int dir;
		int clk = !GPIO_INPUT_GET(GPIO_ID_PIN(ROTARY_CLK_PIN));
		int dt = !GPIO_INPUT_GET(GPIO_ID_PIN(ROTARY_DT_PIN));
		int state = (clk << 1) | dt;

		switch (state) {
		case 0:
			if (armed) {
				if (rotCallback)
					rotCallback(!dir);
				armed = 0;
			}
			break;
		case 1:
		case 2:
			if (armed)
				dir = state - 1;
			break;
		case 3:
			armed = 1;
			break;
		}
	}
}

void ICACHE_FLASH_ATTR ROTARY_init(void (*buttonCallback)(void), void (*rotationCallback)(int dir))
{
	btnCallback = buttonCallback;
	rotCallback = rotationCallback;

	// Set pin functions
	PIN_FUNC_SELECT(ROTARY_SW_MUX, ROTARY_SW_FUNC);
	PIN_FUNC_SELECT(ROTARY_CLK_MUX, ROTARY_CLK_FUNC);
	PIN_FUNC_SELECT(ROTARY_DT_MUX, ROTARY_DT_FUNC);

	// Register interrupt handler
	ETS_GPIO_INTR_ATTACH(rotary_handler, NULL);
	gpio_output_set(0, 0, 0, BIT(ROTARY_SW_PIN) | BIT(ROTARY_CLK_PIN) | BIT(ROTARY_DT_PIN));

	// Clear status
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(ROTARY_SW_PIN) | BIT(ROTARY_CLK_PIN) | BIT(ROTARY_DT_PIN));

	// Enable negative edge on pin SW, both edges on CLK and DT
	gpio_pin_intr_state_set(GPIO_ID_PIN(ROTARY_SW_PIN),  GPIO_PIN_INTR_NEGEDGE);
	gpio_pin_intr_state_set(GPIO_ID_PIN(ROTARY_CLK_PIN), GPIO_PIN_INTR_ANYEDGE);
	gpio_pin_intr_state_set(GPIO_ID_PIN(ROTARY_DT_PIN),  GPIO_PIN_INTR_ANYEDGE);
}

