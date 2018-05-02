
#ifndef  M_COMM_H_
#define  M_COM_H_

#pragma once
#include "stdafx.h"
#include <iostream>
#include <string>// ע����C++��׼����<string>������<string.h>����.h����C�����е�ͷ�ļ�
//#include <Windows.h>
#include "MyThreadPool.h"//ע�⣬��ͷ�ļ����ܷŵ�ǰ��
#include "MyThread.h"
#include "TestTask.h"
#include "json.h"

////////////////////////////////////////////////////////////////////////////

using namespace std;

#define  WIN_64_DEF       1

#define  MYPORT                9002
#define  APP_PORT              5678
#define  MAX_APP_TCP_NUM            10000
#define  MAX_HARD_TCP_NUM           10000
////////////////////////////////////////////////////////////////////////////
#define  REC_SIZE         1024*100*1 //Ӳ�������˿����ݻ���
#define  FULL_SIZE        1024*100*1
#define  HARD_DATA_SIZE   1024*100*1  //���洢�Ľ�����ĵ���Ӳ������
#define  GUID_LEN 64
////////////////////////////////////////////////////////////////////////////
typedef   struct   RecUnit{   //�Զ���

	unsigned  char  RecData[REC_SIZE];
	unsigned  long  DataLen;
	SOCKET  SocketNum;
}SocketRecUnit;
/////////////////////////////////////////////////////////////////////////
typedef   struct   HardUnit{   //�Զ�������������������ݽṹ

	unsigned  char  RecData[HARD_DATA_SIZE];
	unsigned  int  DataLen;
	int  cmd;
	SOCKET  SocketNum;
	
}HardData;


//////////////////////////////////////////////////////////////////////////// 
#define     RF_CARD_NUM              1*10000  //�ɴ洢�Ŀ�������
#define     RF_CARD_LEN              4
////////////////////////////////////////////////////////////////////////////
#define    DATA_HEAD_LEN                  4+19 //�̶�Э��ͷ����
#define    DATA_CLIENT_DATA_NUM_LEN       2  //�������������ݸ�����ÿ���������ݵĳ���
#define    DATA_CLIENT_DATA_LEN           6  //ÿ���������ݶ����� ����+״̬+����
#define    DATA_CARD_LEN                  (DATA_CLIENT_DATA_NUM_LEN+DATA_CLIENT_DATA_LEN)
#define    DATA_END_LEN                   1  //У��
#define    APP_MIN_DATA_LEN               (DATA_HEAD_LEN+DATA_END_LEN) //APP��С���ݰ�
#define    DATA_WHOLE_PACKET_LEN          (DATA_HEAD_LEN+DATA_CLIENT_DATA_NUM_LEN+DATA_CLIENT_DATA_LEN+DATA_END_LEN)
////////////////////////////////////////////////////////////////////////////
#define     SEVER_TO_CLIENT       0x5A
#define     CLIENT_TO_SEVER       0x5B
#define     CLIENT_GPS_INFO       0x5C    //Ӳ���ϴ�GPS��λ����Ϣ
#define     CLIENT_BASE_STATION_INFO       0x5D    //Ӳ���ϴ���GPS��λ����Ϣ���ϴ���վ��Ϣ
////////////////////////////////////////////////////
#define     APP_SET_LOCK          0x2A    //APP������������
#define     APP_SET_UNLOCK        0x2B    //APP���ý�������
#define     APP_CONFIG_WEILAN     0x2C    //APP������ͣΧ��
#define     APP_REGESTER_USER     0x1E    //APPע���û�
#define     APP_SET_DENG_LU       0x1F    //APP��¼����
#define     APP_SET_ZHUXIAO       0x20    //APPע������
#define     APP_SET_BANGDING      0x2D    //APP���豸����
#define     APP_SET_SHANCHU       0x2E    //APPɾ���豸����
#define     APP_SET_GET_STATE     0x2F    //APP��ѯ�豸״̬����
#define     APP_GET_ALL_DEVICE_STATE     0x30    //APP��ѯ��ǰ�û����������豸״̬����
#define     APP_REQUEST_WEILAN_ALARM     0x31  //��ѯΧ����������
#define     APP_REQUEST_WEILAN_DATA      0x32  //��ѯΧ������
////////////////////////////////////////////////////////////////////////////
typedef  struct RFIDcard {
	unsigned  char  Data[RF_CARD_LEN];//EPC����	
	unsigned  char  State;	//��״̬
	unsigned  char  LockFlag; //������־
	unsigned  char  DataLen; //���ų���
	unsigned  char  UpdataFlag; //�����ϴ���־
	unsigned  char  ReadCardDevice; //�����豸��
	unsigned  char  ValidDataFlag;	//��Ч���ݱ�־
	unsigned  long  socket; 

}RFcard; //�ýṹ��ռ24�ֽڣ����������ֽ��˷�,4B���뵼��

//����hash��ͻ������ݽڵ�
typedef struct _NODE 
{ 
  int data; 
  struct _NODE* next; 
}NODE; 
  
typedef struct _HASH_TABLE 
{ 
  NODE* value[10]; 
}HASH_TABLE; 

///////////////////////////////////////////////////////////////////////////
extern  HANDLE  RecEvent[2];  //���¼�
extern  HANDLE  WX_CheckAlarmEvent[2];
extern  HANDLE  WX_GetTokenEvent[2];//΢�Ż�ȡACESS_TOKEN�¼�
////////////////////////////////////////////////
extern  char m_uuid[GUID_LEN];
extern  SocketRecUnit   m_SocReUnit; //�Զ������ݻ���ṹ�壬����socket����
extern  SocketRecUnit   mAPP_RevUnit;
extern  string   card_list;//card�б�
extern  unsigned  long  totalcnt;
extern  RFcard  RfCardData[RF_CARD_NUM];
extern  CMyThreadPool  WorkDatathreadpool; //Ӳ���������ݽ���̳߳�
extern  CMyThreadPool  SaveDatathreadpool;//Ӳ���ϴ����ݴ洢�̳߳�
extern  CMyThreadPool  APPthreadpool; //APP���������̳߳�
extern  CRITICAL_SECTION  m_RECdataLock;
extern  CRITICAL_SECTION  mAPP_RECLock;
extern  CRITICAL_SECTION  mSaveDataLock;//Ӳ���ϴ������ݴ洢�̻߳�����
extern  CRITICAL_SECTION  card_list_Lock;
///////////////////////////////////////////////////////////////////////////
char*  MakeUUID();
int  ParseJsonFromString(char *mstr , Json::Value  *mJson); 
///////////////////////////////
int   TranslateJson(const string strData);
unsigned  char    DataS_Compare(unsigned  char * p1  ,unsigned  char  *p2, unsigned  short  len);
unsigned  char  * FindByteBuff(unsigned  char  *src , unsigned  long srclen ,  unsigned char *dec , unsigned  short datalen );
char   SaveClientData(SOCKET ClientS , int cmd , unsigned  char * src, unsigned  int  len);
char   RecClientData(SOCKET ClientS ,  unsigned char * src, unsigned  int  len );
char  SaveDataSQL(SOCKET ClientS ,int Command, unsigned char * src, unsigned  int  len);
int   SaveHardStateInfo(SOCKET ClientS , Json::Value  mJsonValue);
int   SaveGPSData(SOCKET ClientS ,  unsigned  char * src ,unsigned  int  len);
int   SaveBaseStationData(SOCKET   ClientS ,  unsigned  char * src ,unsigned  int  len);
char  RecAPPData(SOCKET ClientS, unsigned char * src, unsigned  int  len );
char  SaveAPPData(SOCKET ClientS ,int Command, char * src, unsigned  int  len);
int  CheckUserToken(SOCKET ClientS , string tok);
int  InitInsertBikeDatabaseUser(void);
int  InitInsertBikeDatabaseCardinfo(void);
int  UpdateCardState(void);
int  APPUpdateUserState(SOCKET ClientS ,Json::Value  mJsonValue);
int  FindToken(string  token);
int  getSQLstr(string  m_findstr, Json::Value  mJsonValue );
int  responseCheckTokenErr(SOCKET ClientS);
/////////////////////////////////////////////////////////////////////
int  responseAPPregester(SOCKET ClientS ,int Command, char * src, unsigned  int  len);
int  responseAPPdenglu(SOCKET ClientS ,int Command, char * src, unsigned  int  len);
int  responseAPPbangding(SOCKET ClientS ,Json::Value  mJsonValue);
int  responseAPPbind (SOCKET ClientS ,int Command, char * src, unsigned  int  len);
int  responseAPPgetstate(SOCKET ClientS ,Json::Value  mJsonValue);
int  responseAPPjiebang(SOCKET ClientS ,Json::Value  mJsonValue);
string  getNullStr(const char* str ); 
int  GetVaule_CheckStr(string *str , Json::Value  mJsonValue , string  str_value);
int  responseAPPgetalldevicestate(SOCKET ClientS ,Json::Value  mJsonValue);
///////////////////////////////////////////////////////////////////////
int  responseAPPrequest_weilan_alarm(SOCKET   ClientS ,Json::Value  mJsonValue);
///////////////////////////////////////////////////////////////////////
int  APPCheckAlarmTable(SOCKET ClientS , string gps , string  card);
///////////////////////////////////////////////////////////////////////
double GPSdufen_to_du(double dufen);
int  PanDuanWeiLan(string  weilan_radius , string  weilan_gps  , string  gps);
int  APPSetWeiLan(SOCKET   ClientS ,Json::Value  mJsonValue);
int  APPUnSetWeiLan(SOCKET   ClientS ,Json::Value  mJsonValue);
int  APPConfigWeiLan(SOCKET   ClientS ,Json::Value  mJsonValue);
int  responseAPPrequest_weilan_data(SOCKET  ClientS , Json::Value  m_json);
///////////////////////////////////////////////////////////////////////
DWORD  WINAPI  WX_CheckAlarmThread (LPVOID lpParameter);
DWORD  WINAPI  WX_GetTokenThread (LPVOID lpParameter);
////////////////////�����ʼ�SMTP��ʽSSL��������//////////////////////////////
int  Email_SMTP_send_simple(string  smtp_server, string  SenderEmail, string SenderAccount, string SenderPWD,
						string RecEmail, Json::Value json_email);
//GBK����ת����UTF8����
int GBKToUTF8(unsigned char * lpGBKStr, unsigned char * lpUTF8Str, int nUTF8StrLen);
string  GetUTF8FromGBK(string gbk_src);
string UTF8ToGBK(const char* strUTF8);
/////////////////////////////////////////////////////////////////////////
#endif



