#include <stdio.h>
//#include <windows.h>
#include "codegen.h"
#include "memctl.h"

void main()
{
	int (* p1) (char *, ...) = printf;
	int (* func) (int, int (*) (char *, ...), char *);
	int i, sz;
	char Msg [] = "We are in printf\n\0";

	platformInitCodeDesc(&currCode, F_ALLOC | F_TEMPBUF);
	_push(ebp, 0, FLAGS_REG, 0); //push ebp
	_mov(ebp, esp, 0, FLAGS_REG | FLAGS_REG2, 0);// mov ebp, esp
	_push(ebp, 0x10, FLAGS_SCALE1 | FLAGS_DISP8, 0);// push [ebp + 10h]
	_call(ebp, 0x0C, FLAGS_SCALE1 | FLAGS_DISP8, 0);// call [ebp + 0Ch]
	_pop(eax, 0, FLAGS_REG, 0);// pop eax
	_mov(eax, ebp, 0x08, FLAGS_REG | FLAGS_SCALE1 | FLAGS_DISP8, 0);//mov eax, [ebp + 08h]
	_pop(ebp, 0, FLAGS_REG, 0);// pop ebp
	_ret(RET_NEAR);//ret
	platformPrintTempBuffer();
	memVirtualProtectExec(currCode.tempBuffer, currCode.size);
	func = (int (*) (int, int (*) (char *, ...), char *)) currCode.tempBuffer;
  	//FlushInstructionCache(GetCurrentProcess(), NULL, 0);
	i = func (21, p1, Msg);
	printf("Func returns: %d\n", i);
	memVirtualProtectNormal(currCode.tempBuffer, currCode.size);

	platformInitCodeDesc(&currCode, F_RESET | F_TEMPBUF);
	//_push(ebp, 0, FLAGS_REG, 0); //push ebp
	//_add(ebp, 0x04, 0, FLAGS_REG | FLAGS_IMM8, 0);// add ebp, 04h
	platformPrologue(0, 0);
	_push(ebp, 0x10, FLAGS_SCALE1 | FLAGS_DISP8, 0);// push [ebp + 10h]
	_call(ebp, 0x0C, FLAGS_SCALE1 | FLAGS_DISP8, 0);// call [ebp + 0Ch]
	//_pop(eax, 0, FLAGS_REG, 0);// pop eax
	//_add(esp, 0x04, 0, FLAGS_REG | FLAGS_IMM8, 0);// add esp, 04h
	platformCorrectStack(0x04);
	_mov(eax, ebp, 0x08, FLAGS_REG | FLAGS_SCALE1 | FLAGS_DISP8, 0); //mov eax, [ebp+08h]
	//_sub(ebp, 0x04, 0, FLAGS_REG | FLAGS_IMM8, 0);// sub ebp, 04h
	//_pop(ebp, 0, FLAGS_REG, 0); //pop ebp
	platformEpilogue(0);
	_ret(RET_NEAR); //ret

	currCode.Code = platformGrantExecRegion(currCode.ip, &sz);
	//printf("Granted Code = %p Size = %d\n", currCode.Code, sz);
	platformInitCodeRegion(currCode.Code, sz);
	//printf("Acquired\n");
	//i = platformCallGeneratedCode(currCode.Code, 3, (int) 21, (int) p1, (int) Msg);
	i = platformCallGeneratedCode(currCode.Code, (int) 21, (int) p1, (int) Msg);
	platformFreeExecRegion(currCode.Code, sz);
	printf("Func returns: %d\n", i);

	pushTest();
	movTest();
	prefixTest();
	popTest();
	callTest();

	return;
}