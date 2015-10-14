//
//
// codegen.ñ
// codegen_x86
//
// Created by Andrey Utkin on 07.04.2015
//
// (C) 2015 Andrey Utkin. All rights reserved
//

#include "codegen.h"
//#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "memctl.h"

codeDesc currCode;

void platformPrintTempBuffer()
{
	int i;

	printf("Instruction pointer: %04X\n", currCode.ip);
	printf("Current temporary buffer contents: ");
	for (i = 0; i < currCode.ip; i++) printf("%02X ", (BYTE)currCode.tempBuffer[i]);
	printf("\nDone.\n");
}

void platformInitCodeDesc(codeDesc * cd, int flags)
{
	char *p;
	int sz;
	DWORD prt;

	if (cd == NULL) {
		cd = (codeDesc *) malloc(sizeof(codeDesc));
		cd->size = 0;
		cd->ip = 0;
		cd->tempBuffer = NULL;
		cd->Code = NULL;
	}
	if ((cd->size == 0) || (flags & F_RESET)){
		if (flags & F_RESET) free(cd->tempBuffer);
		cd->tempBuffer = (char *) malloc(MINIMAL_CODE_PAGE);
		cd->size = MINIMAL_CODE_PAGE;
		cd->ip = 0;
		cd->Code = NULL;
		return;
	}
	if (flags & F_ALLOC) sz = MINIMAL_CODE_PAGE;
		else if (flags & F_REALLOC) sz = cd->size * 2;
			else return; 
	if (flags & F_TEMPBUF){
		p = (char *) malloc(sz);
		if (flags & F_REALLOC) { 
			memcpy(p, cd->tempBuffer, cd->ip);
			free(cd->tempBuffer);
			cd->tempBuffer = p;
		} else {
			cd->tempBuffer = p;
			cd->ip = 0;
		}
		cd->size = sz;
	} else if (flags & F_VIRT) {
		//p = (char *) VirtualAlloc(NULL, sz, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		//VirtualLock(p, sz);
		//VirtualProtect(p, sz, PAGE_EXECUTE_READWRITE, &prt);
		p = (char *) memVirtualAlloc(sz);
		memVirtualLock(p, sz);
		memVirtualProtectNormal(p, sz);
		if (flags & F_REALLOC) { 
			memcpy(p, cd->Code, cd->ip);
			cd->Code = p;
			//VirtualUnlock(cd->Code, sz / 2);
			//VirtualFree(cd->Code, sz / 2, MEM_RELEASE);
			memVirtualUnlock(cd->Code, sz / 2);
			memVirtualFree(cd->Code, sz / 2);
		} else {
			cd->Code = p;
			cd->ip = 0;
		}
		cd->size = sz;
	}
}

void *platformGrantExecRegion(int size, int *actSize)
{
	int sz = MINIMAL_CODE_PAGE;
	void *ptr;

	while (sz < size) sz += MINIMAL_CODE_PAGE;
	ptr = memVirtualAlloc(sz);
	if (ptr == NULL) return NULL;
	//printf("Allocated ptr = %08X size = %04X\n", ptr, sz);
	if (memVirtualLock(ptr, sz)) {
		//printf("Unable to lock\n");
		memVirtualFree(ptr, sz);
		return NULL;
	}
	if (memVirtualProtectExec(ptr, sz)) {
		//printf("Unable to protect: %s\n", strerror(errno));
		memVirtualUnlock(ptr, sz);
		memVirtualFree(ptr, sz);
		return NULL;
	}
	*actSize = sz;
	return ptr;
}

int platformReleaseExecRegion(void *addr, int size)
{
	if (addr == NULL) return -1;
	return memVirtualProtectNormal(addr, size);
}

int platformAcquireExecRegion(void *addr, int size)
{
	if (addr == NULL) return -1;
	return memVirtualProtectExec(addr, size);
}

void platformFreeExecRegion(void *addr, int size)
{
	if (addr == NULL) return;
	memVirtualProtectNormal(addr, size);
	memVirtualUnlock(addr, size);
	memVirtualFree(addr, size);
}

void platformInitCodeRegion(void *addr, int size)
{
	if (addr == NULL) return;
	if (platformReleaseExecRegion(addr, size)) return;
	if ((currCode.ip != 0) && (currCode.tempBuffer != NULL) && (size >= currCode.ip)) {
		memcpy(addr, currCode.tempBuffer, currCode.ip);
	}
	platformAcquireExecRegion(addr, size);
}

/*int platformCallGeneratedCode(void *Code, int parNum, ...)
{
	va_list vl;
	int res;
	int Par1, Par2, Par3, Par4;

	va_start(vl, parNum);

	if (parNum == 0) {
		int (*func)() = (int (*)())Code;
		res = func();
	} else if (parNum == 1) {
		int (*func)(int) = (int (*)(int))Code;
		Par1 = va_arg(vl, int);
		res = func(Par1);
	} if (parNum == 2) {
		int (*func)(int, int) = (int (*)(int, int))Code;
		Par1 = va_arg(vl, int);
		Par2 = va_arg(vl, int);
		res = func(Par1, Par2);
	} if (parNum == 3) {
		int (*func)(int, int, int) = (int (*)(int, int, int))Code;
		Par1 = va_arg(vl, int);
		Par2 = va_arg(vl, int);
		Par3 = va_arg(vl, int);
		//printf("Parameters extracted: Par1 = %d Par2 = %d Par3 = %d\n", Par1, Par2, Par3);
		res = func(Par1, Par2, Par3);
	} if (parNum == 4) {
		int (*func)(int, int, int, int) = (int (*)(int, int, int, int))Code;
		Par1 = va_arg(vl, int);
		Par2 = va_arg(vl, int);
		Par3 = va_arg(vl, int);
		Par4 = va_arg(vl, int);
		res = func(Par1, Par2, Par3, Par4);
	}

	va_end(vl);
	return res;
}*/

void platformPrologue(int buffSize, int flags)
{
	_add(ebp, 0x04, 0, FLAGS_REG | FLAGS_IMM8, 0);
	if (flags & PROLOGUE_LOCAL_BUFFER) {
		_push(esi, 0, FLAGS_REG, 0);// push esi
		int bs = buffSize;
		while (bs % 4 != 0) bs++;
		_mov(esi, esp, 0, FLAGS_REG | FLAGS_REG2, 0);// mov esi, esp
		_sub(esp, bs, 0, FLAGS_REG | FLAGS_IMM32, 0);// sub esp, bs
	}
	if (flags & PROLOGUE_EBX) _push(ebx, 0, FLAGS_REG, 0);// push ebx
	if (flags & PROLOGUE_EDI) _push(edi, 0, FLAGS_REG, 0);// push edi
}

void platformEpilogue(int flags)
{
	_sub(ebp, 0x04, 0, FLAGS_REG | FLAGS_IMM8, 0);
	if (flags & PROLOGUE_EDI) _pop(edi, 0, FLAGS_REG, 0);// pop edi
	if (flags & PROLOGUE_EBX) _pop(ebx, 0, FLAGS_REG, 0);// pop ebx
	if (flags & PROLOGUE_LOCAL_BUFFER) {
		_mov(esp, esi, 0, FLAGS_REG | FLAGS_REG2, 0);// mov esp, esi
		_pop(esi, 0, FLAGS_REG, 0);// pop esi
	}
}

void platformCorrectStack(int disp)
{
	_add(esp, disp, 0, FLAGS_REG | FLAGS_IMM32, 0);
}

int platformCallGeneratedCode(void *Code, ...)
{
	int res;

	int (*func)() = (int (*)())Code;
	res = func();

	return res;
}

/* NOP */
void _nop()
{
	while (currCode.size-currCode.ip < 1) 		platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);
	*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x90;
	currCode.ip++;
}

/* undefined opcode, UD2 */
void _ud2()
{
	while (currCode.size-currCode.ip < 2) 		platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);
	while (isPrefix(*(BYTE *) (currCode.tempBuffer + currCode.ip-1))) currCode.ip--;
	*(WORD *) (currCode.tempBuffer + currCode.ip) = 0x0B0F;
	currCode.ip += 2;
}

/* RET and RETF */
void _ret(int rt)
{
	while (currCode.size-currCode.ip < 1) 		platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);
	*(BYTE *) (currCode.tempBuffer + currCode.ip) = (rt & RET_FAR)? 0xCB : 0xC3;
	currCode.ip++;
}

/* RETN imm16, far and near */
void _retn(int op, int rt)
{
	while (currCode.size-currCode.ip < 1) 		platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);
	*(BYTE *) (currCode.tempBuffer + currCode.ip) = (rt & RET_FAR)? 0xCA : 0xC2;
	currCode.ip++;
	*(WORD *) (currCode.tempBuffer + currCode.ip) = (WORD) op;
	currCode.ip += 2;
}

/* EFlags management */

/* clc: Clear Carry Flag */
void _clc()
{
	while (currCode.size-currCode.ip < 1) 		platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);
	*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xF8;
	currCode.ip++;
}

/* cld: Clear Direction Flag */
void _cld()
{
	while (currCode.size-currCode.ip < 1) 		platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);
	*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xFC;
	currCode.ip++;
}

/* stc: Set Carry Flag */
void _stc()
{
	while (currCode.size-currCode.ip < 1) 		platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);
	*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xF9;
	currCode.ip++;
}

/* std: Set Direction Flag */
void _std()
{
	while (currCode.size-currCode.ip < 1) 		platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);
	*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xFD;
	currCode.ip++;
}

/* PUSH reg32, PUSH reg16, PUSH Seg, PUSH r/m32, PUSH imm8, PUSH imm32 */
void _push(int op, int disp, int flags, int prefixes)
{
	int td;
	int sib;

	while (currCode.size-currCode.ip < 7) 			platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);
	switch (op) {
	  case eax:
	  case ebx:
	  case ecx:
	  case edx:
	  case esi:
	  case edi:
	  case esp:
	  case ebp:
	    if (flags & FLAGS_REG) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x50 + (op & ~REG_DWORD);
		currCode.ip++;
		break;
	    }// push reg32
	  case ax:
	  case bx:
	  case cx:
	  case dx:
	  case si:
	  case di:
	  case sp:
	  case bp:
	    if (flags & FLAGS_REG) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x50 + (op & ~REG_WORD);
		currCode.ip++;
		break;
	    }// push reg16
	  case cs:
	    if (flags & FLAGS_REG) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x0E;
		currCode.ip++;
		break;
	    }// push cs
	  case ds:
	    if (flags & FLAGS_REG) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x1E;
		currCode.ip++;
		break;
	    }// push ds
	  case ss:
	    if (flags & FLAGS_REG) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x16;
		currCode.ip++;
		break;
	    }// push ss
	  case es:
	    if (flags & FLAGS_REG) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x06;
		currCode.ip++;
		break;
	    }// push es
	  case fs:
	    if (flags & FLAGS_REG) {
		*(WORD *) (currCode.tempBuffer + currCode.ip) = 0xA00F;
		currCode.ip += 2;
		break;
	    }// push fs
	  case gs:
	    if (flags & FLAGS_REG) {
		*(WORD *) (currCode.tempBuffer + currCode.ip) = 0xA80F;
		currCode.ip += 2;
		break;
	    }// push gs
	  default:
		if (flags & FLAGS_IMM8) {/* push imm8 */
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x6A;
			currCode.ip++;
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) op;
			currCode.ip++;
			break;
		} else if ((flags & FLAGS_IMM16) || (flags & FLAGS_IMM32)) {/* push imm32 */
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x68;
			currCode.ip++;
			*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op;
			currCode.ip += 4;
			break;
		}
		if (flags & FLAGS_MEM) {/* push [mem32] */
			encodePrefixes(prefixes);
			*(WORD *) (currCode.tempBuffer + currCode.ip) = 0x35FF;
			currCode.ip += 2;
			*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op;
			currCode.ip += 4;
			break;			
		}
		encodeModrmSibMono(op, 0xFF, 6, disp, flags | FLAGS_OPCODE1, prefixes);// push r/m32
		break;
	}
}

/* POP reg16, POP reg32, POP r/m32, POP Seg (excluding cs) */
void _pop(int op, int disp, int flags, int prefixes)
{
	int td;
	int sib;

	while (currCode.size-currCode.ip < 7) platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);
	switch (op) {
	  case eax:
	  case ebx:
	  case ecx:
	  case edx:
	  case esi:
	  case edi:
	  case esp:
	  case ebp:
	    if (flags & FLAGS_REG) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x58 + (op & ~REG_DWORD);
		currCode.ip++;
		break;
	    }// pop reg32
	  case ax:
	  case bx:
	  case cx:
	  case dx:
	  case si:
	  case di:
	  case sp:
	  case bp:
	    if (flags & FLAGS_REG) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x58 + (op & ~REG_WORD);
		currCode.ip++;
		break;
	    }// pop reg16
	  case ds:
	    if (flags & FLAGS_REG) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x1F;
		currCode.ip++;
		break;
	    }// pop ds
	  case ss:
	    if (flags & FLAGS_REG) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x17;
		currCode.ip++;
		break;
	    }// pop ss
	  case es:
	    if (flags & FLAGS_REG) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x07;
		currCode.ip++;
		break;
	    }// pop es
	  case fs:
	    if (flags & FLAGS_REG) {
		*(WORD *) (currCode.tempBuffer + currCode.ip) = 0xA10F;
		currCode.ip += 2;
		break;
	    }// pop fs
	  case gs:
	    if (flags & FLAGS_REG) {
		*(WORD *) (currCode.tempBuffer + currCode.ip) = 0xA90F;
		currCode.ip += 2;
		break;
	    }// pop gs
	  default:
		if (flags & FLAGS_MEM) {
			encodePrefixes(prefixes);
			*(WORD *) (currCode.tempBuffer + currCode.ip) = 0x058F;
			currCode.ip += 2;
			*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op;
			currCode.ip += 4;
			break;			
		}// pop [mem32]
		encodeModrmSibMono(op, 0x8F, 0, disp, flags | FLAGS_OPCODE1, prefixes);// pop r/m32
		break;
	}
}

/* MOV reg32, reg32; MOV reg32, imm32; MOV r/m32, imm32; MOV reg32, r/m32; MOV r/m32, reg32  */
void _mov(int op1, int op2, int disp, int flags, int prefixes)
{
	int td;
	int sib, modrm;

	while (currCode.size-currCode.ip < 11) platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);

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
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x89;
			currCode.ip++;	
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = (op1 & ~REG_DWORD) + ((op2 & ~REG_DWORD) << 3)+ 0xC0;
			currCode.ip++;		
			break;
		    }// mov reg32, reg32
		  default:
			if (flags & FLAGS_MEM) {
				encodePrefixes(prefixes);
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x8B;
				currCode.ip++;	
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op1 & ~REG_DWORD) << 3)+ 0x05;
				currCode.ip++;			
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				break;
			}// mov reg32, [mem32]
			if (flags & FLAGS_SCALE) {
				encodeModrmSibMulti(op1, op2, 0x8B, disp, flags | FLAGS_OPCODE1, prefixes);
				break;
			}// mov reg32, r/m32
			if (flags & FLAGS_IMM){
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xB8 + (op1 & ~REG_DWORD);
				currCode.ip++;
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
				break;
			}// mov reg32, imm32
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
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xC7;
				currCode.ip++;	
				*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x05;
				currCode.ip++;			
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op1;
				currCode.ip += 4;
			} else
				encodeModrmSibMono(op1, 0xC7, 0, disp, flags | FLAGS_OPCODE1, prefixes);
			if (*(WORD *) (currCode.tempBuffer + currCode.ip - 2) != 0x0B0F){
				*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op2;
				currCode.ip += 4;
			}
			break;
		}// mov r/m32, imm32
		if ((flags & FLAGS_MEM) && (flags & FLAGS_REG2)) {
			encodePrefixes(prefixes);
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x89;
			currCode.ip++;	
			*(BYTE *) (currCode.tempBuffer + currCode.ip) = ((op2 & ~REG_DWORD) << 3)+ 0x05;
			currCode.ip++;			
			*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op1;
			currCode.ip += 4;
			break;
		}// mov [mem32], reg32
		if (flags | FLAGS_REG2)
			encodeModrmSibMulti(op2, op1, 0x89, disp, flags | FLAGS_OPCODE1, prefixes);// mov r/m32, reg32
		else
			_ud2();
		break;
	}
}

/* CALL reg32, CALL r/m32 */
void _call(int op, int disp, int flags, int prefixes)
{
	int td;
	int sib;

	while (currCode.size-currCode.ip < 7) platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);
	switch (op) {
	  case eax:
	  case ebx:
	  case ecx:
	  case edx:
	  case esi:
	  case edi:
	  case esp:
	  case ebp:
	    if (flags & FLAGS_REG) {
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xFF;
		currCode.ip++;
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xD0 + (op & ~REG_DWORD);
		currCode.ip++;
		break;
	    }// call reg32
	  default:
		if (flags & FLAGS_MEM) {
			encodePrefixes(prefixes);
			*(WORD *) (currCode.tempBuffer + currCode.ip) = 0x15FF;
			currCode.ip += 2;
			*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op;
			currCode.ip += 4;
			break;			
		}// call [mem32]
		encodeModrmSibMono(op, 0xFF, 2, disp, flags | FLAGS_OPCODE1, prefixes);// call r/m32
		break;
	}	
}

/* Jcc rel8, Jcc rel32 */
void _jcc(int offs, int flags, int prefixes)
{
	while (currCode.size-currCode.ip < 10) platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);
	if (flags & JUMP_SHORT) {
		encodePrefixes(prefixes);
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0x70 + (BYTE)(((WORD) flags & 0xFF00) >> 8);
		currCode.ip++;
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) offs;
		currCode.ip++;
	} else if (flags & JUMP_NEAR) {
		encodePrefixes(prefixes);
		*(WORD *) (currCode.tempBuffer + currCode.ip) = 0x800F + ((WORD) flags & 0xFF00);
		currCode.ip += 2;
		*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) offs;
		currCode.ip += 4;
	} else _ud2();
}

/* JMP reg32, JMP r/m32, JMP rel8, JMP rel32 */
void _jmp(int op, int jt, int disp, int flags, int prefixes)
{
	int td;
	int sib;

	while (currCode.size-currCode.ip < 11) platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);

	if (jt & JUMP_SHORT) {
		encodePrefixes(prefixes);
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xEB;
		currCode.ip++;
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) op;
		currCode.ip++;
		return;
	} else if (JUMP_NEAR) {
		encodePrefixes(prefixes);
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xE9;
		currCode.ip++;
		*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op;
		currCode.ip += 4;
		return;
	}

	switch (op) {
	  case eax:
	  case ebx:
	  case ecx:
	  case edx:
	  case esi:
	  case edi:
	  case esp:
	  case ebp:
	    if (flags & FLAGS_REG) {
		encodePrefixes(prefixes);
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xFF;
		currCode.ip++;
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xE0 + (op & ~REG_DWORD);
		currCode.ip++;
		break;
	    }// jmp reg32
	  default:
		if (flags & FLAGS_MEM) {
			encodePrefixes(prefixes);
			*(WORD *) (currCode.tempBuffer + currCode.ip) = 0x25FF;
			currCode.ip += 2;
			*(DWORD *) (currCode.tempBuffer + currCode.ip) = (DWORD) op;
			currCode.ip += 4;
			break;			
		}// jmp [mem32]
		encodeModrmSibMono(op, 0xFF, 4, disp, flags | FLAGS_OPCODE1, prefixes);// jmp r/m32
		break;
	}	
}

/* LOOPcc rel8 */
void _loopcc(int offs, int flags, int prefixes)
{
	while (currCode.size-currCode.ip < 6) platformInitCodeDesc(&currCode, F_REALLOC | F_TEMPBUF);
	if (flags & LOOP_ABS) {
		encodePrefixes(prefixes);
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xE2;
		currCode.ip++;
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) offs;
		currCode.ip++;
	} else if (flags & (LOOP_EQUAL | LOOP_ZERO)) {
		encodePrefixes(prefixes);
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xE1;
		currCode.ip++;
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) offs;
		currCode.ip++;
	} else if (flags & (LOOP_NOT_EQUAL | LOOP_NOT_ZERO)) {
		encodePrefixes(prefixes);
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = 0xE0;
		currCode.ip++;
		*(BYTE *) (currCode.tempBuffer + currCode.ip) = (BYTE) offs;
		currCode.ip++;
	} else _ud2();
}