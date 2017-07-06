#pragma once
#include <MMC.h>
#include <Windows.h>
#include "targetver.h"
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <vector>
#include <TlHelp32.h>
#include <wchar.h>

using namespace std;

int MMC()
{
	// Init members
	m_hProcess = INVALID_HANDLE_VALUE;
	m_dwProcessId = 0;
	// Just for safety, I clear out the modules vector
	m_Modules.clear();
}

// Default constructor: won't attach to any process

// This constructor will attach to a specific process (default CS:GO)
int MMC(const std::string& ProcessName = "csgo.exe")
{
	// Init members
	m_hProcess = INVALID_HANDLE_VALUE;
	m_dwProcessId = 0;
	// Just for safety, I clear out the modules vector
	m_Modules.clear();
	// Attach and throw if the function failed so we can manage the fail
	if (!Attach(ProcessName))
		throw;
	
};

