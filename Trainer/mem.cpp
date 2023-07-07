#include "mem.h"

void mem::Patch(BYTE* dst, BYTE* src, unsigned int size)
{
	DWORD oldProtec;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtec);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldProtec, &oldProtec);

}

void mem::Nop(BYTE* dst, unsigned size)
{
	DWORD oldProtec;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtec);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldProtec, &oldProtec);
}

uintptr_t mem::findDMAAddr(uintptr_t ptr, std::vector<unsigned> offset)
{
	uintptr_t addr = ptr;
	for (unsigned int i : offset)
	{
		addr = *reinterpret_cast<uintptr_t*>(addr);
		addr += offset[i];
	}
	return addr;
}