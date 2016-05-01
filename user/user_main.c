#include <ets_sys.h>
#include <osapi.h>
#include "mqtt.h"
#include "config.h"
#include "gpio.h"
#include "user_interface.h"
#include "os_type.h"
#include "user_config.h"
#include "driver/i2c.h"
#include "driver/i2c_oled.h"
#include "beerctrl.h"
#include "display.h"
#include "rotary.h"


#define CONFIG_MQTT
//#define CONFIG_VIRT_SENSOR

#define DELAY 1000 /* milliseconds */
#define DS18B20_SCAN_CNT 60

static os_timer_t ds18b20_timer;
#ifdef CONFIG_VIRT_SENSOR
static int16_t virt_temp = 15 << 4;
#endif


static void ICACHE_FLASH_ATTR timer_cb(void *arg)
{
	static int state;
	static int cnt;
	int16_t temp;

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
#ifdef CONFIG_VIRT_SENSOR
			if (1) {
				temp = virt_temp;
#else
			if (!ds18b20_get_temp(0, &temp)) {
#endif
				BCTRL_ReportNewReading(0, temp);
				if (cnt >= DS18B20_SCAN_CNT) {
					char buf[16];
					uint8_t str_url[265];
					// Send temperature to Thingspeak.com
					temp_to_string(temp, buf, sizeof(buf));
					os_sprintf(str_url, "api_key=%s&field1=%s", KEY_THINGSPEAK, buf);
					http_post("http://api.thingspeak.com/update", str_url, NULL);
					os_printf("-sensor_temp: %s\n", buf);
				}
			}
			if (cnt >= DS18B20_SCAN_CNT) {
				cnt = 0;
				state = 0;
			} else {
				state = 1;
			}
			break;
		default:
			break;
	}
	if (cnt)
		BCTRL_Trigger();
}

static void ICACHE_FLASH_ATTR uart_init(void)
{
	/* Set Uart0 baudrate */
	/* Default is 74880, which is non-standard and pl2303 doesn't support */
	uart_div_modify(0, UART_CLK_FREQ / 115200);
}

#ifdef CONFIG_VIRT_SENSOR
static void ICACHE_FLASH_ATTR onRotary(int dir)
{
	if (dir > 0)
		virt_temp++;
	else
		virt_temp--;
}
#endif

void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init();
	i2c_init();
	OLED_Init();
#ifdef CONFIG_VIRT_SENSOR
	ROTARY_init(NULL, onRotary);
#endif

	BCTRL_Init(DISPLAY_BeerCtrlEvent);

	os_timer_disarm(&ds18b20_timer);
	os_timer_setfn(&ds18b20_timer, (os_timer_func_t *)timer_cb, (void *)0);
	os_timer_arm(&ds18b20_timer, DELAY, 1);

#ifdef CONFIG_MQTT
	wifi_set_opmode(STATION_MODE);
	CFG_Load();
	MQTT_Start();
#endif /* CONFIG_MQTT */

	os_printf("System started ...%u\n", system_get_chip_id());
}
