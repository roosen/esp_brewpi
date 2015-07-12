/*
 * ds18b20.c
 *
 *  Created on: May 4, 2015
 *      Author: henri
 */
#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>

#include "driver/onewire.h"
#include "driver/ds18b20.h"
#include "../user/httpclient.h"


#define MAX_SENSORS 2
#define SCRATCHPAD_SIZE 9

#define DS1820_CONVERT_T            0x44
#define DS1820_WRITE_SCRATCHPAD 	0x4E
#define DS1820_READ_SCRATCHPAD      0xBE
#define DS1820_COPY_SCRATCHPAD 		0x48
#define DS1820_READ_EEPROM 			0xB8
#define DS1820_READ_PWRSUPPLY 		0xB4


static int num_sensors;
static uint8_t addrs[MAX_SENSORS][8];
static int16_t t[MAX_SENSORS];


void ICACHE_FLASH_ATTR ds18b20_scan(void)
{
	onewire_init();

	num_sensors = 0;
	while (num_sensors < MAX_SENSORS && onewire_search(addrs[num_sensors])) {
		uint8_t *addr = addrs[num_sensors];

		os_printf("Found Device @ %02x %02x %02x %02x %02x %02x %02x %02x: ", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7]);
		if (crc8(addr, 8)) {
			os_printf( "CRC mismatch, crc=%xd, addr[7]=%xd\r\n", crc8(addr, 7), addr[7]);
			continue;
		}

		switch (addr[0]) {
		case 0x10:
			num_sensors++;
			os_printf("Device is DS18S20 family.\r\n");
			break;
		case 0x28:
			num_sensors++;
			os_printf("Device is DS18B20 family.\r\n");
			break;
		default:
			os_printf("Device is unknown family.\r\n");
			break;
		}
	}
	if (!num_sensors) {
		os_printf("No DS18B20 detected, sorry.\r\n");
	}
}

void ICACHE_FLASH_ATTR ds18b20_convert_all(void)
{
	onewire_reset();
	onewire_skip(); // select all sensors
	onewire_write(DS1820_CONVERT_T, 1); // perform temperature conversion
}

void ICACHE_FLASH_ATTR ds18b20_read_all(void)
{
	int i, j;

	for (j = 0; j < num_sensors; j++) {
		uint8_t *addr = addrs[j];
		uint8_t data[SCRATCHPAD_SIZE];

		onewire_reset();
		onewire_select(addr);
		onewire_write(DS1820_READ_SCRATCHPAD, 0); // read scratchpad

		for (i = 0; i < SCRATCHPAD_SIZE; i++)
			data[i] = onewire_read();

		if (crc8(data, 9)) {
			os_printf("[%d] Scratchpad CRC error!\r\n");
		} else {
			t[j] = *(int16_t *)data;

#ifdef CONFIG_DEBUG_SCRATCHPAD
			os_printf("(\r\n");

			for (i = 0; i < SCRATCHPAD_SIZE; i++)
				os_printf("%2x ", data[i]);

			os_printf(")\r\n");
#endif
		}
	}
}

int ICACHE_FLASH_ATTR ds18b20_get_temp(int idx, int16_t *temp)
{
	if (idx < 0 || idx >= num_sensors)
		return -1;

	if (temp)
		*temp = t[idx];

	return 0;
}

void ICACHE_FLASH_ATTR ds18b20_temp_to_string(int16_t temp, char *buf, int sz)
{
	int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract, i;
	unsigned char *data = (unsigned char *)&temp;

	LowByte = data[0];
	HighByte = data[1];
	TReading = (HighByte << 8) + LowByte;
	SignBit = TReading & 0x8000;  // test most sig bit
	if (SignBit) // negative
		TReading = (TReading ^ 0xffff) + 1; // 2's comp

	Whole = TReading >> 4;  // separate off the whole and fractional portions
	Fract = (TReading & 0xf) * 100 / 16;

	os_sprintf(buf, "%c%d.%02d", SignBit ? '-' : '+', Whole, Fract);
}
