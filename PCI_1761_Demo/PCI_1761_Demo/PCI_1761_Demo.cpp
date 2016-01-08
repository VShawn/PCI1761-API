// PCI_1761_Demo.cpp : 定义控制台应用程序的入口点。
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
		//输出功能 Output function
		pci1761.SetR(0x00);//设置所有输出继电器断开,set all relays open.
		SLEEP(100);
		pci1761.SetR(0xFF);//设置所有输出继电器闭合,set all relays close
		SLEEP(100);
		pci1761.SetR(0x01);//设置0号继电器闭合，其余断开,set relay 0 close,relay1-relay7 open.
		SLEEP(100);

		pci1761.SetR(2, 1);//2号继电器设置为闭合，其余保持原状,set relay 2 close,others keep their status.
		SLEEP(100);

		pci1761.SetR(5, 0);//5号继电器设置为断开，其余保持原状,set relay 5 open,others keep their status.
		SLEEP(100);

		do
		{
			//获取IDI0的上升沿，遇到上升沿返回true
			//if IDI0 gets a Rising Edge return true
			if (pci1761.GetRisingEdgeIDI(0))
				printf("IDI0 RisingEdge\r\n");

			//获取IDI1的下降沿，遇到下降沿返回true
			//if IDI1 gets a TrailingEdge return true
			if (pci1761.GetTrailingEdgeIDI(1))
				printf("IDI1 TrailingEdge\r\n");

			//获取7号输入端口的电平状态，高电平返回true.
			//Get IDI7 status, high return true, low return false.
			if (pci1761.GetIDI(7))
				printf("IDI7 is high\r\n");
			else
				printf("IDI7 is low\r\n");
		} while (true);
	}
	return 0;
}

