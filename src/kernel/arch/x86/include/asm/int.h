/*
 *
 * Carina
 * src/kernel/arch/x86/include/asm/int.h
 *
 * Copyright (C) 2016 Bastiaan Teeuwen <bastiaan.teeuwen170@gmail.com>
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

#ifndef _INT_H
#define _INT_H

/* Software interrupts */
extern void _int0(void);
extern void _int1(void);
extern void _int2(void);
extern void _int3(void);
extern void _int4(void);
extern void _int5(void);
extern void _int6(void);
extern void _int7(void);
extern void _int8(void);
extern void _int9(void);
extern void _int10(void);
extern void _int11(void);
extern void _int12(void);
extern void _int13(void);
extern void _int14(void);
extern void _int15(void);
extern void _int16(void);
extern void _int17(void);
extern void _int18(void);
extern void _int19(void);
extern void _int20(void);
extern void _int21(void);
extern void _int22(void);
extern void _int23(void);
extern void _int24(void);
extern void _int25(void);
extern void _int26(void);
extern void _int27(void);
extern void _int28(void);
extern void _int29(void);
extern void _int30(void);
extern void _int31(void);

/* Hardware interrupts */
extern void _int32(void);
extern void _int33(void);
extern void _int34(void);
extern void _int35(void);
extern void _int36(void);
extern void _int37(void);
extern void _int38(void);
extern void _int39(void);
extern void _int40(void);
extern void _int41(void);
extern void _int42(void);
extern void _int43(void);
extern void _int44(void);
extern void _int45(void);
extern void _int46(void);
extern void _int47(void);

extern void _int48(void);
extern void _int49(void);
extern void _int50(void);
extern void _int51(void);
extern void _int52(void);
extern void _int53(void);
extern void _int54(void);
extern void _int55(void);
extern void _int56(void);
extern void _int57(void);
extern void _int58(void);
extern void _int59(void);
extern void _int60(void);
extern void _int61(void);
extern void _int62(void);
extern void _int63(void);
extern void _int64(void);
extern void _int65(void);
extern void _int66(void);
extern void _int67(void);
extern void _int68(void);
extern void _int69(void);
extern void _int70(void);
extern void _int71(void);
extern void _int72(void);
extern void _int73(void);
extern void _int74(void);
extern void _int75(void);
extern void _int76(void);
extern void _int77(void);
extern void _int78(void);
extern void _int79(void);
extern void _int80(void);
extern void _int81(void);
extern void _int82(void);
extern void _int83(void);
extern void _int84(void);
extern void _int85(void);
extern void _int86(void);
extern void _int87(void);
extern void _int88(void);
extern void _int89(void);
extern void _int90(void);
extern void _int91(void);
extern void _int92(void);
extern void _int93(void);
extern void _int94(void);
extern void _int95(void);
extern void _int96(void);
extern void _int97(void);
extern void _int98(void);
extern void _int99(void);
extern void _int100(void);
extern void _int101(void);
extern void _int102(void);
extern void _int103(void);
extern void _int104(void);
extern void _int105(void);
extern void _int106(void);
extern void _int107(void);
extern void _int108(void);
extern void _int109(void);
extern void _int110(void);
extern void _int111(void);
extern void _int112(void);
extern void _int113(void);
extern void _int114(void);
extern void _int115(void);
extern void _int116(void);
extern void _int117(void);
extern void _int118(void);
extern void _int119(void);
extern void _int120(void);
extern void _int121(void);
extern void _int122(void);
extern void _int123(void);
extern void _int124(void);
extern void _int125(void);
extern void _int126(void);
extern void _int127(void);
extern void _int128(void);
extern void _int129(void);
extern void _int130(void);
extern void _int131(void);
extern void _int132(void);
extern void _int133(void);
extern void _int134(void);
extern void _int135(void);
extern void _int136(void);
extern void _int137(void);
extern void _int138(void);
extern void _int139(void);
extern void _int140(void);
extern void _int141(void);
extern void _int142(void);
extern void _int143(void);
extern void _int144(void);
extern void _int145(void);
extern void _int146(void);
extern void _int147(void);
extern void _int148(void);
extern void _int149(void);
extern void _int150(void);
extern void _int151(void);
extern void _int152(void);
extern void _int153(void);
extern void _int154(void);
extern void _int155(void);
extern void _int156(void);
extern void _int157(void);
extern void _int158(void);
extern void _int159(void);
extern void _int160(void);
extern void _int161(void);
extern void _int162(void);
extern void _int163(void);
extern void _int164(void);
extern void _int165(void);
extern void _int166(void);
extern void _int167(void);
extern void _int168(void);
extern void _int169(void);
extern void _int170(void);
extern void _int171(void);
extern void _int172(void);
extern void _int173(void);
extern void _int174(void);
extern void _int175(void);
extern void _int176(void);
extern void _int177(void);
extern void _int178(void);
extern void _int179(void);
extern void _int180(void);
extern void _int181(void);
extern void _int182(void);
extern void _int183(void);
extern void _int184(void);
extern void _int185(void);
extern void _int186(void);
extern void _int187(void);
extern void _int188(void);
extern void _int189(void);
extern void _int190(void);
extern void _int191(void);
extern void _int192(void);
extern void _int193(void);
extern void _int194(void);
extern void _int195(void);
extern void _int196(void);
extern void _int197(void);
extern void _int198(void);
extern void _int199(void);
extern void _int200(void);
extern void _int201(void);
extern void _int202(void);
extern void _int203(void);
extern void _int204(void);
extern void _int205(void);
extern void _int206(void);
extern void _int207(void);
extern void _int208(void);
extern void _int209(void);
extern void _int210(void);
extern void _int211(void);
extern void _int212(void);
extern void _int213(void);
extern void _int214(void);
extern void _int215(void);
extern void _int216(void);
extern void _int217(void);
extern void _int218(void);
extern void _int219(void);
extern void _int220(void);
extern void _int221(void);
extern void _int222(void);
extern void _int223(void);
extern void _int224(void);
extern void _int225(void);
extern void _int226(void);
extern void _int227(void);
extern void _int228(void);
extern void _int229(void);
extern void _int230(void);
extern void _int231(void);
extern void _int232(void);
extern void _int233(void);
extern void _int234(void);
extern void _int235(void);
extern void _int236(void);
extern void _int237(void);
extern void _int238(void);
extern void _int239(void);
extern void _int240(void);
extern void _int241(void);
extern void _int242(void);
extern void _int243(void);
extern void _int244(void);
extern void _int245(void);
extern void _int246(void);
extern void _int247(void);
extern void _int248(void);
extern void _int249(void);
extern void _int250(void);
extern void _int251(void);
extern void _int252(void);
extern void _int253(void);
extern void _int254(void);
extern void _int255(void);

void (*ints[IDT_ENTRIES]) (void) = {
	_int0,
	_int1,
	_int2,
	_int3,
	_int4,
	_int5,
	_int6,
	_int7,
	_int8,
	_int9,
	_int10,
	_int11,
	_int12,
	_int13,
	_int14,
	_int15,
	_int16,
	_int17,
	_int18,
	_int19,
	_int20,
	_int21,
	_int22,
	_int23,
	_int24,
	_int25,
	_int26,
	_int27,
	_int28,
	_int29,
	_int30,
	_int31,

	_int32,
	_int33,
	_int34,
	_int35,
	_int36,
	_int37,
	_int38,
	_int39,
	_int40,
	_int41,
	_int42,
	_int43,
	_int44,
	_int45,
	_int46,
	_int47,

	_int48,
	_int49,
	_int50,
	_int51,
	_int52,
	_int53,
	_int54,
	_int55,
	_int56,
	_int57,
	_int58,
	_int59,
	_int60,
	_int61,
	_int62,
	_int63,
	_int64,
	_int65,
	_int66,
	_int67,
	_int68,
	_int69,
	_int70,
	_int71,
	_int72,
	_int73,
	_int74,
	_int75,
	_int76,
	_int77,
	_int78,
	_int79,
	_int80,
	_int81,
	_int82,
	_int83,
	_int84,
	_int85,
	_int86,
	_int87,
	_int88,
	_int89,
	_int90,
	_int91,
	_int92,
	_int93,
	_int94,
	_int95,
	_int96,
	_int97,
	_int98,
	_int99,
	_int100,
	_int101,
	_int102,
	_int103,
	_int104,
	_int105,
	_int106,
	_int107,
	_int108,
	_int109,
	_int110,
	_int111,
	_int112,
	_int113,
	_int114,
	_int115,
	_int116,
	_int117,
	_int118,
	_int119,
	_int120,
	_int121,
	_int122,
	_int123,
	_int124,
	_int125,
	_int126,
	_int127,
	_int128,
	_int129,
	_int130,
	_int131,
	_int132,
	_int133,
	_int134,
	_int135,
	_int136,
	_int137,
	_int138,
	_int139,
	_int140,
	_int141,
	_int142,
	_int143,
	_int144,
	_int145,
	_int146,
	_int147,
	_int148,
	_int149,
	_int150,
	_int151,
	_int152,
	_int153,
	_int154,
	_int155,
	_int156,
	_int157,
	_int158,
	_int159,
	_int160,
	_int161,
	_int162,
	_int163,
	_int164,
	_int165,
	_int166,
	_int167,
	_int168,
	_int169,
	_int170,
	_int171,
	_int172,
	_int173,
	_int174,
	_int175,
	_int176,
	_int177,
	_int178,
	_int179,
	_int180,
	_int181,
	_int182,
	_int183,
	_int184,
	_int185,
	_int186,
	_int187,
	_int188,
	_int189,
	_int190,
	_int191,
	_int192,
	_int193,
	_int194,
	_int195,
	_int196,
	_int197,
	_int198,
	_int199,
	_int200,
	_int201,
	_int202,
	_int203,
	_int204,
	_int205,
	_int206,
	_int207,
	_int208,
	_int209,
	_int210,
	_int211,
	_int212,
	_int213,
	_int214,
	_int215,
	_int216,
	_int217,
	_int218,
	_int219,
	_int220,
	_int221,
	_int222,
	_int223,
	_int224,
	_int225,
	_int226,
	_int227,
	_int228,
	_int229,
	_int230,
	_int231,
	_int232,
	_int233,
	_int234,
	_int235,
	_int236,
	_int237,
	_int238,
	_int239,
	_int240,
	_int241,
	_int242,
	_int243,
	_int244,
	_int245,
	_int246,
	_int247,
	_int248,
	_int249,
	_int250,
	_int251,
	_int252,
	_int253,
	_int254,
	_int255
};

#endif
