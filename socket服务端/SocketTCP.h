//��Ϊ��ɶ˿ڷ����,ҵ���߼����ڶ��̺߳�����(IOCP_Run)����д
//CSocketTCP tcpServer;
//tcpServer.Create("",9999,true);
//tcpServer.Listen();
//while(true)
//{
//	if (0 != tcpServer.Accept())
//	{
//		//error
//		break;
//	}
//
//}

#pragma once
#include "stdafx.h"
#include <process.h>
#include <winsock2.h>
#include <set>
#include <Windows.h>//<Windows.h>���ͷ�ļ�������<winsock2.h>ǰ��
#pragma comment(lib,"ws2_32.lib")
////////////////////////////////////
#define   DATA_LEN   4096*10

///////////////////////////////////
using namespace std;
////////////////////////////////////
enum  OPERATION_TYPE
{
	RECV_POSTED = 0,
	SEND_POSTED
};
struct IOCPContextKey //ȫ�ֽṹ��
{
	IOCPContextKey()//��ʼ���xֵ
	{
		NumberOfBytesRecv = 0;
		NumberOfBytesSend = 0;
		Buffer.len = DATA_LEN;
		Buffer.buf = szMessage;
	}
	OPERATION_TYPE opType;
	SOCKET   clientSocket;
	WSABUF  Buffer;
    char  szMessage[DATA_LEN];
	DWORD          NumberOfBytesRecv;
	DWORD          NumberOfBytesSend;

};


class CMutex
{
public:
	CMutex(void)
	{
		InitializeCriticalSection(&section_);
	}
	~CMutex(void)
	{
		DeleteCriticalSection(&section_);
	}
	void enter()
	{
		EnterCriticalSection(&section_); //�����������ٽ���
	}
	void leave()
	{
		LeaveCriticalSection(&section_); //�������˳��ٽ���
	}
	struct Lock
	{
		Lock(CMutex& s) : s(s)
		{
			s.enter(); 
		}  
		~Lock()
		{
			s.leave();
		}  
	private:
		CMutex& s;  
	};
private:
	CMutex& operator=(const CMutex& );
	CRITICAL_SECTION section_;
};

class CSocketTCP
{
public:
	CSocketTCP(void);
	~CSocketTCP(void);
	/* brif ����tcp
	* param[1]tcp��ip��ַ; param[2]tcp�Ķ˿�;param[3]�Ƿ�����˿�����
	* return 0 success;-1����У��ʧ��;-2����socketʧ��;-3�󶨶˿�ʧ��
	*/
	int Create(char* cIP ="",int iPort = 0,bool bRebind = false);

	/* brif tcp��������˿�;
	* param[1]����δ������ӵ�tcp�ں˳���;
	* return >0 ���͵����ݳ���;-1�������ʧ��
	*/
	int Listen(int lNum = 5);

	/* brif tcp�����µĿͻ���,�˺�������;
	* return 0 �ɹ��µĿͻ������ӣ���0ʧ��
	*/
	int Accept();

	//�ر�tcp
	void Close();

	// ��ȡ������
	int GetLastError();

	// ��ȡtcp��ip��ַ
	char* GetSocketIP();

	// ��ȡtcp�Ķ˿�
	int GetSocketPort();

	//��ɶ˿ڵĶ��߳�ִ�к���
	int IOCP_Run();

	//�ر����еĿͻ���
	void closeAllClient();
public:

	//��Ϊ�ͻ��˵ı�socket;
	SOCKET m_sockfd;

	//��Ϊ�����ʱ�򣬹������ӵ�tcp������
	SOCKET m_newClinetSockfd;
public:
	//��ɶ˿�
	HANDLE  m_CompletionPort;

	//��ɶ˿�Ӧ�ò����ں��ν�ֵ,�����ʼ��������ʹ��
	//����ṹ������Ϊ�������ݽ�����չ
	WSAOVERLAPPED  m_overlap;

	//�ͻ�������������key�б�
	set<IOCPContextKey*> m_setIOCPKEY;

	//�����̵߳ľ��
	set<HANDLE> m_IOCPThread;

	//��
	CMutex m_mutex;

};