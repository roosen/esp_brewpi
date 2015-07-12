#include "beerctrl.h"
#include "driver/gpio16.h"
#include "driver/i2c_oled.h"

#define CONFIG_KP   4
#define CONFIG_TEMP 11
#define PERIOD      256

struct beerctrl_t {
	int ctrl;
	int temp;
	int output;
	int kp;
};

static struct beerctrl_t b;

static void ICACHE_FLASH_ATTR setFridge(int state)
{
	switch (state) {
	case BCTRL_FRIDGE_COOL:
		gpio16_output_set(0);
		OLED_Print(1, 7, "C", 1);
		break;
	case BCTRL_FRIDGE_HEAT:
		gpio16_output_set(1);
		OLED_Print(1, 7, "H", 1);
		break;
	case BCTRL_FRIDGE_OFF:
	default:
		gpio16_output_set(1);
		OLED_Print(1, 7, "-", 1);
		break;
	}
}

void ICACHE_FLASH_ATTR BCTRL_Init(void)
{
	gpio16_output_set(1);
	gpio16_output_conf();

	BCTRL_SetKP(CONFIG_KP);
	BCTRL_SetTemp(CONFIG_TEMP << 4);
	BCTRL_SetCtrl(BCTRL_CTRL_AUTOMATIC);
	setFridge(BCTRL_FRIDGE_OFF);
}

void ICACHE_FLASH_ATTR BCTRL_SetKP(int kp)
{
	unsigned char buf[16];

	b.kp = kp;

	os_sprintf(buf, "%d", b.kp);
	OLED_Print(14, 7, "KP:", 1);
	OLED_Print(17, 7, buf, 1);
}

void ICACHE_FLASH_ATTR BCTRL_SetTemp(int16_t temp)
{
	unsigned char buf[16];

	b.temp = temp;

	temp_to_string(b.temp, buf, sizeof(buf));
	OLED_Print(3, 7, "T:", 1);
	OLED_Print(5, 7, buf, 1);
}

void ICACHE_FLASH_ATTR BCTRL_SetCtrl(int ctrl)
{
	b.ctrl = ctrl;
	if (b.ctrl == BCTRL_CTRL_AUTOMATIC) {
		OLED_Print(0, 7, "A", 1);
	} else {
		OLED_Print(0, 7, "M", 1);
	}
}

void ICACHE_FLASH_ATTR BCTRL_SetFridge(int state)
{
	if (b.ctrl == BCTRL_CTRL_MANUAL) {
		setFridge(state);
	}
}

void ICACHE_FLASH_ATTR BCTRL_ReportNewReading(int idx, int16_t temp)
{
	if (b.ctrl == BCTRL_CTRL_AUTOMATIC) {
		unsigned char buf[16];
		int32_t error = b.temp - temp;

		os_sprintf(buf, "%d", error);
		OLED_Print(0, 3, "err:", 1);
		OLED_Print(5, 3, buf, 1);

		b.output = b.kp * error;

		if (b.output > PERIOD)
			b.output = PERIOD;
		else if (b.output < -PERIOD)
			b.output = -PERIOD;

		os_sprintf(buf, "%d", b.output);
		OLED_Print(0, 4, "out:", 1);
		OLED_Print(5, 4, buf, 1);
	}
}

void ICACHE_FLASH_ATTR BCTRL_Trigger(void)
{
	static int cnt;

	if (cnt >= -b.output) {
		setFridge(BCTRL_FRIDGE_OFF);
	} else {
		setFridge(BCTRL_FRIDGE_COOL);
	}

	cnt++;
	if (cnt >= PERIOD)
		cnt = 0;
}
