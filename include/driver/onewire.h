#ifndef __ONEWIRE_H__
#define __ONEWIRE_H__

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"

#if 0
#define ONEWIRE_MUX		PERIPHS_IO_MUX_GPIO2_U
#define ONEWIRE_FUNC	FUNC_GPIO2
#define ONEWIRE_PIN		2
#else
#define ONEWIRE_MUX		PERIPHS_IO_MUX_MTCK_U
#define ONEWIRE_FUNC	FUNC_GPIO13
#define ONEWIRE_PIN		13
#endif

#define ONEWIRE_SEARCHROM 			0xF0
#define ONEWIRE_READROM				0x33
#define ONEWIRE_MATCHROM 			0x55
#define ONEWIRE_SKIPROM             0xCC
#define ONEWIRE_ALARMSEARCH			0xEC

void onewire_init();
int onewire_search(uint8_t *addr);
void onewire_search_reset();

uint8_t onewire_reset(void);

void onewire_select(const uint8_t rom[8]);
void onewire_skip();

void onewire_write(uint8_t v, int power);
uint8_t onewire_read();

uint8_t crc8(const uint8_t *addr, uint8_t len);

#endif
