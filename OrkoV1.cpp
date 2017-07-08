#pragma once
#include "stdafx.h"
#include "MMC.h"
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include "Offsets.h"


MMC* Mem;

bool TriggerOn = false;
bool HopOn = true;

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
			// Alternative Function
			/*Sleep(10); //Delay before shooting
			mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, NULL, NULL);
			Sleep(10); //Delay between shots
			mouse_event(MOUSEEVENTF_LEFTUP, NULL, NULL, NULL, NULL);
			Sleep(10); //Delay after shooting */

			Sleep(10); 
			Mem->Write<int>(Mem->ClientDLL_Base + m_dwForceAttack, 5);
			Sleep(10); 
			Mem->Write<int>(Mem->ClientDLL_Base + m_dwForceAttack, 4);
			Sleep(10); 
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
			Mem->Write<int>(Mem->ClientDLL_Base +m_dwForceJump, 6);
		}
	}
} 

	int main()
	{
		
		Mem = new MMC();
			std::cout << "OrkoV1" << std::endl;
			//Trigger();
			EasterJump();
			delete Mem; 
			return 0;
		
	};

