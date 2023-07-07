#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "memExternal.h"

void Ammo()
{
	// get process id
	DWORD procId = GetProcId(L"ac_client.exe");

	// get module base address
	uintptr_t moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

	// get handle to process
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	//Resolve base address of the pointer chain
	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x10f4f4;
	std::cout << "Dynamic ptr base addr = " << "0x" << std::hex << dynamicPtrBaseAddr << std::endl;

	//resolve ammo pointer chain
	std::vector<unsigned int> ammoOffset = { 0x374, 0x14, 0x0 };

	uintptr_t ammoAddr = findDMAAddr(hProcess, dynamicPtrBaseAddr, ammoOffset);
	std::cout << "ammoAddr = " << "0x" << std::hex << ammoAddr << std::endl;

	//read ammo value
	int ammoValue{};
	ReadProcessMemory(hProcess, reinterpret_cast<BYTE*>(ammoAddr), &ammoValue, sizeof(ammoValue), nullptr);
	std::cout << "current ammo = " << std::dec << ammoValue << std::endl;

	//write to memory
	int newAmmo{ 200 };
	WriteProcessMemory(hProcess, reinterpret_cast<BYTE*>(ammoAddr), &newAmmo, sizeof(newAmmo), nullptr);

	//readprocess again
	ReadProcessMemory(hProcess, reinterpret_cast<BYTE*>(newAmmo), &newAmmo, sizeof(newAmmo), nullptr);
	std::cout << "new ammo = " << std::dec << newAmmo << std::endl;
};

void Health()
{
	//get process id
	DWORD procId = GetProcId(L"ac_client.exe");

	// get module base address
	uintptr_t moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

	// get handle to process
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

	//Resolve base address of the pointer chain
	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x10f4f4;
	std::cout << "Dynamic ptr base addr = " << "0x" << std::hex << dynamicPtrBaseAddr << std::endl;

	//resolve ammo pointer chain
	std::vector<unsigned int> healthOffset = { 0xF8 };

	uintptr_t healthAddr = findDMAAddr(hProcess, dynamicPtrBaseAddr, healthOffset);
	std::cout << "ammoAddr = " << "0x" << std::hex << healthAddr << std::endl;

	int healthValue{};
	ReadProcessMemory(hProcess, reinterpret_cast<BYTE*>(healthAddr), &healthValue, sizeof(healthValue), nullptr);
	std::cout << "current health = " << std::dec << healthValue << std::endl;

	int newHealth{ 1000 };
	WriteProcessMemory(hProcess, reinterpret_cast<BYTE*>(healthAddr), &newHealth, sizeof(healthValue), nullptr);
	std::cout << "new health = " << std::dec << newHealth << std::endl;


};

void ExV()
{
	HANDLE hProcess{};
	uintptr_t moduleBase{}, localPlayer{}, healthAddr{};
	bool bHealth = false, bAmmo = false, bRecoil = false;

	const int newValue = 9999;
	DWORD procId = GetProcId(L"ac_client.exe");
	if (procId)
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

		moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

		localPlayer = moduleBase + 0x10f4f4;

		healthAddr = findDMAAddr(hProcess, localPlayer, { 0xf8 });

	}
	else
	{
		std::cout << "process not found!\n";
		getchar();
	}

	DWORD dwExit{};

	while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
	{
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			bHealth = !bHealth;
		}

		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			bAmmo = !bHealth;
			if (bAmmo)
			{
				// FF 06 = inc[esi] FF 06 is bytes 
				memEx::PatchEx(reinterpret_cast<BYTE*>(moduleBase + 0x637e9), (BYTE*)"\xFF\x06", 2, hProcess);
			}
			else
			{
				// FF 0E = dec[esi] 
				memEx::PatchEx(reinterpret_cast<BYTE*>(moduleBase + 0x637e9), (BYTE*)"\xFF\x0E", 2, hProcess);
			}
		}

		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			bRecoil = !bRecoil;
			if (bRecoil)
			{
				memEx::NopEx(reinterpret_cast<BYTE*>(moduleBase + 0x63786), 10, hProcess);
			}
			else
			{
				memEx::PatchEx(reinterpret_cast<BYTE*>(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10, hProcess);
			}
		}

		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			
		}

		//continuous wirte or frezze
		if (bHealth)
		{
			memEx::PatchEx(reinterpret_cast<BYTE*>(healthAddr), (BYTE*)(&newValue), sizeof(newValue), hProcess);
		}
		Sleep(10);
	}
	std::cout << "PROCESS NOT FOUND!!!" << std::endl;
	getchar();
}

/*int main()
{

	
	return 0;

}*/


