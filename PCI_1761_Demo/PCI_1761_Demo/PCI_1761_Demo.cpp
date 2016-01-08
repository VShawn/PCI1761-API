// PCI_1761_Demo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Class\IOCard\PCI1761.h"


using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	PCI1761 pci1761;
	//init PCI-1761,success return true.
	if (pci1761.init())
	{
		//������� Output function
		pci1761.SetR(0x00);//������������̵����Ͽ�,set all relays open.
		SLEEP(100);
		pci1761.SetR(0xFF);//������������̵����պ�,set all relays close
		SLEEP(100);
		pci1761.SetR(0x01);//����0�ż̵����պϣ�����Ͽ�,set relay 0 close,relay1-relay7 open.
		SLEEP(100);

		pci1761.SetR(2, 1);//2�ż̵�������Ϊ�պϣ����ౣ��ԭ״,set relay 2 close,others keep their status.
		SLEEP(100);

		pci1761.SetR(5, 0);//5�ż̵�������Ϊ�Ͽ������ౣ��ԭ״,set relay 5 open,others keep their status.
		SLEEP(100);

		do
		{
			//��ȡIDI0�������أ����������ط���true
			//if IDI0 gets a Rising Edge return true
			if (pci1761.GetRisingEdgeIDI(0))
				printf("IDI0 RisingEdge\r\n");

			//��ȡIDI1���½��أ������½��ط���true
			//if IDI1 gets a TrailingEdge return true
			if (pci1761.GetTrailingEdgeIDI(1))
				printf("IDI1 TrailingEdge\r\n");

			//��ȡ7������˿ڵĵ�ƽ״̬���ߵ�ƽ����true.
			//Get IDI7 status, high return true, low return false.
			if (pci1761.GetIDI(7))
				printf("IDI7 is high\r\n");
			else
				printf("IDI7 is low\r\n");
		} while (true);
	}
	return 0;
}

