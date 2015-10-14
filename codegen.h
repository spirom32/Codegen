//
//
// codegen.h
// codegen_x86
//
// Created by Andrey Utkin on 06.04.2015
//
// (C) 2015 Andrey Utkin. All rights reserved
//

#ifndef _codegen_h_
#define _codegen_h

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef DWORD
	#define DWORD unsigned int
#endif
#ifndef WORD
	#define WORD unsigned short
#endif
#ifndef BYTE
	#define BYTE unsigned char
#endif

typedef struct {
	int size;
	int ip;
	char *tempBuffer;
	char *Code;
} codeDesc;

#define F_ALLOC   0x00000001
#define F_REALLOC 0x00000002
#define F_RESET   0x00000004
#define F_TEMPBUF 0x00000100
#define F_VIRT    0x00000200

#define RET_NEAR 0x00
#define RET_FAR  0x01

#define FLAGS_MEM 0x01000000
#define FLAGS_IMM8  0x00000001
#define FLAGS_IMM16 0x00000002
#define FLAGS_IMM32 0x00000004
#define FLAGS_IMM (FLAGS_IMM8 | FLAGS_IMM16 | FLAGS_IMM32)
#define FLAGS_BASE_EBP 0x00000008
#define FLAGS_DISP8 0x00000100
#define FLAGS_DISP16 0x00000200
#define FLAGS_DISP32 0x00000400
#define FLAGS_DISP (FLAGS_DISP8 | FLAGS_DISP16 | FLAGS_DISP32)
#define FLAGS_SCALE1 0x00000010
#define FLAGS_SCALE2 0x00000020
#define FLAGS_SCALE4 0x00000040
#define FLAGS_SCALE8 0x00000080
#define FLAGS_SCALE (FLAGS_SCALE1 | FLAGS_SCALE2 | FLAGS_SCALE4 | FLAGS_SCALE8)
#define FLAGS_REG 0x02000000
#define FLAGS_REG2 0x04000000
#define FLAGS_OPCODE1 0x10000000
#define FLAGS_OPCODE2 0x20000000
#define FLAGS_OPCODE3 0x40000000
#define FLAGS_OPCODE4 0x80000000

#define REG_DWORD 0x80000000
#define REG_WORD  0x40000000
#define REG_BYTE  0x20000000
#define REG_SIZED (REG_DWORD | REG_WORD | REG_BYTE)
#define REG_SEGMENT 0x10000000
#define REG_TYPED (REG_SIZED | REG_SEGMENT)

#define PREFIX_OPSIZE_OVERRIDE 0x0001
#define PREFIX_ADSIZE_OVERRIDE 0x0002
#define PREFIX_BRUNCH_TAKEN    0x0004
#define PREFIX_BRUNCH_NTAKEN   0x0008
#define PREFIX_LOCK            0x0010
#define PREFIX_REP             0x0020
#define PREFIX_REPN            0x0040
#define PREFIX_CS_OVERRIDE     0x0100
#define PREFIX_DS_OVERRIDE     0x0200
#define PREFIX_SS_OVERRIDE     0x0400
#define PREFIX_ES_OVERRIDE     0x0800
#define PREFIX_FS_OVERRIDE     0x1000
#define PREFIX_GS_OVERRIDE     0x2000

#define REG_CS 0x00000000
#define REG_DS 0x00000001
#define REG_SS 0x00000002
#define REG_ES 0x00000003
#define REG_FS 0x00000004
#define REG_GS 0x00000005

#define REG_AL    0x00000000
#define REG_BL	  0x00000003
#define REG_CL    0x00000001
#define REG_DL	  0x00000002
#define REG_AH    0x00000004
#define REG_BH	  0x00000007
#define REG_CH    0x00000005
#define REG_DH	  0x00000006

#define REG_AX    0x00000000
#define REG_BX	  0x00000003
#define REG_CX    0x00000001
#define REG_DX	  0x00000002
#define REG_SP    0x00000004
#define REG_BP	  0x00000005
#define REG_SI    0x00000006
#define REG_DI	  0x00000007

#define REG_EAX    0x00000000
#define REG_EBX	  0x00000003
#define REG_ECX    0x00000001
#define REG_EDX	  0x00000002
#define REG_ESP    0x00000004
#define REG_EBP	  0x00000005
#define REG_ESI    0x00000006
#define REG_EDI	  0x00000007

#define al (REG_BYTE | REG_AL)
#define bl (REG_BYTE | REG_BL)
#define cl (REG_BYTE | REG_CL)
#define dl (REG_BYTE | REG_DL)
#define ah (REG_BYTE | REG_AH)
#define bh (REG_BYTE | REG_BH)
#define ch (REG_BYTE | REG_CH)
#define dh (REG_BYTE | REG_DH)

#define ax (REG_WORD | REG_AX)
#define bx (REG_WORD | REG_BX)
#define cx (REG_WORD | REG_CX)
#define dx (REG_WORD | REG_DX)
#define sp (REG_WORD | REG_SP)
#define bp (REG_WORD | REG_BP)
#define si (REG_WORD | REG_SI)
#define di (REG_WORD | REG_DI)

#define eax (REG_DWORD | REG_EAX)
#define ebx (REG_DWORD | REG_EBX)
#define ecx (REG_DWORD | REG_ECX)
#define edx (REG_DWORD | REG_EDX)
#define esp (REG_DWORD | REG_ESP)
#define ebp (REG_DWORD | REG_EBP)
#define esi (REG_DWORD | REG_ESI)
#define edi (REG_DWORD | REG_EDI)

#define cs (REG_SEGMENT | REG_CS)
#define ds (REG_SEGMENT | REG_DS)
#define ss (REG_SEGMENT | REG_SS)
#define es (REG_SEGMENT | REG_ES)
#define fs (REG_SEGMENT | REG_FS)
#define gs (REG_SEGMENT | REG_GS)

#define JUMP_NEAR  0x02 // Jxx rel32
#define JUMP_SHORT 0x01 // Jxx rel8

/*
* Conditional jump (Jcc)
*/

#define JUMP_ABOVE             0x0700 // CF == 0 && ZF == 0
#define JUMP_ABOVE_EQUAL       0x0300 // CF == 0
#define JUMP_BELOW             0x0200 // CF == 1
#define JUMP_BELOW_EQUAL       0x0600 // CF == 1 || ZF == 1
#define JUMP_CARRY             0x0200 // CF == 1
#define JUMP_ECX_0             0x7300 // ECX == 0x00000000
#define JUMP_EQUAL             0x0400 // ZF == 1
#define JUMP_GREATER           0x0F00 // ZF == 0 && SF == OF
#define JUMP_GREATER_EQUAL     0x0D00 // SF == OF
#define JUMP_LESS              0x0C00 // SF != OF
#define JUMP_LESS_EQUAL        0x0E00 // ZF == 1 || SF != OF
#define JUMP_NOT_ABOVE         0x0600 // CF == 1 || ZF == 1
#define JUMP_NOT_ABOVE_EQUAL   0x0200 // CF == 1
#define JUMP_NOT_BELOW         0x0300 // CF == 0
#define JUMP_NOT_BELOW_EQUAL   0x0700 // CF == 0 && ZF == 0
#define JUMP_NOT_CARRY         0x0300 // CF == 0
#define JUMP_NOT_EQUAL         0x0500 // ZF == 0
#define JUMP_NOT_GREATER       0x0E00 // ZF == 1 || SF != OF
#define JUMP_NOT_GREATER_EQUAL 0x0C00 // SF != OF
#define JUMP_NOT_LESS          0x0D00 // SF == OF
#define JUMP_NOT_LESS_EQUAL    0x0F00 // ZF ==0 && SF == OF
#define JUMP_NOT_OVERFLOW      0x0100 // OF == 0
#define JUMP_NOT_PARITY        0x0B00 // PF == 0
#define JUMP_NOT_SIGN          0x0900 // SF == 0
#define JUMP_NOT_ZERO          0x0500 // ZF == 0
#define JUMP_OVERFLOW          0x0000 // OF == 1
#define JUMP_PARITY            0x0A00 // PF == 1
#define JUMP_PARITY_EVEN       0x0A00 // PF == 1
#define JUMP_PARITY_ODD        0x0B00 // PF == 0
#define JUMP_SIGN              0x0800 // SF == 1
#define JUMP_ZERO              0x0400 // ZF == 1

/*
* Conditional loop (LOOPcc)
*/

#define LOOP_ABS       0x01
#define LOOP_EQUAL     0x02
#define LOOP_NOT_EQUAL 0x04
#define LOOP_ZERO      0x08
#define LOOP_NOT_ZERO  0x10

/*
* Prologue flags
*/

#define PROLOGUE_LOCAL_BUFFER 0x0001
#define PROLOGUE_EBX          0x0002
#define PROLOGUE_EDI          0x0004

extern codeDesc currCode;

void platformPrintTempBuffer();
void platformInitCodeDesc(codeDesc * cd, int flags);
int platformCallGeneratedCode(void *Code, ...);
void platformInitCodeRegion(void *addr, int size);
void platformFreeExecRegion(void *addr, int size);
int platformAcquireExecRegion(void *addr, int size);
int platformReleaseExecRegion(void *addr, int size);
void *platformGrantExecRegion(int size, int *actSize);
void platformPrologue(int buffSize, int flags);
void platformEpilogue(int flags);
void platformCorrectStack(int disp);
void _nop();
void _ud2();
void _ret(int rt);
void _retn(int op, int rt);
void _clc();
void _cld();
void _stc();
void _std();
void _push(int op, int disp, int flags, int prefixes);
void _pop(int op, int disp, int flags, int prefixes);
void _mov(int op1, int op2, int disp, int flags, int prefixes);
void _jcc(int offs, int flags, int prefixes);
void _jmp(int op, int jt, int disp, int flags, int prefixes);
void _loopcc(int offs, int flags, int prefixes);
void _call(int op, int disp, int flags, int prefixes);
void _add(int op1, int op2, int disp, int flags, int prefixes);
void _sub(int op1, int op2, int disp, int flags, int prefixes);
void _and(int op1, int op2, int disp, int flags, int prefixes);
void _cmp(int op1, int op2, int disp, int flags, int prefixes);
void encodeModrmSibMono(int op, DWORD opcode, int digit, int disp, int flags, int prefixes);
void encodeModrmSibMulti(int op1, int op2, DWORD opcode, int disp, int flags, int prefixes);
int isRegister(int op);
int isPrefix(int op);
void encodePrefixes(int prefixes);

#endif