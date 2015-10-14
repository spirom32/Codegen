//
//
// codegen_arith.ñ
// codegen_x86
//
// Created by Andrey Utkin on 15.04.2015
//
// (C) 2015 Andrey Utkin. All rights reserved
//

#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include "memctl.h"


/* ADD reg32, reg32; ADD reg32, imm32; ADD reg32, imm8; ADD r/m32, imm32; ADD r/m32, imm8; ADD reg32, r/m32; ADD r/m32, reg32  */
void _add(int op1, int op2, int disp, int flags, int prefixes)
{
	int td;
	int sib, modrm;

	while (currCode.size-currCode.ip < 15) platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);

	switch (op1) {
	  case eax:
	  case ebx:
	  case ecx:
	  case edx:
	  case esp:
	  case ebp:
	  case esi:
	  case edi:
	    if (flags & FLAGS_REG) {
		switch (op2) {
		  case eax:
		  case ecx:
		  case edx:
		  case ebx:
		  case esp:
		  case ebp:
		  case esi:
		  case edi:
	    	    if (flags & FLAGS_REG2) {
			encodePrefixes(prefixes);
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x01;
			currCode.ip++;	
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = (op1 & ~REG_DWORD) + ((op2 & ~REG_DWORD) << 3)+ 0xC0;
			currCode.ip++;		
			break;
		    }// add reg32, reg32
		  default:
			if (flags & FLAGS_MEM) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x03;
				currCode.ip++;	
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op1 & ~REG_DWORD) << 3)+ 0x05;
				currCode.ip++;			
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				break;
			}// add reg32, [mem32]
			if (flags & FLAGS_SCALE) {
				encodeModrmSibMulti(op1, op2, 0x03, disp, flags | FLAGS_OPCODE1, prefixes);
				break;
			}// add reg32, r/m32
			if ((flags & (FLAGS_IMM16 | FLAGS_IMM32)) && (op1 == eax)){
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x05;
				currCode.ip++;
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				break;
			}// add eax, imm32
			if (flags & FLAGS_IMM8) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x83;
				currCode.ip++;
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (op1 & ~REG_DWORD) + 0xC0;
				currCode.ip++;	
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) op2;
				currCode.ip++;
				break;
			}// add reg32, imm8
			if (flags & (FLAGS_IMM16 | FLAGS_IMM32)) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x81;
				currCode.ip++;
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (op1 & ~REG_DWORD) + 0xC0;
				currCode.ip++;	
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				break;
			}// add reg32, imm32 (excluding eax)
			_ud2();
			break;
		}
		break;
	    }
	  default:
		if (flags & FLAGS_IMM) {
			if (flags & FLAGS_REG2) {
				_ud2();
				break;
			}
			if (flags & FLAGS_MEM) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (flags & FLAGS_IMM8)? 0x83 : 0x81;
				currCode.ip++;	
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x05;
				currCode.ip++;			
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op1;
				currCode.ip += 4;
			} else
				encodeModrmSibMono(op1, (flags & FLAGS_IMM8)? 0x83 : 0x81, 0, disp, flags | FLAGS_OPCODE1, prefixes);
			if (*(WORD *) (currCode.tempBuffer + currCode.ip - 2) != 0x0B0F){
				if (flags & FLAGS_IMM8) {
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) op2;
				currCode.ip++;
				} else {
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				}
			}
			break;
		}// add r/m32, imm32
		if ((flags & FLAGS_MEM) && (flags & FLAGS_REG2)) {
			encodePrefixes(prefixes);
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x01;
			currCode.ip++;	
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op2 & ~REG_DWORD) << 3)+ 0x05;
			currCode.ip++;			
			*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op1;
			currCode.ip += 4;
			break;
		}// add [mem32], reg32
		if (flags & FLAGS_REG2)
			encodeModrmSibMulti(op2, op1, 0x01, disp, flags | FLAGS_OPCODE1, prefixes);// add r/m32, reg32
		else
			_ud2();
		break;
	}
}

/* AND reg32, reg32; AND reg32, imm32; AND reg32, imm8; AND r/m32, imm32; AND r/m32, imm8; AND reg32, r/m32; AND r/m32, reg32  */
void _and(int op1, int op2, int disp, int flags, int prefixes)
{
	int td;
	int sib, modrm;

	while (currCode.size-currCode.ip < 15) platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);

	switch (op1) {
	  case eax:
	  case ebx:
	  case ecx:
	  case edx:
	  case esp:
	  case ebp:
	  case esi:
	  case edi:
	    if (flags & FLAGS_REG) {
		switch (op2) {
		  case eax:
		  case ecx:
		  case edx:
		  case ebx:
		  case esp:
		  case ebp:
		  case esi:
		  case edi:
	    	    if (flags & FLAGS_REG2) {
			encodePrefixes(prefixes);
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x21;
			currCode.ip++;	
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = (op1 & ~REG_DWORD) + ((op2 & ~REG_DWORD) << 3)+ 0xE0;
			currCode.ip++;		
			break;
		    }// and reg32, reg32
		  default:
			if (flags & FLAGS_MEM) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x23;
				currCode.ip++;	
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op1 & ~REG_DWORD) << 3)+ 0x25;
				currCode.ip++;			
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				break;
			}// and reg32, [mem32]
			if (flags & FLAGS_SCALE) {
				encodeModrmSibMulti(op1, op2, 0x23, disp, flags | FLAGS_OPCODE1, prefixes);
				break;
			}// and reg32, r/m32
			if ((flags & (FLAGS_IMM16 | FLAGS_IMM32)) && (op1 == eax)){
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x25;
				currCode.ip++;
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				break;
			}// and eax, imm32
			if (flags & FLAGS_IMM8) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x83;
				currCode.ip++;
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (op1 & ~REG_DWORD) + 0xE0;
				currCode.ip++;	
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) op2;
				currCode.ip++;
				break;
			}// and reg32, imm8
			if (flags & (FLAGS_IMM16 | FLAGS_IMM32)) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x81;
				currCode.ip++;
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (op1 & ~REG_DWORD) + 0xE0;
				currCode.ip++;	
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				break;
			}// and reg32, imm32 (excluding eax)
			_ud2();
			break;
		}
		break;
	    }
	  default:
		if (flags & FLAGS_IMM) {
			if (flags & FLAGS_REG2) {
				_ud2();
				break;
			}
			if (flags & FLAGS_MEM) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (flags & FLAGS_IMM8)? 0x83 : 0x81;
				currCode.ip++;	
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x25;
				currCode.ip++;			
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op1;
				currCode.ip += 4;
			} else
				encodeModrmSibMono(op1, (flags & FLAGS_IMM8)? 0x83 : 0x81, 4, disp, flags | FLAGS_OPCODE1, prefixes);
			if (*(WORD *) (currCode.tempBuffer + currCode.ip - 2) != 0x0B0F){
				if (flags & FLAGS_IMM8) {
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) op2;
				currCode.ip++;
				} else {
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				}
			}
			break;
		}// and r/m32, imm32
		if ((flags & FLAGS_MEM) && (flags & FLAGS_REG2)) {
			encodePrefixes(prefixes);
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x21;
			currCode.ip++;	
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op2 & ~REG_DWORD) << 3)+ 0x25;
			currCode.ip++;			
			*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op1;
			currCode.ip += 4;
			break;
		}// and [mem32], reg32
		if (flags & FLAGS_REG2)
			encodeModrmSibMulti(op2, op1, 0x21, disp, flags | FLAGS_OPCODE1, prefixes);// and r/m32, reg32
		else
			_ud2();
		break;
	}
}

/* CMP reg32, reg32; CMP reg32, imm32; CMP reg32, imm8; CMP r/m32, imm32; CMP r/m32, imm8; CMP reg32, r/m32; CMP r/m32, reg32  */
void _cmp(int op1, int op2, int disp, int flags, int prefixes)
{
	int td;
	int sib, modrm;

	while (currCode.size-currCode.ip < 15) platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);

	switch (op1) {
	  case eax:
	  case ebx:
	  case ecx:
	  case edx:
	  case esp:
	  case ebp:
	  case esi:
	  case edi:
	    if (flags & FLAGS_REG) {
		switch (op2) {
		  case eax:
		  case ecx:
		  case edx:
		  case ebx:
		  case esp:
		  case ebp:
		  case esi:
		  case edi:
	    	    if (flags & FLAGS_REG2) {
			encodePrefixes(prefixes);
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x39;
			currCode.ip++;	
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = (op1 & ~REG_DWORD) + ((op2 & ~REG_DWORD) << 3)+ 0xF8;
			currCode.ip++;		
			break;
		    }// cmp reg32, reg32
		  default:
			if (flags & FLAGS_MEM) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x3B;
				currCode.ip++;	
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op1 & ~REG_DWORD) << 3)+ 0x3D;
				currCode.ip++;			
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				break;
			}// cmp reg32, [mem32]
			if (flags & FLAGS_SCALE) {
				encodeModrmSibMulti(op1, op2, 0x3B, disp, flags | FLAGS_OPCODE1, prefixes);
				break;
			}// cmp reg32, r/m32
			if ((flags & (FLAGS_IMM16 | FLAGS_IMM32)) && (op1 == eax)){
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x3D;
				currCode.ip++;
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				break;
			}// cmp eax, imm32
			if (flags & FLAGS_IMM8) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x83;
				currCode.ip++;
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (op1 & ~REG_DWORD) + 0xF8;
				currCode.ip++;	
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) op2;
				currCode.ip++;
				break;
			}// cmp reg32, imm8
			if (flags & (FLAGS_IMM16 | FLAGS_IMM32)) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x81;
				currCode.ip++;
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (op1 & ~REG_DWORD) + 0xF8;
				currCode.ip++;	
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				break;
			}// cmp reg32, imm32 (excluding eax)
			_ud2();
			break;
		}
		break;
	    }
	  default:
		if (flags & FLAGS_IMM) {
			if (flags & FLAGS_REG2) {
				_ud2();
				break;
			}
			if (flags & FLAGS_MEM) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (flags & FLAGS_IMM8)? 0x83 : 0x81;
				currCode.ip++;	
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x3D;
				currCode.ip++;			
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op1;
				currCode.ip += 4;
			} else
				encodeModrmSibMono(op1, (flags & FLAGS_IMM8)? 0x83 : 0x81, 7, disp, flags | FLAGS_OPCODE1, prefixes);
			if (*(WORD *) (currCode.tempBuffer + currCode.ip - 2) != 0x0B0F){
				if (flags & FLAGS_IMM8) {
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) op2;
				currCode.ip++;
				} else {
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				}
			}
			break;
		}// cmp r/m32, imm32
		if ((flags & FLAGS_MEM) && (flags & FLAGS_REG2)) {
			encodePrefixes(prefixes);
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x39;
			currCode.ip++;	
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op2 & ~REG_DWORD) << 3)+ 0x3D;
			currCode.ip++;			
			*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op1;
			currCode.ip += 4;
			break;
		}// cmp [mem32], reg32
		if (flags & FLAGS_REG2)
			encodeModrmSibMulti(op2, op1, 0x39, disp, flags | FLAGS_OPCODE1, prefixes);// cmp r/m32, reg32
		else
			_ud2();
		break;
	}
}

/* SUB reg32, reg32; SUB reg32, imm32; SUB reg32, imm8; SUB r/m32, imm32; SUB r/m32, imm8; SUB reg32, r/m32; SUB r/m32, reg32  */
void _sub(int op1, int op2, int disp, int flags, int prefixes)
{
	int td;
	int sib, modrm;

	while (currCode.size-currCode.ip < 15) platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);

	switch (op1) {
	  case eax:
	  case ebx:
	  case ecx:
	  case edx:
	  case esp:
	  case ebp:
	  case esi:
	  case edi:
	    if (flags & FLAGS_REG) {
		switch (op2) {
		  case eax:
		  case ecx:
		  case edx:
		  case ebx:
		  case esp:
		  case ebp:
		  case esi:
		  case edi:
	    	    if (flags & FLAGS_REG2) {
			encodePrefixes(prefixes);
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x29;
			currCode.ip++;	
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = (op1 & ~REG_DWORD) + ((op2 & ~REG_DWORD) << 3)+ 0xE8;
			currCode.ip++;		
			break;
		    }// sub reg32, reg32
		  default:
			if (flags & FLAGS_MEM) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x2B;
				currCode.ip++;	
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op1 & ~REG_DWORD) << 3)+ 0x2D;
				currCode.ip++;			
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				break;
			}// sub reg32, [mem32]
			if (flags & FLAGS_SCALE) {
				encodeModrmSibMulti(op1, op2, 0x2B, disp, flags | FLAGS_OPCODE1, prefixes);
				break;
			}// sub reg32, r/m32
			if ((flags & (FLAGS_IMM16 | FLAGS_IMM32)) && (op1 == eax)){
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x2D;
				currCode.ip++;
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				break;
			}// sub eax, imm32
			if (flags & FLAGS_IMM8) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x83;
				currCode.ip++;
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (op1 & ~REG_DWORD) + 0xE8;
				currCode.ip++;	
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) op2;
				currCode.ip++;
				break;
			}// sub reg32, imm8
			if (flags & (FLAGS_IMM16 | FLAGS_IMM32)) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x81;
				currCode.ip++;
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (op1 & ~REG_DWORD) + 0xE8;
				currCode.ip++;	
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				break;
			}// sub reg32, imm32 (excluding eax)
			_ud2();
			break;
		}
		break;
	    }
	  default:
		if (flags & FLAGS_IMM) {
			if (flags & FLAGS_REG2) {
				_ud2();
				break;
			}
			if (flags & FLAGS_MEM) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (flags & FLAGS_IMM8)? 0x83 : 0x81;
				currCode.ip++;	
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x2D;
				currCode.ip++;			
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op1;
				currCode.ip += 4;
			} else
				encodeModrmSibMono(op1, (flags & FLAGS_IMM8)? 0x83 : 0x81, 5, disp, flags | FLAGS_OPCODE1, prefixes);
			if (*(WORD *) (currCode.tempBuffer + currCode.ip - 2) != 0x0B0F){
				if (flags & FLAGS_IMM8) {
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) op2;
				currCode.ip++;
				} else {
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				}
			}
			break;
		}// sub r/m32, imm32
		if ((flags & FLAGS_MEM) && (flags & FLAGS_REG2)) {
			encodePrefixes(prefixes);
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x29;
			currCode.ip++;	
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op2 & ~REG_DWORD) << 3)+ 0x2D;
			currCode.ip++;			
			*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op1;
			currCode.ip += 4;
			break;
		}// sub [mem32], reg32
		if (flags & FLAGS_REG2)
			encodeModrmSibMulti(op2, op1, 0x29, disp, flags | FLAGS_OPCODE1, prefixes);// sub r/m32, reg32
		else
			_ud2();
		break;
	}
}
