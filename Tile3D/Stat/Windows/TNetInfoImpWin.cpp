#include "TNetInfoImpWin.h"
#include <WinSock2.h>
#include <Iphlpapi.h>
#include <stdio.h>

void TNetInfoImpWin::ObtainNetInfo(TString & macAddr, TString & ipAddr)
{
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);

	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//����������ص���ERROR_BUFFER_OVERFLOW
		//��˵��GetAdaptersInfo�������ݵ��ڴ�ռ䲻��,ͬʱ�䴫��stSize,��ʾ��Ҫ�Ŀռ��С
		//��Ҳ��˵��ΪʲôstSize����һ��������Ҳ��һ�������
		//�ͷ�ԭ�����ڴ�ռ�
		delete pIpAdapterInfo;
		//���������ڴ�ռ������洢����������Ϣ
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//�ٴε���GetAdaptersInfo����,���pIpAdapterInfoָ�����
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}

	if (ERROR_SUCCESS == nRel)
	{
		//���������Ϣ
		//�����ж�����,���ͨ��ѭ��ȥ�ж�
		if (pIpAdapterInfo)
		{
			char macbuf[32];
			memset(macbuf, 0, sizeof(macbuf));
			for (UINT i = 0; i < pIpAdapterInfo->AddressLength; i++){
				sprintf(macbuf+2*i, "%02x", pIpAdapterInfo->Address[i]);
			}
			
			macAddr = macbuf;

			//���������ж�IP, ����ֻ��ȡ��һ������
			IP_ADDR_STRING *pIpAddrString = &(pIpAdapterInfo->IpAddressList);
			if (pIpAddrString) {
				ipAddr = pIpAddrString->IpAddress.String;
			}
		}

	}
	return;
}

