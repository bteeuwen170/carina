/*
 *
 * Elarix
 * src/kernel/include/termios.h
 *
 * Copyright (C) 2016 - 2017 Bastiaan Teeuwen <bastiaan@mkcl.nl>
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

#ifndef _TERMIOS_H
#define _TERMIOS_H

/* #define IGNBRK	
#define BRKINT	
#define IGNPAR	
#define PARMRK	
#define INPCK	
#define ISTRIP	
#define INLCR	
#define IGNCR	
#define ICRNL	
#define IUCLC	
#define IXON		
#define IXANY	
#define IXOFF	
#define IMAXBEL	 */

/* Not going to use all CCs */
#define NUM_CC	17

#define VINTR		0
#define VQUIT		1
#define VERASE		2
#define VKILL		3
#define VEOF		4
#define VTIME		5
#define VMIN		6
#define VSWTC		7
#define VSTART		8
#define VSTOP		9
#define VSUSP		10
#define VEOL		11
#define VREPRINT	12
#define VDISCARD	13
#define WERASE		14
#define VLNEXT		15
#define VEOL2		16

struct termios {
	u32	c_iflag;
	u32	c_oflag;
	u32	c_cflag;
	u32	c_lflag;
	u8	c_line;
	u8	c_cc[NUM_CC];
};

struct winsize {
	u16	ws_row;
	u16	ws_col;
	u16	ws_xpixel;
	u16	wx_ypixel;
};

#endif
