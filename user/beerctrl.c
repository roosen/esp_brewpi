#include <ets_sys.h>
#include <osapi.h>
#include "beerctrl.h"
#include "cooler.h"
#include "heater.h"


#define CONFIG_KP   4
#define CONFIG_KI   1
#define CONFIG_KD   1
#define CONFIG_TEMP 4
#define CONFIG_DEADBAND 10
#define PERIOD      256

#define CONFIG_DEBUG_PID

struct beerctrl_t {
	int ctrl;
	int16_t temp;
	int32_t output;
	int32_t kp;
	int32_t ki;
	int32_t kd;
	int32_t integral;
	uint32_t lasttime;
	int32_t lasterror;
	int32_t out_min;
	int32_t out_max;
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

	BCTRL_SetLimits(-PERIOD, PERIOD);
	BCTRL_SetKP(CONFIG_KP);
	BCTRL_SetKI(CONFIG_KI);
	BCTRL_SetKD(CONFIG_KD);
	BCTRL_SetTemp(CONFIG_TEMP << 4);
	BCTRL_SetCtrl(BCTRL_CTRL_AUTOMATIC);
	setFridge(BCTRL_FRIDGE_OFF);
	b.lasttime = system_get_time();
}

void ICACHE_FLASH_ATTR BCTRL_SetLimits(int32_t min, int32_t max)
{
	b.out_min = min;
	b.out_max = max;
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
		int32_t  error = b.temp - temp;
		uint32_t now = system_get_time() / 1000000;
		int32_t  deriviative;
		int32_t dt = (int32_t)(now - b.lasttime);

		if (eventCb)
			eventCb(BCTRL_EVENT_ERR_BASE + idx, error);

		/* integral calculation */
		b.integral += error * dt;
		if (b.integral > b.out_max)
			b.integral = b.out_max;
		else if (b.integral < b.out_min)
			b.integral = b.out_min;

		/* deriviative calculation */
		deriviative = (error - b.lasterror) / dt;

		/* output calculation */
		b.output = b.kp * error + b.ki * b.integral + b.kd * deriviative;
		if (b.output > b.out_max)
			b.output = b.out_max;
		else if (b.output < b.out_min)
			b.output = b.out_min;

		b.lasttime = now;
		b.lasterror = error;

		if (eventCb)
			eventCb(BCTRL_EVENT_OUT_BASE + idx, b.output);

#ifdef CONFIG_DEBUG_PID
		os_printf("PID:\n");
		os_printf("\tdt: %d\n", dt);
		os_printf("\terror: %d\n", error);
		os_printf("\tintegral: %d\n", b.integral);
		os_printf("\tderiviative: %d\n", deriviative);
		os_printf("\toutput: %d\n", b.output);
		MQTT_Pub(dt, error, b.integral, deriviative, b.output);
#endif
	}
	if (eventCb)
		eventCb(BCTRL_EVENT_READING_BASE + idx, temp);
}

void ICACHE_FLASH_ATTR BCTRL_Trigger(void)
{
	if (b.ctrl == BCTRL_CTRL_AUTOMATIC) {
		static int cnt;

		/* "PWM" for cooling and heating */
		if (b.output < (b.out_min + CONFIG_DEADBAND) ||
				(b.output < -CONFIG_DEADBAND && cnt < -b.output))
			setFridge(BCTRL_FRIDGE_COOL);
		else if (b.output > (b.out_max - CONFIG_DEADBAND) ||
				(b.output > CONFIG_DEADBAND && cnt < b.output))
			setFridge(BCTRL_FRIDGE_HEAT);
		else
			setFridge(BCTRL_FRIDGE_OFF);

		cnt++;
		/* Wrap */
		if (cnt >= -b.out_min)
			cnt = 0;
	}
}

