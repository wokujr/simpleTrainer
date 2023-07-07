#include <iostream>
#include <TlHelp32.h>
#include "mem.h"

DWORD WINAPI HackThread(HMODULE hModule)
{
	AllocConsole();

	FILE * f;
	freopen_s(&f, "CONOUT$","w", stdout);
	std::cout << "Running...\n";

	uintptr_t moduleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(L"ac_client.exe"));

	bool bHealth = false, bAmmo = false, bRecoil = false;

	//hack look
	while (true)
	{
		//key input
		if (GetAsyncKeyState(VK_END) & 1)
		{
			break;
		}

		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			bHealth = true;

		}

		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			bAmmo = true;
		}

		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			bRecoil = true;
			if (bRecoil)
			{
				//nop code
				mem::Nop(reinterpret_cast<BYTE*>(moduleBase + 0x63786),10);

			}
			else
			{
				// write back the original
				// 50 8D 4C 24 1C 51 8B CE FF D2 the original
				mem::Patch(reinterpret_cast<BYTE*>(moduleBase + 0x63786), 
					(BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10);
			}
		}

		uintptr_t* localPlayerPtr = reinterpret_cast<uintptr_t*>(moduleBase + 0x10f4f4);

		if (localPlayerPtr)
		{
			if (bHealth)
			{
				*reinterpret_cast<int*>(*localPlayerPtr + 0xf8) = 9999;
			}
			if (bAmmo)
			{
				uintptr_t ammoAdr = mem::findDMAAddr(moduleBase + 0x10f4f4, {0x374, 0x14, 0x0});
				int* ammo = reinterpret_cast<int*>(ammoAdr);
				*ammo = 9999;

				*reinterpret_cast<int*>(mem::findDMAAddr(moduleBase + 0x10f4f4, {0x374, 0x14, 0x0})) = 9999;
			}
		}
		Sleep(5);
	}

	//cleanup & eject
	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);

}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			{
				CloseHandle(CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(HackThread), hModule, 0, nullptr));
			}
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}
