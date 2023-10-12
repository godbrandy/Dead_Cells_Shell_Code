// Dead_Cells_External_Shell_Code_Injection.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"

int main()
{
	Game game{ L"deadcells.exe", L"" };
	ShellCode shell;
	Pattern OHKO_pattern{ "\x2B\x4D\xDC\x89\x4D\xD0", "xxxxxx", game, (char*)0x10000000, (char*)0x20000000 };
	Cheat OHKO_cheat{ game, OHKO_pattern, shell.OHKO_shell_code, shell.OHKO_bytes_to_erase };

	while (!game.IsRunning())
	{
		game.Initialize();
		Sleep(5);
	}

	// Sleep for 2 seconds to give the game enough time to allocate all the memory
	Sleep(2000);

	std::print("Inizialization complete.\n");

	assert(OHKO_cheat.FindHookAddr());
	if (OHKO_cheat.FindHookAddr())
	{
		std::print("Address of OHKO found.\n");
	}

	while (game.IsRunning())
	{
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)

		{
			OHKO_cheat.NopMemory();
			OHKO_cheat.WriteMemory();
		}

		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			if (!OHKO_cheat.IsBufferEmpty())
			{
				OHKO_cheat.RestoreMemory();
			}
		}

		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			break;
		}

		Sleep(5);
	}

	return 0;
}