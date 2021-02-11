#include <stdio.h>

/*
Copyright (u) 2021 Devine Lu Linvega

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE.
*/

#include "uxn.h"

#pragma mark - Operations

/* clang-format off */

void   setflag(Uint8 *st, char flag, int b) { if(b) *st |= flag; else *st &= (~flag); }
int    getflag(Uint8 *st, char flag) { return *st & flag; }
Uint8  mempeek8(Uxn *u, Uint16 s) { return u->ram.dat[s]; }
Uint16 mempeek16(Uxn *u, Uint16 s) { return (u->ram.dat[s] << 8) + (u->ram.dat[s + 1] & 0xff); }

void   wspush8(Uxn *u, Uint8 b) { u->wst.dat[u->wst.ptr++] = b; }
Uint8  wspop8(Uxn *u) { return u->wst.dat[--u->wst.ptr]; }
Uint8  wspeek8(Uxn *u, Uint8 o) { return u->wst.dat[u->wst.ptr - o]; }
void   wspush16(Uxn *u, Uint16 s) { wspush8(u,s >> 8); wspush8(u,s & 0xff); }
Uint16 wspop16(Uxn *u) { return wspop8(u) + (wspop8(u) << 8); }
Uint16 wspeek16(Uxn *u, Uint8 o) { return (u->wst.dat[u->wst.ptr - o] << 8) + u->wst.dat[u->wst.ptr - o + 1]; }


Uint8 rspop8(Uxn *u){
	return u->rst.dat[--u->rst.ptr];
}

void   rspush16(Uxn *u, Uint16 a) { 


	u->rst.dat[u->rst.ptr++] = (a >> 8) & 0xff;  
	u->rst.dat[u->rst.ptr++] = a & 0xff;  

}
Uint16 rspop16(Uxn *u) { 
	return rspop8(u) + (rspop8(u) << 8); 
}


/* I/O */
void op_brk(Uxn *u) { setflag(&u->status,FLAG_HALT, 1); }
void op_li1(Uxn *u) { u->literal += 1; }
void op_lix(Uxn *u) { u->literal += u->ram.dat[u->ram.ptr++]; }
void op_nop(Uxn *u) { printf("NOP"); (void)u; }
void op_ior(Uxn *u) { Device *dev = &u->dev[mempeek8(u, u->devr)]; if(dev) wspush8(u, dev->read(dev, wspop8(u))); }
void op_iow(Uxn *u) { Uint8 a = wspop8(u); Device *dev = &u->dev[mempeek8(u, u->devw)]; if(dev) dev->write(dev, a); }
void op_ldr(Uxn *u) { Uint16 a = wspop16(u); wspush8(u, u->ram.dat[a]); }
void op_str(Uxn *u) { Uint16 a = wspop16(u); Uint8 b = wspop8(u); u->ram.dat[a] = b; }
/* Logic */
void op_jmp(Uxn *u) { u->ram.ptr = wspop16(u); }
void op_jsr(Uxn *u) { rspush16(u, u->ram.ptr); u->ram.ptr = wspop16(u); }
void op_rts(Uxn *u) { u->ram.ptr = rspop16(u); }
/* Stack */
void op_pop(Uxn *u) { wspop8(u); }
void op_dup(Uxn *u) { wspush8(u, wspeek8(u, 1)); }
void op_swp(Uxn *u) { Uint8 b = wspop8(u), a = wspop8(u); wspush8(u, b); wspush8(u, a); }
void op_ovr(Uxn *u) { Uint8 a = wspeek8(u,2); wspush8(u, a); }
void op_rot(Uxn *u) { Uint8 c = wspop8(u), b = wspop8(u), a = wspop8(u); wspush8(u, b); wspush8(u, c); wspush8(u, a); }
void op_and(Uxn *u) { Uint8 a = wspop8(u), b = wspop8(u); wspush8(u, b & a); }
void op_ora(Uxn *u) { Uint8 a = wspop8(u), b = wspop8(u); wspush8(u, b | a); }
void op_rol(Uxn *u) { Uint8 a = wspop8(u), b = wspop8(u); wspush8(u, b << a); }
/* Arithmetic */
void op_add(Uxn *u) { Uint8 a = wspop8(u), b = wspop8(u); wspush8(u, b + a); }
void op_sub(Uxn *u) { Uint8 a = wspop8(u), b = wspop8(u); wspush8(u, b - a); }
void op_mul(Uxn *u) { Uint8 a = wspop8(u), b = wspop8(u); wspush8(u, b * a); }
void op_div(Uxn *u) { Uint8 a = wspop8(u), b = wspop8(u); wspush8(u, b / a); }
void op_equ(Uxn *u) { Uint8 a = wspop8(u), b = wspop8(u); wspush8(u, b == a); }
void op_neq(Uxn *u) { Uint8 a = wspop8(u), b = wspop8(u); wspush8(u, b != a); }
void op_gth(Uxn *u) { Uint8 a = wspop8(u), b = wspop8(u); wspush8(u, b > a); }
void op_lth(Uxn *u) { Uint8 a = wspop8(u), b = wspop8(u); wspush8(u, b < a); }
/* --- */
void op_ior16(Uxn *u) { Uint8 a = wspop8(u); Device *dev = &u->dev[mempeek8(u, u->devr)]; if(dev) wspush16(u, (dev->read(dev, a) << 8) + dev->read(dev, a + 1)); }
void op_iow16(Uxn *u) { Uint8 a = wspop8(u); Uint8 b = wspop8(u); Device *dev = &u->dev[mempeek8(u, u->devw)]; if(dev) { dev->write(dev, b); dev->write(dev, a); } }
void op_ldr16(Uxn *u) { Uint16 a = wspop16(u); wspush16(u, (u->ram.dat[a] << 8) + u->ram.dat[a + 1]); }
void op_str16(Uxn *u) { Uint16 a = wspop16(u); Uint16 b = wspop16(u); u->ram.dat[a] = b >> 8; u->ram.dat[a + 1] = b & 0xff; }
/* Stack(16-bits) */
void op_pop16(Uxn *u) { wspop16(u); }
void op_dup16(Uxn *u) { wspush16(u, wspeek16(u, 2)); }
void op_swp16(Uxn *u) { Uint16 b = wspop16(u), a = wspop16(u); wspush16(u, b); wspush16(u, a); }
void op_ovr16(Uxn *u) { Uint16 a = wspeek16(u, 4); wspush16(u, a); }
void op_rot16(Uxn *u) { Uint16 c = wspop16(u), b = wspop16(u), a = wspop16(u); wspush16(u, b); wspush16(u, c); wspush16(u, a); }
void op_and16(Uxn *u) { Uint16 a = wspop16(u), b = wspop16(u); wspush16(u, b & a); }
void op_ora16(Uxn *u) { Uint16 a = wspop16(u), b = wspop16(u); wspush16(u, b | a); }
void op_rol16(Uxn *u) { Uint16 a = wspop16(u), b = wspop16(u); wspush16(u, b << a); }
/* Arithmetic(16-bits) */
void op_add16(Uxn *u) { Uint16 a = wspop16(u), b = wspop16(u); wspush16(u, b + a); }
void op_sub16(Uxn *u) { Uint16 a = wspop16(u), b = wspop16(u); wspush16(u, b - a); }
void op_mul16(Uxn *u) { Uint16 a = wspop16(u), b = wspop16(u); wspush16(u, b * a); }
void op_div16(Uxn *u) { Uint16 a = wspop16(u), b = wspop16(u); wspush16(u, b / a); }
void op_equ16(Uxn *u) { Uint16 a = wspop16(u), b = wspop16(u); wspush8(u, b == a); }
void op_neq16(Uxn *u) { Uint16 a = wspop16(u), b = wspop16(u); wspush8(u, b != a); }
void op_gth16(Uxn *u) { Uint16 a = wspop16(u), b = wspop16(u); wspush8(u, b > a); }
void op_lth16(Uxn *u) { Uint16 a = wspop16(u), b = wspop16(u); wspush8(u, b < a); }

void (*ops[])(Uxn *u) = {
	op_brk, op_nop, op_li1, op_lix, op_ior, op_iow, op_ldr, op_str, 
	op_jmp, op_jsr, op_nop, op_rts, op_nop, op_nop, op_nop, op_nop, 
	op_pop, op_dup, op_swp, op_ovr, op_rot, op_and, op_ora, op_rol,
	op_add, op_sub, op_mul, op_div, op_equ, op_neq, op_gth, op_lth,
	/* 16-bit */
	op_brk, op_nop, op_li1, op_lix, op_ior16, op_iow16, op_ldr16, op_str16, 
	op_jmp, op_jsr, op_nop, op_rts, op_nop, op_nop, op_nop, op_nop, 
	op_pop16, op_dup16, op_swp16, op_ovr16, op_rot16, op_and16, op_ora16, op_rol16,
	op_add16, op_sub16, op_mul16, op_div16, op_equ16, op_neq16, op_gth16, op_lth16
};

Uint8 opr[][2] = { 
	{0,0}, {0,0}, {0,0}, {0,0}, {1,1}, {1,0}, {2,1}, {3,0},
	{2,0}, {2,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
	{1,0}, {1,2}, {2,2}, {3,3}, {3,3}, {2,1}, {2,1}, {2,1},
	{2,1}, {2,1}, {2,1}, {2,1}, {2,1}, {2,1}, {2,1}, {2,1},
	/* 16-bit */
	{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, /* TODO */
	{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, /* TODO */
	{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, /* TODO */
	{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}  /* TODO */
};

/* clang-format on */

int
haltuxn(Uxn *u, char *name, int id)
{
	printf("Error: %s#%04x, at 0x%04x\n", name, id, u->counter);
	return 0;
}

int
lituxn(Uxn *u, Uint8 instr)
{
	if(u->wst.ptr >= 255)
		return haltuxn(u, "Stack overflow", instr);
	wspush8(u, instr);
	u->literal--;
	return 1;
}

int
opcuxn(Uxn *u, Uint8 instr)
{
	Uint8 op = instr & 0x1f;
	setflag(&u->status, FLAG_SHORT, (instr >> 5) & 1);
	setflag(&u->status, FLAG_SIGN, (instr >> 6) & 1); /* usused */
	setflag(&u->status, FLAG_COND, (instr >> 7) & 1);
	if(getflag(&u->status, FLAG_SHORT))
		op += 32;
	if(u->wst.ptr < opr[op][0])
		return haltuxn(u, "Stack underflow", op);
	if(u->wst.ptr + opr[op][1] - opr[op][0] >= 255)
		return haltuxn(u, "Stack overflow", instr);
	if(!getflag(&u->status, FLAG_COND) || (getflag(&u->status, FLAG_COND) && wspop8(u)))
		(*ops[op])(u);
	return 1;
}

int
stepuxn(Uxn *u, Uint8 instr)
{
	if(u->literal > 0)
		return lituxn(u, instr);
	else
		return opcuxn(u, instr);
}

int
evaluxn(Uxn *u, Uint16 vec)
{
	u->literal = 0;
	u->ram.ptr = vec;
	setflag(&u->status, FLAG_HALT, 0);
	while(!(u->status & FLAG_HALT)) {
		Uint8 instr = u->ram.dat[u->ram.ptr++];
		if(!stepuxn(u, instr))
			return 0;
		u->counter++;
	}
	return 1;
}

int
bootuxn(Uxn *u)
{
	size_t i;
	char *cptr = (char *)&u;
	for(i = 0; i < sizeof u; i++)
		cptr[i] = 0;
	return 1;
}

int
loaduxn(Uxn *u, char *filepath)
{
	FILE *f;
	if(!(f = fopen(filepath, "rb")))
		return haltuxn(u, "Missing input.", 0);
	fread(u->ram.dat, sizeof(u->ram.dat), 1, f);
	u->devr = 0xfff8;
	u->devw = 0xfff9;
	u->vreset = mempeek16(u, 0xfffa);
	u->vframe = mempeek16(u, 0xfffc);
	u->verror = mempeek16(u, 0xfffe);
	printf("Uxn loaded[%s] vrst:%04x vfrm:%04x verr:%04x.\n",
		filepath,
		u->vreset,
		u->vframe,
		u->verror);
	return 1;
}

/* to start: evaluxn(u, u->vreset); */

Device *
portuxn(Uxn *u, char *name, Uint8 (*rfn)(Device *, Uint8), Uint8 (*wfn)(Device *, Uint8))
{
	Device *d = &u->dev[u->devices++];
	d->read = rfn;
	d->write = wfn;
	d->len = 0;
	printf("Device #%d: %s \n", u->devices - 1, name);
	return d;
}
