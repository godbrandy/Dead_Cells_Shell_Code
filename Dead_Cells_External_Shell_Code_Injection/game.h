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
	DWORD IsRunning() const;
	bool OpenProcHandle();
	HANDLE GetProcHandle() const { return proc_handle; };
	std::wstring GetModuleName() const { return module_name; };

private:
	std::wstring exe_name;
	std::wstring module_name;
	DWORD proc_id{ 0 };
	HANDLE snapshot_proc{ nullptr };
	HANDLE snapshot_module{ nullptr };
	HANDLE proc_handle{ nullptr };
};