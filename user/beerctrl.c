#include "beerctrl.h"
#include "driver/gpio16.h"
#include "driver/i2c_oled.h"

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
	BCTRL_SetTemp(0);
	BCTRL_SetCtrl(BCTRL_CTRL_MANUAL);
	setFridge(BCTRL_FRIDGE_OFF);
}

void ICACHE_FLASH_ATTR BCTRL_SetKP(int kp)
{
	b.kp = kp;
}

void ICACHE_FLASH_ATTR BCTRL_SetTemp(int16_t temp)
{
	unsigned char buf[16];
	b.temp = temp;

	temp_to_string(temp, buf, sizeof(buf));

	OLED_Print(3, 7, "SET:", 1);
	OLED_Print(7, 7, buf, 1);
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
		int32_t error = b.temp - temp;

		b.output = b.kp * -error;

		if (b.output > 1000)
			b.output = 1000;
		else if (b.output < 0)
			b.output = 0;
	}
}

void ICACHE_FLASH_ATTR BCTRL_Trigger(void)
{
	static cnt;

	if (cnt >= b.output) {
		setFridge(BCTRL_FRIDGE_OFF);
	} else {
		setFridge(BCTRL_FRIDGE_COOL);
	}

	cnt++;
	if (cnt >= 1000)
		cnt = 0;
}
