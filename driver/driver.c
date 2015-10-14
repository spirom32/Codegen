#include "pciide32.h"

UNICODE_STRING DeviceName;
UNICODE_STRING SymbolicLinkName;
PDEVICE_OBJECT deviceObject = NULL;
PNPAGED_LOOKASIDE_LIST glLookaside = NULL;
LIST_ENTRY glListHead = {NULL, NULL};

#include "io.h"
#include "ec.h"

NTSTATUS ReadPortByte(IN PVOID Buffer)
{
ULONG PortNum = ((PREAD_PORT_INFO) Buffer)->PortNum;
NTSTATUS ns = STATUS_SUCCESS;
USHORT Port = (USHORT)(PortNum) & 0x0FFFF;
BYTE RByte = INB(Port);
((PREAD_PORT_INFO) Buffer)->RByte = RByte;
return ns;
}

NTSTATUS ReadPortLong(IN PVOID Buffer)
{
ULONG PortNum = ((PREAD_PORT_INFO) Buffer)->PortNum;
NTSTATUS ns = STATUS_SUCCESS;
USHORT Port = (USHORT)(PortNum) & 0x0FFFF;
DWORD RDword = IND(Port);
((PREAD_PORT_INFO) Buffer)->RDword = RDword;
//DbgPrint("ReadPortLong:: Port: %08X Data: %08X", Port, RDword);
return ns;
}

NTSTATUS WritePortByte(IN PVOID Buffer)
{
ULONG PortNum = ((PWRITE_PORT_INFO) Buffer)->PortNum;
UCHAR BData = ((PWRITE_PORT_INFO) Buffer)->WByte;
NTSTATUS ns = STATUS_SUCCESS;
USHORT Port = (USHORT)(PortNum) & 0x0FFFF;
OUTB(Port, BData);
return ns;
}

NTSTATUS WritePortLong(IN PVOID Buffer)
{
ULONG PortNum = ((PWRITE_PORT_INFO) Buffer)->PortNum;
ULONG DData = ((PWRITE_PORT_INFO) Buffer)->WDword;
NTSTATUS ns = STATUS_SUCCESS;
USHORT Port = (USHORT)(PortNum) & 0x0FFFF;
OUTD(Port, DData);
return ns;
}

NTSTATUS ReadMsr(IN PVOID Buffer)
{
ULONG MsrNum = ((PRDMSR_INFO) Buffer)->MsrNum;
NTSTATUS ns = STATUS_SUCCESS;
ULONG64 MSR = RDMSR(MsrNum);
((PRDMSR_INFO) Buffer)->RMANS.QuadPart = MSR;
return ns;
}

NTSTATUS WriteMsr(IN PVOID Buffer)
{
ULONG MsrNum = ((PWRMSR_INFO) Buffer)->MsrNum;
NTSTATUS ns = STATUS_SUCCESS;
ULONG MSRLo = ((PWRMSR_INFO) Buffer)->MDLoPart;
ULONG MSRHi = ((PWRMSR_INFO) Buffer)->MDHiPart;
WRMSR(MsrNum, MSRLo, MSRHi);
return ns;
}

NTSTATUS GetCPUSpeed(IN PVOID Buffer)
{
ULONG TargetCPU = ((PGCS_INFO) Buffer)->TargetCPU;
NTSTATUS ns = STATUS_SUCCESS;
ULONG64 TSC0, TSC1;
TSC0 = RDTSC();
KeStallExecutionProcessor(50000);
TSC1 = RDTSC();
((PGCS_INFO) Buffer)->Speed.QuadPart = TSC1-TSC0;
return ns;
}

NTSTATUS WriteShadow(IN PVOID Buffer)
{
NTSTATUS ns = STATUS_SUCCESS;
ULONG PhyAddr = ((PWRITE_MEM_INFO) Buffer)->PhyAddr;
ULONG Data = ((PWRITE_MEM_INFO) Buffer)->Data;
UCHAR Bt = (UCHAR)(Data) & 0x0FF;
PBYTE MappedSh;
ULONG64 msr0, msr1, msr2, msr3, msr4;
CLI();
msr0 = RDMSR(0x0C0010010);
msr1 = RDMSR(0x026C);
msr2 = RDMSR(0x026D);
msr3 = RDMSR(0x026E);
msr4 = RDMSR(0x026F);
WRMSR(0x0C0010010, (ULONG)((msr0 & 0x0FFFFFFFF) | 0x000C0000), (ULONG)((msr0>>32) & 0x0FFFFFFFF));
WRMSR(0x026C, (ULONG)((msr1 & 0x0FFFFFFFF) | 0x18181818), (ULONG)(((msr1>>32) & 0x0FFFFFFFF) | 0x18181818));
WRMSR(0x026D, (ULONG)((msr2 & 0x0FFFFFFFF) | 0x18181818), (ULONG)(((msr2>>32) & 0x0FFFFFFFF) | 0x18181818));
WRMSR(0x026E, (ULONG)((msr3 & 0x0FFFFFFFF) | 0x18181818), (ULONG)(((msr3>>32) & 0x0FFFFFFFF) | 0x18181818));
WRMSR(0x026F, (ULONG)((msr4 & 0x0FFFFFFFF) | 0x18181818), (ULONG)(((msr4>>32) & 0x0FFFFFFFF) | 0x18181818));
MappedSh = MapMMIO(PhyAddr, 0x01);
if (MappedSh != NULL) {
* MappedSh = Bt;
UnMapMMIO(MappedSh, 0x01);
} else ns = STATUS_UNSUCCESSFUL;
WRMSR(0x0C0010010, (ULONG)(msr0 & 0x0FFFFFFFF), (ULONG)((msr0>>32) & 0x0FFFFFFFF));
WRMSR(0x026C, (ULONG)(msr1 & 0x0FFFFFFFF), (ULONG)((msr1>>32) & 0x0FFFFFFFF));
WRMSR(0x026D, (ULONG)(msr2 & 0x0FFFFFFFF), (ULONG)((msr2>>32) & 0x0FFFFFFFF));
WRMSR(0x026E, (ULONG)(msr3 & 0x0FFFFFFFF), (ULONG)((msr3>>32) & 0x0FFFFFFFF));
WRMSR(0x026F, (ULONG)(msr4 & 0x0FFFFFFFF), (ULONG)((msr4>>32) & 0x0FFFFFFFF));
STI();
WBINVD();
return ns;
}

NTSTATUS WriteVideo(IN PVOID Buffer)
{
NTSTATUS ns = STATUS_SUCCESS;
ULONG PhyAddr = ((PWRITE_MEM_INFO) Buffer)->PhyAddr;
ULONG Data = ((PWRITE_MEM_INFO) Buffer)->Data;
UCHAR Bt = (UCHAR)(Data) & 0x0FF;
PBYTE MappedSh;
ULONG64 msr0, msr1, msr2, msr3, msr4;
CLI();
msr0 = RDMSR(0x0C0010010);
msr1 = RDMSR(0x0268);
msr2 = RDMSR(0x0269);
msr3 = RDMSR(0x026A);
msr4 = RDMSR(0x026B);
WRMSR(0x0C0010010, (ULONG)((msr0 & 0x0FFFFFFFF) | 0x000C0000), (ULONG)((msr0>>32) & 0x0FFFFFFFF));
WRMSR(0x0268, (ULONG)((msr1 & 0x0FFFFFFFF) | 0x18181818), (ULONG)(((msr1>>32) & 0x0FFFFFFFF) | 0x18181818));
WRMSR(0x0269, (ULONG)((msr2 & 0x0FFFFFFFF) | 0x18181818), (ULONG)(((msr2>>32) & 0x0FFFFFFFF) | 0x18181818));
WRMSR(0x026A, (ULONG)((msr3 & 0x0FFFFFFFF) | 0x18181818), (ULONG)(((msr3>>32) & 0x0FFFFFFFF) | 0x18181818));
WRMSR(0x026B, (ULONG)((msr4 & 0x0FFFFFFFF) | 0x18181818), (ULONG)(((msr4>>32) & 0x0FFFFFFFF) | 0x18181818));
MappedSh = MapMMIO(PhyAddr, 0x01);
if (MappedSh != NULL) {
* MappedSh = Bt;
UnMapMMIO(MappedSh, 0x01);
} else ns = STATUS_UNSUCCESSFUL;
WRMSR(0x0C0010010, (ULONG)(msr0 & 0x0FFFFFFFF), (ULONG)((msr0>>32) & 0x0FFFFFFFF));
WRMSR(0x0268, (ULONG)(msr1 & 0x0FFFFFFFF), (ULONG)((msr1>>32) & 0x0FFFFFFFF));
WRMSR(0x0269, (ULONG)(msr2 & 0x0FFFFFFFF), (ULONG)((msr2>>32) & 0x0FFFFFFFF));
WRMSR(0x026A, (ULONG)(msr3 & 0x0FFFFFFFF), (ULONG)((msr3>>32) & 0x0FFFFFFFF));
WRMSR(0x026B, (ULONG)(msr4 & 0x0FFFFFFFF), (ULONG)((msr4>>32) & 0x0FFFFFFFF));
STI();
WBINVD();
return ns;
}

NTSTATUS EnableWriteICH2(IN PVOID Buffer)
{
NTSTATUS ns = STATUS_SUCCESS;
UCHAR Bt = 0;
OUTD(0x0CF8, 0x08000F8E0);
OUTB(0x0CFF, 0x0FF);
OUTD(0x0CF8, 0x08000F84C);
Bt = INB(0x0CFE);
Bt|=0x01;
OUTD(0x0CF8, 0x08000F84C);
OUTB(0x0CFE, Bt);
return ns;
}

NTSTATUS MapPhyAddress(IN PVOID Buffer, IN BOOLEAN Is64bit)
{
NTSTATUS ns = STATUS_SUCCESS;
ULONG PhyAddr = ((PMAP_MMIO_INFO) Buffer)->PhyAddr;
ULONG _Size = ((PMAP_MMIO_INFO) Buffer)->_Size;
PVOID MappedMMIO = NULL;
PVOID UserAddr = NULL;
ULONG UserAddr32 = 0;
ULONG64 ptr = 0;
PMDL pMdl = NULL;
MappedMMIO = MapMMIO(PhyAddr, _Size);
if (MappedMMIO == NULL) return STATUS_UNSUCCESSFUL;
pMdl = IoAllocateMdl(MappedMMIO, _Size, FALSE, FALSE, NULL);
if (pMdl != NULL) {
	MmBuildMdlForNonPagedPool(pMdl);
	__try {
	UserAddr = MmMapLockedPagesSpecifyCache(pMdl, UserMode, MmNonCached, NULL, FALSE, NormalPagePriority);
	}
	_except(EXCEPTION_EXECUTE_HANDLER) { 
	UserAddr = NULL;
	}
	if (UserAddr != NULL) {
		DbgPrint("Mapped successfully");
	} else {
		IoFreeMdl(pMdl);
		UnMapMMIO(MappedMMIO, _Size);
		return STATUS_UNSUCCESSFUL;
	}
} else return STATUS_UNSUCCESSFUL;
AddMMIOMapping(PhyAddr, _Size, MappedMMIO, pMdl, UserAddr);
if (Is64bit == TRUE) ((PMAP_MMIO_INFO) Buffer)->UserAddr = UserAddr; else {
ptr = (ULONG64)(UserAddr);
UserAddr32 = (ULONG)(ptr) & 0x0FFFFFFFF;
((PMAP_MMIO_INFO) Buffer)->UserAddr32 = UserAddr32;
}
return ns;
}

VOID CleanupMMIOMappings() {
PMAP_MMIO_INFO_LIST Entry;
while (IsListEmpty(&glListHead) != TRUE) {
Entry=CONTAINING_RECORD(RemoveHeadList(&glListHead), MAP_MMIO_INFO_LIST, MMList);
DbgPrint("Deleting entry at address %p... Physical address: %08X Size: %08X", Entry, Entry->Info.PhysAddr, Entry->Info._Size);
MmUnmapLockedPages(Entry->Info.UserAddr, Entry->Info.MDLAddr);
IoFreeMdl(Entry->Info.MDLAddr);
UnMapMMIO(Entry->Info.MappedMMIO, Entry->Info._Size);
ExFreeToNPagedLookasideList(glLookaside, Entry);
}
ExDeleteNPagedLookasideList(glLookaside);
ExFreePool(glLookaside);
return;
}

NTSTATUS UnmapPhyAddress(IN PVOID Buffer, IN BOOLEAN Is64bit)
{
NTSTATUS ns = STATUS_SUCCESS;
MMIO_MAPPINGS_INFO MMapInfo = {0, 0, NULL, NULL, NULL};
ULONG UserAddr32 = 0;
ULONG _Size = 0;
#ifdef _WIN64
ULONG64 ptr;
#else
ULONG ptr;
#endif
PVOID UserAddr = NULL;
if (Is64bit == TRUE) {
UserAddr = ((PUNMAP_MMIO_INFO) Buffer)->Unmapx64.UserAddr; 
_Size = ((PUNMAP_MMIO_INFO) Buffer)->Unmapx64._Size; 
} else {
UserAddr32 = ((PUNMAP_MMIO_INFO) Buffer)->Unmapx86.UserAddr32;
_Size = ((PUNMAP_MMIO_INFO) Buffer)->Unmapx86._Size; 
#ifdef _WIN64
ptr = (ULONG64)(UserAddr32);
#else
ptr = UserAddr32;
#endif
UserAddr = (PVOID)(ptr);
}
if (RemoveMMIOMapping(&MMapInfo, UserAddr, _Size) == TRUE) {
MmUnmapLockedPages(MMapInfo.UserAddr, MMapInfo.MDLAddr);
IoFreeMdl(MMapInfo.MDLAddr);
UnMapMMIO(MMapInfo.MappedMMIO, MMapInfo._Size);
} else ns = STATUS_UNSUCCESSFUL;
return ns;
}

VOID TestEC () {
BYTE i = 0;
BYTE Buffer[256];
DbgPrint("Reading started");
if (ECEnterBurst() != 1) DbgPrint("Unable to enter burst mode");
for (i=0x00; i<=0xff; i++) if (ECReadByte(i, &Buffer[i]) != 1) DbgPrint("Failed at %02X", i);
ECExitBurst();
DbgPrint("Reading done");
//for (i=0x10; i<=0x01f; i++) DbgPrint("Buffer contents: %02X", Buffer[i]);
}

VOID DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
	CleanupMMIOMappings();
	DPRINT("Driver unloaded");

	IoDeleteSymbolicLink(&SymbolicLinkName);
    IoDeleteDevice(deviceObject);

	return;
}

NTSTATUS DriverDispatcher(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp)
{
    PIO_STACK_LOCATION pisl;
	NTSTATUS ns = STATUS_SUCCESS;
	PCSTR Data;

    pisl = IoGetCurrentIrpStackLocation(Irp);
    
    Irp->IoStatus.Information = 0;

    Irp->IoStatus.Status = ns;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return ns;
}

NTSTATUS DrvDeviceIoControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp)
{
    PIO_STACK_LOCATION pisl;
	DWORD dwBytesReturned = 0;
	NTSTATUS ns = STATUS_SUCCESS;

    pisl = IoGetCurrentIrpStackLocation(Irp);
   
    //DbgPrint("%08X", pisl->Parameters.DeviceIoControl.IoControlCode);

	switch (pisl->Parameters.DeviceIoControl.IoControlCode) 
	{
	case IOCTL_READ_PORT_BYTE:
			if ((pisl->Parameters.DeviceIoControl.OutputBufferLength >= 4) && (pisl->Parameters.DeviceIoControl.InputBufferLength >= 4))
			{
			ns = ReadPortByte(Irp->AssociatedIrp.SystemBuffer);
			dwBytesReturned = 4;
			} else ns = STATUS_BUFFER_TOO_SMALL;
			break;
	case IOCTL_READ_PORT_LONG:
			if ((pisl->Parameters.DeviceIoControl.OutputBufferLength >= 4) && (pisl->Parameters.DeviceIoControl.InputBufferLength >= 4))
			{
			//DbgPrint("ReadPortLong");
			ns = ReadPortLong(Irp->AssociatedIrp.SystemBuffer);
			dwBytesReturned = 4;
			} else ns = STATUS_BUFFER_TOO_SMALL;
			break;
	case IOCTL_WRITE_PORT_BYTE:
			if ((pisl->Parameters.DeviceIoControl.InputBufferLength >= 8))
			{
			ns = WritePortByte(Irp->AssociatedIrp.SystemBuffer);
			dwBytesReturned = 0;
			} else ns = STATUS_BUFFER_TOO_SMALL;
			break;
	case IOCTL_WRITE_PORT_LONG:
			if ((pisl->Parameters.DeviceIoControl.InputBufferLength >= 8))
			{
			ns = WritePortLong(Irp->AssociatedIrp.SystemBuffer);
			dwBytesReturned = 0;
			} else ns = STATUS_BUFFER_TOO_SMALL;
			break;
	case IOCTL_RDMSR:
			if ((pisl->Parameters.DeviceIoControl.OutputBufferLength >= 8) && (pisl->Parameters.DeviceIoControl.InputBufferLength >= 4))
			{
			ns = ReadMsr(Irp->AssociatedIrp.SystemBuffer);
			dwBytesReturned = 8;
			} else ns = STATUS_BUFFER_TOO_SMALL;
			break;
	case IOCTL_WRMSR:
			if ((pisl->Parameters.DeviceIoControl.InputBufferLength >= 12))
			{
			ns = WriteMsr(Irp->AssociatedIrp.SystemBuffer);
			dwBytesReturned = 0;
			} else ns = STATUS_BUFFER_TOO_SMALL;
			break;
	case IOCTL_GET_CPU_SPEED:
			if ((pisl->Parameters.DeviceIoControl.OutputBufferLength >= 8) && (pisl->Parameters.DeviceIoControl.InputBufferLength >= 4))
			{
			ns = GetCPUSpeed(Irp->AssociatedIrp.SystemBuffer);
			dwBytesReturned = 8;
			} else ns = STATUS_BUFFER_TOO_SMALL;
			break;
	case IOCTL_WRITE_SHADOW:
			if ((pisl->Parameters.DeviceIoControl.InputBufferLength >= 8))
			{
			ns = WriteShadow(Irp->AssociatedIrp.SystemBuffer);
			dwBytesReturned = 0;
			} else ns = STATUS_BUFFER_TOO_SMALL;
			break;
	case IOCTL_WRITE_VIDEO:
			if ((pisl->Parameters.DeviceIoControl.InputBufferLength >= 8))
			{
			ns = WriteVideo(Irp->AssociatedIrp.SystemBuffer);
			dwBytesReturned = 0;
			} else ns = STATUS_BUFFER_TOO_SMALL;
			break;
	case IOCTL_ENABLE_WRITE_ICH2:
			ns = EnableWriteICH2(Irp->AssociatedIrp.SystemBuffer);
			dwBytesReturned = 0;
			break;
	case IOCTL_ENABLE_WRITE_ICH5:
			ns = EnableWriteICH2(Irp->AssociatedIrp.SystemBuffer);
			dwBytesReturned = 0;
			break;
	case IOCTL_MAP_PHYSICAL_ADDRESS_RANGE:
		#ifndef _WIN64
			if ((pisl->Parameters.DeviceIoControl.OutputBufferLength >= 4) && (pisl->Parameters.DeviceIoControl.InputBufferLength >= 8))
			{
			ns = MapPhyAddress(Irp->AssociatedIrp.SystemBuffer, FALSE);
			dwBytesReturned = 4;
			} else ns = STATUS_BUFFER_TOO_SMALL;
		#else
			if (IoIs32bitProcess(Irp) == TRUE) {
			if ((pisl->Parameters.DeviceIoControl.OutputBufferLength >= 4) && (pisl->Parameters.DeviceIoControl.InputBufferLength >= 8))
			{
			ns = MapPhyAddress(Irp->AssociatedIrp.SystemBuffer, FALSE);
			dwBytesReturned = 4;
			} else ns = STATUS_BUFFER_TOO_SMALL;
			} else {
			if ((pisl->Parameters.DeviceIoControl.OutputBufferLength >= 8) && (pisl->Parameters.DeviceIoControl.InputBufferLength >= 8))
			{
			ns = MapPhyAddress(Irp->AssociatedIrp.SystemBuffer, TRUE);
			dwBytesReturned = 8;
			} else ns = STATUS_BUFFER_TOO_SMALL;
			}
		#endif
			break;
	case IOCTL_UNMAP_PHYSICAL_ADDRESS_RANGE:
		#ifndef _WIN64
			if (pisl->Parameters.DeviceIoControl.InputBufferLength >= 8)
			{
			ns = UnmapPhyAddress(Irp->AssociatedIrp.SystemBuffer, FALSE);
			dwBytesReturned = 0;
			} else ns = STATUS_BUFFER_TOO_SMALL;
		#else
			if (IoIs32bitProcess(Irp) == TRUE) {
			if (pisl->Parameters.DeviceIoControl.InputBufferLength >= 8)
			{
			ns = UnmapPhyAddress(Irp->AssociatedIrp.SystemBuffer, FALSE);
			dwBytesReturned = 0;
			} else ns = STATUS_BUFFER_TOO_SMALL;
			} else {
			if (pisl->Parameters.DeviceIoControl.InputBufferLength >= 12)
			{
			ns = UnmapPhyAddress(Irp->AssociatedIrp.SystemBuffer, TRUE);
			dwBytesReturned = 0;
			} else ns = STATUS_BUFFER_TOO_SMALL;
			}
		#endif
			break;
	default: 
			ns = STATUS_INVALID_DEVICE_REQUEST;
			break;
	}

    Irp->IoStatus.Status = ns;
    Irp->IoStatus.Information = dwBytesReturned;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return ns;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,
                     IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS st;
	PCWSTR dDeviceName       = L"\\Device\\devPCIIDE32";
    PCWSTR dSymbolicLinkName = L"\\DosDevices\\slPCIIDE32";
	PDRIVER_DISPATCH *ppdd;

	DPRINT("Driver loaded");
    
    RtlInitUnicodeString(&DeviceName,       dDeviceName);
    RtlInitUnicodeString(&SymbolicLinkName, dSymbolicLinkName);
    
    st = IoCreateDevice(DriverObject,
		                0,  
		                &DeviceName,  
                        FILE_DEVICE_NULL,
						0,   
                        FALSE,  
						&deviceObject);
    
    if (st == STATUS_SUCCESS)
      st = IoCreateSymbolicLink(&SymbolicLinkName, 
	                            &DeviceName);   

	if (st == STATUS_SUCCESS) 
		glLookaside = ExAllocatePool(NonPagedPool, sizeof(NPAGED_LOOKASIDE_LIST));
		if (glLookaside!=NULL) {
			ExInitializeNPagedLookasideList(glLookaside, NULL, NULL, 0, sizeof(MAP_MMIO_INFO_LIST), 'OIMM', 0);
			InitializeListHead(&glListHead);
			DbgPrint("Lookaside list initialized successfully");
		} else st = STATUS_DEVICE_CONFIGURATION_ERROR;
   
	ppdd = DriverObject->MajorFunction;
            
    	ppdd [IRP_MJ_CREATE] =
	ppdd [IRP_MJ_CLOSE]  = 
   	ppdd [IRP_MJ_WRITE ] = DriverDispatcher;
    	ppdd[IRP_MJ_DEVICE_CONTROL] = DrvDeviceIoControl;

	DriverObject->DriverUnload = DriverUnload;

    return st;
}


