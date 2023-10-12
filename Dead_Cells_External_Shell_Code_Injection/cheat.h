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
	BYTE* hook_addr{ nullptr };
	bool og_bytes_saved{ false };
	uintptr_t alloc_addr{ 0 };
	size_t len;
	std::vector<BYTE> buffer;
	std::vector<BYTE> shell_code;
	Game& game;
	Pattern& pattern;
};