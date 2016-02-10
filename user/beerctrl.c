#include <ets_sys.h>

#include "beerctrl.h"
#include "cooler.h"
#include "heater.h"


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
static void (*eventCb)(int event, int state);

static void ICACHE_FLASH_ATTR setFridge(int state)
{
	switch (state) {
	case BCTRL_FRIDGE_COOL:
		HEATER_setState(HEATER_STATE_OFF);
		COOLER_setState(COOLER_STATE_COOLING);
		break;
	case BCTRL_FRIDGE_HEAT:
		COOLER_setState(COOLER_STATE_OFF);
		HEATER_setState(HEATER_STATE_HEATING);
		break;
	case BCTRL_FRIDGE_OFF:
	default:
		COOLER_setState(COOLER_STATE_OFF);
		HEATER_setState(COOLER_STATE_OFF);
		break;
	}
	if (eventCb)
		eventCb(BCTRL_EVENT_FRIDGE, state);
}

void ICACHE_FLASH_ATTR BCTRL_Init(void (*eventCallback)(int event, int state))
{
	eventCb = eventCallback;

	COOLER_init();
	HEATER_init();

	BCTRL_SetKP(CONFIG_KP);
	BCTRL_SetTemp(CONFIG_TEMP << 4);
	BCTRL_SetCtrl(BCTRL_CTRL_AUTOMATIC);
	setFridge(BCTRL_FRIDGE_OFF);
}

void ICACHE_FLASH_ATTR BCTRL_SetKP(int kp)
{
	b.kp = kp;
	if (eventCb)
		eventCb(BCTRL_EVENT_KP, kp);
}

void ICACHE_FLASH_ATTR BCTRL_SetTemp(int16_t temp)
{
	b.temp = temp;
	if (eventCb)
		eventCb(BCTRL_EVENT_TEMP, temp);
}

void ICACHE_FLASH_ATTR BCTRL_SetCtrl(int ctrl)
{
	b.ctrl = ctrl;
	if (eventCb)
		eventCb(BCTRL_EVENT_CTRL, ctrl);
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

		if (eventCb)
			eventCb(BCTRL_EVENT_ERR_BASE + idx, error);

		b.output = b.kp * error;

		if (b.output > PERIOD)
			b.output = PERIOD;
		else if (b.output < -PERIOD)
			b.output = -PERIOD;

		if (eventCb)
			eventCb(BCTRL_EVENT_OUT_BASE + idx, b.output);
	}
	if (eventCb)
		eventCb(BCTRL_EVENT_READING_BASE + idx, temp);
}

void ICACHE_FLASH_ATTR BCTRL_Trigger(void)
{
	if (b.ctrl == BCTRL_CTRL_AUTOMATIC) {
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
}
