/*
 * display.c
 *
 *  Created on: Jul 26, 2015
 *      Author: roosen
 */
#include <ets_sys.h>

#include "driver/i2c_oled.h"
#include "beerctrl.h"
#include "display.h"


void ICACHE_FLASH_ATTR DISPLAY_BeerCtrlEvent(int event, int state)
{
	unsigned char buf[16];

	switch (event) {
	case BCTRL_EVENT_FRIDGE:
		switch (state) {
		case BCTRL_FRIDGE_COOL:
			OLED_Print(1, 7, "C", 1);
			break;
		case BCTRL_FRIDGE_HEAT:
			OLED_Print(1, 7, "H", 1);
			break;
		case BCTRL_FRIDGE_OFF:
		default:
			OLED_Print(1, 7, "-", 1);
			break;
		}
		break;
	case BCTRL_EVENT_KP:
		os_sprintf(buf, "%02d", state);
		OLED_Print(14, 7, "KP:", 1);
		OLED_Print(17, 7, buf, 1);
		break;
	case BCTRL_EVENT_TEMP:
		temp_to_string(state, buf, sizeof(buf));
		OLED_Print(3, 7, "T:", 1);
		OLED_Print(5, 7, buf, 1);
		break;
	case BCTRL_EVENT_CTRL:
		if (state == BCTRL_CTRL_AUTOMATIC) {
			OLED_Print(0, 7, "A", 1);
		} else {
			OLED_Print(0, 7, "M", 1);
		}
		break;
	default:
		break;
	}

	if (event >= BCTRL_EVENT_ERR_BASE && event < BCTRL_EVENT_ERR_BASE + BCTRL_EVENT_MAX_INDEX) {
		os_sprintf(buf, "%05d", state);
		OLED_Print(0, 3, "err:", 1);
		OLED_Print(5, 3, buf, 1);
	} else if (event >= BCTRL_EVENT_OUT_BASE && event < BCTRL_EVENT_OUT_BASE + BCTRL_EVENT_MAX_INDEX) {
		os_sprintf(buf, "%05d", state);
		OLED_Print(0, 4, "out:", 1);
		OLED_Print(5, 4, buf, 1);
	} else if (event >= BCTRL_EVENT_READING_BASE && event < BCTRL_EVENT_READING_BASE + BCTRL_EVENT_MAX_INDEX) {
		temp_to_string(state, buf, sizeof(buf));
		OLED_Print(0, 0, buf, 2);
		os_printf("Temp (%02d): %s Celsius\r\n", event - BCTRL_EVENT_READING_BASE, buf);
	} else if (event == DISPLAY_EVENT_TEST) {
		os_sprintf(buf, "%04d", state);
		OLED_Print(0, 1, "tst1:", 1);
		OLED_Print(5, 1, buf, 1);
	} else if (event == DISPLAY_EVENT_TEST_2) {
		os_sprintf(buf, "%04d", state);
		OLED_Print(0, 2, "tst2:", 1);
		OLED_Print(5, 2, buf, 1);
	}
}
