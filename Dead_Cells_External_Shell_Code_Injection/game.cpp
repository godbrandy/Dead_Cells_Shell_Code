#include "stdafx.h"

Game::Game(const std::wstring& exe_name, const std::wstring& module_name)
	: exe_name{ exe_name },
	module_name{ module_name }
{
	// Initialize the class
}

Game::~Game()
{
	// Close the handles if there are any open
	if (proc_handle)
	{
		CloseHandle(proc_handle);
	}
	if (snapshot_proc)
	{
		CloseHandle(snapshot_proc);
	}
	if (snapshot_module)
	{
		CloseHandle(snapshot_module);
	}
}

void Game::Initialize()
{
	// Initialize the game by opening a handle to the process
	GetProcessId();
	OpenProcHandle();
}

bool Game::GetProcessId()
{

	// Create an empty structor of type PROCESSENTRY32
	PROCESSENTRY32 process_entry = { sizeof(PROCESSENTRY32) };

	// Create a snapshot of the processes
	snapshot_proc = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (snapshot_proc == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Failed to create snapshot.\n";
		return false;
	}

	else
	{
		// Start looping through the processes to find the one of interest
		if (Process32First(snapshot_proc, &process_entry))
		{
			do
			{
				// Compare the name of the process with the name given by the user
				if (_wcsicmp(process_entry.szExeFile, exe_name.data()) == 0)
				{
					// Return the process ID if the names match
					proc_id = process_entry.th32ProcessID;
					return true;
				}

			} while (Process32Next(snapshot_proc, &process_entry));
		}
	}

	return false;
}

MODULEENTRY32 Game::GetModule()
{
	// Create an empty structure of type MODULEENTRY32
	MODULEENTRY32 module_entry{ sizeof(MODULEENTRY32) };

	// Create a snapshot of 64-bit and 32-bit modules of the process of interest
	snapshot_module = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, proc_id);

	if (snapshot_module == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Failed to create snapshot of the modules.\n";
		return module_entry;
	}

	else
	{
		// Start looping through the modules to find the one of interest
		if (Module32First(snapshot_module, &module_entry))
		{
			do
			{
				// Compare the name of the modules to the one given by the user
				if (_wcsicmp(module_entry.szModule, module_name.data()) == 0)
				{
					// Return the module structure if the names match
					return module_entry;
				}

			} while (Module32Next(snapshot_module, &module_entry));
		}
	}

	std::wcout << "No module with the name '" << proc_id << "' was found.\n";
	return module_entry;
}

bool Game::IsRunning()
{
	return proc_id;
}

bool Game::OpenProcHandle()
{
	proc_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc_id);

	if (proc_handle)
	{
		return true;
	}
	else
	{
		return false;
	}
}
