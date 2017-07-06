#pragma once
#include <Windows.h>
#include <TlHelp32.h>

class MMC
{
private:
	
	DWORD dwPID;
	HANDLE hProcess;
	


public:
	MODULEENTRY32 ClientDLL;
	MODULEENTRY32 EngineDLL;
	DWORD ClientDLL_Base, ClientDLL_size;
	DWORD EngineDLL_Base, EngineDLL_size;
	


	bool Attach(char* ProcessName)
	{
		HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		const WCHAR* procNameChar;
		int nChars = MultiByteToWideChar(CP_ACP, 0, ProcessName, -1, NULL, 0);
		procNameChar = new WCHAR[nChars];
		MultiByteToWideChar(CP_ACP, 0, ProcessName, -1, (LPWSTR)procNameChar, nChars);

		do
			if (!wcscmp(procEntry.szExeFile, procNameChar))
			{
				this->dwPID = procEntry.th32ProcessID;
				CloseHandle(hPID);
				this->hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->dwPID);
				return true;
			}
		while (Process32Next(hPID, &procEntry));

		CloseHandle(hPID);
		return false;
}

	MODULEENTRY32 GetModule(char* ModuleName)
	{
		HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
		MODULEENTRY32 mENTRY;
		mENTRY.dwSize = sizeof(mENTRY);

		const WCHAR *modNameChar;
		int nChars = MultiByteToWideChar(CP_ACP, 0, ModuleName, -1, NULL, 0);
		modNameChar = new WCHAR[nChars];
		MultiByteToWideChar(CP_ACP, 0, ModuleName, -1, (LPWSTR)modNameChar, nChars);

		do
			if (!wcscmp(mENTRY.szModule, modNameChar))
			{
				CloseHandle(hModule);
				return mENTRY;
			}
		while (Module32Next(hModule, &mENTRY));
		CloseHandle(hModule);
		mENTRY.modBaseAddr = 0x0;
		return mENTRY;
}

	template<class c>
	c Read(DWORD dwAddress)
	{
		c val;
		ReadProcessMemory(hProcess, (LPVOID)dwAddress, &val, sizeof(c), NULL);
		return val;
	}

	template<class c>
	BOOL Write(DWORD dwAddress, c, ValueToWrite)
	{
		return WriteProcessMemory(hProcess, (LPVOID)dwAddress, &ValueToWrite, sizeof(c) NULL);
	}

	DWORD GetProcID() { return this->dwPID; }
	HANDLE GetProcHandle() { return this->hProcess; }

	MMC()
	{
		this->hProcess = NULL;
		this->dwPID = NULL;
		try {
			if (!Attach("csgo.exe")) throw 1;
			this->ClientDLL = GetModule("client.dll");
			this->EngineDLL = GetModule("engine.dll");
			this->ClientDLL_Base = (DWORD)this->ClientDLL.modBaseAddr;
			this->EngineDLL_Base = (DWORD)this->EngineDLL.modBaseAddr;
			if (this->ClientDLL_Base == 0x0) throw 2;
			if (this->EngineDLL_Base == 0x0) throw 3;
			this->ClientDLL_size = this->ClientDLL.dwSize;
			this->EngineDLL_size = this->EngineDLL.dwSize;
		}
		catch (int iEx) {
			switch (iEx)
			{
			case 1: MessageBoxA(NULL, "CS:GO must be running", "ERROR", MB_ICONSTOP | MB_OK); exit(0); break;
			case 2: MessageBoxA(NULL, "Couldn't find Client.dll", "ERROR", MB_ICONSTOP | MB_OK); exit(0); break;
			case 3: MessageBoxA(NULL, "Couldn't find Engine.dll", "ERROR", MB_ICONSTOP | MB_OK); exit(0); break;
			default: throw;
			}
		}
		catch (...) {
			MessageBoxA(NULL, "Unhandled exception thrown", "ERROR", MB_ICONSTOP | MB_OK);
			exit(0);
		}
	}

	~MMC()
	{
		CloseHandle(this->hProcess);
	}


};