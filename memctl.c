//
//
// memctl.ñ
// codegen_x86
//
// Created by Andrey Utkin on 12.04.2015
//
// (C) 2015 Andrey Utkin. All rights reserved
//

#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include "memctl.h"

#ifdef _WIN32

#include <windows.h>

void *memVirtualAlloc(int size)
{
	return VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
}

int memVirtualLock(void *addr, int size)
{
	return VirtualLock(addr, size);
}

int memVirtualUnlock(void *addr, int size)
{
	return VirtualUnlock(addr, size);
}

int memVirtualProtectExec(void *addr, int size)
{
	DWORD prt;

	return VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &prt);
}

int memVirtualProtectNormal(void *addr, int size)
{
	DWORD prt;

	return VirtualProtect(addr, size, PAGE_READWRITE, &prt);
}

int memVirtualFree(void *addr, int size)
{
	return VirtualFree(addr, size, MEM_RELEASE);
}

#else

#include <sys/mman.h>

#ifndef __CYGWIN__

void *memVirtualAlloc(int size)
{
	void *ptr;
	posix_memalign(&ptr, 0x1000, size);
	return ptr;
	//return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
}

int memVirtualLock(void *addr, int size)
{
	return mlock(addr, size);
}

int memVirtualUnlock(void *addr, int size)
{
	return munlock(addr, size);
}

int memVirtualProtectExec(void *addr, int size)
{
	return mprotect(addr, size, PROT_READ | PROT_EXEC);
}

int memVirtualProtectNormal(void *addr, int size)
{
	return mprotect(addr, size, PROT_READ | PROT_WRITE);
}

int memVirtualFree(void *addr, int size)
{
	return munmap(addr, size);
}

#else

void *memVirtualAlloc(int size)
{
	//void *ptr;
	//posix_memalign(&ptr, 0x1000, size);
	//return ptr;
	return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
}

int memVirtualLock(void *addr, int size)
{
	return mlock(addr, size);
}

int memVirtualUnlock(void *addr, int size)
{
	return munlock(addr, size);
}

int memVirtualProtectExec(void *addr, int size)
{
	return mprotect(addr, size, PROT_READ | PROT_EXEC);
}

int memVirtualProtectNormal(void *addr, int size)
{
	return mprotect(addr, size, PROT_READ | PROT_WRITE);
}

int memVirtualFree(void *addr, int size)
{
	return munmap(addr, size);
}

#endif

#endif