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


//#define CONFIG_WIFI

#define DELAY 1000 /* milliseconds */
#define DS18B20_SCAN_CNT 60

static os_timer_t ds18b20_timer;

extern void ds18b20_publish(void);


static void ICACHE_FLASH_ATTR timer_cb(void *arg)
{
	static int state;
	static int cnt;

	cnt++;
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
			if (cnt >= DS18B20_SCAN_CNT) {
				ds18b20_publish();
				cnt = 0;
				state = 0;
			} else {
				state = 1;
			}
			break;
		default:
			break;
	}
}

static void ICACHE_FLASH_ATTR uart_init(void)
{
	/* Set Uart0 baudrate */
	/* Default is 74880, which is non-standard and pl2303 doesn't support */
	uart_div_modify(0, UART_CLK_FREQ / 115200);
}

void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init();
	i2c_init();
	OLED_Init();

	os_timer_disarm(&ds18b20_timer);
	os_timer_setfn(&ds18b20_timer, (os_timer_func_t *)timer_cb, (void *)0);
	os_timer_arm(&ds18b20_timer, DELAY, 1);

#ifdef CONFIG_WIFI
	wifi_set_opmode(STATION_MODE);
	CFG_Load();
	MQTT_Start();
#endif /* CONFIG_WIFI */

	os_printf("System started ...\n");
}
