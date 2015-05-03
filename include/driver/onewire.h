#ifndef __ONEWIRE_H__
#define __ONEWIRE_H__

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"

#define ONEWIRE_MUX		PERIPHS_IO_MUX_GPIO2_U
#define ONEWIRE_FUNC	FUNC_GPIO2
#define ONEWIRE_PIN		2

#define ONEWIRE_SEARCHROM 			0xF0
#define ONEWIRE_READROM				0x33
#define ONEWIRE_MATCHROM 			0x55
#define ONEWIRE_SKIPROM             0xCC
#define ONEWIRE_ALARMSEARCH			0xEC

#define DS1820_CONVERT_T            0x44
#define DS1820_WRITE_SCRATCHPAD 	0x4E
#define DS1820_READ_SCRATCHPAD      0xBE
#define DS1820_COPY_SCRATCHPAD 		0x48
#define DS1820_READ_EEPROM 			0xB8
#define DS1820_READ_PWRSUPPLY 		0xB4

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