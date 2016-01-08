#pragma once

#include "compatibility.h"
#include "bdaqctrl.h"

using namespace Automation::BDaq;
//-----------------------------------------------------------------------------------
// Configure the following three parameters before running the demo
//-----------------------------------------------------------------------------------
#ifndef deviceDescription
#define  deviceDescription  L"PCI-1761,BID#0"
#endif 

class PCI1761
{
public:
	PCI1761(){ entityCount++; };
	~PCI1761(){
		Sleep(50);
		entityCount--;
		if (entityCount == 0)
		{
			if (PCI1761::instantDiCtrl != NULL)
			{
				PCI1761::instantDiCtrl->Dispose();
				PCI1761::instantDiCtrl = NULL;
			}
			if (PCI1761::instantDoCtrl != NULL)
			{
				PCI1761::instantDoCtrl->Dispose();
				PCI1761::instantDoCtrl = NULL;
			}
		}
	};

	bool init()
	{
		if (hasOpened)
			return hasOpened;

		ErrorCode ret = Success;
		// Step 1: Create a 'InstantDiCtrl' for DI function.
		// Step 1: Create a instantDoCtrl for DO function.
		PCI1761::instantDiCtrl = AdxInstantDiCtrlCreate();
		PCI1761::instantDoCtrl = AdxInstantDoCtrlCreate();
		DeviceInformation devInfo(deviceDescription);
		ret = PCI1761::instantDiCtrl->setSelectedDevice(devInfo);
		if (BioFailed(ret))
			return false;
		ret = PCI1761::instantDoCtrl->setSelectedDevice(devInfo);
		if (BioFailed(ret))
			return false;

		hasOpened = true;

		return hasOpened;
	}
	//获取IDI X的状态
	//输入ID 0-7
	bool GetIDI(int ID)
	{
		if (!hasOpened)
			return false;
		byte bufferForReading[2] = { 0 };
		instantDiCtrl->Read(0, 1, bufferForReading);
		char x = 0x01 << ID;
		if ((bufferForReading[readPort] & x) != 0x00)
			return true;
		else
			return false;
	}
	//获取IDI X的上升沿，返回true时触发 
	//输入ID 0-7
	bool GetRisingEdgeIDI(int ID)
	{
		if (!hasOpened)
			return false;

		char x = 0x01 << ID;


		bool last = (PCI1761::lastSignalIDI_RisingEdge & x) != 0x00;
		bool now = GetIDI(ID, PCI1761::lastSignalIDI_RisingEdge);

		if (last == false && now == true)
		{
			Sleep(1);
			if (GetIDI(ID))
				return true;
		}
		return false;
	}
	//获取IDI X的下降沿，返回true时触发
	//输入ID 0-7
	bool GetTrailingEdgeIDI(int ID)
	{
		if (!hasOpened)
			return false;

		char x = 0x01 << ID;

		bool last = (PCI1761::lastSignalIDI_TrailingEdge & x) != 0x00;
		bool now = GetIDI(ID, PCI1761::lastSignalIDI_TrailingEdge);

		if (last == true && now == false)
		{
			Sleep(1);
			if (GetIDI(ID) == false)
				return true;
		}
		return false;
	}


	//设置继电器R X的状态
	//int ID继电器ID 0-7
	//设置继电器状态 true 常闭 false 常开
	void SetR(int ID, bool status)
	{
		if (status)
			PCI1761::bufferForR = PCI1761::bufferForR | 0x01 << ID;
		else
		{
			byte x = 0;
			switch (ID)
			{
			case 0:x = 0xFE; break;// 1111 1110
			case 1:x = 0xFD; break;// 1111 1101
			case 2:x = 0xFB; break;// 1111 1011
			case 3:x = 0xF7; break;// 1111 0101
			case 4:x = 0xEF; break;// 1110 1111
			case 5:x = 0xDF; break;// 1101 1111
			case 6:x = 0xBF; break;// 1011 1111
			case 7:x = 0x7F; break;// 0111 1111
			default:
				return;
			}
			PCI1761::bufferForR = PCI1761::bufferForR & x;
		}
		byte  bufferForWriting[64] = { 0 };//the first element is used for start port
		bufferForWriting[0] = PCI1761::bufferForR;
		PCI1761::instantDoCtrl->Write(0, 1, bufferForWriting);		//ret = instantDoCtrl->Write(startPort, portCount, bufferForWriting);
	}
	//设置继电器R0-7的状态
	//输入0xFF表示全开，0x00全闭
	void SetR(byte status)
	{
		PCI1761::bufferForR = status;
		byte  bufferForWriting[64] = { 0 };//the first element is used for start port
		bufferForWriting[0] = PCI1761::bufferForR;
		PCI1761::instantDoCtrl->Write(0, 1, bufferForWriting);		//ret = instantDoCtrl->Write(startPort, portCount, bufferForWriting);
	}


private:
	static int entityCount;//有几个实例在运行，只有为0时才释放1761句柄
	static bool hasOpened;

	static InstantDiCtrl *instantDiCtrl;//Create a 'InstantDiCtrl' for DI function.
	static InstantDoCtrl *instantDoCtrl;//Create a instantDoCtrl for DO function.
	const int readPort = 0;

	static byte lastSignalIDI_RisingEdge;
	static byte lastSignalIDI_TrailingEdge;

	static byte bufferForR ;


	//获取IDI X的状态
	//输入ID 0-7
	bool GetIDI(int ID, byte& last)
	{
		if (!hasOpened)
			return false;
		byte bufferForReading[2] = { 0 };
		instantDiCtrl->Read(0, 1, bufferForReading);

		last = bufferForReading[readPort];
		char x = 0x01 << ID;
		if ((bufferForReading[readPort] & x) != 0x00)
			return true;
		else
			return false;
	}
};