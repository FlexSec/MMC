// OrkoV1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MMC.h"
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>


MMC* Mem;

// Offsets

#define m_dwLocalPlayer 0x00A9E8E4
#define  m_iCrossHairID 0x0000AA70
#define m_iTeamNum 0x000000F0
#define m_dwEntityList 0x04AC0CA4
#define m_bDormant 0x000000E9
#define m_dwForceAttack 0x02F00DF4
#define m_dwForceJump 0x04F5782C
#define m_dwForceLeft 0x02F00DB8
#define  m_dwForceRight 0x02F00DC4
#define  m_fFlags 0x00000100
#define m_FL_ONGROUND (1 << 0)

bool TriggerOn = false;
bool HopOn = false;

void Trigger()
{
	while (true)
	{

		if (GetAsyncKeyState(VK_F1))		//SET KEY HERE
		{
			TriggerOn = !TriggerOn;
			if (TriggerOn) std::cout << "TBOT now ON" << std::endl;
			else std::cout << "TBOT now OFF" << std::endl;
			Sleep(200);
		}

		if (!TriggerOn) continue;


		DWORD LocalPlayer_Base = Mem->Read<DWORD>(Mem->ClientDLL_Base + m_dwLocalPlayer);
		int LocalPlayer_inCross = Mem->Read<int>(LocalPlayer_Base + m_iCrossHairID);
		int LocalPlayer_Team = Mem->Read<int>(LocalPlayer_Base + m_iTeamNum);


		DWORD Trigger_EntityBase = Mem->Read<DWORD>(Mem->ClientDLL_Base + m_dwEntityList + ((LocalPlayer_inCross - 1) * 0x10));
		int Trigger_EntityTeam = Mem->Read<int>(Trigger_EntityBase + m_iTeamNum);
		bool Trigger_EntityDormant = Mem->Read<bool>(Trigger_EntityBase + m_bDormant);



		if ((LocalPlayer_inCross > 0 && LocalPlayer_inCross <= 64) && (Trigger_EntityBase != NULL) && (Trigger_EntityTeam != LocalPlayer_Team) && (!Trigger_EntityDormant))
		{
			Sleep(10); //Delay before shooting
			mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, NULL, NULL);
			Sleep(10); //Delay between shots
			mouse_event(MOUSEEVENTF_LEFTUP, NULL, NULL, NULL, NULL);
			Sleep(10); //Delay after shooting
		}
	}
}

void EasterJump()
{

	while (true)
	{

		if (GetAsyncKeyState(VK_SHIFT))		//SET KEY HERE
		{
			HopOn = !HopOn;
			if (HopOn) std::cout << "Happy Easter" << std::endl;
			else std::cout << "Grounded" << std::endl;
			Sleep(200);
		}

		if (!HopOn) continue;

		
		DWORD LocalPlayer_Base = Mem->Read<DWORD>(Mem->ClientDLL_Base + m_dwLocalPlayer);
		DWORD ForceJump = Mem->Read<DWORD>(Mem->ClientDLL_Base + m_dwForceJump);
		int flags = Mem->Read<DWORD>(LocalPlayer_Base + m_fFlags); 

		if (m_fFlags & m_FL_ONGROUND && GetAsyncKeyState(VK_SHIFT)) {
			Mem->Write<DWORD>(Mem->ClientDLL_Base, m_dwForceJump, 6);
		}
	}
}

	int main()
	{
		
		Mem = new MMC();
			std::cout << "TBOT" << std::endl;
			Trigger();
			EasterJump();
			delete Mem; 
			return 0;
		
	};
