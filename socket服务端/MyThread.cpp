#include "MyThread.h"
#include"task.h"
#include "MyThreadPool.h"
#include<cassert>
#include "mcom.h"

#define WAIT_TIME 20

CMyThread::CMyThread(CBaseThreadPool*threadPool)
{
	m_pTask=NULL;
	//m_bIsActive=false;
	m_pThreadPool=threadPool;
	m_hEvent=CreateEvent(NULL,false,false,NULL);//�����¼��������¼�ID��ֵ���߳���ı���
	m_bIsExit=false;
}

//bool CMyThread::m_bIsActive=false;
CMyThread::~CMyThread(void)
{
	CloseHandle(m_hEvent);
	CloseHandle(m_hThread);
}

bool CMyThread::startThread()
{
	m_hThread=CreateThread(0,0,threadProc,this,0,&m_threadID);//m_threadID�������̵߳�id
	if(m_hThread==INVALID_HANDLE_VALUE)
	{
		return false;
	}
	return true;
}

bool CMyThread::startRECThread()
{
	m_hThread=CreateThread(0,0,SaveDatathreadProc,this,0,&m_threadID);//m_threadID�������̵߳�id
	if(m_hThread==INVALID_HANDLE_VALUE)
	{
		return false;
	}
	return true;
}

bool CMyThread::startAPPThread()
{
	m_hThread=CreateThread(0,0,RecvAPPthread,this,0,&m_threadID);//m_threadID�������̵߳�id
	if(m_hThread==INVALID_HANDLE_VALUE)
	{
		return false;
	}
	return true;
}

bool CMyThread::suspendThread()
{
	ResetEvent(m_hEvent);
	return true;
}
//����������֪ͨ�̼߳���ִ�С�
bool CMyThread::resumeThread()
{
	SetEvent(m_hEvent);//�����¼������߳�
	return   true;
}
//////////////////////////////���������߳�////////////////////////////////////////////
DWORD WINAPI CMyThread::threadProc( LPVOID pParam )
{
	CMyThread *pThread=(CMyThread*)pParam;
	while(!pThread->m_bIsExit)//�ж��߳��˳���־��Ϊ1
	{
 		DWORD ret=WaitForSingleObject(pThread->m_hEvent,INFINITE);//�߳�һֱ����
		if(ret==WAIT_OBJECT_0)
		{
			if(pThread->m_pTask)//�ж��߳������������ָ�벻�գ�˵��������
			{			
				pThread->m_pTask->taskRecClientProc();//ִ���������еľ�����롣
				pThread->m_pTask=NULL;
				pThread->m_pThreadPool->SwitchActiveThread(pThread);//Ϊ��ǰ�Ĺ����߳�ȥ������в���������
			}
		}
	}
	return 0;
}

//////////////////////////////����APP�����߳�////////////////////////////////////////////
DWORD WINAPI CMyThread::RecvAPPthread( LPVOID pParam )
{
	CMyThread *pThread=(CMyThread*)pParam;
	while(!pThread->m_bIsExit)//�ж��߳��˳���־��Ϊ1
	{
 		DWORD ret=WaitForSingleObject(pThread->m_hEvent,INFINITE);//�߳�һֱ����
		if(ret==WAIT_OBJECT_0)
		{
			if(pThread->m_pTask)//�ж��߳��е�����ָ�벻�գ�˵��������
			{			
				pThread->m_pTask->taskAPPRecProc();//ִ���������еľ�����롣
				pThread->m_pTask=NULL;
				pThread->m_pThreadPool->SwitchActiveThread(pThread);//Ϊ��ǰ�Ĺ����߳�ȥ������в���������
			}
		}
	}
	return 0;
}
/////////////////////////////�洢Ӳ�������߳�///////////////////////////////////////////
DWORD WINAPI CMyThread::SaveDatathreadProc( LPVOID pParam )
{
	CMyThread *pThread=(CMyThread*)pParam;
	while(!pThread->m_bIsExit)//�ж��߳��˳���־��Ϊ1
	{
 		DWORD ret=WaitForSingleObject(pThread->m_hEvent,INFINITE);//�߳�һֱ����
		if(ret==WAIT_OBJECT_0)
		{
			if(pThread->m_pTask)//�ж��߳��е�����ָ�벻�գ�˵��������
			{			
				pThread->m_pTask->taskSaveSQL();//ִ���������еľ�����롣
				pThread->m_pTask=NULL;
				pThread->m_pThreadPool->SwitchActiveThread(pThread);//Ϊ��ǰ�Ĺ����߳�ȥ������в���������
			}
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
//������������߳��ࡣ
bool CMyThread::assignTask( CTask*pTask )
{
	assert(pTask);
	if(!pTask)
		return false;
	m_pTask=pTask;
	
	return true;
}
//��ʼִ������
bool CMyThread::startTask()
{
	resumeThread();
	return true;
}
