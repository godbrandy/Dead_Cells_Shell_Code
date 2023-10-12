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

	if (alloc_addr)
	{
		VirtualFreeEx(game.GetProcHandle(), (uintptr_t*)alloc_addr, 0, MEM_RELEASE);
	}
	// Allocated a region of memory in the target process
	alloc_addr = (uintptr_t)VirtualAllocEx(game.GetProcHandle(), NULL, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	// Find the distance between the alloc_addr and the hook_addr
	uintptr_t relative_addr = alloc_addr - (uintptr_t)hook_addr - 5;

	if (relative_addr && alloc_addr)
	{
		VirtualProtectEx(game.GetProcHandle(), hook_addr, len, PAGE_EXECUTE_READWRITE, &old_protection);

		// Write the 0xE8 code of the call
		WriteProcessMemory(game.GetProcHandle(), hook_addr, &call_code, sizeof(call_code), nullptr);

		// Write the rest of the address (4 bytes in this case)
		WriteProcessMemory(game.GetProcHandle(), hook_addr + 1, &relative_addr, sizeof(relative_addr), nullptr);

		// Write the shell code to the region of memory allocated earlier
		WriteProcessMemory(game.GetProcHandle(), (uintptr_t*)alloc_addr, shell_code.data(), shell_code.size(), nullptr);

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

bool Cheat::FindHookAddr()
{
	// Find the address of the first byte of the pattern
	return hook_addr = (BYTE*)pattern.MemoryScan();
}

