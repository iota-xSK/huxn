#ifndef UXN_UXN_H
#define UXN_UXN_H

/*
Copyright (c) 2021 Devine Lu Linvega

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE.
*/

typedef unsigned char Uint8;
typedef signed char Sint8;
typedef unsigned short Uint16;
typedef signed short Sint16;
typedef unsigned int Uint32;

#define PAGE_PROGRAM 0x0100

#define DEVPEEK16(o, x) \
	{ \
		(o) = (d->dat[(x)] << 8) + d->dat[(x) + 1]; \
	}
#define DEVPOKE16(x, y) \
	{ \
		d->dat[(x)] = (y) >> 8; \
		d->dat[(x) + 1] = (y); \
	}
#define GETVECTOR(d) ((d)->dat[0] << 8 | (d)->dat[1])

typedef struct Device {
	struct Uxn *u;
	Uint8 dat[16];
	Uint8 (*dei)(struct Device *d, Uint8);
	void (*deo)(struct Device *d, Uint8);
} Device;

/* clang-format off */

#define GETVEC(d) ((d)[0] << 8 | (d)[1])
#define POKDEV(x, y) { d[(x)] = (y) >> 8; d[(x) + 1] = (y); }
#define PEKDEV(o, x) { (o) = (d[(x)] << 8) + d[(x) + 1]; }

/* clang-format on */

typedef struct {
	Uint8 dat[254], err, ptr;
} Stack;

typedef struct Uxn {
	Uint8 *ram, *dev;
	Stack *wst, *rst;
	Uint8 (*dei)(struct Uxn *u, Uint8 addr);
	void (*deo)(struct Uxn *u, Uint8 addr, Uint8 value);
} Uxn;

typedef Uint8 Dei(Uxn *u, Uint8 addr);
typedef void Deo(Uxn *u, Uint8 addr, Uint8 value);

int uxn_boot(Uxn *u, Uint8 *ram, Dei *dei, Deo *deo);
int uxn_eval(Uxn *u, Uint16 pc);
int uxn_halt(Uxn *u, Uint8 instr, Uint8 err, Uint16 addr);

/* TODO: remove */

Device *uxn_port(Uxn *u, Uint8 id, Uint8 (*deifn)(Device *, Uint8), void (*deofn)(Device *, Uint8));
#endif /* UXN_UXN_H */
