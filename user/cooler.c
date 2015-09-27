/*
 * cooler.c
 *
 *  Created on: Sep 26, 2015
 *      Author: roosen
 */
#include <ets_sys.h>

#include "cooler.h"
#include "driver/gpio16.h"


void ICACHE_FLASH_ATTR COOLER_init(void)
{
	gpio16_output_set(1);
	gpio16_output_conf();
}

void ICACHE_FLASH_ATTR COOLER_setState(int state)
{
	if (state == COOLER_STATE_COOLING)
		gpio16_output_set(0);
	else
		gpio16_output_set(1);
}
