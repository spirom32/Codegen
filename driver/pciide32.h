#include <ntddk.h>

#define DEBUG

#ifdef DEBUG
  #define DPRINT DbgPrint
#else
  #define DPRINT
#endif

#define IOCTL_READ_PORT_LONG			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_PORT_BYTE			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, 0, 0)
#define IOCTL_MAP_PHYSICAL_ADDRESS_RANGE	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, 0, 0)
#define IOCTL_UNMAP_PHYSICAL_ADDRESS_RANGE	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, 0, 0)
#define IOCTL_ENABLE_WRITE_ICH5			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, 0, 0)
#define IOCTL_ENABLE_WRITE_ICH2			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, 0, 0)
#define IOCTL_WRITE_PORT_LONG			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, 0, 0)
#define IOCTL_WRITE_VIDEO                                      CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, 0, 0)
#define IOCTL_WRITE_SHADOW                                  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x808, 0, 0)
#define IOCTL_GET_CPU_SPEED                                  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x809, 0, 0)
#define IOCTL_RDMSR                                             	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80A, 0, 0)
#define IOCTL_WRMSR                                             	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80B, 0, 0)
#define IOCTL_READ_PORT_BYTE			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_READ_EC_BYTE			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80D, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_EC_BYTE			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EC_COMMAND			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_READ_EC_DATA			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_EC_DATA			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef UCHAR BYTE;
typedef USHORT WORD;
typedef ULONG DWORD;
typedef BYTE * PBYTE;
typedef WORD * PWORD;
typedef DWORD * PDWORD;

typedef struct RDMSR_INFO {
	union {
		struct {
			ULONG MsrNum;
			ULONG TargetProcessor;
		};
		struct {
			union {
				struct {
					ULONG LoPart;
					ULONG HiPart;
				};
				ULONG64 QuadPart;
			};
		} RMANS;
	};
} RdmsrInfo, *PRDMSR_INFO, *PRdmsrInfo;

typedef struct WRMSR_INFO {
ULONG MsrNum;
union {
	struct {
		ULONG MDLoPart;
		ULONG MDHiPart;
	};
	ULONG64 QuadPart;
};
ULONG TargetProcessor;
} WrmsrInfo, *PWRMSR_INFO, *PWrmsrInfo;

typedef struct READ_PORT_INFO {
	union {
		ULONG PortNum;
		union {
			BYTE RByte;
			WORD RWord;
			DWORD RDword;
		};
	};
} ReadPortInfo, * PREAD_PORT_INFO, * PReadPortInfo;

typedef struct WRITE_PORT_INFO {
ULONG PortNum;
union {
	BYTE WByte;
	WORD WWord;
	DWORD WDword;
};
} WritePortInfo, * PWRITE_PORT_INFO, * PWritePortInfo;

typedef struct WRITE_MEM_INFO {
ULONG PhyAddr;
ULONG Data;
} WriteMemInfo, * PWRITE_MEM_INFO, * PWriteMemInfo;

typedef struct _MAP_MMIO_INFO {
union {
	struct {
		ULONG PhyAddr; 
		ULONG _Size; 
	};
	PVOID UserAddr;
	ULONG UserAddr32;
};
} MAP_MMIO_INFO, MapMmioInfo, * PMAP_MMIO_INFO, * PMapMmioInfo;

typedef struct _UNMAP_MMIO_INFO {
union {
	struct {
		PVOID UserAddr;
		ULONG _Size;
	} Unmapx64;
	struct {
		ULONG UserAddr32;
		ULONG _Size;
	} Unmapx86;
};
} UNMAP_MMIO_INFO, UnmapMmioInfo, * PUNMAP_MMIO_INFO, * PUnmapMmioInfo;

typedef struct GCS_INFO {
union {
	ULARGE_INTEGER Speed;
	ULONG TargetCPU;
};
} GCSInfo, * PGCS_INFO, * PGCSInfo;

typedef struct _MMIO_MAPPINGS_INFO {
ULONG PhysAddr;
ULONG _Size;
PVOID MappedMMIO;
PVOID UserAddr;
PMDL MDLAddr;
} MMIO_MAPPINGS_INFO, MMInfo, * PMMIO_MAPPINGS_INFO, * PMMInfo;

typedef struct _MAP_MMIO_INFO_LIST {
	LIST_ENTRY MMList;
	MMIO_MAPPINGS_INFO Info;
} MAP_MMIO_INFO_LIST, MMInfoList, * PMAP_MMIO_INFO_LIST, * PMMInfoList;

typedef struct _READ_EC_INFO {
	union {	
		BYTE Offs;
		BYTE Data;
	};
} READ_EC_INFO, ReadECInfo, * PREAD_EC_INFO, * PReadECInfo;

typedef struct _WRITE_EC_INFO {
	BYTE Offs;
	BYTE Data;
} WRITE_EC_INFO, WriteECInfo, * PWRITE_EC_INFO, * PWriteECInfo;

typedef struct _READ_EC_DATA_INFO {
	union {	
		struct {
			BYTE Offs;
			BYTE Len;
		};
		BYTE Data[256];
	};
} READ_EC_DATA_INFO, ReadECDataInfo, * PREAD_EC_DATA_INFO, * PReadECDataInfo;

typedef struct _WRITE_EC_DATA_INFO {
	BYTE Offs;
	BYTE Len;
	BYTE Data[256];
} WRITE_EC_DATA_INFO, WriteECDataInfo, * PWRITE_EC_DATA_INFO, * PWriteECDataInfo;

VOID DriverUnload (IN PDRIVER_OBJECT DriverObject);

NTSTATUS DriverDispatcher(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp);

NTSTATUS DrvDeviceIoControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp);

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,
                     IN PUNICODE_STRING RegistryPath);