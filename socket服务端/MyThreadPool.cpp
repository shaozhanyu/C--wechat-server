#include "MyThreadPool.h"
#include "MyThread.h"
#include "Task.h"
#include<cassert>
#include<iostream>
#include "MyQueue.h"
CMyThreadPool::CMyThreadPool()
{
	
}
//////////////////////����num��Ӳ������߳�//////////////////////////////
void  CMyThreadPool::CreatWorkDataThread(int num)
{
	m_bIsExit=false;
	for(int i=0;i<num;i++)
	{
		CMyThread *p =new CMyThread(this);
		m_IdleThreadStack.push(p);//���̳߳���Ŀ����߳�ջ����ѹ�˴������߳����ָ��
		p->startThread();//�����߳�
	}
}
//////////////////////����num���洢�߳�//////////////////////////////
void  CMyThreadPool::CreatSaveDataThread(int num)
{
	m_bIsExit=false;
	for(int i=0;i<num;i++)
	{
		CMyThread *p =new CMyThread(this);
		m_IdleThreadStack.push(p);//���̳߳���Ŀ����߳�ջ����ѹ�˴������߳����ָ��
		p->startRECThread();//�����߳�		
	}
}

/////////////////////����num��APP����߳�///////////////////////////////
void  CMyThreadPool::CreatAPPDataThread(int num)
{
	m_bIsExit=false;
	for(int i=0;i<num;i++)
	{
		CMyThread *p =new CMyThread(this);
		m_IdleThreadStack.push(p);//���̳߳���Ŀ����߳�ջ����ѹ�˴������߳����ָ��
		p->startAPPThread();//�����߳�
		
	}
}

CMyThreadPool::~CMyThreadPool(void)
{
}
CMyThread* CMyThreadPool::PopIdleThread()
{
	CMyThread *pThread=m_IdleThreadStack.pop();//�ӿ����߳�ջ��ȡ�����е��߳�
	//pThread->m_bIsActive=true;
	return pThread;
}
//���̴߳ӻ����ȡ������������߳�ջ�С���ȡ֮ǰ�жϴ�ʱ��������Ƿ����������������Ϊ��ʱ�Ź��𡣷�����������ȡ�������ִ�С�
bool CMyThreadPool::SwitchActiveThread( CMyThread  *t)
{
	if(!m_TaskQueue.isEmpty())//������в�Ϊ�գ�����ȡ����ִ�С�
	{
		CTask *pTask=NULL;
		pTask=m_TaskQueue.pop();//ջ��ȡ������
		//std::cout<<"�߳�ID��"<<t->m_threadID<<"   ִ��������: "<<pTask->getID()<<std::endl;
	
		t->assignTask(pTask); //Ϊ�߳��������ָ����䵱ǰȡ��������ָ��
		t->startTask();	// �����̵߳��������񷽷�����ʼִ������
	}
 	else//�������Ϊ�գ����̹߳���
	{
		m_ActiveThreadList.removeThread(t);//�ӻ�߳��б���ɾ����ǰ�̣߳���Ϊ���¿��������ÿ�����
		m_IdleThreadStack.push(t); //�����߳�ջ��ѹ����߳�
	}
	return true;
}
////////////////////////////////////////////////////////////////
bool CMyThreadPool::addTask( CTask  *t,  PRIORITY priority )
{
	assert(t);
	if(!t||m_bIsExit)
		return false;	
	CTask *task=NULL;
	//std::cout<<"[����ID:"<<t->getID()<<"]��ӳɹ���"<<std::endl;
	if(priority== NORMAL)
	{
		m_TaskQueue.push(t);//�������������С�
	}
	else if(HIGH)
	{
		m_TaskQueue.pushFront(t);//�����ȼ�����
	}
	
	if(!m_IdleThreadStack.isEmpty())//���ڿ����̡߳����ÿ����̴߳�������
	{
			task=m_TaskQueue.pop();//ȡ����ͷ����
			if(task==NULL)
			{
				//static unsigned long  cnt=0;
				std::cout<<"����ȡ������!!!"<<std::endl;
				return 0;
			}
			CMyThread*pThread = PopIdleThread();  //�ӿ����߳�ջ��ȡ�����е��߳�
			//std::cout<<"���߳�ID:"<<pThread->m_threadID<<"�� ִ������ID:��"<<task->getID()<<"��"<<std::endl;
			m_ActiveThreadList.addThread(pThread);
			pThread->assignTask(task); //
			pThread->startTask(); // ��ʼִ������SetEvent�¼������������߳�	
	}
	return 0;
	
}
bool CMyThreadPool::start()
{
	return 0;
}
CTask* CMyThreadPool::GetNewTask()
{
	if(m_TaskQueue.isEmpty())
	{
		return NULL;
	}
	CTask *task=m_TaskQueue.pop();//ȡ����ͷ����
	if(task==NULL)
	{
		std::cout<<"����ȡ������"<<std::endl;
		return 0;
	}
	return task;
}
bool CMyThreadPool::destroyThreadPool()
{
	
	m_bIsExit=true;
	m_TaskQueue.clear();
	m_IdleThreadStack.clear();
	m_ActiveThreadList.clear();
	return true;
}
