#pragma once
#include "stdafx.h"

class Cheat
{
public:
	Cheat(Game& game, Pattern& pattern, const std::vector<BYTE>& shell_code, size_t len);
	~Cheat();

	void WriteMemory();
	void NopMemory();
	void RestoreMemory();
	bool FindHookAddr();
	bool IsBufferEmpty() { return buffer.empty(); };

private:
	class VirtualDeallocate
	{
	public:
		VirtualDeallocate(HANDLE proc_handle)
			:
			proc_handle{ proc_handle }
		{}
		VirtualDeallocate() = default;
		void operator()(void* ptr) const
		{
			if (ptr)
			{
				VirtualFreeEx(proc_handle, ptr, 0, MEM_RELEASE);
			}
		}
	private:
		HANDLE proc_handle;
	};

private:
	BYTE* hook_addr{ nullptr };
	bool og_bytes_saved{ false };
	std::unique_ptr<uintptr_t, VirtualDeallocate> alloc_addr;
	size_t len;
	std::vector<BYTE> buffer;
	std::vector<BYTE> shell_code;
	Game& game;
	Pattern& pattern;
};