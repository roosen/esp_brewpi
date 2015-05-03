#include "ets_sys.h"
#include "osapi.h"
#include "mqtt.h"
#include "config.h"
#include "gpio.h"
#include "user_interface.h"
#include "os_type.h"
#include "user_config.h"
#include "driver/i2c.h"
#include "driver/i2c_oled.h"

#define DELAY 1000 /* milliseconds */

static os_timer_t ds18b20_timer;
static volatile bool OLED;

extern void ets_wdt_disable(void);


static void ICACHE_FLASH_ATTR timer_cb(void *arg)
{
	static int state;

	switch (state % 3) {
		case 0:
			ds18b20_scan();
			state = 1;
			break;
		case 1:
			ds18b20_convert_all();
			state = 2;
			break;
		case 2:
			ds18b20_read_all();
			state = 0;
			break;
		default:
			break;
	}
}


void user_init(void)
{
	os_timer_disarm(&ds18b20_timer);
	os_timer_setfn(&ds18b20_timer, (os_timer_func_t *)timer_cb, (void *)0);
	os_timer_arm(&ds18b20_timer, DELAY, 1);

  	i2c_init();
  	OLED = OLED_Init();

	OLED_Print(2, 0, "ESP8266 MQTT OLED", 1);

	wifi_set_opmode(STATION_MODE);
	CFG_Load();
	MQTT_Start();
	os_printf("System started ...\n");
}
