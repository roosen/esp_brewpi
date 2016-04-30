#include <ets_sys.h>

#include "beerctrl.h"
#include "cooler.h"
#include "heater.h"


#define CONFIG_KP   4
#define CONFIG_KI   1
#define CONFIG_KD   1
#define CONFIG_TEMP 11
#define PERIOD      256

struct beerctrl_t {
	int ctrl;
	int temp;
	int output;
	int kp;
	int ki;
	int kd;
	int32_t integral;
	uint32_t lasttime;
	int32_t lasterror;
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
	BCTRL_SetKI(CONFIG_KI);
	BCTRL_SetKD(CONFIG_KD);
	BCTRL_SetTemp(CONFIG_TEMP << 4);
	BCTRL_SetCtrl(BCTRL_CTRL_AUTOMATIC);
	setFridge(BCTRL_FRIDGE_OFF);
	b.lasttime = system_get_time();
}

void ICACHE_FLASH_ATTR BCTRL_SetKP(int kp)
{
	b.kp = kp;
	if (eventCb)
		eventCb(BCTRL_EVENT_KP, kp);
}

void ICACHE_FLASH_ATTR BCTRL_SetKI(int ki)
{
	b.ki = ki;
	if (eventCb)
		eventCb(BCTRL_EVENT_KI, ki);
}

void ICACHE_FLASH_ATTR BCTRL_SetKD(int kd)
{
	b.kd = kd;
	if (eventCb)
		eventCb(BCTRL_EVENT_KD, kd);
}

void ICACHE_FLASH_ATTR BCTRL_SetTemp(int16_t temp)
{
	b.temp = temp;
	if (eventCb)
		eventCb(BCTRL_EVENT_TEMP, temp);
}

int16_t ICACHE_FLASH_ATTR BCTRL_GetTemp(void)
{
	return b.temp;
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
		uint32_t now = system_get_time() / 1000000;
		int32_t deriviative;

		uint32_t dt = now - b.lasttime;

		if (eventCb)
			eventCb(BCTRL_EVENT_ERR_BASE + idx, error);

		b.integral += error * dt;
		deriviative = (error - b.lasterror) / dt;
		b.output = b.kp * error + b.ki * b.integral + b.kd * deriviative;

		b.lasttime = now;
		b.lasterror = error;

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
