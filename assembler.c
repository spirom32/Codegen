//
//
// assembler.c
// codegen_x86
//
// Created by Andrey Utkin on 10.04.2015
//
// (C) 2015 Andrey Utkin. All rights reserved
//

#include <windows.h>
#include "codegen.h"

int isRegister(int op)
{
	if ((BYTE)(op & ~REG_TYPED) > 7) return 0;
	return 1;
}

int isPrefix(int op)
{
	switch (op) {
	  case 0xF0:
	  case 0xF2:
	  case 0xF3:
	  case 0x2E:
	  case 0x3E:
	  case 0x66:
	  case 0x67:
	  case 0x26:
	  case 0x36:
	  case 0x64:
	  case 0x65:
		return 1;
		break;
	  default:
		return 0;
		break;
	}
}

void encodePrefixes(int prefixes)
{
	if (prefixes & PREFIX_LOCK) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xF0;
		currCode.ip++;
	} else if (prefixes & PREFIX_REP) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xF3;
		currCode.ip++;
	} else if (prefixes & PREFIX_REPN) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xF2;
		currCode.ip++;
	}

	if (prefixes & PREFIX_BRUNCH_TAKEN) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x3E;
		currCode.ip++;
	} else if (prefixes & PREFIX_BRUNCH_NTAKEN) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x2E;
		currCode.ip++;
	} else if (prefixes & PREFIX_CS_OVERRIDE) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x2E;
		currCode.ip++;
	} else if (prefixes & PREFIX_DS_OVERRIDE) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x3E;
		currCode.ip++;
	} else if (prefixes & PREFIX_SS_OVERRIDE) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x36;
		currCode.ip++;
	} else if (prefixes & PREFIX_ES_OVERRIDE) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x26;
		currCode.ip++;
	} else if (prefixes & PREFIX_FS_OVERRIDE) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x64;
		currCode.ip++;
	} else if (prefixes & PREFIX_GS_OVERRIDE) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x65;
		currCode.ip++;
	}

	if (prefixes & PREFIX_OPSIZE_OVERRIDE) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x66;
		currCode.ip++;
	}

	if (prefixes & PREFIX_ADSIZE_OVERRIDE) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x67;
		currCode.ip++;
	}
}

void encodeModrmSibMono(int op, DWORD opcode, int digit, int disp, int flags, int prefixes)
{	
	BYTE reg = digit << 3;
	BYTE mod, sib;
	int td;
	BYTE opsize;

	if (flags & FLAGS_DISP8) mod = 0x40;
	else if (flags & (FLAGS_DISP32 | FLAGS_DISP16)) mod = 0x80;
	else mod = 0x00;

	encodePrefixes(prefixes);

	if (flags & FLAGS_OPCODE1) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) opcode;
		currCode.ip++;
		opsize = 1;
	} else if (flags & FLAGS_OPCODE2) {
		*(WORD *) (currCode.tempBuffer + currCode.ip) = (WORD) opcode;
		currCode.ip += 2;
		opsize = 2;
	} else if (flags & FLAGS_OPCODE3) {
		*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) (opcode & 0x00FFFFFF);
		currCode.ip += 3;
		opsize = 3;
	} else if (flags & FLAGS_OPCODE4) {
		*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) opcode;
		currCode.ip += 4;
		opsize = 4;
	} else {
		_ud2();
		return;
	}

	if (flags & FLAGS_SCALE1) {
			switch (op) {
	  		  case eax:
	  		  case ebx:
	  		  case ecx:
	  		  case edx:
	  		  case esi:
	  		  case edi:
			  case ebp:
				if (flags & FLAGS_DISP8) {			
					*(BYTE *) (currCode.tempBuffer + currCode.ip) = (flags & FLAGS_BASE_EBP)? 0x04 + mod + reg : mod + reg + (op & ~REG_DWORD);
					currCode.ip++;	
					if (flags & FLAGS_BASE_EBP) {
						* (BYTE *) (currCode.tempBuffer + currCode.ip) = 0x28 + (op & ~REG_DWORD);
					currCode.ip++;
					}
					*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) disp;
					currCode.ip++;	
				} else if (flags & FLAGS_DISP32) {	
					*(BYTE *) (currCode.tempBuffer + currCode.ip) = (flags & FLAGS_BASE_EBP)? 0x04 + mod + reg : mod + reg + (op & ~REG_DWORD);
					currCode.ip++;	
					if (flags & FLAGS_BASE_EBP) {
						* (BYTE *) (currCode.tempBuffer + currCode.ip) = 0x28 + (op & ~REG_DWORD);
					currCode.ip++;
					}
					*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) disp;
					currCode.ip += 4;
				} else {
					if (op == ebp) {	
						if (! (flags & FLAGS_BASE_EBP))	 {		
							*(WORD *) (currCode.tempBuffer + currCode.ip) = 0x0045 + reg;
							currCode.ip += 2;
						} else {
							*(DWORD *) (currCode.tempBuffer + currCode.ip) = 0x00002D44 + reg;
							currCode.ip += 3;
						}
					} else {
						*(BYTE *) (currCode.tempBuffer + currCode.ip) = (flags & FLAGS_BASE_EBP)? 0x04 + mod + reg: mod + reg + (op & ~REG_DWORD);
						currCode.ip++;	
						if (flags & FLAGS_BASE_EBP) {
							* (BYTE *) (currCode.tempBuffer + currCode.ip) = 0x28 + (op & ~REG_DWORD);
							currCode.ip++;
						}					
					}
				}
				break;
			  case esp:
				if (flags & FLAGS_DISP8) {			
					*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x04 + mod + reg;
					currCode.ip++;	
					*(BYTE *) (currCode.tempBuffer + currCode.ip) = (flags & FLAGS_BASE_EBP)? 0x2C : 0x24;
					currCode.ip++;
					*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) disp;
					currCode.ip++;
				} else if (flags & FLAGS_DISP32) {	
					*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x04 + reg + mod;
					currCode.ip++;	
					*(BYTE *) (currCode.tempBuffer + currCode.ip) = (flags & FLAGS_BASE_EBP)? 0x2C : 0x24;
					currCode.ip++;
					*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) disp;
					currCode.ip += 4;
				} else {			
					*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x04 + mod +reg;
					currCode.ip++;	
					*(BYTE *) (currCode.tempBuffer + currCode.ip) = (flags & FLAGS_BASE_EBP)? 0x2C : 0x24;
					currCode.ip++;
				}
				break;
			}
			return;
		}
		if (flags & (FLAGS_SCALE2 | FLAGS_SCALE4 | FLAGS_SCALE8)) {
			if (flags & FLAGS_SCALE2) sib = 0x45;
			else if (flags & FLAGS_SCALE4) sib = 0x85;
			else if (flags & FLAGS_SCALE8) sib = 0xC5;
			switch (op) {
	  		  case eax:
	  		  case ebx:
	  		  case ecx:
	  		  case edx:
	  		  case esi:
	  		  case edi:
			  case ebp:
				if (flags & FLAGS_BASE_EBP) {
					if (flags & FLAGS_DISP8) {
						*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x04 + mod + reg;
						currCode.ip++;	
						*(BYTE *) (currCode.tempBuffer + currCode.ip) = sib + (op & ~REG_DWORD)*8;
						currCode.ip++;
						*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) disp;
						currCode.ip++;	
					} else if (flags & FLAGS_DISP32) {
						*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x04 + mod + reg;
						currCode.ip++;	
						*(BYTE *) (currCode.tempBuffer + currCode.ip) = sib + (op & ~REG_DWORD)*8;
						currCode.ip++;
						*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) disp;
						currCode.ip += 4;
					} else {
						*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x44 + mod + reg;
						currCode.ip++;	
						*(BYTE *) (currCode.tempBuffer + currCode.ip) = sib + (op & ~REG_DWORD)*8;
						currCode.ip++;
						*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x00;
						currCode.ip++;	
					}
				} else {
					if (!(flags & (FLAGS_DISP8 | FLAGS_DISP32))) td = 0x00; else td = disp;			
					*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x04 + reg;
					currCode.ip++;	
					*(BYTE *) (currCode.tempBuffer + currCode.ip) = sib + (op & ~REG_DWORD)*8;
					currCode.ip++;
					*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) td;
					currCode.ip += 4;			
				}
				break;
			  default:
				currCode.ip -= opsize;
				_ud2();
				break;
			}
			return;
		}
	currCode.ip -= opsize;
	_ud2();
}

void encodeModrmSibMulti(int op1, int op2, DWORD opcode, int disp, int flags, int prefixes)
{
	BYTE modrm, sib;
	BYTE opsize;

	if (!isRegister(op1) || !isRegister(op2)) {
		_ud2();
		return;
	}

	encodePrefixes(prefixes);

	if (flags & FLAGS_OPCODE1) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) opcode;
		currCode.ip++;
		opsize = 1;
	} else if (flags & FLAGS_OPCODE2) {
		*(WORD *) (currCode.tempBuffer + currCode.ip) = (WORD) opcode;
		currCode.ip += 2;
		opsize = 2;
	} else if (flags & FLAGS_OPCODE3) {
		*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) (opcode & 0x00FFFFFF);
		currCode.ip += 3;
		opsize = 3;
	} else if (flags & FLAGS_OPCODE4) {
		*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) opcode;
		currCode.ip += 4;
		opsize = 4;
	} else {
		_ud2();
		return;
	}

	if (flags & FLAGS_SCALE1) {
		if (!(flags & FLAGS_BASE_EBP)){
			if ((flags & FLAGS_DISP8) || (op2 == ebp)) modrm = 0x40;
			else if (flags & FLAGS_DISP32) modrm = 0x80;
			else modrm = 0x00;
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op1 & ~REG_DWORD) << 3)+ modrm + (op2 & ~REG_DWORD);
			currCode.ip++;	
			if (op2 == esp) {
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x24;
				currCode.ip++;
			}
			if (flags & FLAGS_DISP8) {
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) disp;
				currCode.ip++;
			} else if (flags & FLAGS_DISP32) {
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) disp;
				currCode.ip += 4;
			} else if (op2 == ebp) {
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x00;
				currCode.ip++;	
			}
		} else {
			if ((flags & FLAGS_DISP8) || (op2 == ebp)) modrm = 0x40;
			else if (flags & FLAGS_DISP32) modrm = 0x80;
			else modrm = 0x00;	
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op1 & ~REG_DWORD) << 3)+ modrm + 0x04;
			currCode.ip++;	
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x28 + (op2 & ~REG_DWORD);
			currCode.ip++;
			if (flags & FLAGS_DISP8) {
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) disp;
				currCode.ip++;
			} else if ((op2 == ebp) && (!(flags & FLAGS_DISP8))) {
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x00;
				currCode.ip++;
			} else if (flags & FLAGS_DISP32) {
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) disp;
				currCode.ip += 4;
			}
		}
		return;
	}
	if ((flags & (FLAGS_SCALE2 | FLAGS_SCALE4 | FLAGS_SCALE8)) && (op2 != esp)) {
		if (flags & FLAGS_SCALE2) sib = 0x40;
		else if (flags & FLAGS_SCALE4) sib = 0x80;
		else sib = 0xC0;
		if (!(flags & FLAGS_BASE_EBP)) {
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op1 & ~REG_DWORD) << 3)+ 0x04;
			currCode.ip++;
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op2 & ~REG_DWORD) << 3) + 0x05 + sib;
			currCode.ip++;
			if (!(flags & (FLAGS_DISP8 | FLAGS_DISP32)))
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = 0x00;
			else
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) disp;
			currCode.ip += 4;
		} else {
			if (flags & FLAGS_DISP32) modrm = 0x84;
			else modrm = 0x44;	
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op1 & ~REG_DWORD) << 3)+ modrm;
			currCode.ip++;
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op2 & ~REG_DWORD) << 3) + 0x05 + sib;
			currCode.ip++;
			if (!(flags & (FLAGS_DISP8 | FLAGS_DISP32))){
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x00;
				currCode.ip++;
			} else if (flags & FLAGS_DISP8) {
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) disp;
				currCode.ip++;
			} else if (flags & FLAGS_DISP32) {
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) disp;
				currCode.ip += 4;
			}
		}
		return;
	}
	currCode.ip -= opsize;
	_ud2();
}