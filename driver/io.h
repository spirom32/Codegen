
UCHAR INB(IN USHORT Port) {
UCHAR RByte;
#ifndef _WIN64
__asm {
mov dx, Port
in al, dx
mov RByte, al
}
#else
RByte = __inbyte(Port);
#endif
return RByte;
}

ULONG IND(IN USHORT Port) {
ULONG RDword;
#ifndef _WIN64
__asm {
mov dx, Port
in eax, dx
mov RDword, eax
}
#else
RDword = __indword(Port);
#endif
return RDword;
}

VOID OUTB(IN USHORT Port, IN UCHAR BData) {
#ifndef _WIN64
__asm {
mov dx, Port
mov al, BData
out dx, al
}
#else
__outbyte(Port, BData);
#endif
return;
}

VOID OUTD(IN USHORT Port, IN ULONG DData) {
#ifndef _WIN64
__asm {
mov dx, Port
mov eax, DData
out dx, eax
}
#else
__outdword(Port, DData);
#endif
return;
}

ULONG64 RDMSR(IN ULONG Register) {
ULONG HiPart, LoPart;
ULONG64 MsrData;
#ifndef _WIN64
__asm {
mov ecx, Register
rdmsr
mov HiPart, edx
mov LoPart, eax
}
MsrData = ((ULONG64)(HiPart)<<32) | LoPart;
#else
MsrData = __readmsr(Register);
#endif
return MsrData;
}

VOID WRMSR(IN ULONG Register, IN ULONG LoPart, IN ULONG HiPart ) {
ULONG64 MsrData;
#ifndef _WIN64
__asm {
mov ecx, Register
mov eax, LoPart
mov edx, HiPart
wrmsr
}
#else
MsrData = ((ULONG64)(HiPart)<<32) | LoPart;
__writemsr(Register, MsrData);
#endif
return;
}

ULONG64 RDTSC() {
ULONG HiPart, LoPart;
ULONG64 TSC;
#ifndef _WIN64
__asm {
rdtsc
mov HiPart, edx
mov LoPart, eax
}
TSC = ((ULONG64)(HiPart)<<32) | LoPart;
#else
TSC = __rdtsc();
#endif
return TSC;
}

PVOID MapMMIO(IN ULONG PhyAddr, IN ULONG _Size) {
PHYSICAL_ADDRESS phy = {0, 0};
phy.HighPart = 0;
phy.LowPart = PhyAddr;
return MmMapIoSpace(phy, _Size, MmNonCached);
}

VOID UnMapMMIO(IN PVOID StartAddr, IN ULONG _Size) {
MmUnmapIoSpace(StartAddr, _Size);
return;
}

VOID STI() {
#ifndef _WIN64
__asm{
sti
}
#else
_enable();
#endif;
return;
} 

VOID CLI() {
#ifndef _WIN64
__asm{
cli
}
#else
_disable();
#endif;
return;
} 

VOID WBINVD() {
#ifndef _WIN64
__asm{
wbinvd
}
#else
__wbinvd();
#endif;
return;
} 

BOOLEAN AddMMIOMapping(IN ULONG PhyAddr, IN ULONG _Size, IN PVOID MappedMMIO, IN PMDL pMdl, IN PVOID UserAddr) {
PMAP_MMIO_INFO_LIST MmList = NULL;
MmList = ExAllocateFromNPagedLookasideList(glLookaside);
if (MmList == NULL) return FALSE;
MmList->Info.PhysAddr = PhyAddr;
MmList->Info._Size = _Size;
MmList->Info.MappedMMIO = MappedMMIO;
MmList->Info.MDLAddr = pMdl;
MmList->Info.UserAddr = UserAddr;
InsertHeadList(&glListHead, &(MmList->MMList));
return TRUE;
}

BOOLEAN RemoveMMIOMapping(IN PMMIO_MAPPINGS_INFO PMmi, IN PVOID UserAddr, IN ULONG _Size) {
PLIST_ENTRY Next;
PMAP_MMIO_INFO_LIST Entry;
Next = glListHead.Flink;
while (Next != &glListHead) {
Entry = CONTAINING_RECORD(Next, MAP_MMIO_INFO_LIST, MMList);
if ((Entry->Info._Size == _Size) && (Entry->Info.UserAddr == UserAddr)) break;
Next = Next->Flink;
}
if (Next == &glListHead) return FALSE;
PMmi->PhysAddr = Entry->Info.PhysAddr;
PMmi->_Size = Entry->Info._Size;
PMmi->UserAddr = Entry->Info.UserAddr;
PMmi->MappedMMIO = Entry->Info.MappedMMIO;
PMmi->MDLAddr = Entry->Info.MDLAddr;
RemoveEntryList(Next);
ExFreeToNPagedLookasideList(glLookaside, Entry);
return TRUE;
}