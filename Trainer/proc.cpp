#include "proc.h"

DWORD GetProcId(const wchar_t* procName)
{
	DWORD ProcId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);
		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					ProcId = procEntry.th32ProcessID;
					break;
				}
			}
			while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return ProcId;
}

uintptr_t GetModuleBaseAddress(DWORD procID, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = reinterpret_cast<uintptr_t>(modEntry.modBaseAddr);
					break;
				}
			while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

uintptr_t findDMAAddr(HANDLE hProc, uintptr_t ptr, std::vector<unsigned> offset)
{
	uintptr_t addr = ptr;
	for (unsigned int i : offset)
	{
		ReadProcessMemory(hProc, reinterpret_cast<BYTE*>(addr), &addr, sizeof(addr), nullptr);
		addr += i;
	}
	return addr;
}

template<typename T>
T readMemory(HANDLE procHandle, LPVOID addr)
{
	T val;
	ReadProcessMemory(procHandle, addr, &val, sizeof(T), nullptr);
	return val;
}

template<typename T>
void writeMemory(HANDLE procHandle, LPVOID addr, T val)
{
	WriteProcessMemory(procHandle, addr, &val, sizeof(T), nullptr);
}