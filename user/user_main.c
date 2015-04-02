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

extern void ets_wdt_disable(void);

static volatile bool OLED;

void user_init(void)
{
  	i2c_init();
  	OLED = OLED_Init();

	OLED_Print(2, 0, "ESP8266 MQTT OLED", 1);

	wifi_set_opmode(STATION_MODE);
	CFG_Load();
	MQTT_Start();
	os_printf("System started ...\n");
}
