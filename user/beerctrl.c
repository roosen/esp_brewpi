#include "beerctrl.h"
#include "driver/gpio16.h"
#include "driver/i2c_oled.h"

struct beerctrl_t {
	int ctrl;
	int temp;
};

static struct beerctrl_t b;

static void setFridge(int state)
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

void BCTRL_Init(void)
{
	gpio16_output_conf();
	BCTRL_SetTemp(0);
	BCTRL_SetCtrl(BCTRL_CTRL_MANUAL);
	setFridge(BCTRL_FRIDGE_OFF);
}

void BCTRL_SetTemp(int t)
{
	unsigned char buf[16];
	b.temp = t;
	os_sprintf(buf, "%d", t);

	OLED_Print(3, 7, "SET:", 1);
	OLED_Print(7, 7, buf, 1);
}

void BCTRL_SetCtrl(int ctrl)
{
	b.ctrl = ctrl;
	if (b.ctrl == BCTRL_CTRL_AUTOMATIC) {
		OLED_Print(0, 7, "A", 1);
	} else {
		OLED_Print(0, 7, "M", 1);
	}
}

void BCTRL_SetFridge(int state)
{
	if (b.ctrl == BCTRL_CTRL_MANUAL) {
		setFridge(state);
	}
}
