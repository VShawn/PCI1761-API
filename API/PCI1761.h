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
	//��ȡIDI X��״̬
	//����ID 0-7
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
	//��ȡIDI X�������أ�����trueʱ���� 
	//����ID 0-7
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
	//��ȡIDI X���½��أ�����trueʱ����
	//����ID 0-7
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


	//���ü̵���R X��״̬
	//int ID�̵���ID 0-7
	//���ü̵���״̬ true ���� false ����
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
	//���ü̵���R0-7��״̬
	//����0xFF��ʾȫ����0x00ȫ��
	void SetR(byte status)
	{
		PCI1761::bufferForR = status;
		byte  bufferForWriting[64] = { 0 };//the first element is used for start port
		bufferForWriting[0] = PCI1761::bufferForR;
		PCI1761::instantDoCtrl->Write(0, 1, bufferForWriting);		//ret = instantDoCtrl->Write(startPort, portCount, bufferForWriting);
	}


private:
	static int entityCount;//�м���ʵ�������У�ֻ��Ϊ0ʱ���ͷ�1761���
	static bool hasOpened;

	static InstantDiCtrl *instantDiCtrl;//Create a 'InstantDiCtrl' for DI function.
	static InstantDoCtrl *instantDoCtrl;//Create a instantDoCtrl for DO function.
	const int readPort = 0;

	static byte lastSignalIDI_RisingEdge;
	static byte lastSignalIDI_TrailingEdge;

	static byte bufferForR ;


	//��ȡIDI X��״̬
	//����ID 0-7
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