#include <stdio.h>

#include "../uxn.h"
#include "system.h"

/*
Copyright (c) 2022-2025 Devine Lu Linvega, Andrew Alderwick

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE.
*/

char *boot_rom_path;
char *boot_ram_path;

static void
system_print(char *name, Stack *s)
{
	Uint8 i;
	fprintf(stderr, "%s ", name);
	for(i = s->ptr - 8; i != (Uint8)(s->ptr); i++)
		fprintf(stderr, "%02x%c", s->dat[i], i == 0xff ? '|' : ' ');
	fprintf(stderr, "<%02x\n", s->ptr);
}

static int
system_load(Uint8 *rom, Uint8 *ram, char *rom_path, char *ram_path)
{
	FILE *program = fopen(rom_path, "rb");
	if(program) {
		int i = 0, l = fread(rom, 0x10000 - PAGE_PROGRAM, 1, program);
		while(l && ++i < RAM_PAGES)
			l = fread(rom + 0x10000 * i - PAGE_PROGRAM, 0x10000, 1, program);
		fclose(program);
	}
	FILE *raminit = fopen(rom_path, "rb");
	if(raminit) {
		int i = 0, l = fread(ram, 0x10000 - PAGE_PROGRAM, 1, raminit);
		while(l && ++i < RAM_PAGES)
			l = fread(ram + 0x10000 * i - PAGE_PROGRAM, 0x10000, 1, raminit);
		fclose(raminit);
	}
	return !!raminit && !!program;
}

int
system_error(char *msg, const char *err)
{
	fprintf(stderr, "%s: %s\n", msg, err), fflush(stderr);
	return 0;
}

int
system_boot(Uint8 *ram, Uint8 *rom, char *ram_path, char *rom_path,  int has_args)
{
	uxn.ram = ram;
	uxn.rom = rom;
	boot_ram_path = ram_path;
	boot_rom_path = rom_path;
	uxn.dev[0x17] = has_args;
	if(ram && rom && system_load(uxn.rom + PAGE_PROGRAM, uxn.ram + PAGE_PROGRAM, rom_path, ram_path))
		return uxn_eval(PAGE_PROGRAM);

	return 0;
}

int
system_reboot(int soft)
{
	int i;
	for(i = 0x0; i < 0x100; i++) uxn.dev[i] = 0;
	for(i = soft ? 0x100 : 0; i < 0x10000; i++) uxn.ram[i] = 0;
	uxn.wst.ptr = uxn.rst.ptr = 0;
	return system_boot(uxn.ram, uxn.rom, boot_ram_path, boot_rom_path, 0);
}

/* IO */

Uint8
system_dei(Uint8 addr)
{
	switch(addr) {
	case 0x4: return uxn.wst.ptr;
	case 0x5: return uxn.rst.ptr;
	default: return uxn.dev[addr];
	}
}

void
system_deo(Uint8 port)
{
	switch(port) {
	case 0x3: {
		Uint16 value;
		Uint16 addr = PEEK2(uxn.dev + 2);
		Uint8 *aptr = uxn.ram + addr;
		Uint16 length = PEEK2(aptr + 1);
		if(uxn.ram[addr] == 0x0) {
			unsigned int a = PEEK2(aptr + 3) * 0x10000 + PEEK2(aptr + 5);
			unsigned int b = a + length;
			value = uxn.ram[addr + 7];
			for(; a < b; uxn.ram[a++] = value);
		} else if(uxn.ram[addr] == 0x1) {
			unsigned int a = PEEK2(aptr + 3) * 0x10000 + PEEK2(aptr + 5);
			unsigned int b = a + length;
			unsigned int c = PEEK2(aptr + 7) * 0x10000 + PEEK2(aptr + 9);
			for(; a < b; uxn.ram[c++] = uxn.ram[a++]);
		} else if(uxn.ram[addr] == 0x2) {
			unsigned int a = PEEK2(aptr + 3) * 0x10000 + PEEK2(aptr + 5);
			unsigned int b = a + length;
			unsigned int c = PEEK2(aptr + 7) * 0x10000 + PEEK2(aptr + 9);
			unsigned int d = c + length;
			for(; b >= a; uxn.ram[--d] = uxn.ram[--b]);
		} else
			fprintf(stderr, "Unknown Expansion Command 0x%02x\n", uxn.ram[addr]);
		break;
	}
	case 0x4:
		uxn.wst.ptr = uxn.dev[4];
		break;
	case 0x5:
		uxn.rst.ptr = uxn.dev[5];
		break;
	case 0xe:
		system_print("WST", &uxn.wst);
		system_print("RST", &uxn.rst);
		break;
	}
}
