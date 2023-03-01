/*
Copyright (c) 2021 Devine Lu Linvega

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE.
*/

#define PAGE_PROGRAM 0x0100

/* clang-format off */

#define POKE16(d, v) { (d)[0] = (v) >> 8; (d)[1] = (v); }
#define PEEK16(d) ((d)[0] << 8 | (d)[1])

/* clang-format on */

typedef unsigned char Uint8;
typedef signed char Sint8;
typedef unsigned short Uint16;
typedef signed short Sint16;
typedef unsigned int Uint32;

typedef struct {
	Uint8 dat[255], ptr;
} Stack;

typedef struct Uxn {
	Uint8 *ram, *dev;
	Stack *wst, *rst;
	Uint8 (*dei)(struct Uxn *u, Uint8 addr);
	void (*deo)(struct Uxn *u, Uint8 addr, Uint8 value);
} Uxn;

typedef Uint8 Dei(Uxn *u, Uint8 addr);
typedef void Deo(Uxn *u, Uint8 addr, Uint8 value);

int uxn_halt(Uxn *u, Uint8 instr, Uint8 err, Uint16 addr);
int uxn_boot(Uxn *u, Uint8 *ram, Dei *dei, Deo *deo);
int uxn_eval(Uxn *u, Uint16 pc);