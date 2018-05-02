#pragma once
#include "stdafx.h"
#include "SocketTCP.h"
#include "mcom.h"

//////////////////////////////////////////////////////
#define  FLAG_THRED_EXIT  0xFFFFFFFF

CTestTask *p=new CTestTask(1);//��������
///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
unsigned __stdcall WorkerThread( void* pArguments )
{
	CSocketTCP* pParent = reinterpret_cast<CSocketTCP*>(pArguments);
	if (NULL == pParent)
	{
		//log
		return -1;
	}
	return pParent->IOCP_Run();

} 
/////////////////Winsocket��ʼ��/////////////////////////////////
class windowsSocketInit
{
public:
	windowsSocketInit():
	  m_init(false)
	  {
		  WORD wVersionRequested;
		  WSADATA wsaData;
		  int err;

		  wVersionRequested = MAKEWORD(2, 2);
		  //����dll
		  err = WSAStartup(wVersionRequested, &wsaData);
		  if (err != 0) 
		  {
			  return;
		  }
		  else
		  {
			  printf("����WSAdll�ɹ�!\r\n");
		  }
		  if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) 
		  {
			  //printf("Could not find a usable version of Winsock.dll\n");
			  WSACleanup();
			  return ;
		  }
		  else
		  {
			  m_init = true;
			  //printf("The Winsock 2.2 dll was found okay\n");
		  }
	  }
	  ~windowsSocketInit()
	  {
		  //ж��dll
		  if (m_init)
		  {
			  WSACleanup();
		  }

	  }
private: 
	bool m_init;
};

CSocketTCP::CSocketTCP()
{
	static windowsSocketInit loadDLLOnce;
	m_sockfd = -1;
	m_newClinetSockfd = -1;
	m_CompletionPort = INVALID_HANDLE_VALUE;
	memset(&m_overlap,0,sizeof(WSAOVERLAPPED));
}
CSocketTCP::~CSocketTCP()
{
	//�ȴ����̹߳رգ���������
	Close();
}


int  CSocketTCP::Create(char* cIP,int iPort,bool bRebind)
{
	if (NULL == cIP || 0 > iPort)
	{
		return -1;
	}
	char opt=1;
	if((m_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
	{
#ifdef  _PrintError_
		perror("socket");
#endif
		return -2;
	}
	else
	{
		printf("����socket�׽���\r\n");
	}
	// ����IOCP���ں˶���
	/****
	 * ��Ҫ�õ��ĺ�����ԭ�ͣ�
	 * HANDLE WINAPI CreateIoCompletionPort(
     *    __in   HANDLE FileHandle,		// �Ѿ��򿪵��ļ�������߿վ����һ���ǿͻ��˵ľ��
     *    __in   HANDLE ExistingCompletionPort,	// �Ѿ����ڵ�IOCP���
     *    __in   ULONG_PTR CompletionKey,	// ��ɼ���������ָ��I/O��ɰ���ָ���ļ�
     *    __in   DWORD NumberOfConcurrentThreads // ��������ͬʱִ������߳�����һ���ƽ���CPU������*2
     * );
	 ****/
	// Create completion port�������ں˶���IOCP
	m_CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);////������ɶ˿ھ��
	if (NULL == m_CompletionPort){	// ����IO�ں˶���ʧ��
		printf("CreateIoCompletionPort failed. Error: %d" , GetLastError());
		system("pause");
		return -1;
	}
	// ��SOCKET������
	struct sockaddr_in clientAddr;
	clientAddr.sin_family=AF_INET;
	clientAddr.sin_port=htons(iPort);
	if(bRebind)
	{
		setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));//SO_REUSEADDR���ö˿��ͷź������Ϳ��Ա��ٴ�ʹ��
	}
	if(0 == strlen(cIP))
	{
		clientAddr.sin_addr.s_addr=htonl(INADDR_ANY); //��������IP��ַ��
	}
	else
	{
		clientAddr.sin_addr.s_addr=inet_addr(cIP); //����ָ����IP
	}

	int ret = bind(m_sockfd, (struct sockaddr *)&clientAddr, sizeof(clientAddr));//���׽���
	if ( -1 == ret)
	{
#ifdef _PrintError
		printf("bind failed %s \n", strerror(errno));
#endif
		return -3;
	}
	else
	{
		printf("���׽��ֳɹ���\r\n");
	}

	//ĳЩ�������Ҫ���δ����������ݷ��ͳ�ȥ���ٹر�socket����ͨ�������ó�������Ҫ��
	struct linger {
		u_short l_onoff;
		u_short l_linger;
	};
	linger m_sLinger;
	m_sLinger.l_onoff = 1; 
	//�ڵ���closesocket()ʱ��������δ�����꣬����ȴ�
	// ��m_sLinger.l_onoff=0;�����closesocket()��ǿ�ƹر�
	m_sLinger.l_linger = 2; //���õȴ�ʱ��Ϊ2��
	setsockopt(m_sockfd, SOL_SOCKET, SO_LINGER, ( const char* )&m_sLinger, sizeof( linger ) );

	//��send(),recv()��������ʱ��������״����ԭ�򣬷��ղ���Ԥ�ڽ���,�������շ�ʱ�ޣ�
	int nNetTimeout=1000;//1��
	//����ʱ��
	setsockopt(m_sockfd , SOL_SOCKET , SO_SNDTIMEO ,(char *)&nNetTimeout,sizeof(int));
	//����ʱ��
	//setsockopt(m_sockfd , SOL_SOCKET , SO_RCVTIMEO ,(char *)&nNetTimeout,sizeof(int));

	return 0;
}


int  CSocketTCP::Listen(int lNum )
{
	if(0 > lNum)
	{
		return  -1;
	}
	if(-1 == m_sockfd)
	{
		return -2;
	}
	if(listen(m_sockfd,lNum)<0)
	{
#ifdef _PrintError_
		perror("listen");
#endif
		return -3;
	}
	else
	{
		printf("���������ɹ�!\r\n");
	}

// ����IOCP�߳�-

	// ȷ���������ĺ�����������4����8����
	SYSTEM_INFO   systeminfo;
	GetSystemInfo(&systeminfo);
	int workThredNum = systeminfo.dwNumberOfProcessors * 2;
	if (0 > workThredNum)
	{
		return -4;
	}
	
	printf("CPU��������%d\r\n" , workThredNum/2);
	// ���ڴ������ĺ������������߳�
	for (int i = 0; i < workThredNum; i++)
	{
		unsigned threadID;
		/*********_beginthreadex()�����ڴ������߳�ʱ����䲢��ʼ��һ��_tiddata�顣
		���_tiddata����Ȼ���������һЩ��Ҫ�̶߳�������ݡ�
		��ʵ�����߳�����ʱ�����Ƚ�_tiddata�����Լ���һ������������
		Ȼ�����̵߳��ñ�׼C���п⺯����strtok()ʱ�ͻ���ȡ��_tiddata��ĵ�ַ�ٽ���Ҫ���������ݴ���_tiddata���С�
		����ÿ���߳̾�ֻ����ʺ��޸��Լ������ݶ�����ȥ�۸������̵߳������ˡ�
		��ˣ�����ڴ�������ʹ�ñ�׼C���п��еĺ���ʱ������ʹ��_beginthreadex()������CreateThread()��
		*********/
		HANDLE hWorkThread = (HANDLE)_beginthreadex( NULL, 0, &WorkerThread, this, 0, &threadID );//�����߳�,thisָ��
		m_IOCPThread.insert(hWorkThread);//���½����߳̾��������������

	}
	// ��ʼ����IO����
	printf( "socket�������Ѿ��������ڵȴ��ͻ��˽���....\r\n");
	
	return 0;

}

void CSocketTCP::closeAllClient()
{
	//�ỽ�������߳�,��֪ͨ�˳��߳�;
	//close all thread
	int iThreadNum = m_IOCPThread.size();
	if (0 < iThreadNum)
	{
		//notice all thread exit
		for (int i = 0; i < iThreadNum;++i)
		{
			DWORD  dwNumberOfBytesTrlansferred = FLAG_THRED_EXIT;
			DWORD  dwCompletlonKey = 0;
			PostQueuedCompletionStatus(m_CompletionPort,dwNumberOfBytesTrlansferred,DWORD(dwCompletlonKey), &m_overlap);
		}

		//wait for thread close;
		for (set<HANDLE>::iterator itor = m_IOCPThread.begin();itor != m_IOCPThread.end();++itor)
		{
			HANDLE threadHandle = (*itor);
			WaitForSingleObject(threadHandle,INFINITE);
			CloseHandle(threadHandle);
		}
		m_IOCPThread.clear();

	}

	//clear resource
	if (INVALID_HANDLE_VALUE != m_CompletionPort )
	{
		CloseHandle(m_CompletionPort);
		m_CompletionPort = INVALID_HANDLE_VALUE;
	}

	for (set<IOCPContextKey*>::iterator itor = m_setIOCPKEY.begin();itor != m_setIOCPKEY.end();++itor)
	{
		IOCPContextKey* pIOCPKey = (*itor);
		if (NULL == pIOCPKey)
		{
			continue;
		}
		//close all client
		shutdown(pIOCPKey->clientSocket,SD_BOTH);
		closesocket(pIOCPKey->clientSocket);
		delete pIOCPKey;
		pIOCPKey = NULL;
	}
	m_setIOCPKEY.clear();
}
/*****
IOCP�˿�����������߳�
********/
int CSocketTCP::IOCP_Run()
{
	DWORD dwBytesTransfered = 0;
	
	//SOCKET   *pVoidContextKey = NULL;
	//set<IOCPContextKey*> pVoidContextKey;
	//OVERLAPPED* pOverlapped = NULL;
	int* pnThreadNo = NULL;
	//IOCPContextKey* pIOCPContext = new  IOCPContextKey; //��ָ�����new����Ϊû��ʵ��������������ֱ��ʵ�������ڴ��
	IOCPContextKey* pVoidContextKey = NULL;
	IOCPContextKey* pIOCPContext = NULL ; //��ָ�����new����Ϊû��ʵ��������������ֱ��ʵ�������ڴ��
	LPOVERLAPPED	lpOverlapped = NULL;	//�ص��ṹָ��
	SOCKET nSocket = -1;
	DWORD sendBytes = 0;
	DWORD recvBytes = 0;
	DWORD Flags = 0;
	BOOL  nRetCode = false;
	//LPOVERLAPPED_ENTRY lpCompletionPortEntries;
	//PULONG          ulNumEntriesRemoved = new PULONG;
	while (true)
	{
		/***GetQueuedCompletionStatus���ܣ���ȡ�������״̬��
		����ֵ�����óɹ����򷵻ط�����ֵ��������ݴ���lpNumberOfBytes��lpCompletionKey��lpoverlapped�����С�
		ʧ���򷵻���ֵ��****/
		/**/
		nRetCode  = GetQueuedCompletionStatus(m_CompletionPort,&dwBytesTransfered,(PULONG_PTR)&pVoidContextKey,
			&lpOverlapped,INFINITE);
		
	
		pIOCPContext = (IOCPContextKey *)pVoidContextKey;//��ȡ��ǰ����,��ָ��ָ��һ��ʵ��ָ��


		if (TRUE == nRetCode && FLAG_THRED_EXIT == dwBytesTransfered && 0 == pVoidContextKey)
		{
			//service exit thread
			break;
		}
		if (FALSE == nRetCode && 0 == dwBytesTransfered  && NULL == pVoidContextKey)
		{
			//CloseHandle(m_CompletionPort);
			//��ɶ˿ڹر�
			break;
		}
		if (TRUE == nRetCode && 0 == dwBytesTransfered  && NULL != pVoidContextKey)//����͑��˔��_
		{
			//client close tcp
			if (0 == m_setIOCPKEY.size())
			{
				break;
			}
			{
				CMutex::Lock lock(m_mutex);//�����ṹ����lock����ʼ�������������캯�������������Զ������ͷŽ���
				m_setIOCPKEY.erase(pIOCPContext);//ɾ����ǰ����
			}
			shutdown(pIOCPContext->clientSocket,SD_BOTH);
			closesocket(pIOCPContext->clientSocket);
			delete pIOCPContext;//�ͷ���ָ��
			pIOCPContext = NULL;

			continue;
		}
		if (NULL == pVoidContextKey)
		{
			//����keyΪNULL
			continue;
		}

		
		if (TRUE == nRetCode && lpOverlapped && pVoidContextKey )//�ɹ��Ĳ���
		{
			
			nSocket = pIOCPContext->clientSocket; //��ȡ��ǰ��socket����

			//ҵ���߼�����
			switch (pIOCPContext->opType)
			{
			case  RECV_POSTED: //1) �յ��ͻ���������								
			{
				//totalcnt+=dwBytesTransfered;
				//printf("���������ۼƣ�%d\n" , totalcnt);

				EnterCriticalSection(&m_RECdataLock);//Ӳ�����ݽ����̼߳���

				if (pIOCPContext->Buffer.len < REC_SIZE)
				{
					memcpy(m_SocReUnit.RecData, pIOCPContext->Buffer.buf, pIOCPContext->Buffer.len);//ȫ�ֱ�����������			
					m_SocReUnit.DataLen = dwBytesTransfered;
					//nSocket = pIOCPContext->clientSocket; //��ȡ��ǰ��socket����

					m_SocReUnit.SocketNum = nSocket;
					//Ӳ�����������̳߳أ����Ӳ������.��ӽ�������У��Ŷӵȴ��߳�������
					//��������������ʵ���Ǵ�����Ҫִ�е��������������һ�Σ������һ�Ρ�ʵ������û�д洢����ȫ�ֱ���
					WorkDatathreadpool.addTask(p, NORMAL);//��ѯ���е��̲߳����ѣ��Ӷ�ִ��������ľ������
													
				}
				LeaveCriticalSection(&m_RECdataLock);//����

				//Ͷ���¸�RECV����
				memset(pIOCPContext->Buffer.buf, 0, pIOCPContext->Buffer.len);//���ջ�����0
				pIOCPContext->Buffer.len = DATA_LEN;//���û����С
				pIOCPContext->NumberOfBytesRecv = 0;
				pIOCPContext->NumberOfBytesSend = 0;
				pIOCPContext->opType = RECV_POSTED;
				int iRecv = WSARecv(nSocket, &pIOCPContext->Buffer, 1, &recvBytes, &Flags, &m_overlap, NULL);//����socket����
				if (SOCKET_ERROR == iRecv && WSA_IO_PENDING != WSAGetLastError())
				{
					//���մ��������socket
					if (0 == m_setIOCPKEY.size())
					{
						break;
					}
					{
						CMutex::Lock lock(m_mutex);//�����ṹ����lock����ʼ�������������캯�������������Զ������ͷŽ���
						m_setIOCPKEY.erase(pIOCPContext);
					}
					shutdown(pIOCPContext->clientSocket, SD_BOTH);
					closesocket(pIOCPContext->clientSocket);
					delete pIOCPContext;
					pIOCPContext = NULL;
					continue;
				}
				else
				{
					//ok
				}
				/**********/

#if    0
				pIOCPContext->NumberOfBytesRecv = dwBytesTransfered;
				pIOCPContext->opType = SEND_POSTED;
				memcpy(pIOCPContext->Buffer.buf, "RECOK!", 6);
				//���÷������ݳ���(������Ȳ����ܳ����յ�����ĳ���)
				pIOCPContext->Buffer.len = 6;
				//do your logic by default send origin data
				//�������� WSASend����������֧��һ�η��Ͷ��BUFFER������,������send�ĵ��ô�����ʵ�ʼ���֤����ʹ��WSASend�������50%��������������
				int iRetSend = WSASend(nSocket, &pIOCPContext->Buffer, 1, &sendBytes, 0, &m_overlap, NULL);
				if (SOCKET_ERROR == iRetSend && WSA_IO_PENDING != WSAGetLastError())
				{
					//���ʹ��������socket
					if (0 == m_setIOCPKEY.size())
					{
						break;
					}
					{
						CMutex::Lock lock(m_mutex);//�����ṹ����lock����ʼ�������������캯�������������Զ������ͷŽ���
						m_setIOCPKEY.erase(pIOCPContext);
					}
					shutdown(pIOCPContext->clientSocket, SD_BOTH);
					closesocket(pIOCPContext->clientSocket);
					delete pIOCPContext;
					pIOCPContext = NULL;
					continue;
				}
				else
				{
					//ok
				}
#endif

				//������յ����ݳ���dwBytesTransfered��û�е�����Ҫ�ĳ��ȣ����������һ��WSARecv����д������ʱ�򲢰����
				//buffer ָ����Ŵ�ָ��ĺ��棬ֱ�����ܵĳ���Ϊ��������ĳ���
#if    0
				//����ʾ��Ϊ���ؿͻ�����������:
				//�յ��ͻ��˷��͹���������
				pIOCPContext->NumberOfBytesRecv = dwBytesTransfered;
				pIOCPContext->opType = SEND_POSTED;

				//���÷������ݳ���(������Ȳ����ܳ����յ�����ĳ���)
				pIOCPContext->Buffer.len = dwBytesTransfered;
				//do your logic by default send origin data
				//�������� WSASend����������֧��һ�η��Ͷ��BUFFER������,������send�ĵ��ô�����ʵ�ʼ���֤����ʹ��WSASend�������50%��������������
				int iRetSend = WSASend(nSocket, &pIOCPContext->Buffer, 1, &sendBytes, 0, &m_overlap, NULL);
				if (SOCKET_ERROR == iRetSend && WSA_IO_PENDING != WSAGetLastError())
				{
					//���ʹ��������socket
					if (0 == m_setIOCPKEY.size())
					{
						break;
					}
					{
						CMutex::Lock lock(m_mutex);//�����ṹ����lock����ʼ�������������캯�������������Զ������ͷŽ���
						m_setIOCPKEY.erase(pIOCPContext);
					}
					shutdown(pIOCPContext->clientSocket, SD_BOTH);
					closesocket(pIOCPContext->clientSocket);
					delete pIOCPContext;
					pIOCPContext = NULL;
					continue;
				}
				else
				{
					//ok
				}
#endif

			}break;
#if   1
			case  SEND_POSTED:
				//ҵ���߼�����
				//2> ���������������봦����Ǽ���Ͷ����Ϣ��
			{

				pIOCPContext->NumberOfBytesSend += dwBytesTransfered;
				//�鿴������û�з������(��Ŀǰ�ӵ�������ȫ�����ص�Ĭ������,Ӧ�÷������ݳ���Ϊԭʼ���յ������ݳ���)
				if (pIOCPContext->NumberOfBytesSend  <  pIOCPContext->NumberOfBytesRecv)
				{
					//û�з�����ɣ���������,����ͼ�����������
					pIOCPContext->Buffer.buf = pIOCPContext->Buffer.buf + pIOCPContext->NumberOfBytesSend;
					pIOCPContext->Buffer.len = (pIOCPContext->NumberOfBytesRecv - pIOCPContext->NumberOfBytesSend);

					int iRetSend = WSASend(nSocket, &pIOCPContext->Buffer, 1, &sendBytes, 0, &m_overlap, NULL); //��������ʣ���   

					if (SOCKET_ERROR == iRetSend && WSA_IO_PENDING != WSAGetLastError())
					{
						//���ʹ��������socket
						if (0 == m_setIOCPKEY.size())
						{
							break;
						}
						{
							CMutex::Lock lock(m_mutex);//�����ṹ����lock����ʼ�������������캯�������������Զ������ͷŽ���
							m_setIOCPKEY.erase(pIOCPContext);
						}
						shutdown(pIOCPContext->clientSocket, SD_BOTH);
						closesocket(pIOCPContext->clientSocket);
						delete pIOCPContext;
						pIOCPContext = NULL;
						continue;
					}
					else
					{
						//ok
					}

				}
				else //�������
				{
					//Ͷ���¸�RECV����
					memset(pIOCPContext->Buffer.buf, 0, pIOCPContext->Buffer.len);//���ջ�����0
					pIOCPContext->Buffer.len = DATA_LEN;//���û����С
					pIOCPContext->NumberOfBytesRecv = 0;
					pIOCPContext->NumberOfBytesSend = 0;
					pIOCPContext->opType = RECV_POSTED;
					int iRecv = WSARecv(nSocket, &pIOCPContext->Buffer, 1, &recvBytes, &Flags, &m_overlap, NULL);//����socket����
					if (SOCKET_ERROR == iRecv && WSA_IO_PENDING != WSAGetLastError())
					{
						//���մ��������socket
						if (0 == m_setIOCPKEY.size())
						{
							break;
						}
						{
							CMutex::Lock lock(m_mutex);//�����ṹ����lock����ʼ�������������캯�������������Զ������ͷŽ���
							m_setIOCPKEY.erase(pIOCPContext);
						}
						shutdown(pIOCPContext->clientSocket, SD_BOTH);
						closesocket(pIOCPContext->clientSocket);
						delete pIOCPContext;
						pIOCPContext = NULL;
						continue;
					}
					else
					{
						//ok
					}
				}
			}break;
#endif
			default:
				break;
			}//end switch

		}
		
	}//end while
	return 0;
}

void CSocketTCP::Close()
{
	//�رտͻ���
	closeAllClient();

	//�رշ����socket
	if (-1 != m_sockfd)
	{
		//closesocket(m_sockfd);
		m_sockfd = -1;
	}
}

int CSocketTCP::GetLastError()
{
	return WSAGetLastError();
}

char* CSocketTCP::GetSocketIP()
{
	sockaddr_in sin;
	int len = sizeof(sin);
	if(getsockname(m_sockfd, (struct sockaddr *)&sin, &len) != 0)
	{
		//printf("getsockname() error:%s\n", strerror(errno));
		return NULL;
	}
	return inet_ntoa(sin.sin_addr);
}

int CSocketTCP::GetSocketPort()
{
	sockaddr_in sin;
	int len = sizeof(sin);
	if(getsockname(m_sockfd, (struct sockaddr *)&sin, &len) != 0)
	{
		//printf("getsockname() error:%s\n", strerror(errno));
		return 0;
	}
	return ntohs(sin.sin_port);
}