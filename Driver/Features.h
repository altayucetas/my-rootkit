#pragma once

#include <ntifs.h>
#include <ntdef.h>
#include <minwindef.h>
#include <ntstrsafe.h>

#define HIDE_PROC CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CHNG_PROT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct {
	int pid;
	int protLevel;
} ProtectLevel;

extern "C" {
	NTKERNELAPI NTSTATUS IoCreateDriver (
		IN PUNICODE_STRING DriverName, OPTIONAL
		IN PDRIVER_INITIALIZE InitializationFunction
	);

	NTKERNELAPI PCHAR NTAPI PsGetProcessImageFileName(
			_In_ PEPROCESS Process
		);
}

NTSTATUS InializeOffsets();
//NTSTATUS CreateClose(PDEVICE_OBJECT deviceObject, PIRP irp);
//NTSTATUS IOControl(PDEVICE_OBJECT deviceObject, PIRP irp);
NTSTATUS HideProcess(int pid);
NTSTATUS ChangeProtectionLevel(int pid, BYTE protectionLevel);

typedef struct eprocess_offsets {
	DWORD Token_offset;
	DWORD ActiveProcessLinks_offset;
	DWORD protection_offset;
}exprocess_offsets, * peprocess_offsets;

/*typedef struct protection_levels {
	BYTE PS_PROTECTED_SYSTEM;
	BYTE PS_PROTECTED_WINTCB;
	BYTE PS_PROTECTED_WINDOWS;
	BYTE PS_PROTECTED_AUTHENTICODE;
	BYTE PS_PROTECTED_WINTCB_LIGHT;
	BYTE PS_PROTECTED_WINDOWS_LIGHT;
	BYTE PS_PROTECTED_LSA_LIGHT;
	BYTE PS_PROTECTED_ANTIMALWARE_LIGHT;
	BYTE PS_PROTECTED_AUTHENTICODE_LIGHT;
}protection_level, * Pprotection_levels;*/

/*protection_level global_protection_levels = {
	.PS_PROTECTED_SYSTEM = 0x72,
	.PS_PROTECTED_WINTCB = 0x62,
	.PS_PROTECTED_WINDOWS = 0x52,
	.PS_PROTECTED_AUTHENTICODE = 0x12,
	.PS_PROTECTED_WINTCB_LIGHT = 0x61,
	.PS_PROTECTED_WINDOWS_LIGHT = 0x51,
	.PS_PROTECTED_LSA_LIGHT = 0x41,
	.PS_PROTECTED_ANTIMALWARE_LIGHT = 0x31,
	.PS_PROTECTED_AUTHENTICODE_LIGHT = 0x11
};*/

