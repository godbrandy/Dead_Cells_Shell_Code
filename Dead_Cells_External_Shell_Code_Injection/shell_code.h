#pragma once
#include "stdafx.h"

struct ShellCode
{
	std::vector<BYTE> god_mode_shell_code
	{	0x83, 0xB8, 0x70, 0x01, 0x00, 0x00, 0x01,		// cmp	DWORD PTR [eax+0170h], 1
		0x0F, 0x85, 0x01, 0x00, 0x00, 0x00,				// jne	$+1 (skip the ret)
		0xC3,											// ret
		0x2B, 0x4D, 0xDC,								// sub	ecx,[ebp-024h]
		0xB9, 0x00, 0x00, 0x00, 0x00,					// mov	ecx,0
		0x89, 0x4D, 0xD0,								// mov	DWORD PTR [ebp-030h],ecx
		0xC3											// ret
	};											
	size_t god_mode_bytes_to_erase{ 6 };
	bool god_mode_active{ false };

	// insert here the shell code and bytes to erase from other cheats
};