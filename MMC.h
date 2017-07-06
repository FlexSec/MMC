#pragma once
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <vector>
#include <string.h>
#include <TlHelp32.h>
#include <wchar.h>

using namespace std;
class MMC
{
private: // Private members: We are going to setup some getters later on!
	HANDLE m_hProcess; // The HANDLE to the process to attach
	DWORD m_dwProcessId; // The Process Id of the process to attach
	std::vector<MODULEENTRY32> m_Modules; // std::vector containing all the modules we grab from the process

public:
	// Attach to a process based on strProcessName
	// Returns true on success, false on failure
	bool Attach(const wchar_t*& ProcessName)
	{
		// First of all we create a snapshot handle specific for processes
		// (notice the usage of TH32CS_SNAPPROCESS) so we are able to call Process32First/Next
		// Remeber to close it when you don't use it anymore!
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		// Check if the snapshot created is valid
		if (hSnapshot == INVALID_HANDLE_VALUE) return false;

		// Create the helper struct that will contain all the infos about the current process
		// while we loop through all the running processes
		PROCESSENTRY32 ProcEntry;
		// Remember to set the dwSize member of ProcEntry to sizeof(PROCESSENTRY32)
		ProcEntry.dwSize = sizeof(PROCESSENTRY32);

		// Call Process32First
		if (Process32First(hSnapshot, &ProcEntry))
		{
			// Notice that you have to enable Multi-Byte character set in order
			// to avoid converting everything.
			// strcmp is not the only way to compare 2 strings ofc, work with your imagination
			if (!wcscmp(ProcEntry.szExeFile, ProcessName))
			{
				// If we are here it means that the process has been found, we can
				// open an handle to it and return it
				// But first of all we have to close the snapshot handle!
				CloseHandle(hSnapshot);
				// Open an handle and set the m_hProcess member using OpenProcess
				// (Notice the usage of PROCESS_ALL_ACCESS flag in order to grant read/write privileges)
				m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcEntry.th32ProcessID);
				// Store the process id into m_dwProcessId
				m_dwProcessId = ProcEntry.th32ProcessID;
				// Return true meaning success
				return true;
			}
		}
		else
		{
			// If the Process32First call failed, it means that there is no
			// process running in the first place, we can return directly.
			CloseHandle(hSnapshot);
			return false;
		}

		// If we are here it means that the Process32First call returned TRUE, but the first process
		// wasn't the process that we were searching for. Now we can loop through the processes
		// using Process32Next
		while (Process32Next(hSnapshot, &ProcEntry))
		{
			// We do the same check we did for Process32First
			if (!wcscmp(ProcEntry.szExeFile, ProcessName))
			{
				// If we are here it means that the process has been found, we can
				// open an handle to it and set the m_hProcess member using OpenProcess
				// (Notice the usage of PROCESS_ALL_ACCESS flag in order to grant read/write privileges)
				m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcEntry.th32ProcessID);
				// Store the process id into m_dwProcessId
				m_dwProcessId = ProcEntry.th32ProcessID);
				// Return true meaning success
				return true;
			}
		}
		// Continue loop while the Process32Next call returns TRUE meaning that there are still processes to check

		// If we are here it means that the process has not been found or that there are no processes to scan for anymore.
		// We can close the snapshot handle and return false.
		CloseHandle(hSnapshot);
		return false;
	}

	// Grabs a module and adds it to m_Modules if found based on strModuleName
	// Returns true on success, false on failure
	bool GrabModule(const wchar_t*& strModuleName)
	{
		// First of all we create a snapshot handle specific for modules
		// (notice the usage of TH32CS_SNAPMODULE) so we are able to call Module32First/Next
		// Remeber to close it when you don't use it anymore!
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_dwProcessId);
		// Check if the snapshot created is valid
		if (hSnapshot == INVALID_HANDLE_VALUE) return false;

		// Create the helper struct that will contain all the infos about the current module
		// while we loop through all the loaded modules
		MODULEENTRY32 ModEntry;
		// Remember to set the dwSize member of ModEntry to sizeof(MODULEENTRY32)
		ModEntry.dwSize = sizeof(MODULEENTRY32);

		// Call Module32First
		if (Module32First(hSnapshot, &ModEntry))
		{
			// Notice that you have to enable Multi-Byte character set in order
			// to avoid converting everything.
			// strcmp is not the only way to compare 2 strings ofc, work with your imagination
			if (!wcscmp(ModEntry.szModule, strModuleName))
			{
				// If we are here it means that the module has been found, we can add the module to the vector
				// But first of all we have to close the snapshot handle!
				CloseHandle(hSnapshot);
				// Add ModEntry to the m_Modules vector
				m_Modules.push_back(ModEntry); // You can add a check here to see if the module is already there ;)
											   // Return true meaning success
				return true;
			}
		}
		else
		{
			// If the Process32First call failed, it means that there is no
			// process running in the first place, we can return directly.
			CloseHandle(hSnapshot);
			return false;
		}

		// If we are here it means that the Module32First call returned TRUE, but the first module
		// wasn't the module that we were searching for. Now we can loop through the modules
		// using Module32Next
		while (Module32Next(hSnapshot, &ModEntry))
		{
			// We do the same check we did for Module32First
			if (!wcscmp(ModEntry.szModule, strModuleName))
			{
				// If we are here it means that the module has been found, we can add the module to the vector
				// But first of all we have to close the snapshot handle!
				CloseHandle(hSnapshot);
				// Add ModEntry to the m_Modules vector
				m_Modules.push_back(ModEntry); // You can add a check here to see if the module is already there ;)
											   // Return true meaning success
				return true;
			}
		}
		// Continue loop while the Module32Next call returns TRUE meaning that there are still modules to check

		// If we are here it means that the module has not been found or that there are no modules to scan for anymore.
		// We can close the snapshot handle and return false.
		CloseHandle(hSnapshot);
		return false;
	}

	// RPM/WPM wrappers

	// Read a value from memory and put it in Value
	// Returns true on success, false on failure
	template <class T>
	inline bool Read(DWORD dwAddress, T& Value)
	{
		return ReadProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(dwAddress), Value, sizeof(T), NULL) == TRUE;
	}
	// Write a Value in memory
	// Returns true on success, false on failure
	template <class T>
	inline bool Write(DWORD dwAddress, const T& Value)
	{
		return WriteProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(dwAddress), Value, sizeof(T), NULL) == TRUE;
	}

	// Getters
	HANDLE GetHandle() { return m_hProcess; }
	DWORD GetProcId() { return m_dwProcessId; }
	std::vector<MODULEENTRY32> GetModules() { return m_Modules; }
};