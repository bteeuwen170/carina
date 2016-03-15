/*
 *
 * Carina
 * Header for Framebuffer Driver
 *
 * Copyright (C) 2015 Bastiaan Teeuwen <bastiaan.teeuwen170@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 */

#ifndef __FB_H__
#define __FB_H__

#include <stdint.h>

#define FB_CNT			4

uint8_t fb_focus;

uint16_t *fb_buffer;
uint16_t fb_buffers[FB_CNT];

uint16_t fb_x[FB_CNT];
uint16_t fb_y[FB_CNT];

uint8_t fb_fgcolor;
uint8_t fb_bgcolor;

void fb_init(const uint8_t fb);

void fb_cur_set(void);
void fb_cur_style(const uint8_t style);

void fb_clr(void);
void fb_scrl_dwn(void);

#endif
