/*
 * Driver for rotary encoder
 *
 * Copyright (C) 2016 Henri Roosen <henri.roosen@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef USER_ROTARY_H_
#define USER_ROTARY_H_

#include <osapi.h>

#define ROTARY_SW_MUX   (PERIPHS_IO_MUX_GPIO0_U)
#define ROTARY_SW_FUNC  (FUNC_GPIO0)
#define ROTARY_SW_PIN   0

#define ROTARY_CLK_MUX  (PERIPHS_IO_MUX_GPIO5_U)
#define ROTARY_CLK_FUNC (FUNC_GPIO5)
#define ROTARY_CLK_PIN  5

#define ROTARY_DT_MUX   (PERIPHS_IO_MUX_GPIO4_U)
#define ROTARY_DT_FUNC  (FUNC_GPIO4)
#define ROTARY_DT_PIN   4

void ROTARY_init(void (*buttonCallback)(void), void (*rotationCallback)(int dir));


#endif /* USER_ROTARY_H_ */
