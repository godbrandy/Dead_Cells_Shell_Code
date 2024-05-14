#include "stdafx.h"

Cheat::Cheat(Game& game, Pattern& pattern, const std::vector<BYTE>& shell_code, size_t len)
	: game{ game },
	pattern{ pattern },
	shell_code{ shell_code },
	len{ len },
	buffer(len)	
{
	// Initialize the class
}

Cheat::~Cheat() 
{
	// Destroy the class
}
void Cheat::WriteMemory()
{
	DWORD old_protection{};
	BYTE call_code{ 0xE8 };

	// Allocated a region of memory in the target process
	alloc_addr.reset((uintptr_t*)VirtualAllocEx(game.GetProcHandle(), NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));

	// if on 64-bit mode, i need to subtract 9 to fine the relative address (8-byte address + 1 call opcode)
#ifdef  _WIN64
	size_t IP_minus{ 9 };
	// if on 32-bit mode, i need to subtract 5 to fine the relative address (4-byte address + 1 call opcode)
#else
	size_t IP_minus{ 5 };
#endif //  _WIN64

	// Find the distance between the alloc_addr and the hook_addr
	uintptr_t relative_addr = (uintptr_t)alloc_addr.get() - (uintptr_t)hook_addr - IP_minus;

	if (relative_addr && alloc_addr)
	{
		VirtualProtectEx(game.GetProcHandle(), hook_addr, len, PAGE_EXECUTE_READWRITE, &old_protection);

		// Write the 0xE8 code of the call
		WriteProcessMemory(game.GetProcHandle(), hook_addr, &call_code, sizeof(call_code), nullptr);

		// Write the rest of the address (4 bytes in this case)
		WriteProcessMemory(game.GetProcHandle(), hook_addr + 1, &relative_addr, sizeof(relative_addr), nullptr);

		// Write the shell code to the region of memory allocated earlier
		WriteProcessMemory(game.GetProcHandle(), alloc_addr.get(), shell_code.data(), shell_code.size(), nullptr);

		VirtualProtectEx(game.GetProcHandle(), hook_addr, len, old_protection, &old_protection);
	}
}

void Cheat::NopMemory()
{
	DWORD old_protection{};

	// Create a vector of nops
	std::vector<BYTE> nop(len, 0x90);

	// Change the protection of the region
	VirtualProtectEx(game.GetProcHandle(), hook_addr, len, PAGE_EXECUTE_READWRITE, &old_protection);

	// Save the bytes that will be erased in the buffer
	if (!og_bytes_saved)
	{
		ReadProcessMemory(game.GetProcHandle(), hook_addr, buffer.data(), len, nullptr);
		og_bytes_saved = true;
	}

	// Erase the bytes
	WriteProcessMemory(game.GetProcHandle(), hook_addr, nop.data(), len, nullptr);

	// Restore the old protection
	VirtualProtectEx(game.GetProcHandle(), hook_addr, len, old_protection, &old_protection);
}

void Cheat::RestoreMemory()
{
	DWORD old_protection{};

	// Change the protection of the region
	VirtualProtectEx(game.GetProcHandle(), hook_addr, len, PAGE_EXECUTE_READWRITE, &old_protection);

	// Write back the original bytes which were stored in the buffer
	WriteProcessMemory(game.GetProcHandle(), hook_addr, buffer.data(), len, nullptr);

	// Restore the old protection
	VirtualProtectEx(game.GetProcHandle(), hook_addr, len, old_protection, &old_protection);
}

uintptr_t Cheat::FindHookAddr()
{
	// Find the address of the first byte of the pattern
	hook_addr = (BYTE*)pattern.MemoryScan();
	return (uintptr_t)hook_addr;
}

