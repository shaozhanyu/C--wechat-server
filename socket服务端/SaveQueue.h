#pragma once
#include <deque>
#include "MyMutex.h"
#include "mcom.h"

///////////////Ӳ�����洢�����࣬���������ݶ���/////////////////////////////
class CMySaveQueue
{

public:
	CMySaveQueue(void);
	~CMySaveQueue(void);

public:
	HardData* pop();//����
	bool push(HardData  *t);//�����β
	bool pushFront(HardData  *t);//����嵽���ס�
	bool isEmpty();//�ж϶��п�
	bool clear();//��ն���

private:
	deque<HardData*>m_DataQueue;//���ݴ洢�ṹ�����

	CMyMutex   m_data_mutex;
};
