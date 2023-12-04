// Dead_Cells_External_Shell_Code_Injection.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "stdafx.h"

int main()
{
	Game game{ L"deadcells.exe", L"" };
	ShellCode shell;
	Pattern god_mode_pattern{ "\x2B\x4D\xDC\x89\x4D\xD0", "xxxxxx", game, (char*)0x10000000, (char*)0x20000000 };
	Cheat god_mode_cheat{ game, god_mode_pattern, shell.god_mode_shell_code, shell.god_mode_bytes_to_erase };

	while (!game.IsRunning())
	{
		game.Initialize();
		Sleep(5);
	}

	// Sleep for 2 seconds to give the game enough time to allocate all the memory
	Sleep(2000);

	std::print("Inizialization complete.\n");
	
	assert(god_mode_cheat.FindHookAddr());
	if (god_mode_cheat.FindHookAddr())
	{
		std::print("GodMode's hook address found.\nPress NUM Pad 1 to activate.\n");
	}

	while (game.IsRunning())
	{
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			shell.god_mode_active = !shell.god_mode_active;

			if (shell.god_mode_active)
			{
				god_mode_cheat.NopMemory();
				god_mode_cheat.WriteMemory();
				std::print("God Mode Activated.\n");
			}

			if (!shell.god_mode_active)
			{
				if (!god_mode_cheat.IsBufferEmpty())
				{
					god_mode_cheat.RestoreMemory();
					std::print("God Mode Deactivated.\n");
				}
			}
		}

		if (GetAsyncKeyState(VK_NUMPAD2) & 1)
		{
			
		}

		if (GetAsyncKeyState(VK_NUMPAD3) & 1)
		{
			break;
		}

		Sleep(5);
	}

	return 0;
}