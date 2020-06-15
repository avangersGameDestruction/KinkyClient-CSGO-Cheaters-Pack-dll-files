#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

DWORD GetProcId(const char* processID) {
	DWORD ID = 0;
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 Entry;
		Entry.dwSize = sizeof(Entry);

		if (Process32First(snap, &Entry)) {
			do {
				if (!_stricmp(Entry.szExeFile, processID)) {
					ID = Entry.th32ProcessID;
					break;
				}
			} while (Process32Next(snap, &Entry));
		}
	}
	CloseHandle(snap);
	return ID;
}

int main()
{
	const char* Path = "C:\\KinkyClient\\kinkyclient.dll";
	const char* Game = "csgo.exe";
	DWORD ID = 0;

	while (!ID) {
		ID = GetProcId(Game);
		Sleep(30);
	}
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, 0, ID);
	if (process && process != INVALID_HANDLE_VALUE) {
		void* location = VirtualAllocEx(process, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		WriteProcessMemory(process, location, Path, strlen(Path) + 1, 0);
		HANDLE thread = CreateRemoteThread(process, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, location, 0, 0);
		if (thread) {
			CloseHandle(thread);
		}
	}
	if (process) {
		CloseHandle(process);
	}
}