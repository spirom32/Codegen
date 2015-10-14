//
//
// memctl.h
// codegen_x86
//
// Created by Andrey Utkin on 12.04.2015
//
// (C) 2015 Andrey Utkin. All rights reserved
//

void *memVirtualAlloc(int size);
int memVirtualLock(void *addr, int size);
int memVirtualUnlock(void *addr, int size);
int memVirtualProtectExec(void *addr, int size);
int memVirtualProtectNormal(void *addr, int size);
int memVirtualFree(void *addr, int size);

#ifdef _WIN32

#define MINIMAL_CODE_PAGE 256

#else

#define MINIMAL_CODE_PAGE 4096

#endif
