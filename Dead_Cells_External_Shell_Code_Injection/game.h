#pragma once
#include "stdafx.h"

class Game
{
public:
	Game(const std::wstring& exe_name, const std::wstring& module_name);
	~Game();

	void Initialize();
	bool GetProcessId();
	MODULEENTRY32 GetModule();
	bool IsRunning();
	bool OpenProcHandle();
	HANDLE GetProcHandle() { return proc_handle; };
	std::wstring GetModuleName() { return module_name; };

private:
	std::wstring exe_name;
	std::wstring module_name;
	DWORD proc_id{ 0 };
	HANDLE snapshot_proc{ nullptr };
	HANDLE snapshot_module{ nullptr };
	HANDLE proc_handle{ nullptr };
};