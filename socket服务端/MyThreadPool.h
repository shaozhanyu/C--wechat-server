#pragma once
#include<list>
#include "MyMutex.h"
#include "MyStack.h"
#include "MyList.h"
#include"MyQueue.h"

class CMyThread;
class CTask;
enum PRIORITY
{
	NORMAL,
	HIGH
};
class CBaseThreadPool
{
public:
	virtual bool SwitchActiveThread(CMyThread*)=0;
};

class CMyThreadPool:public CBaseThreadPool
{

public:
	CMyThreadPool(void);
	~CMyThreadPool(void);
	void  CreatWorkDataThread(int num);//����Ӳ�����ݴ��������������˴����̷߳���
	void  CreatSaveDataThread(int num);//��������Ӳ�����������
	void  CreatAPPDataThread(int num);//����APP���ݴ��������
	
public:
	virtual CMyThread * PopIdleThread(); //ȡ�����е��߳�������Ķ���
	virtual bool SwitchActiveThread(CMyThread*);//
	virtual CTask  *GetNewTask();

public:
	//priorityΪ���ȼ��������ȼ������񽫱����뵽���ס�
	bool addTask(CTask*t,PRIORITY priority);//������������
	bool start();//��ʼ���ȡ�
	bool destroyThreadPool();
private:
	int m_nThreadNum;
	bool m_bIsExit;
	
	CMyStack m_IdleThreadStack;//��������߳�ջ
	CMyList m_ActiveThreadList;//�����߳��б�,���ڹ������߳�
	CMyQueue m_TaskQueue;//�ȴ�������������
};

