// socket�����.cpp : �������̨Ӧ�ó������ڵ㡣
///////////////////////////////////////////////
#pragma once
#include "stdafx.h"
#include <iostream>
#include <process.h>
#include <winsock2.h>
#include <ObjBase.h>
//#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <string>// ע����C++��׼����<string>������<string.h>����.h����C�����е�ͷ�ļ�
#include <fstream>
#include <WinBase.h>
#include "SocketTCP.h"
#include "SocketAPP.h"
#include "mcom.h"
#include "CSmtp.h"//���ʼ���

//#include <Windows.h>////<Windows.h>���ͷ�ļ�������<winsock2.h>ǰ��
///////////////////////////////////////////

#include "MyThreadPool.h"//ע�⣬��ͷ�ļ���ͷ��windows.h,//<Windows.h>���ͷ�ļ�������<winsock2.h>ǰ��
#include "MyThread.h"  ////<Windows.h>���ͷ�ļ�������<winsock2.h>ǰ��
#include "TestTask.h"
///////////////////////////////////////////
#ifdef  WIN_64_DEF
#include <mysql.h>
#pragma comment(lib,"libmysql.lib") 
#endif

#pragma comment (lib,"ole32.lib") 
#pragma comment (lib, "ws2_32.lib")  //���� ws2_32.dll
///////////////////////////////////////////////////
class  CMySaveQueue;

using namespace std;
////////////////////////////////////////////////////
//IOCPContextKey* pContextKey = new IOCPContextKey; //����һ��ṹ���ڴ�,��IOCP��ɶ˿ڴ�������

///////////////////////////////////////////////////
#define TEST_BUFFER_SIZE       1000
///////////////////////////////////////////////////
HANDLE  RecEvent[2];  //���¼� 
HANDLE  WX_CheckAlarmEvent[2];//΢�Ų�ѯ��������¼�
HANDLE  WX_GetTokenEvent[2];//΢�Ż�ȡACESS_TOKEN�¼�
//////////////////////////////////////////////////////
char buffer[TEST_BUFFER_SIZE]={
"shaozhanyu-socketconncetOK!!"
};
char m_uuid[GUID_LEN] = { 0 }; 
//////////////////////////////////////////////////////
string  RecStr="";//����cstring �����
SOCKET servSock;
#ifdef  WIN_64_DEF
MYSQL mysql;
MYSQL_RES* result;
MYSQL_ROW row; 
#endif
///////////////////////////////////////////////////////
CSocketTCP   IOCPsocket;//�����Զ����࣬�����Ĺ��캯���У�������win2sock��������
CSocketAPP   APPsocket;
unsigned  long  totalcnt=0;//�ܼ���
SocketRecUnit   m_SocReUnit; //Ӳ���������ݻ���ṹ�壬����socket����
SocketRecUnit   mAPP_RevUnit; //APP���ݻ���ṹ�壬����socket����
string   card_list;//card�б�
/////////////////////////////////////////////	
int len=0;  
int  reccnt=0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////
int   socketnum[100]={0};
int   sockecnt=0;//socket�ͻ���������
RFcard  RfCardData[RF_CARD_NUM];

CMyThreadPool  WorkDatathreadpool; //Ӳ���������ݽ���̳߳�
CMyThreadPool  SaveDatathreadpool;//Ӳ���ϴ����ݴ洢�̳߳�
CMyThreadPool  APPthreadpool; //APP���������̳߳�

CRITICAL_SECTION  m_RECdataLock;//Ӳ�����ݽ��ٽ�������
CRITICAL_SECTION  mAPP_RECLock;//APP���ݽ����ٽ�������
CRITICAL_SECTION  mSaveDataLock;//Ӳ���ϴ������ݴ洢�߳��ٽ�������
CRITICAL_SECTION  card_list_Lock;//Ӳ���ϴ������ݴ洢�߳��ٽ�������
////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////Ӳ��socket����˼����ȴ�����accept///////////////////////////////////////////////////////////////
int   socket_accept_thread(void *ptr)
{
	SOCKADDR_IN     connetAdrr;
	int len = sizeof(sockaddr);

	SOCKET  max_value = 0xfffffffffffffffe;

	while (true) { //��ѭ���ȴ�����accept

		printf("������listen������Ӳ��socket��: %d", IOCPsocket.m_sockfd);
		IOCPsocket.m_newClinetSockfd = accept(IOCPsocket.m_sockfd, (struct sockaddr*)&connetAdrr, &len);//�ȴ�socket�ͻ�������

		if (IOCPsocket.m_newClinetSockfd == INVALID_SOCKET)
		{
			Sleep(500);
			printf("Ӳ���ͻ���������Чaccept����ֵ%d\r\n", IOCPsocket.m_newClinetSockfd);
			IOCPsocket.closeAllClient(); //�ر���ɶ˿ڣ��ͷ������Դ���������
			IOCPsocket.Create("", MYPORT, true);//����SOCKET����ˣ����Ӳ������
			IOCPsocket.Listen(10);
			continue;
		}
		else  if (IOCPsocket.m_newClinetSockfd < max_value)
		{
			if (IOCPsocket.m_setIOCPKEY.size() < MAX_HARD_TCP_NUM) //�ж�������
			{
				printf("\n����һ��Ӳ���ͻ��ˣ�%s,�ۼ�������%d\r\n", inet_ntoa(connetAdrr.sin_addr), IOCPsocket.m_setIOCPKEY.size());
			}
			else
			{
				printf("Ӳ������������\n");
				shutdown(IOCPsocket.m_newClinetSockfd, SD_BOTH);
				closesocket(IOCPsocket.m_newClinetSockfd);
				continue;
			}
		}

		try {

			IOCPContextKey * pContextKey = new IOCPContextKey; //����һ��ṹ���ڴ�,��IOCP��ɶ˿ڴ�������													
			CMutex::Lock lock(IOCPsocket.m_mutex);//�����������ٽ���,���캯���е��õ�

			memset(pContextKey->szMessage, 0, pContextKey->Buffer.len);//��ǰ������0
			memset(pContextKey->Buffer.buf, 0, pContextKey->Buffer.len);//��ǰ������0
			pContextKey->clientSocket = IOCPsocket.m_newClinetSockfd;//���浱ǰ���Ӻ�
			pContextKey->opType = RECV_POSTED;
			IOCPsocket.m_setIOCPKEY.insert(pContextKey);//�������һ���ͻ���key	
			//��IOCP��ɶ˿ڣ�ע���������(ULONG_PTR)pContextKey�����ǿ��ת��DWORDС�ˣ�64λϵͳ��Ұָ��
			CreateIoCompletionPort((HANDLE)IOCPsocket.m_newClinetSockfd, IOCPsocket.m_CompletionPort, (ULONG_PTR)pContextKey, 0);//��ǰ���Ӱ󶨵�IOCP��ɶ˿�
																																 //Ͷ�ݵ�һ��rev��Ϣ
			DWORD Flags = 0;
			int iRev = WSARecv(IOCPsocket.m_newClinetSockfd, &pContextKey->Buffer, 1, &pContextKey->NumberOfBytesRecv, &Flags,
				&IOCPsocket.m_overlap, NULL);
			//printf("��IOCP�˿ڳɹ����Ѿ�Ͷ�ݽ����¼�\r\n");
			//int iRetSend = WSASend(IOCPsocket.m_newClinetSockfd , (LPWSABUF)buffer ,1,(LPDWORD)strlen(buffer) ,0, &IOCPsocket.m_overlap,NULL); //����ʣ���   

		}
		catch (const bad_alloc * E) {
			cout << "pContextKey�����ڴ�ռ�ʧ��" << E << endl;	
			//system("pause");
			//return -1;
		}
	}
	return 0;

}

////////////////////////////����˵ȴ�����///////////////////////////////////////////////////////////////////////
int   APP_accept_thread(void *ptr)
{
	SOCKADDR_IN    connetAdrr;
	int len = sizeof(sockaddr);
	SOCKET  max_value = 0xfffffffffffffffe;
	while (true) { //�ȴ�����accept

		printf("������listen������APP���socket��: %d", APPsocket.m_sockfd);
		APPsocket.m_newClinetSockfd = accept(APPsocket.m_sockfd, (struct sockaddr*)&connetAdrr, &len); //�ȴ�socket�ͻ�������
		if (APPsocket.m_newClinetSockfd == INVALID_SOCKET)
		{
			Sleep(500);
			printf("APP�ͻ���������Чaccept����ֵ%d\n", APPsocket.m_newClinetSockfd);
			APPsocket.closeAllClient(); //�ر���ɶ˿ڣ��ͷ������Դ
			APPsocket.Create("", APP_PORT, true); //����socket�������APP����
			APPsocket.Listen(10);
			continue;
		}
		else  if (APPsocket.m_newClinetSockfd < max_value)
		{
			if (APPsocket.m_setIOCPKEY.size() < MAX_APP_TCP_NUM) //�ж�������
			{
				printf("\n����һ��APP�ͻ��ˣ�%s, �ۼ�������%d\r\n", inet_ntoa(connetAdrr.sin_addr), APPsocket.m_setIOCPKEY.size());
			}
			else
			{
				printf("APP����������\n");
				shutdown(APPsocket.m_newClinetSockfd, SD_BOTH);
				closesocket(APPsocket.m_newClinetSockfd);
				continue;
			}


		}
		try {
			
			IOCPContextKeyAPP* pContextKey = new IOCPContextKeyAPP;//�½��û����ݻ�����

			CLockMutex::Lock lock(APPsocket.m_mutex);//�����������ٽ���,���캯���е��õ�
			
			memset(pContextKey->szMessage, 0, pContextKey->Buffer.len);//��ǰ������0
			pContextKey->clientSocket = APPsocket.m_newClinetSockfd;//���浱ǰsocket���Ӻ�
			pContextKey->opType = APP_RECV_POSTED;
			APPsocket.m_setIOCPKEY.insert(pContextKey);//���һ���ͻ���key��������
			//��IOCP��ɶ˿ڣ�ע���������(ULONG_PTR)pContextKey�����ǿ��ת��DWORDС�ˣ�64λϵͳ��Ұָ��
			CreateIoCompletionPort((HANDLE)APPsocket.m_newClinetSockfd, APPsocket.m_CompletionPort, (ULONG_PTR)pContextKey, 0);//��ǰsocket�󶨵�IOCP��ɶ˿�

																															   //Ͷ�ݵ�һ��rev��Ϣ;
			DWORD Flags = 0;
			int iRev = WSARecv(APPsocket.m_newClinetSockfd, &pContextKey->Buffer, 1, &pContextKey->NumberOfBytesRecv, &Flags,
				&APPsocket.m_overlap, NULL);
		}
		catch (const bad_alloc * E) {
			cout << "pContextKey�����ڴ�ռ�ʧ��" << E << endl;
			//system("pause");
			//return -1;
		}
	}

	return 0;

}

////////////////////////��ʱ���������߳�////////////////////////////////////
DWORD  WINAPI  SaveFileDataThread (LPVOID lpParameter)
{  
	FILE *fpin;
	time_t   rawtime;
	struct tm *curTime;
	char filename[256];

	while(true)  
	{  
		//ÿ�ε�5000����   
		int nIndex = WaitForMultipleObjects(1, RecEvent, FALSE,2000);     

		if (nIndex == WAIT_OBJECT_0 + 1)   
		{  
			//�ڶ����¼�����   //ExitThread(0);   //break;  
			/*********/
			time(&rawtime);//��ȡ��ǰʱ��
			curTime = localtime ( &rawtime );
			//strcat(filename,inet_ntoa(connetAdrr.sin_addr));
			sprintf(filename,"%04d-%02d-%02d-%02d-%02d-%02d.txt",curTime->tm_year+1900,
			curTime->tm_mon+1,curTime->tm_mday,curTime->tm_hour,curTime->tm_min,
			curTime->tm_sec);
			
			//printf("%s",filename);
			fpin=fopen(filename,"wb");//��д�ķ�ʽ���ļ�
			if(!fpin)//�ж��ļ����Ƿ�ɹ�
			{
				printf("file creat wrong\n");
				exit(1);
			}
			fclose(fpin);//�ر��ļ�
			memset(filename , 0 ,sizeof(filename));
			/**********/
		}   
		else if (nIndex == WAIT_OBJECT_0) //��һ���¼�����    
		{   
			//��һ���¼�  
			if(m_SocReUnit.DataLen)
			time(&rawtime);//��ȡ��ǰʱ��
			curTime = localtime ( &rawtime );
			sprintf(filename,"%04d-%02d-%02d-%02d-%02d-%02d.jpg",curTime->tm_year+1900,
			curTime->tm_mon+1,curTime->tm_mday,curTime->tm_hour,curTime->tm_min,
			curTime->tm_sec);
			//printf("%s",filename);
			fpin=fopen(filename,"wb");//��д�ķ�ʽ���ļ�
			if(!fpin)//�ж��ļ����Ƿ�ɹ�
			{
				printf("file creat wrong\n");
				exit(1);
			}
			fseek(fpin, 0, SEEK_END);//ָ���ļ�ĩβ
			fwrite (m_SocReUnit.RecData , sizeof(unsigned char), m_SocReUnit.DataLen , fpin);
			//printf("�ļ�д�����!\n");//д�������ʾ
			fclose(fpin);//�ر��ļ�
			//printf("jpg�ļ�����ɹ�!\n");//

		}    
		else if (nIndex == WAIT_TIMEOUT) //��ʱ500����    
		{   //��ʱ������ʱ��    
	
		}   
	}  
	 printf("�߳̽���\n");  

	 return 0;
	
}


// ������Ϣ���߳�ִ�к���
DWORD WINAPI ServerSendThread(LPVOID IpParam)
{
	printf("socket�����̣߳�\r\n");
	
	//WaitForSingleObject();
	return 0;
}

#ifdef  WIN_64_DEF
//////////////////////��ʼ���������ݿ��û���/////////////////////////////////
int  InitInsertBikeDatabaseUser()
{
	int i,j;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";    
	char  card[]="01000000";
	unsigned  char  cardhex[4]={0x01,0x00,0x00,0x00};//���ŵ��ֽ���ʾ4���ֽ�
	DWORD  card4bytes = 0x01000000;
	char  insertUserSQL[]= "INSERT INTO user(username, userpwd,card,bikename,token,time)    \
			 VALUES(NULL,NULL,'12000000',NULL,'12345', NULL )";
    char *pch;
	unsigned int port = 3306;        
        
    MYSQL myCont;
//    MYSQL_RES *result;
//    MYSQL_ROW sql_row;
    int res;
   if (mysql_init(&myCont) == NULL)//��ʼ��mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {
        //mysql_query(&myCont, "SET NAMES utf8"); //���ñ����ʽ
      
    }
    else
    {
        cout << "connect failed!" << endl;
		mysql_close(&myCont);
		return 0;
    }

	pch=strstr(insertUserSQL,"12000000");
	if(pch!=NULL)
	{
		printf("\nfindSUCESS!\n");
	}
	else
	{	
		mysql_close(&myCont);
		return 0;
	}
	card4bytes =0;
	for(i=0;i<1000;i++)
	{
		cardhex[3] = card4bytes&0x000000ff;
		cardhex[2] = (card4bytes>>8)&0x000000ff;
		cardhex[1] = (card4bytes>>16)&0x000000ff;
		cardhex[0] = (card4bytes>>24)&0x000000ff;
		
		for(j=0;j<4;j++)
		sprintf(&card[2*j], "%02x", cardhex[j]); //Сд16 ���ƣ����ռ2��λ�ã��Ҷ��룬���㲹0

		memcpy(pch,card,8);
		res = mysql_query(&myCont, insertUserSQL);
        
		if (!res) 
		{
			printf("Inserted %lu rows\n", (unsigned long)mysql_affected_rows(&myCont));
		} 
		else 
		{
			fprintf(stderr, "Insert error %d: %s\n", mysql_errno(&myCont),
			mysql_error(&myCont));
		}
		card4bytes++;
	}
	mysql_close(&myCont);
	return 1;
}
/////////////////////��ʼ���������ݿ⿨��/////////////////////////////////////////////
int  InitInsertBikeDatabaseCardinfo()
{
	int i,j;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";    
	char  card[]="01000000";
	unsigned int port = 3306; 

	unsigned  char  cardhex[4]={0x01,0x00,0x00,0x00};//���ŵ��ֽ���ʾ4���ֽ�
	DWORD  card4bytes = 0x01000000;
	string str_card , str_uuid;
	
    //char *pch;	       
        
    MYSQL myCont;
//    MYSQL_RES *result;
//    MYSQL_ROW sql_row;
    int res;


   if (mysql_init(&myCont) == NULL)//��ʼ��mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {
        //mysql_query(&myCont, "SET NAMES utf8"); //���ñ����ʽ
       
    }
    else
    {
        cout << "connect failed!" << endl;
		mysql_close(&myCont);
		return 0;
    }

	///////////////////////////////////////////////////////////////////////

	card4bytes =0;
	for(i=0;i<10000;i++)
	{
		cardhex[3] = card4bytes&0x000000ff;
		cardhex[2] = (card4bytes>>8)&0x000000ff;
		cardhex[1] = (card4bytes>>16)&0x000000ff;
		cardhex[0] = (card4bytes>>24)&0x000000ff;
		
		for(j=0;j<4;j++)
		sprintf(&card[2*j], "%02x", cardhex[j]); //Сд16 ���ƣ����ռ2��λ�ã��Ҷ��룬���㲹0
		str_card = card;
		str_uuid = MakeUUID();
		string  InsertCard  = "INSERT INTO cardinfo( card , used ,serialnumber ,device ,time)    \
			 VALUES(  '" + str_card + "',NULL ,'" + str_uuid + "', NULL , NOW() )";

		res = mysql_query(&myCont,   (const  char *)InsertCard.c_str());
        
		if (!res) 
		{
			printf("Inserted %lu rows\n", (unsigned long)mysql_affected_rows(&myCont));
		} 
		else 
		{
			fprintf(stderr, "Insert error %d: %s\n", mysql_errno(&myCont),
			mysql_error(&myCont));
		}
		card4bytes++;
	}
	
    mysql_close(&myCont);
	return 1;
}


/////////////////////���¿�״̬/////////////////////////////////////////////
int  UpdateCardState()
{
//	int i;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";    
	char bike_username[32]="";
	char bike_userpwd[32]="";
	char bike_token[66]="";
	int  bike_app_socket=0;
	int  bike_setlock=0;
	int  bike_update_card =0;
	char bike_card[32]="";
	int  bike_card_socket =1;
	char bike_card_state='I';
	int  bike_card_lock=0;
	char bike_bike_name[64]="";
	char bike_gps[50]=",A,3158.4608,N,11848.3737,E,10.05";
	//struct tm   bike_time;  
/***
	char  UpdateCardStateSQL[]= "UPDATE user SET username='shaozhanyu', userpwd='123456',token='abcd1234567890',app_socket=454,\
				setlock=1,update_card=1,card_socket=251,card_state='IN',card_lock=0,gps=',A,3158.4608,N,11848.3737,E,10.05'  \
				WHERE bikename='abcd'";
***/
	char  UpdateCardStateSQL[]= "UPDATE user SET  userpwd='123456',token='abcd1234567890',app_socket=454,\
				setlock=1,update_card=1,card_socket=251,card_state='IN',card_lock=0,gps=',A,3158.4608,N,11848.3737,E,10.05',  \
				time='2017-5-10-10:02:05', bikename='lvju'   WHERE  username='shaozhanyu'";

//    char *pch;
	unsigned int port = 3306;        
        
    MYSQL myCont;
   // MYSQL_RES *result;
//    MYSQL_ROW sql_row;
    int res;


   if (mysql_init(&myCont) == NULL)//��ʼ��mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {
        //mysql_query(&myCont, "SET NAMES utf8"); //���ñ����ʽ
        printf("connect SQL sucess!\n");
    }
    else
    {
        cout << "connect failed!" << endl;
		mysql_close(&myCont);
		return 0;
    }

	///////////////////////////////////////////////////////////////////////
	
	res = mysql_query(&myCont, UpdateCardStateSQL); //ִ��SQL���
	printf("update SQL sucess!\n");
    mysql_close(&myCont);

	return 1;

}

#endif

////////////////////����UUID//////////////////////////////////////
char*  MakeUUID( )
{
	 
	char *uuid = m_uuid;
    GUID guid; //���� 

    if (CoCreateGuid(&guid))  
    {  
        fprintf(stderr, "create guid error\n");  
        return NULL;
    }  

    _snprintf(m_uuid, sizeof(m_uuid),  
        "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
        guid.Data1, guid.Data2, guid.Data3,  
        guid.Data4[0], guid.Data4[1], guid.Data4[2],  
        guid.Data4[3], guid.Data4[4], guid.Data4[5],  
        guid.Data4[6], guid.Data4[7]);  

    printf("guid: %s\n", m_uuid);  
	return uuid;
}
//////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	FILE *fpin;
	time_t   rawtime;
	struct tm *curTime;
	char filename[256];
    int i=0,j=0,mymax=-99999;
	//char a;
	char inputcmd[50];
	int ret=0;	
	int  feizuse=1;
	int  thread1=15;
	int  thread2=18;
	int  thread3=21;
	int  thread4=23;
	int  thread5=27;
	int *pst=&thread1;
	int *pst2=&thread2;
	int *pst3 = &thread3;
	int *pst4 = &thread4;
	int *pst5 = &thread5;
	sockecnt=0;//socket�ͻ���������

	//InitInsertBikeDatabaseCardinfo();//��ʼ������card
#if  0
//����Ӳ���ϴ�GPS����
//���ݸ�ʽ�� ͷ+����+GPS���� 
unsigned  char     GpsData[120]={
0x96,0x69,0x96,0x69,
0x5C,
0x01,0x00,0x01,0x01,
0x01,0x02,0x03,0x04,
0x01,
0x00,0x08,
0x00,0x05,
0x30,0x30,0x30,0x30,0x30,
0x01,0x06,0x00,0x00,0x00,0x04,0x49,0x01,
0xff
};

char   gpstr[40]="A,3158.4608,N,11848.3737,E,10.05";//GPSά�ȣ����ȣ��ٶ�
memcpy(GpsData +31, gpstr ,strlen(gpstr));
GpsData[15]=strlen(gpstr)+8;
SaveGPSData(1234 , (char*)(GpsData+23) , GpsData[14]*256+GpsData[15] );
#endif		

	//InitInsertBikeDatabaseUser();
	//InitInsertBikeDatabaseCardinfo();
	//UpdateCardState();//�޸�һ�е�ĳ���ֶ�ֵ

	char  json_str[]={"{\
	\"errno\": 0,\
	\"error\":\"succ\",\
	\"data\":\
	{\
		\"card\":\"00000002\",\
		\"card_state\":\"In\",\
		\"card_lock\":1,\
		\"token\":\"EFEW-EFW-EFW-WEFE\",\
		\"bikename\":\"lvju\",\
		\"gps\":\"024813.640,A,3158.4608,N,11848.3737,E,10.05,324.27,150706\",\
		\"time\":\"2017-5-10-17:10:20\"\
	}\
	}"};

	Json::Reader reader;
	Json::Value json_object;
	if (!reader.parse(json_str, json_object))
	return 0;
	//std::cout << json_object["error"] << std::endl;
	//std::cout << json_object["data"]["token"] << std::endl;    
	//std::cout << json_object["data"]["card"] << std::endl;

	//json_object["data"]["shao"] = Json::Value("value_string"); 

	//std::cout << json_object["data"]["shao"] << std::endl;

	std::cout << json_object << std::endl;

	MakeUUID();//����UUID
	//////////////////////////////////////////////////////////////////
	
	//���߳�ִ������������
	WorkDatathreadpool.CreatWorkDataThread(10);//��������߳�
	SaveDatathreadpool.CreatSaveDataThread(20);//�����洢�߳�
	//APPthreadpool.CreatAPPDataThread(10);//����APP����߳�

	//////////////////////////////////////////////////////////////////
	IOCPsocket.Create("" , MYPORT , true);//����SOCKET����ˣ����Ӳ������
	//APPsocket.Create("" ,APP_PORT ,true); //����socket�������APP����

	
	IOCPsocket.Listen(10);
	//APPsocket.Listen(10);

	// �������ڷ������ݵ��߳�
	HANDLE acceptThread = (HANDLE)_beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))socket_accept_thread, pst2, 0, 0); 
	//HANDLE APPacceptThread = (HANDLE)_beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))APP_accept_thread, pst3, 0, 0); 

	//RecEvent[0]=CreateEvent(NULL, FALSE, FALSE, NULL);  
	WX_CheckAlarmEvent[0] = CreateEvent(NULL, FALSE, FALSE, NULL);//�����¼�������΢�Ų�ѯ�����б�����ģ����Ϣ
	WX_GetTokenEvent[0] = CreateEvent(NULL, FALSE, FALSE, NULL);//�����¼�,���ڻ�ȡ΢��TOKEN

	memset(m_SocReUnit.RecData ,0,m_SocReUnit.DataLen);//���ջ�����0
	
	InitializeCriticalSection(&m_RECdataLock);//��ʼ��Ӳ������������
	//InitializeCriticalSection(&mAPP_RECLock);//��ʼ��APP����������
	InitializeCriticalSection(&mSaveDataLock);//��ʼ��Ӳ���ϴ������ݴ洢�̻߳�����
	InitializeCriticalSection(&card_list_Lock);//card�б������
	
	HANDLE WX_AlarmThread = (HANDLE)_beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))WX_CheckAlarmThread, pst4, 0, 0); 
	HANDLE WX_TokenThread = (HANDLE)_beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))WX_GetTokenThread, pst5, 0, 0); 

	//////////////////////////////////////////////////////////////////////////
	while(1)
	{
		printf("�ȴ��������!\n");//
		printf("--------save--------��������Ϊjpg�ļ���!\n");//
		printf("--------total-------��ʾ�ܽ����ֽ�����!\n");//
		printf("--------exit--------�����˳���!\n");//
		printf("--����---��");//
		scanf("%s" ,inputcmd);
		if(strstr(inputcmd,"save"))
		{						
			time(&rawtime);//��ȡ��ǰʱ��
			curTime = localtime ( &rawtime );
			sprintf(filename,"%04d-%02d-%02d-%02d-%02d-%02d.jpg",curTime->tm_year+1900,
			curTime->tm_mon+1,curTime->tm_mday,curTime->tm_hour,curTime->tm_min,
			curTime->tm_sec);
			printf("%s",filename);
			fpin=fopen(filename,"wb");//��д�ķ�ʽ���ļ�
			if(!fpin)//�ж��ļ����Ƿ�ɹ�
			{
				printf("file creat wrong\n");
				exit(1);
			}
			fwrite (card_list.c_str() , sizeof( char), card_list.length() , fpin);

			printf("�ļ�д�����!\n");//д�������ʾ
			fclose(fpin);//�ر��ļ�
			printf("jpg�ļ�����ɹ�!\n");//
		}

		if(strstr(inputcmd,"total"))
		{
			printf("ȫ�������ֽ���Ϊ��%d\r\n" , totalcnt);
		}
		if(strstr(inputcmd,"exit"))
		{
			system("pause");
			break;
		}
	}

	return 0;

}
////////////////////�����ʼ�SMTP��ʽSSL��������//////////////////////////////
int  Email_SMTP_send_simple(string  smtp_server , string  SenderEmail ,string SenderAccount , string SenderPWD, string RecEmail , Json::Value json_email)
{
	bool bError = false;

	try
	{
		CSmtp mail;
		mail.SetCharSet("gb2312");//���ñ���

	#define  test_gmail_ssl

	#if defined(test_gmail_tls)
		mail.SetSMTPServer("smtp.gmail.com", 587);
		mail.SetSecurityType(USE_TLS);
	#elif defined(test_gmail_ssl)
		mail.SetSMTPServer(smtp_server.c_str(), 465);
		mail.SetSecurityType(USE_SSL);
	#elif defined(test_hotmail_TLS)
		mail.SetSMTPServer("smtp.live.com", 25);
		mail.SetSecurityType(USE_TLS);
	#elif defined(test_aol_tls)
		mail.SetSMTPServer("smtp.aol.com", 587);
		mail.SetSecurityType(USE_TLS);
	#elif defined(test_yahoo_ssl)
		mail.SetSMTPServer("plus.smtp.mail.yahoo.com", 465);
		mail.SetSecurityType(USE_SSL);
	#endif

		mail.SetLogin(SenderAccount.c_str());
		mail.SetPassword(SenderPWD.c_str());
		mail.SetSenderName("User");
		mail.SetSenderMail(SenderEmail.c_str());
		mail.SetReplyTo(SenderEmail.c_str());
		mail.SetSubject("����֪ͨ");
		mail.AddRecipient(RecEmail.c_str());
		mail.SetXPriority(XPRIORITY_NORMAL);
		mail.SetXMailer("The Bat! (v3.02) Professional");
		//mail.AddMsgLine("Hello,");
		//mail.AddMsgLine("");
		//mail.AddMsgLine("...");
		//mail.AddMsgLine("How are you today?");
		//string  gbk_str = "�����豸�����ˣ��뼰ʱ�鿴��" ;
		mail.AddMsgLine("�����豸�����ˣ��뼰ʱ�鿴��");
		//mail.AddMsgLine("Regards");
		//mail.ModMsgLine(5, "��ռ��");
		///mail.DelMsgLine(2);
		
		string  gbk_str = "�����豸�ǣ� " + UTF8ToGBK(json_email["bikename"].asString().c_str());

		mail.AddMsgLine(gbk_str.c_str() );
		gbk_str = "����״̬�� " + UTF8ToGBK(json_email["state"].asString().c_str());
		mail.AddMsgLine(gbk_str.c_str());
		gbk_str = "����ʱ�䣺 " + UTF8ToGBK(json_email["alarm_time"].asString().c_str());
		mail.AddMsgLine(gbk_str.c_str());
		//mail.ModMsgLine(2, email_content.c_str());
		//mail.DelMsgLine(2);
		//mail.AddMsgLine("User");

		//mail.AddAttachment("../test1.jpg");
		//mail.AddAttachment("c:\\test2.exe");
		//mail.AddAttachment("c:\\test3.txt");
		mail.SetCharSet("gb2312");
		mail.Send();//��������
	}
	catch (ECSmtp e)
	{
		std::cout << "Error: " << e.GetErrorText().c_str() << ".\n";
		bError = true;
		//system("pause");
	}
	if (!bError)
	{
		std::cout << "Mail was send successfully.\n";
		//system("pause");
	}

	return 0;
}

////////////////////////////////////////////////////