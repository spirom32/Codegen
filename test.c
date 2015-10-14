#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
#include "memctl.h"

void pushTest()
{
	printf("pushTest: testing push instruction started...\n");
	platformInitCodeDesc(&currCode, F_RESET);
	//_mov(ebp, 0x54, 0x4545454, FLAGS_IMM8 | FLAGS_BASE_EBP | FLAGS_DISP32 | FLAGS_SCALE8, 0);
	_push(0x45, 0, FLAGS_MEM, 0);// xxx [0x00000045]
	_push(eax, 0, FLAGS_SCALE1, 0);// xxx [eax]
	_push(esp, 0, FLAGS_SCALE1, 0);// xxx [esp]
	_push(ebp, 0, FLAGS_SCALE1, 0);// xxx [ebp]
	_push(esi, 0x34, FLAGS_SCALE1 | FLAGS_DISP8, 0); // xxx [esi + 0x34]
	_push(esp, 0x33, FLAGS_SCALE1 | FLAGS_DISP8, 0);// xxx [esp + 0x33]
	_push(ebp, 0x43, FLAGS_SCALE1 | FLAGS_DISP8, 0);// xxx [ebp + 0x43]
	_push(edi, 0x34, FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + edi + 0x34]
	_push(esp, 0x33, FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + esp + 0x33]
	_push(ebp, 0x43, FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + ebp + 0x43]	
	_push(eax, 0, FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// xxx [ebp + eax]
	_push(esp, 0, FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// xxx [ebp + esp]
	_push(ebp, 0, FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// xxx [ebp + ebp]
	_push(eax, 0, FLAGS_SCALE2, 0);// xxx [eax*2]
	_push(esp, 0, FLAGS_SCALE2, 0);// xxx [esp*2] !invalid opcode!
	_push(ebp, 0, FLAGS_SCALE2, 0);// xxx [ebp*2]
	_push(esi, 0x34, FLAGS_SCALE2 | FLAGS_DISP8, 0);// xxx [esi*2 + 0x34]
	_push(esp, 0x33, FLAGS_SCALE2 | FLAGS_DISP8, 0);// xxx [esp*2 + 0x33] !invalid opcode!
	_push(ebp, 0x43, FLAGS_SCALE2 | FLAGS_DISP8, 0);// xxx [ebp*2 + 0x43]
	_push(edi, 0x34, FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + edi*2 + 0x34]
	_push(esp, 0x33, FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + esp*2 + 0x33] !invalid opcode!
	_push(ebp, 0x43, FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + ebp*2 + 0x43]	
	_push(eax, 0, FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// xxx [ebp + eax*2]
	_push(esp, 0, FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// xxx [ebp + esp*2] !invalid opcode!
	_push(ebp, 0, FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// xxx [ebp + ebp*2]
	_ret(RET_NEAR);//ret
	platformPrintTempBuffer();
}

void popTest()
{
	printf("popTest: testing pop instruction started...\n");
	platformInitCodeDesc(&currCode, F_RESET);
	//_mov(ebp, 0x54, 0x4545454, FLAGS_IMM8 | FLAGS_BASE_EBP | FLAGS_DISP32 | FLAGS_SCALE8, 0);
	_pop(0x45, 0, FLAGS_MEM, 0);// xxx [0x00000045]
	_pop(eax, 0, FLAGS_SCALE1, 0);// xxx [eax]
	_pop(esp, 0, FLAGS_SCALE1, 0);// xxx [esp]
	_pop(ebp, 0, FLAGS_SCALE1, 0);// xxx [ebp]
	_pop(esi, 0x34, FLAGS_SCALE1 | FLAGS_DISP8, 0); // xxx [esi + 0x34]
	_pop(esp, 0x33, FLAGS_SCALE1 | FLAGS_DISP8, 0);// xxx [esp + 0x33]
	_pop(ebp, 0x43, FLAGS_SCALE1 | FLAGS_DISP8, 0);// xxx [ebp + 0x43]
	_pop(edi, 0x34, FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + edi + 0x34]
	_pop(esp, 0x33, FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + esp + 0x33]
	_pop(ebp, 0x43, FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + ebp + 0x43]	
	_pop(eax, 0, FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// xxx [ebp + eax]
	_pop(esp, 0, FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// xxx [ebp + esp]
	_pop(ebp, 0, FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// xxx [ebp + ebp]
	_pop(eax, 0, FLAGS_SCALE2, 0);// xxx [eax*2]
	_pop(esp, 0, FLAGS_SCALE2, 0);// xxx [esp*2] !invalid opcode!
	_pop(ebp, 0, FLAGS_SCALE2, 0);// xxx [ebp*2]
	_pop(esi, 0x34, FLAGS_SCALE2 | FLAGS_DISP8, 0);// xxx [esi*2 + 0x34]
	_pop(esp, 0x33, FLAGS_SCALE2 | FLAGS_DISP8, 0);// xxx [esp*2 + 0x33] !invalid opcode!
	_pop(ebp, 0x43, FLAGS_SCALE2 | FLAGS_DISP8, 0);// xxx [ebp*2 + 0x43]
	_pop(edi, 0x34, FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + edi*2 + 0x34]
	_pop(esp, 0x33, FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + esp*2 + 0x33] !invalid opcode!
	_pop(ebp, 0x43, FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + ebp*2 + 0x43]	
	_pop(eax, 0, FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// xxx [ebp + eax*2]
	_pop(esp, 0, FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// xxx [ebp + esp*2] !invalid opcode!
	_pop(ebp, 0, FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// xxx [ebp + ebp*2]
	_ret(RET_NEAR);//ret
	platformPrintTempBuffer();
}

void callTest()
{
	printf("callTest: testing call instruction started...\n");
	platformInitCodeDesc(&currCode, F_RESET);
	//_mov(ebp, 0x54, 0x4545454, FLAGS_IMM8 | FLAGS_BASE_EBP | FLAGS_DISP32 | FLAGS_SCALE8, 0);
	_call(0x45, 0, FLAGS_MEM, 0);// xxx [0x00000045]
	_call(eax, 0, FLAGS_SCALE1, 0);// xxx [eax]
	_call(esp, 0, FLAGS_SCALE1, 0);// xxx [esp]
	_call(ebp, 0, FLAGS_SCALE1, 0);// xxx [ebp]
	_call(esi, 0x34, FLAGS_SCALE1 | FLAGS_DISP8, 0); // xxx [esi + 0x34]
	_call(esp, 0x33, FLAGS_SCALE1 | FLAGS_DISP8, 0);// xxx [esp + 0x33]
	_call(ebp, 0x43, FLAGS_SCALE1 | FLAGS_DISP8, 0);// xxx [ebp + 0x43]
	_call(edi, 0x34, FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + edi + 0x34]
	_call(esp, 0x33, FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + esp + 0x33]
	_call(ebp, 0x43, FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + ebp + 0x43]	
	_call(eax, 0, FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// xxx [ebp + eax]
	_call(esp, 0, FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// xxx [ebp + esp]
	_call(ebp, 0, FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// xxx [ebp + ebp]
	_call(eax, 0, FLAGS_SCALE2, 0);// xxx [eax*2]
	_call(esp, 0, FLAGS_SCALE2, 0);// xxx [esp*2] !invalid opcode!
	_call(ebp, 0, FLAGS_SCALE2, 0);// xxx [ebp*2]
	_call(esi, 0x34, FLAGS_SCALE2 | FLAGS_DISP8, 0);// xxx [esi*2 + 0x34]
	_call(esp, 0x33, FLAGS_SCALE2 | FLAGS_DISP8, 0);// xxx [esp*2 + 0x33] !invalid opcode!
	_call(ebp, 0x43, FLAGS_SCALE2 | FLAGS_DISP8, 0);// xxx [ebp*2 + 0x43]
	_call(edi, 0x34, FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + edi*2 + 0x34]
	_call(esp, 0x33, FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + esp*2 + 0x33] !invalid opcode!
	_call(ebp, 0x43, FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// xxx [ebp + ebp*2 + 0x43]	
	_call(eax, 0, FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// xxx [ebp + eax*2]
	_call(esp, 0, FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// xxx [ebp + esp*2] !invalid opcode!
	_call(ebp, 0, FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// xxx [ebp + ebp*2]
	_ret(RET_NEAR);//ret
	platformPrintTempBuffer();
}

void prefixTest()
{
	printf("prefixTest: testing prefix encoding started...\n");
	platformInitCodeDesc(&currCode, F_RESET);
	_mov(ebp, esp, 0x04532235, FLAGS_SCALE8 | FLAGS_DISP32 | FLAGS_REG2 | FLAGS_BASE_EBP, PREFIX_FS_OVERRIDE);// mov fs:[ebp + ebp*8 + 04532235h], esp
	_push(eax, 0, FLAGS_SCALE2 | FLAGS_BASE_EBP, PREFIX_ES_OVERRIDE);// push es:[ebp + eax*2]
	_pop(0x00043433, 0, FLAGS_MEM, PREFIX_GS_OVERRIDE);// pop gs:[00043433h]
	_call(esp, 0, FLAGS_SCALE1 | FLAGS_BASE_EBP, PREFIX_DS_OVERRIDE);// call ds:[ebp + esp]
	_ret(RET_NEAR);//ret
	platformPrintTempBuffer();
}

void movTest()
{
	printf("movTest: testing mov instruction started...\n");
	platformInitCodeDesc(&currCode, F_RESET);
	_mov(ebp, 0x54, 0x4545454, FLAGS_IMM8 | FLAGS_BASE_EBP | FLAGS_DISP32 | FLAGS_SCALE8, 0);// mov [ebp + ebp*8 + 04545454h], 54h
	_mov(eax, ebp, 0, FLAGS_REG | FLAGS_REG2, 0);// mov eax, ebp
	_mov(ecx, esi, 0, FLAGS_REG | FLAGS_REG2, 0);// mov ecx, esi
	_mov(ebp, ebp, 0, FLAGS_REG | FLAGS_REG2, 0);// mov ebp, ebp
	_mov(ebp, esp, 0, FLAGS_REG | FLAGS_REG2, 0);// mov ebp, esp
	_mov(esp, esp, 0, FLAGS_REG | FLAGS_REG2, 0);// mov esp, esp
	_ret(RET_NEAR);//ret
	_mov(eax, 0x65, 0, FLAGS_REG | FLAGS_IMM8, 0);// mov eax, 65h
	_mov(esi, 0x45445232, 0, FLAGS_REG | FLAGS_IMM32, 0);// mov esi, 45445232h
	_mov(ebp, 0x34322, 0, FLAGS_REG | FLAGS_IMM32, 0);// mov ebp, 00034322h
	_mov(esp, 0x54214, 0, FLAGS_REG | FLAGS_IMM32, 0);// mov esp, 00054214h
	_ret(RET_NEAR);//ret
	_mov(esp, 0x45, 0, FLAGS_REG | FLAGS_MEM, 0);// mov esp, [00000045h]
	_mov(eax, eax, 0, FLAGS_REG | FLAGS_SCALE1, 0);// mov eax, [eax]
	_mov(edi, esp, 0, FLAGS_REG | FLAGS_SCALE1, 0);// mov edi, [esp]
	_mov(ebp, ebp, 0, FLAGS_REG | FLAGS_SCALE1, 0);// mov ebp, [ebp]
	_mov(esi, esi, 0x34, FLAGS_REG | FLAGS_SCALE1 | FLAGS_DISP8, 0);// mov esi, [esi + 34h]
	_mov(esp, esp, 0x33, FLAGS_REG | FLAGS_SCALE1 | FLAGS_DISP8, 0);// mov esp, [esp + 33h]
	_mov(ebp, ebp, 0x43, FLAGS_REG | FLAGS_SCALE1 | FLAGS_DISP8, 0);// mov ebp, [ebp + 43h]
	_mov(edi, edi, 0x34, FLAGS_REG | FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov edi, [ebp + edi + 34h]
	_mov(esp, esp, 0x33, FLAGS_REG | FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov esp, [ebp + esp + 33h]
	_mov(ebp, ebp, 0x43, FLAGS_REG | FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov ebp, [ebp + ebp + 43h]	
	_mov(eax, eax, 0, FLAGS_REG | FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// mov eax, [ebp + eax]
	_mov(esp, esp, 0, FLAGS_REG | FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// mov esp, [ebp + esp]
	_mov(ebp, ebp, 0, FLAGS_REG | FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// mov ebp, [ebp + ebp]
	_mov(eax, eax, 0, FLAGS_REG | FLAGS_SCALE2, 0);// mov eax, [eax*2]
	_mov(esp, esp, 0, FLAGS_REG | FLAGS_SCALE2, 0);// mov esp, [esp*2] !invalid opcode!
	_mov(ebp, ebp, 0, FLAGS_REG | FLAGS_SCALE2, 0);// mov ebp, [ebp*2]
	_mov(esi, esi, 0x34, FLAGS_REG | FLAGS_SCALE2 | FLAGS_DISP8, 0);// mov esi, [esi*2 + 34h]
	_mov(esp, esp, 0x33, FLAGS_REG | FLAGS_SCALE2 | FLAGS_DISP8, 0);// mov esp, [esp*2 + 33h] !invalid opcode!
	_mov(ebp, ebp, 0x43, FLAGS_REG | FLAGS_SCALE2 | FLAGS_DISP8, 0);// mov ebp, [ebp*2 + 43h]
	_mov(edi, edi, 0x34, FLAGS_REG | FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov edi, [ebp + edi*2 + 34]
	_mov(esp, esp, 0x33, FLAGS_REG | FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov esp, [ebp + esp*2 + 33h] !invalid opcode!
	_mov(ebp, ebp, 0x43, FLAGS_REG | FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov ebp, [ebp + ebp*2 + 43h]	
	_mov(eax, eax, 0, FLAGS_REG | FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// mov eax, [ebp + eax*2]
	_mov(esp, esp, 0, FLAGS_REG | FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// mov esp, [ebp + esp*2] !invalid opcode!
	_mov(ebp, ebp, 0, FLAGS_REG | FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// mov ebp, [ebp + ebp*2]
	_ret(RET_NEAR);//ret
	_mov(0x45, esp, 0, FLAGS_REG2 | FLAGS_MEM, 0);// mov [00000045h], esp
	_mov(eax, eax, 0, FLAGS_REG2 | FLAGS_SCALE1, 0);// mov [eax], eax
	_mov(esp, edi, 0, FLAGS_REG2 | FLAGS_SCALE1, 0);// mov [esp], edi
	_mov(ebp, ebp, 0, FLAGS_REG2 | FLAGS_SCALE1, 0);// mov [ebp], ebp
	_mov(esi, esi, 0x34, FLAGS_REG2 | FLAGS_SCALE1 | FLAGS_DISP8, 0);// mov [esi + 34h], esi
	_mov(esp, esp, 0x33, FLAGS_REG2 | FLAGS_SCALE1 | FLAGS_DISP8, 0);// mov [esp + 33h], esp
	_mov(ebp, ebp, 0x43, FLAGS_REG2 | FLAGS_SCALE1 | FLAGS_DISP8, 0);// mov [ebp + 43h], ebp
	_mov(edi, edi, 0x34, FLAGS_REG2 | FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov [ebp + edi + 34h], edi
	_mov(esp, esp, 0x33, FLAGS_REG2 | FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov [ebp + esp + 33h], esp
	_mov(ebp, ebp, 0x43, FLAGS_REG2 | FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov [ebp + ebp + 43h], ebp	
	_mov(eax, eax, 0, FLAGS_REG2 | FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// mov [ebp + eax], ebp
	_mov(esp, esp, 0, FLAGS_REG2 | FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// mov [ebp + esp], esp
	_mov(ebp, ebp, 0, FLAGS_REG2 | FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// mov [ebp + ebp], ebp
	_mov(eax, eax, 0, FLAGS_REG2 | FLAGS_SCALE2, 0);// mov [eax*2], eax
	_mov(esp, esp, 0, FLAGS_REG2 | FLAGS_SCALE2, 0);// mov [esp*2], esp !invalid opcode!
	_mov(ebp, ebp, 0, FLAGS_REG2 | FLAGS_SCALE2, 0);// mov [ebp*2], ebp
	_mov(esi, esi, 0x34, FLAGS_REG2 | FLAGS_SCALE2 | FLAGS_DISP8, 0);// mov [esi*2 + 34h], esi
	_mov(esp, esp, 0x33, FLAGS_REG2 | FLAGS_SCALE2 | FLAGS_DISP8, 0);// mov [esp*2 + 33h], esp !invalid opcode!
	_mov(ebp, ebp, 0x43, FLAGS_REG2 | FLAGS_SCALE2 | FLAGS_DISP8, 0);// mov [ebp*2 + 43h], ebp
	_mov(edi, edi, 0x34, FLAGS_REG2 | FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov [ebp + edi*2 + 34h], edi
	_mov(esp, esp, 0x33, FLAGS_REG2 | FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov [ebp + esp*2 + 33h], esp !invalid opcode!
	_mov(ebp, ebp, 0x43, FLAGS_REG2 | FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov [ebp + ebp*2 + 43], ebp	
	_mov(eax, eax, 0, FLAGS_REG2 | FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// mov [ebp + eax*2], eax
	_mov(esp, esp, 0, FLAGS_REG2 | FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// mov [ebp + esp*2], esp !invalid opcode!
	_mov(ebp, ebp, 0, FLAGS_REG2 | FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// mov [ebp + ebp*2], ebp
	_ret(RET_NEAR);//ret
	_mov(eax, 0x34, 0, FLAGS_REG | FLAGS_IMM8, 0);// mov eax, 34h
	_mov(ecx, 0x5643, 0, FLAGS_REG | FLAGS_IMM16, 0);// mov ecx, 5643h
	_mov(ebp, 0x23, 0, FLAGS_REG | FLAGS_IMM8, 0);// mov ebp, 23h
	_mov(ebp, 0x232892, 0, FLAGS_REG | FLAGS_IMM32, 0);// mov ebp, 232892h
	_mov(esp, 0x4332234, 0, FLAGS_REG | FLAGS_IMM32, 0);// mov esp, 4332234h
	_ret(RET_NEAR);//ret
	_mov(0x45, 0x21, 0, FLAGS_IMM8 | FLAGS_MEM, 0);// mov [00000045h], 21h
	_mov(eax, 0x21, 0, FLAGS_IMM8 | FLAGS_SCALE1, 0);// mov [eax], 21h
	_mov(esp, 0x21, 0, FLAGS_IMM8 | FLAGS_SCALE1, 0);// mov [esp], 21h
	_mov(ebp, 0x21, 0, FLAGS_IMM8 | FLAGS_SCALE1, 0);// mov [ebp], 21h
	_mov(esi, 0x21, 0x34, FLAGS_IMM8 | FLAGS_SCALE1 | FLAGS_DISP8, 0);// mov [esi + 34h], 21h
	_mov(esp, 0x21, 0x33, FLAGS_IMM8 | FLAGS_SCALE1 | FLAGS_DISP8, 0);// mov [esp + 33h], 21h
	_mov(ebp, 0x21, 0x43, FLAGS_IMM8 | FLAGS_SCALE1 | FLAGS_DISP8, 0);// mov [ebp + 43h], 21h
	_mov(edi, 0x21, 0x34, FLAGS_IMM8 | FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov [ebp + edi + 34h], 21h
	_mov(esp, 0x21, 0x33, FLAGS_IMM8 | FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov [ebp + esp+ 33h], 21h
	_mov(ebp, 0x21, 0x43, FLAGS_IMM8 | FLAGS_SCALE1 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov [ebp + ebp + 43h], 21h	
	_mov(eax, 0x21, 0, FLAGS_IMM8 | FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// mov [ebp + eax], 21h
	_mov(esp, 0x21, 0, FLAGS_IMM8 | FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// mov [ebp + esp], 21h
	_mov(ebp, 0x21, 0, FLAGS_IMM8 | FLAGS_SCALE1 | FLAGS_BASE_EBP, 0);// mov [ebp + ebp], 21h
	_mov(eax, 0x21, 0, FLAGS_IMM8 | FLAGS_SCALE2, 0);// mov [eax*2], 21h
	_mov(esp, 0x21, 0, FLAGS_IMM8 | FLAGS_SCALE2, 0);// mov [esp*2], 21h !invalid opcode!
	_mov(ebp, 0x21, 0, FLAGS_IMM8 | FLAGS_SCALE2, 0);// mov [ebp*2], 21h
	_mov(esi, 0x21, 0x34, FLAGS_IMM8 | FLAGS_SCALE2 | FLAGS_DISP8, 0);// mov [esi*2 + 34h], 21h
	_mov(esp, 0x21, 0x33, FLAGS_IMM8 | FLAGS_SCALE2 | FLAGS_DISP8, 0);// mov [esp*2 + 33h], 21h !invalid opcode!
	_mov(ebp, 0x21, 0x43, FLAGS_IMM8 | FLAGS_SCALE2 | FLAGS_DISP8, 0);// mov [ebp*2 + 43h], 21h
	_mov(edi, 0x21, 0x34, FLAGS_IMM8 | FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov [ebp + edi*2 + 34h], 21h
	_mov(esp, 0x21, 0x33, FLAGS_IMM8 | FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov [ebp + esp*2 + 33h], 21h !invalid opcode!
	_mov(ebp, 0x21, 0x43, FLAGS_IMM8 | FLAGS_SCALE2 | FLAGS_DISP8 | FLAGS_BASE_EBP, 0);// mov [ebp + ebp*2 + 43h], 21h	
	_mov(eax, 0x21, 0, FLAGS_IMM8 | FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// mov [ebp + eax*2], 21h
	_mov(esp, 0x21, 0, FLAGS_IMM8 | FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// mov [ebp + esp*2], 21h !invalid opcode!
	_mov(ebp, 0x21, 0, FLAGS_IMM8 | FLAGS_SCALE2 | FLAGS_BASE_EBP, 0);// mov [ebp + ebp*2], 21h
	_ret(RET_NEAR);//ret
	platformPrintTempBuffer();
}