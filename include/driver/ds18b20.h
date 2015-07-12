/*
 * ds18b20.h
 *
 *  Created on: May 4, 2015
 *      Author: henri
 */

#ifndef DS18B20_H_
#define DS18B20_H_

void ICACHE_FLASH_ATTR ds18b20_scan(void);
void ICACHE_FLASH_ATTR ds18b20_convert_all(void);
void ICACHE_FLASH_ATTR ds18b20_read_all(void);

int ICACHE_FLASH_ATTR ds18b20_get_temp(int idx, int16_t *temp);

#endif /* DS18B20_H_ */
