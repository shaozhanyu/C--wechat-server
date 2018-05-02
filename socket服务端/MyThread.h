#pragma once
#include "windows.h"

class CTask;

class CBaseThreadPool;

class CMyThread
{
public:
	CMyThread(CBaseThreadPool*threadPool);
	~CMyThread(void);
public:
	bool startThread();
	bool startRECThread();
	bool CMyThread::startAPPThread();
	bool suspendThread();
	bool resumeThread();
	bool assignTask(CTask*pTask);
	bool startTask();
	static DWORD WINAPI threadProc(LPVOID pParam);//���������߳�
	static DWORD WINAPI SaveDatathreadProc(LPVOID pParam);//�洢�����߳�
	static DWORD WINAPI RecvAPPthread( LPVOID pParam );
	
	DWORD   m_threadID;//�������̵߳�id
	HANDLE  m_hThread;
	bool    m_bIsExit;
private:
	
	HANDLE  m_hEvent;//�¼�ID
	CTask   *m_pTask; //������ָ��,����һ���޹��࣬ͨ������CTast����Ĵ��麯�������Ե�������CTestTaskʵ�ֵ��麯����
	CBaseThreadPool  *m_pThreadPool;	//�̳߳�ָ�룬����һ���޹��࣬ͨ�����ඨ��Ĵ��麯�������Ե�������ʵ�ֵ��麯����
};
