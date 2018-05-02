
#include <winsock2.h>
#include "TestTask.h"
#include "mcom.h"
#include "stdio.h"
#include "SaveQueue.h"
#include<ctime> 

#ifdef  WIN_64_DEF
#include <mysql.h>
#pragma comment(lib,"libmysql.lib") 
#endif
//////////////////////////////////////////////////////////////////////
CMySaveQueue   m_SaveQueueSQL;//��������ʵ����һ���ṹ�����
CTestTask *pTask =new CTestTask(10);//��������
int  saveCNT=0;
///////////////////////////////////////////////////////////////////////
#define   CONTENT_LEN        1024*100*1
#define   REC_LEN            1024*100*1
#define   SEND_LEN           1024*100*1
//////////////////////////////////////////////////////////////////////
char  WX_access_token[700]="7_wVJ4KaDc9lC1BCI6j6aHpNPh_naiQ1Oswp0xQQK5EACRbSBdY8N8HXrCgCTwd1as5YwetLBbOjsO1JOkGl1MXWG4qpjj7bKR-cVF4jgvGigiP2EQpmbuERykONnoY9LvxqDKSWWgaQpgNkaAHJDbAGAACW";//΢�Ź��ں�access_token
///////////////////////////////////////////////////////////////////////
#define  WX_API_URL             "api.weixin.qq.com"
#define  WX_ID                  "wxa70b2a8a84b770fb"
#define  WX_SECRET              "bd54c7d4cf1fd5f89e4e09bb3e2f342e"
#define  WX_MSG_REQ_URL         "wx.shaohaoqi.cn"
#define  WX_WEB_REQ_URL         "gps.shaohaoqi.cn"
#define  WX_WEB_POST_URL        "wxwebpost.shaohaoqi.cn"
#define  WX_TEMPLATE_ID         "lnJxTDRunbYDKzT_B_8s_RyUiuDbJGb4KQQsdQM0hf0"   
///////////////////////΢�Ź��ں�xml��ʽ/////////////////////////////////////////////////
char  part1[] = "<xml><ToUserName><![CDATA[";
char  part2[]="]]></ToUserName>\r\n<FromUserName><![CDATA[";
char  part3[]="]]></FromUserName>\r\n<CreateTime>";
char  part4[]="</CreateTime>\r\n<MsgType><![CDATA[";
char  part5[]="]]></MsgType>\r\n<Content><![CDATA[";
char  part6[]="]]></Content></xml>";
char  news_part1[]= "]]></MsgType><ArticleCount>";
char  news_part2[]="</ArticleCount><Articles>";
char  news_part3[]="<item><Title><![CDATA[";
char  news_part4[]="]]></Title><Description><![CDATA[";
char  news_part5[]="]]></Description><PicUrl><![CDATA[";
char  news_part6[]="]]></PicUrl><Url><![CDATA[";
char  news_part7[]="]]></Url></item>";
char  news_part8[] = "</Articles></xml>";
///////////////////////////////////////////////////////////////////////////////
typedef  struct  EMAIL_SERVER {
	string  smtp_server;
	string  Email;
	string  EmailAccount;
	string  EmailPwd;
	
}EMAIL_ServerInfo;
//////////////////////////////////////////////////////////////////////////////////////
EMAIL_ServerInfo    mEmailSender;//��������������
/////////////////////////////////////////////////////////////////////////////////////
string  MakeForgetCode( int n );
//////////////////////////////////////////////////////////////////////////////
//��������
int EmailConfig();
//��������
int request(char* hostname, char* api, char* parameters);

time_t convert_string_to_time_t(const std::string & time_string);
int DateCompare(const char* time1,const char* time2);
int PassiveReplyMSG(SOCKET sClient, char *mToUserName, char *mFromUserName,char* parameters);//
int PassiveReponseForgetMSG(SOCKET sClient, char *mToUserName, char *mFromUserName,string wx_openid);
/*******************΢����ҳ��Ȩ��code�����û���opeid*******************************/
int  WX_GetAccessTokenMsg();
int WX_GetUserOpenID(SOCKET sClient , char *buf,  unsigned  long buf_len);
int WX_GetMsgContent(SOCKET sClient,char *rec_buf , unsigned  long  rec_len);
int ResponseNewsMsg(SOCKET sClient, char *mToUserName, char *mFromUserName,char* parameters);
char  JieXiWeiXin(SOCKET   ClientS ,int cmd ,  unsigned char * src, unsigned  long  length);
int  WX_AddCustom(string  kf_account , string  nickname);
int  WX_BindCustom(string  kf_account , string  invite_wx);
int  WX_SendAlarmTemplateMsg(string touser ,string  template_id , string url,Json::Value  mjson_content);
///////////////////////http����//////////////////////////////////////////
void http_parse_request_url( char *buf, char *host, unsigned short *port, char *file_name);
char  http_req_send(char *dst_host ,long dst_port ,char *ref);
///////////////////////http��Ӧ/////////////////////////////////////////////////////
int  http_send_response(SOCKET soc, char *buf, unsigned long buf_len);
char *http_get_type_by_suffix(const char *suffix);
int  http_parse_request_cmd(char *buf, int buflen, char *file_name, char *suffix , char  *ref_context);
int http_response_webfile(SOCKET soc, char *file_name  );
///////////////////////////////////////////////////////////////////////////////////////
int   WX_SetAlarmLock(string card  , int  alarm_lock , string openid );//д�����ݿ���������
int   WX_Send_CardAlarmToUser(SOCKET ClientS , string  DevCard);//���ͱ�����Ϣ��΢��
//////////////////////���ݿ����/////////////////////////////////////////////////////
int   WX_check_userID(string  openid);
string  GetGpsDataFromGPSItem(string  weilan_radius , string  weilan_gps  , string  gps);
Json::Value  WXGetDeviceData(SOCKET   ClientS ,Json::Value  mJsonValue);
int   ResponseDeviceListMsg(SOCKET sClient, char *mToUserName, char *mFromUserName, string  weixin_openID);
int   ResponseDeviceStateMsg(SOCKET sClient, char *mToUserName, char *mFromUserName,string  weixin_openID);
int ResponseMotorStateMsg(SOCKET sClient, char *mToUserName, char *mFromUserName, string weixin_openID);//
int ResponseDeviceOpenMsg(SOCKET sClient, char *mToUserName, char *mFromUserName, string weixin_openID);
Json::Value  WXGetUserBindData(SOCKET   ClientS, Json::Value  mJsonValue);
int  PassiveReponseUnbindMSG(SOCKET sClient, char *mToUserName, char *mFromUserName, string weixin_openID);
////////////////////////////////��ȡ��ҳ�����post����////////////////////////////////////////////////////
int  WX_PostDataHandle(SOCKET sClient,char *rec_buf , unsigned  long  rec_len );
int  WX_ResponsePostDataHandle(SOCKET sClient,char *p_data , unsigned  long  p_len 
								, char *file_name ,char* ref_context);
string  GetPostVauleFromKey(char * key  , char *buf );
int  WX_SetMotorLock(SOCKET sClient, char *buf, unsigned  long buf_len);//�����
int  WX_SetTableMotorLock(string card, int  alarm_lock, string openid);
int  WX_SetDeviceOpen(SOCKET sClient, char *buf, unsigned  long buf_len);
int  WX_SetTableDeviceOpen(string card, int  alarm_lock, string openid);
int  WX_ResponseWebRegister(SOCKET sClient,char *p_data , unsigned  long  p_len ,char* ref_context);
int  WX_ResponseWebForgetRegister(SOCKET sClient,char *p_data , unsigned  long  p_len ,char* ref_context);
int  WX_ResponseWebBind (SOCKET sClient,char *p_data , unsigned  long  p_len ,char* ref_context);
void urldecode(char *p);//url���ı���ת������
////////////////////////////////////////////////////////////////////////////////////
int   Email_Send_CardAlarmToUser(SOCKET ClientS , string  DevCard);
/////////////////////////////////////////////////////////////////////////////////////////////
/* ���峣�� */
#define    HTTP_BUF_SIZE       1024*6     /* �������Ĵ�С */
#define    HTTP_FILENAME_LEN   256     /* �ļ������� */
#define    HTTP_REF_LEN        1024  /***���������󳤶�***/
//////////////////////////////////////////////////////////////////////////////////////////////
/* �����ļ����Ͷ�Ӧ�� Content-Type */
struct doc_type
{
    char *suffix; /* �ļ���׺ */
    char *type;   /* Content-Type */
};
//////////////////////////////////////////////////
struct doc_type file_type[] = 
{
    {"html",    "text/html"  },
    {"gif",     "image/gif"  },
    {"jpeg",    "image/jpeg" },
	{"ico",     "image/*" },
	{"png",     "image/png" },
	{"jpg",     "image/jpg" },
	{"txt",       "txt" },
    { NULL,      NULL   }
};

char *http_res_hdr_tmpl = "HTTP/1.1 200 OK\r\n"
    "Content-Length: %d\r\nConnection: close\r\n"
    "Content-Type: %s\r\n\r\n";
//////////////////////////http�ͻ���//////////////////////////////
#define HTTP_HOST_LEN    256  /* ���������� */
#define HTTP_DEF_PORT    9010   /* ���ӵ�ȱʡ�˿� */

char *http_req_hdr_tmpl = "GET %s HTTP/1.1\r\n"
	"Accept: image/gif,image/jpeg,text/plain,*/*\r\nAccept-Language: zh-cn\r\n"
	"Accept-Encoding: gzip,deflate\r\nHost: %s\r\n"
    "Connection: Keep-Alive\r\n\r\n";
/////////////////////////////////////////////////////////
char  *wx_web_get_user_openid ="/sns/oauth2/access_token?appid=%s&secret=%s&code=%s&grant_type=authorization_code";
////////////////////////////////////////////////////////////////
char *http_post_req_hdr = "POST %s HTTP/1.1\r\n"
	"Accept: image/gif,image/jpeg,text/plain,*/*\r\nAccept-Language: zh-cn\r\n"
	"Accept-Encoding: gzip,deflate\r\nHost: %s\r\n"
	"Content-Length: %d\r\n"
    "Connection: Keep-Alive\r\n\r\n";
char  *wx_add_custom = "/customservice/kfaccount/add?access_token=%s";
char  *wx_send_template_ref = "/cgi-bin/message/template/send?access_token=%s";
char  *wx_get_accesstoken_ref = "/cgi-bin/token?grant_type=client_credential&appid=%s&secret=%s";


////////////////��ʼ��������email����/////////////////////////////////////////////
int EmailConfig()
{
	mEmailSender.smtp_server = "smtp.163.com";
	mEmailSender.Email = "ssou_1985@163.com";
	mEmailSender.EmailAccount = "ssou_1985@163.com";
	mEmailSender.EmailPwd = "qinaide,7758521";
	return 0;
}
////////////////////////��ʱ��ѯ�����������߳�////////////////////////////////////
DWORD  WINAPI  WX_CheckAlarmThread (LPVOID lpParameter)
{  
	EmailConfig();//���÷�����email

	while(true)  
	{  
		//ÿ�ε�2000����   
		int nIndex = WaitForMultipleObjects(1, WX_CheckAlarmEvent, FALSE,5000);     

		if (nIndex == WAIT_OBJECT_0 + 1)   
		{  
			
		}   
		else if (nIndex == WAIT_OBJECT_0) //��һ���¼�����    
		{   			
			
		}    
		else if (nIndex == WAIT_TIMEOUT) //��ʱ    
		{   //��ʱ������ʱ��
			printf("��ʼ��ѯ΢�ű��������ͱ���ģ����Ϣ!\n");//
			//WX_Send_CardAlarmToUser(0 , "");//��ѯ����������΢�ű���ģ����Ϣ
			Email_Send_CardAlarmToUser(0, "");//��ѯ���������ͱ����ʼ�

			//SetEvent(WX_CheckAlarmEvent[0]);//�����¼������߳�
		}   
	}  
	 printf("�߳̽���\n");  

	 return 0;
	
}

////////////////////////��ʱ��ȡ΢��ACCESS_TOKEN�߳�////////////////////////////////////
DWORD  WINAPI  WX_GetTokenThread (LPVOID lpParameter)
{  
	WX_GetAccessTokenMsg();
	while(true)  
	{  
		//ÿ�μ��20����  
		int nIndex = WaitForMultipleObjects(1, WX_GetTokenEvent, FALSE,60000*30);     

		if (nIndex == WAIT_OBJECT_0) //��һ���¼�����    
		{   			
			printf("��ʼ����΢��ACCESS_TOKEN!\n");//
			WX_GetAccessTokenMsg();
		}    
		else if (nIndex == WAIT_TIMEOUT) //��ʱ    
		{   //��ʱ������ʱ��    
			SetEvent(WX_GetTokenEvent[0]);//�����¼������߳�
		}   
	}  
	 printf("�߳̽���\n");  

	 return 0;
	
}
/**************************************************
��ҳ���Ľ���
%BDת���ɵ��ֽ�ֵ�洢
**************************************************/
void urldecode(char *p)  
{  
	long i=0;  
	while(*(p+i))  
	{  
	   if ((*p=*(p+i)) == '%')  
	   {  
			*p=*(p+i+1) >= 'A' ? ((*(p+i+1) & 0XDF) - 'A') + 10 : (*(p+i+1) - '0');  
			*p=(*p) * 16;  
			*p+=*(p+i+2) >= 'A' ? ((*(p+i+2) & 0XDF) - 'A') + 10 : (*(p+i+2) - '0');  
			i+=2;  
	   }  
	   else if (*(p+i)=='+')  
	   {  
			*p=' ';  
	   }  
	   p++;  
	}  
	*p='\0';  
}  
/**************************************************************************
 *
 * ��������: ���������в���, �ֱ�õ�������, �˿ںź��ļ���. �����и�ʽ:
 *           [http://www.baidu.com:8080/index.html]
 *
 * ����˵��: [IN]  buf, �ַ���ָ������;
 *           [OUT] host, ��������;
 *           [OUT] port, �˿�;
 *           [OUT] file_name, �ļ���;
 *
 * �� �� ֵ: void.
 *
 **************************************************************************/
void http_parse_request_url( char *buf, char *host, 
                            unsigned short *port, char *file_name)
{
    int length = 0;
    char port_buf[8];
    char *buf_end = (char *)(buf + strlen(buf));
    char *begin, *host_end, *colon, *file;

    /* ���������Ŀ�ʼλ�� */
    begin = strstr(buf, "//");
    begin = (begin ? begin + 2 : buf);
    
    colon = strchr(begin, ':');
    host_end = strchr(begin, '/');

    if (host_end == NULL)
    {
        host_end = buf_end;
    }
    else
    {   /* �õ��ļ��� */
        file = strrchr(host_end, '/');
        if (file && (file + 1) != buf_end)
            strcpy(file_name, file + 1);
    }

    if (colon) /* �õ��˿ں� */
    {
        colon++;

        length = host_end - colon;
        memcpy(port_buf, colon, length);
        port_buf[length] = 0;
        *port = atoi(port_buf);

        host_end = colon - 1;
    }

    /* �õ�������Ϣ */
    length = host_end - begin;
    memcpy(host, begin, length);
    host[length] = 0;
}
//////////////////////////////////////////////////////////////////
/**************************************************************************
 *
 * ��������: ����http����
 *           [http://www.baidu.com:8080/index.html]
 *
 * ����˵��: [IN]  dst_host, Ŀ�ķ���������;
 *           [IN] dst_port, �˿�;
 *           [IN] ref, ��������ַ���;
 *
 * �� �� ֵ: char.
 *
 **************************************************************************/
char  http_req_send(char *dst_host ,long dst_port ,char *ref)
{
	WSADATA wsa_data;
    SOCKET  http_sock = 0;         /* socket ��� */
    struct sockaddr_in serv_addr;  /* ��������ַ */
    struct hostent *host_ent;
    
    int result = 0, send_len;
    char data_buf[HTTP_BUF_SIZE];
    
    unsigned short port = HTTP_DEF_PORT;
    unsigned long addr;
   
    WSAStartup(MAKEWORD(2,0), &wsa_data); /* ��ʼ�� WinSock ��Դ */

    addr = inet_addr(dst_host);
    if (addr == INADDR_NONE)
    {
        host_ent = gethostbyname(dst_host);
        if (!host_ent)
        {
            printf("[Web] invalid host\n");
            return -1;
        }
        
        memcpy(&addr, host_ent->h_addr_list[0], host_ent->h_length);
    }

    /* ��������ַ */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = addr;

    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* ���� socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (result == SOCKET_ERROR) /* ����ʧ�� */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
        return -1; 
    }

    /* ���� HTTP ���� */
    send_len = sprintf(data_buf, http_req_hdr_tmpl, ref, dst_host);//ƴ��http����
    result = send(http_sock, data_buf, send_len, 0);//����http������

    if (result == SOCKET_ERROR) /* ����ʧ�� */
    {
        printf("[Web] fail to send, error = %d\n", WSAGetLastError());
        return -1; 
    }

    do /* ������Ӧ�����浽�ļ��� */
    {
        result = recv(http_sock, data_buf, HTTP_BUF_SIZE, 0);
        if (result > 0)
        {
            /* ����Ļ����� */
            data_buf[result] = 0;
            printf("%s", data_buf);
        }
    } while(result > 0);

    closesocket(http_sock);
    WSACleanup();

    return 0;
	
}
//////////////////////////http�ͻ���//////////////////////////////
/**************************************************************************
 *
 * ��������: �����ļ���׺���Ҷ�Ӧ�� Content-Type.
 *
 * ����˵��: [IN] suffix, �ļ�����׺;
 *
 * �� �� ֵ: �ɹ������ļ���Ӧ�� Content-Type, ʧ�ܷ��� NULL.
 *
 **************************************************************************/
char *http_get_type_by_suffix(const char *suffix)
{
    struct doc_type *type;

    for (type = file_type; type->suffix; type++)
    {
        if (strcmp(type->suffix, suffix) == 0)
            return type->type;
    }

    return NULL;
}

/**************************************************************************
 *
 * ��������: ��ͻ��˷��� HTTP ��Ӧ.
 *
 * ����˵��: [IN]  buf, �ַ���ָ������;
 *           [IN]  buf_len, buf �ĳ���;
 *
 * �� �� ֵ: �ɹ����ط�0, ʧ�ܷ���0.
 *
 **************************************************************************/
int http_send_response(SOCKET soc, char *buf, unsigned long buf_len)
{
    int read_len=0,  hdr_len=0, send_len=0;
	unsigned  long  file_len=0;
    char *type;
    char read_buf[HTTP_BUF_SIZE];
    char http_header[HTTP_BUF_SIZE];
    char file_name[HTTP_FILENAME_LEN+10] = "", suffix[16] = "";
	char ref_context[HTTP_REF_LEN+5];
    FILE *res_file;
	memset(file_name ,0 ,sizeof(file_name));//һ��Ҫ����ջ���
	memset(ref_context ,0 ,sizeof(ref_context));//һ��Ҫ����ջ���
    /* �õ��ļ����ͺ�׺ */
    http_parse_request_cmd(buf, buf_len, file_name, suffix , ref_context);
	if(strlen(file_name) ==0)
		return  -1;
	printf("file name is %s--------------------------\n",file_name);
    res_file = fopen( file_name, "rb+"); /* �ö����Ƹ�ʽ���ļ� */
	//��������Ŀ¼�ڵ��ļ����ļ���ʾ������"pic\\sr\\zhuce.html" ˫б��ʵ���ǵ���б�ܣ�ǰ�治�ܴ�
    if (res_file == NULL)
    {
        printf("[Web] The file [%s] is not existed\n", file_name);
        return 0;
    }

    fseek(res_file, 0, SEEK_END);
    file_len = ftell(res_file);
    fseek(res_file, 0, SEEK_SET);
    
    type = http_get_type_by_suffix(suffix); /* �ļ���Ӧ�� Content-Type */
    if (type == NULL)
    {
        printf("[Web] There is not the related content type\n");
        return 0;
    }

    /* ���� HTTP �ײ��������� */
    hdr_len = sprintf(http_header, http_res_hdr_tmpl, file_len, type);
    send_len = send(soc, http_header, hdr_len, 0);
    //printf("head send--------------\n%s\n", http_header);
    if (send_len == SOCKET_ERROR)
    {
        fclose(res_file);
        printf("[Web] Fail to send, error = %d\n", WSAGetLastError());
        return 0;
    }

    do /* �����ļ�, HTTP ����Ϣ�� */
    {
        read_len = fread(read_buf, sizeof(char), HTTP_BUF_SIZE, res_file);

        if (read_len > 0)
        {
            send_len = send(soc, read_buf, read_len, 0);
            file_len -= read_len;
        }
    } while ((read_len > 0) && (file_len > 0));

    fclose(res_file);
    printf("file send ok!\n" );
    return 1;
}

/**************************************************************************
 *
 * ��������: ����������, �õ��ļ��������׺. �����и�ʽ:
 *           [GET /ZHUCE.HTML?STATE=abc HTTP/1.1\r\n]
 *	      [GET /?STATE=abc HTTP/1.1\r\n]
*            [GET /ZHUCE.HTML HTTP/1.1\r\n]
*            [GET / HTTP/1.1\r\n]
 * ����˵��: [IN]  buf, �ַ���ָ������;
 *           [IN]  buflen, buf �ĳ���;
 *           [OUT] file_name, �ļ���;
 *           [OUT] suffix, �ļ�����׺;
 *
 * �� �� ֵ: void.
 *
 **************************************************************************/
int http_parse_request_cmd(char *buf, int buflen, char *file_name, char *suffix , char  *ref_context)
{
	int file_len=0;
    int length = 0;
	int ref_len =0;
    char *begin, *end, *bias , *req_end , *pend;
	pend = strstr(buf,"HTTP/1.1\r\n");
	if(pend == NULL)
		return -21;
	*pend='\0';
    /* ���� URL �Ŀ�ʼλ�� */
    begin = strchr(buf, ' ');//���ҵ�һ���ո�
    if(begin)
	{
		if(pend - begin < 0)
			return -11;
		begin++;//������һ���ո�λ��
		req_end = strchr(begin, ' ');//���ҵڶ����ո�
		if(req_end)
		{
			if(pend - req_end < 0)
			return -12;

			bias = strchr(begin, '/');//���ҵ�һ��б��/�����ļ�����ʼλ��
			if(bias)
			{
				if(req_end - bias > 1)//�ļ�������
				{
					length = req_end - bias;//�õ�����ĳ��ȣ����ܰ����ļ����Ͳ�����Ҳ����ֻ�ж���֮һ		
					/* ���� URL �Ľ���λ�� */
					end = strchr(bias, '?');
					if(end)//����������������ҵ�?˵���������������
					{
						if(end - bias > 1)//�ж��ļ����Ƿ���ڣ�������/?�������ݣ�û���ļ���
						{
							file_len = end-bias-1;//ȥ���˵�һ��б��/
							
							ref_len = req_end - end - 1;
							if(ref_len >= HTTP_REF_LEN )
								return -9;
							if( ref_len > 0)//ȷ��������HTTP/1.1֮ǰ
							{
								cout<<"�в������ڣ�"<<endl;
								memcpy(ref_context , end+1 , ref_len);//ȡ���������
								ref_context[ref_len] = 0;
							}
							if(file_len >= HTTP_FILENAME_LEN)
							{
								*pend='H';
								return  -10;
							}
							string str_name ;
									
							str_name.insert(0 , bias+1, file_len);
							size_t  found;
							int next_pos = 0;
							found = str_name.find("/");
							if (found!=std::string::npos)
							{
								memcpy(file_name, ".\\" , strlen(".\\"));
								next_pos = strlen(".\\");
							}
							else
							{								
								next_pos = 0;
							}
							while(1)
							{
								found = str_name.find("/");
								if (found!=std::string::npos)
								{
									str_name.replace(found , 1 , "\\");
								}
								else
									break;
							}
							memcpy(file_name + next_pos, str_name.c_str() , file_len);
							file_name[file_len+next_pos] = 0;
							
							//cout<< file_name<<endl;
							begin = strrchr(file_name, '.');//�������һ���㡣
							if (begin)
								strcpy(suffix, begin + 1);
						}							
					}
					else //û���������
					{	
						
						if(req_end - bias > 1)//�ж��ļ����Ƿ���ڣ�������GET / HTTP/1.1\r\n
						{
							file_len = req_end-bias-1;
							ref_context[0] = 0;
							if(file_len >= HTTP_FILENAME_LEN)
							{
								*pend='H';
								return  -10;
							}
							string  str_name ;
							//int  head_len = strlen(".\\");
							str_name.insert(0 , bias+1, file_len);
							size_t  found;
							int next_pos = 0;
							found = str_name.find("/");
							if (found!=std::string::npos)
							{
								memcpy(file_name, ".\\" , strlen(".\\"));
								next_pos = strlen(".\\");
							}
							else
							{
								next_pos = 0;
							}
							while(1)
							{
								found = str_name.find("/");
								if (found!=std::string::npos)
								{
									str_name.replace(found , 1 , "\\");
								}
								else
									break;
							}
							memcpy(file_name + next_pos, str_name.c_str() , file_len);
							file_name[file_len+next_pos] = 0;
							
							//cout<< file_name<<endl;
							begin = strrchr(file_name, '.');//�������һ���㡣
							if (begin)
								strcpy(suffix, begin + 1);
						}
						
					}
				}
			
			}
		}				
	}		
	*pend='H';
	return 0;
}
/************************************************************

********************************************************/
int http_response_webfile(SOCKET soc, char *file_name )
{
	char *http_res_hdr_tmpl = "HTTP/1.1 200 OK\r\n"
    "Content-Length: %d\r\nConnection: close\r\n"
    "Content-Type: %s\r\n\r\n";
////////
    long read_len=0,  hdr_len=0, send_len=0;
	unsigned  long  file_len=0;
    char *type ,suffix[100];
    char read_buf[1024*10];
    char http_header[1024];
    FILE *res_file;
    char  *ps;
	ps = strrchr(file_name , '.');
	if(ps)
	{
		strcpy(suffix , ps+1 );
	}
	printf("file name is %s--------------------------\n",file_name);
	
    res_file = fopen( file_name, "rb+"); /* �ö����Ƹ�ʽ���ļ� */
	//��������Ŀ¼�ڵ��ļ����ļ���ʾ������"pic\\sr\\zhuce.html" ˫б��ʵ���ǵ���б�ܣ�ǰ�治�ܴ�
    if (res_file == NULL)
    {
        printf("[Web] The file [%s] is not existed\n", file_name);
        return 0;
    }
	type = http_get_type_by_suffix(suffix); /* �ļ���Ӧ�� Content-Type */
    if (type == NULL)
    {
        printf("[Web] There is not the related content type\n");
        return 0;
    }
    fseek(res_file, 0, SEEK_END);
    file_len = ftell(res_file);
    fseek(res_file, 0, SEEK_SET);
    /* ���� HTTP �ײ��������� */
    hdr_len = sprintf(http_header, http_res_hdr_tmpl, file_len, type);
    send_len = send(soc, http_header, hdr_len, 0);
    //printf("head send--------------\n%s\n", http_header);
    if (send_len == SOCKET_ERROR)
    {
        fclose(res_file);
        printf("[Web] Fail to send, error = %d\n", WSAGetLastError());
        return 0;
    }
	cout<<"�����ļ�"<<endl;
    do /* �����ļ�, HTTP ����Ϣ�� */
    {
        read_len = fread(read_buf, sizeof(char), 1024*8, res_file);

        if (read_len > 0)
        {
            send_len = send(soc, read_buf, read_len, 0);
            file_len -= read_len;
        }
    } while ((read_len > 0) && (file_len > 0));

    fclose(res_file);
    printf("file send ok!\n" );
    return 1;
}
/////////////////////////////////����΢��post���ݲ���Ӧhttp 1.1 20 OK.../////////////////////////////////////////////////
char  JieXiWeiXin(SOCKET   sClient ,int cmd ,  unsigned char * src, unsigned  long  length)
{
	int t;
	char *pst = NULL ,*p2= NULL,*ps= NULL , *pd= NULL;
	int ret;
	int len = 0;

	char   WebData[REC_LEN+10]; //���ջ���
	ret = length;
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	printf("��ʼ����:%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);

	memset(WebData , 0 , sizeof(WebData));
	//EnterCriticalSection(&mSaveDataLock);//���洢���ݼ���
	//��������
	memcpy(WebData, src, length);

	//LeaveCriticalSection(&mSaveDataLock);//����

	WebData[ret] = '\0';
    printf(WebData);
	printf("\r\n");
	//printf("APP2222222222\n");
	if(ps = strstr( WebData, "\r\nHost:"))
	{	
		t=strlen("\r\nHost:");
		    
		if(strstr(ps+t,WX_MSG_REQ_URL))//�ж���΢��POST��Ϣ����
		{
			printf("wx_msg_req-------------------------------\r\n");
			WX_GetMsgContent(sClient ,  WebData , length);//����΢����Ϣxml	
		}
		else  if(strstr(ps+t,WX_WEB_REQ_URL))//�ж���΢����ҳ��Ȩ�û���Ϣ����
		{
			printf("wx_web_req-------------------------------\r\n");
			//WX_GetUserOpenID(sClient,WebData ,ret);//��΢�ŷ���������ȡ�û�openID����		
			WX_PostDataHandle(sClient,WebData ,ret);//��ҳ�ύ��POST����
			
		}
		else  if(strstr(ps+t,WX_WEB_POST_URL))//�ж���΢����ҳPOST��������������
		{
			if(strstr( WebData, "GET"))//���������
			{			
				http_send_response(sClient, WebData, ret);// ��ͻ��˷�����Ӧ���� 
			}
			else  if(strstr( WebData, "POST"))//���������
			{
				printf("web post data req-------------------------------\r\n");
				WX_PostDataHandle(sClient,WebData ,ret);//��ҳ�ύ��POST����
			}
						
		}	
        /////////////////////��������������ҳ����///////////////////////////
		else
		{			
			if(strstr( WebData, "GET"))//���������
			{			
				http_send_response(sClient, WebData, ret);// ��ͻ��˷�����Ӧ���� 
			}
			else  if(strstr( WebData, "POST"))//���������
			{
			
			}
		/////////////////////////////////////////////////////////////////////////////
		}//end ��ҳ������
	}
	printf("�رո�socket���ӣ�\r\n");
	closesocket(sClient);
	
	GetLocalTime(&sys);
	printf("�����ظ����:%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);

	return  0; 

}

///////////////////////����΢����Ϣxml///////////////////////////////
int WX_GetMsgContent(SOCKET sClient  ,  char *rec_buf , unsigned  long  rec_len)
{
	char content_header[100];
	char  ToUserName[1024];
	char  FromUserName[1024];
	char  MsgContent[CONTENT_LEN+10];//��ȡ���ݻ���
	char  *revData = rec_buf;
	char send_str[2048+100] = {0};
	int wx_len=0;
	char m_echostr[560];
	//char m_wxopenid[200];
	int t;
	char *pst = NULL ,*p2= NULL,*ps= NULL , *pd= NULL , *pend;
	int  num;
	int  msgType=0;
	//int ret;
	int len = rec_len;
	//��ʼ��������Ϣ	
	memset(send_str,0,sizeof(send_str));
	//ͷ��Ϣ
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2017 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/html\r\n");
	pend = strstr(revData , "HTTP/1.1\r\n");
	if(pend == NULL )
		return -11;
	*pend = '\0';
	p2=strstr(revData , "&timestamp=");
	if(p2) //
	{
		if(pend-p2 < 0 )
			return -21;

		pst = strstr(revData , "echostr=");//����echo_str					
		if(pst && p2)
		{
			if(pend-pst < 0 )
				return -22;
			num=p2-pst-strlen("echostr=");
			if(num < 1 || num > sizeof(m_echostr) - 2)
				return -32;
			//���ҵ�echo_str				
			memcpy(m_echostr,pst+8,num);//������echostr
			m_echostr[num]=0;
			printf(m_echostr);
			printf("\r\n");
			
			sprintf(content_header,"Content-Length: %d\r\n", strlen(m_echostr));
			strcat_s(send_str, content_header);
			strcat_s(send_str, "Connection: close\r\n");
			strcat_s(send_str, "Pragma: no-cache\r\n");
			strcat_s(send_str, "\r\n");
			strcat_s(send_str, m_echostr);
			strcat_s(send_str, "\r\n");
			
			if (send(sClient, send_str, strlen(send_str),0) == -1)
			{
				cout<<"send failed"<<endl;
				//return -101;
			}
			closesocket(sClient);
			return 0;
		}	
		else
		{
			*pend = 'H';
			t=strlen("<Content><![CDATA[");
			ps=strstr(pend + strlen("HTTP/1.1\r\n"),"<xml>");		
			if(ps)
			{
				pd=strstr(ps,"</xml>");
				if(pd)
				{
					int content_len = pd-ps+strlen("</xml>");
					printf("content len:%d\n" , content_len);

				}
			}
			ps=strstr(ps,"<ToUserName><![CDATA[");
			if(ps)
			{
				pd=strstr(ps,"]]></ToUserName>");
				t=strlen("<ToUserName><![CDATA[");
				if(pd)
				{
					memcpy(FromUserName,(ps+t),pd-ps-t);
					FromUserName[pd-ps-t]=0;
					//printf("FromUserName:%s\n" , FromUserName);
				}
				else
				{
					//goto end;
				}
			}
			ps=strstr(pd,"<FromUserName><![CDATA[");
			if(ps)
			{
				pd=strstr(ps,"]]></FromUserName>");
				t=strlen("<FromUserName><![CDATA[");
				if(pd)
				{
					memcpy(ToUserName,(ps+t),pd-ps-t);
					ToUserName[pd-ps-t]=0;
					//printf("ToUserName:%s\n" , ToUserName);
				}else
				{
					//goto end;
				}
			}
			msgType = 0;
		/////////////////////////////////////////////////////////////
			while(1)
			{
				ps=strstr(pd,"<MsgType><![CDATA[text]]");
				if(ps)
				{
					msgType=1;
					break;
				}
				ps=strstr(pd,"<MsgType><![CDATA[image]]");
				if(ps)
				{
					msgType=2;
					break;
				}
				ps=strstr(pd,"<MsgType><![CDATA[voice]]");
				if(ps)
				{
					msgType=3;
					break;
				}
				ps=strstr(pd,"<MsgType><![CDATA[music]]");
				if(ps)
				{
					msgType=4;
					break;
				}
				
				ps=strstr(pd,"<MsgType><![CDATA[event]]></MsgType>");//����˵���ȡ��Ϣʱ���¼�����
				if(ps)
				{						
					ps=strstr(ps,"<Event><![CDATA[CLICK]]></Event>");//����˵���ȡ��Ϣʱ���¼�����
					if(ps)
					{
						
						msgType=5;
						break;
					}
				}
			
				ps=strstr(pd,"<MsgType><![CDATA[event]]></MsgType>");//����˵���ת�����¼�
				if(ps)
				{
					ps=strstr(ps,"<Event><![CDATA[VIEW]]></Event>");//����˵���ȡ��Ϣʱ���¼�����
					if(ps)
					{
						msgType= 6;
						break;
					}
				}
				
				ps=strstr(pd,"<MsgType><![CDATA[event]]></MsgType>\n<Event><![CDATA[scancode_push]]></Event>");
				if(ps)
				{
					msgType=7;
					break;
				}
				ps=strstr(pd,"<MsgType><![CDATA[news]]");
				if(ps)
				{
					msgType= 11;
					break;
				}
				//cout<<"��Ӧ��Ϣ����"<<endl;
				msgType = 100;
				break;
			}
		
			switch(msgType)
			{
				case 1:
				ps=strstr(ps,"<Content><![CDATA[");
			
				t=strlen("<Content><![CDATA[");
				if(ps)
				{
					pd=strstr(ps,"]]></Content>");
					if(pd)
					{
						if( (pd-ps-t) < CONTENT_LEN-2)
						{
							int content_len = pd-ps-t;
							memcpy(MsgContent,(ps+t),content_len);
							MsgContent[pd-ps-t]=0;

							printf("content len:%d\n" , content_len);
						}
						else
						{
							memcpy(MsgContent,(ps+t),CONTENT_LEN-2);
							MsgContent[CONTENT_LEN-1]=0;
						}
					}
					
				}else
				{
					break;
				}
				printf("textMSG\n");
				PassiveReplyMSG(sClient, ToUserName,FromUserName,MsgContent);
				break;
				/********************************************************/
				case 2:
				ps=strstr(ps,"<Content><![CDATA[");
				t=strlen("<Content><![CDATA[");
				if(ps)
				{
					pd=strstr(ps,"]]></Content>");
					if(pd)
					{
						memcpy(MsgContent,(ps+t),pd-ps-t);
						MsgContent[pd-ps-t]=0;
					}
				}
				PassiveReplyMSG(sClient, ToUserName,FromUserName,MsgContent);
				break;
				case 3:
				ps=strstr(ps,"<Content><![CDATA[");
				t=strlen("<Content><![CDATA[");
				if(ps)
				{
					pd=strstr(ps,"]]></Content>");
					if(pd)
					{
						memcpy(MsgContent,(ps+t),pd-ps-t);
						MsgContent[pd-ps-t]=0;
					}
				}
				PassiveReplyMSG(sClient, ToUserName,FromUserName,MsgContent);
				break;
				case 4:
				ps=strstr(ps,"<Content><![CDATA[");
				t=strlen("<Content><![CDATA[");
				if(ps)
				{
					pd=strstr(ps,"]]></Content>");
					if(pd)
					{
						memcpy(MsgContent,(ps+t),pd-ps-t);
						MsgContent[pd-ps-t]=0;
					}
				}
				PassiveReplyMSG(sClient, ToUserName,FromUserName,MsgContent);
				break;
				/*********/
				case 5:
					ps=strstr(ps,"<EventKey><![CDATA[");
					t=strlen("<EventKey><![CDATA[");
					if(ps)
					{	
						pd=strstr(ps,"]]></EventKey>");
						if(pd)
						{
							memcpy(MsgContent,(ps+t),pd-ps-t);
							MsgContent[pd-ps-t]=0;

						}
					}else
					{
						break;
					}
					//printf("%s\n",MsgContent);
					if(strstr(MsgContent , "GPSbtn_list1_1"))//��һ��˵���һ�������
					{
						//cout<<"event news "<<endl;							
						ResponseDeviceStateMsg(sClient, ToUserName,FromUserName,ToUserName  );//�ظ��豸�б�������Ϣ
						
					}
					else if(strstr(MsgContent , "GPSbtn_list1_2"))//��һ��˵��ڶ��������
					{
						//cout<<"event news "<<endl;							
						ResponseMotorStateMsg(sClient, ToUserName,FromUserName,ToUserName  );//�ظ��豸�б�������Ϣ
						
					}
					else if (strstr(MsgContent , "GPSbtn_list1_3"))//��һ��˵������������
					{
						//cout<<"event news "<<endl;							
						ResponseDeviceOpenMsg(sClient, ToUserName,FromUserName,ToUserName  );//�ظ��豸�б�������Ϣ
						
					}
					else if(strstr(MsgContent , "GPSbtn_list2_1"))//�ڶ���˵���һ�������
					{
						ResponseDeviceListMsg(sClient, ToUserName,FromUserName,ToUserName);//�ظ�ͼ����Ϣ
					}
					else if(strstr(MsgContent , "GPSbtn_list3_1"))//�ڶ���˵���һ�������
					{
						//PassiveReponseUnbindMSG(sClient, ToUserName, FromUserName, ToUserName);//�ظ�ͼ����Ϣ
						PassiveReponseForgetMSG(sClient, ToUserName,FromUserName,ToUserName);//�ظ�ͼ����Ϣ
					}
					else if (strstr(MsgContent, "GPSbtn_list3_2"))//�ڶ���˵���һ�������
					{
						PassiveReponseUnbindMSG(sClient, ToUserName, FromUserName, ToUserName);//�ظ�ͼ����Ϣ
					}
					
					else
						PassiveReplyMSG(sClient, ToUserName,FromUserName,MsgContent);
					
				break;
				/*********************************************/
				case 6:						
					ps=strstr(ps,"<EventKey><![CDATA[");
					t=strlen("<EventKey><![CDATA[");
					if(ps)
					{
						pd=strstr(ps,"]]></EventKey>");
						if(pd)
						{
							memcpy(MsgContent,(ps+t),pd-ps-t);
							MsgContent[pd-ps-t]=0;
						}
					}else
					{
						break;
					}
					//printf("%s\n",MsgContent);
					//PassiveReplyMSG(sClient, ToUserName,FromUserName,MsgContent);
					/************/
				
				break;
				case 7:
					ps=strstr(ps,"<EventKey><![CDATA[");
					t=strlen("<EventKey><![CDATA[");
					if(ps)
					{
						pd=strstr(ps,"]]></EventKey>");
						if(pd)
						{
							memcpy(MsgContent,(ps+t),pd-ps-t);
							MsgContent[pd-ps-t]=0;
						}
					}else
					{
						break;
					}
					//printf("%s\n",MsgContent);
					//PassiveReplyMSG(sClient, ToUserName,FromUserName,MsgContent);
				break;
				
				default:
					cout<<"����ʶ����Ϣ"<<endl;
					len=strlen("success");
					memset(m_echostr,0,sizeof(m_echostr));
					strcat_s(m_echostr,"success" );
				
					sprintf_s(content_header,"Content-Length: %d\r\n", len);
					strcat_s(send_str, content_header);
					strcat_s(send_str, "Connection: close\r\n");
					strcat_s(send_str, "Pragma: no-cache\r\n");
					strcat_s(send_str, "\r\n");
					strcat_s(send_str, m_echostr);//��Ϣ���Ⱥ�Content-Length����Ҫһ��,����΢�Żظ���
			
					if (send(sClient, send_str, strlen(send_str),0) == -1)
					{
						cout<<"send failed"<<endl;
						//goto end;
						//return -101;
					}		
				break;
			}
		}
				
	}
	closesocket(sClient);
	return 0;
}
///////////////////////////////���������//////////////////////////////////////////
string  MakeForgetCode( int n )
{
	string  str_code ="";
	int i,j,len;
    char pstr[] = "0123456789";
    len = strlen(pstr);         //���ַ���pstr�ĳ���
    srand(time(0));
    for (i = 0;i < n; i++) 
	{
        j = rand()%len;        //����0~len-1�������
		str_code+=pstr[j];

    }
	return str_code;
}

/////////////////////////�ظ��豸�źͽ����֤��//////////////////////////////////////////////////////
int PassiveReponseUnbindMSG(SOCKET sClient, char *mToUserName, char *mFromUserName, string weixin_openID)
{
	char send_str[2048 + 100] = { 0 };
	memset(send_str, 0, sizeof(send_str));//�������������0��memset���ɸ�ָ����䣬��ֻ�����ָ�����ڵ��ڴ�
										  //ͷ��Ϣ
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2018 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	string  mstr , str_username , str_card , str_SN , str_bikename ,  Url;
	int len = 0;
	char *xml;
	bool  getsucess = false;

	Json::Value  m_json;
	const char user[] = "root";
	const char pswd[] = "123456";
	const char host[] = "localhost";
	char table[] = "bike";
	unsigned int port = 3306;

	MYSQL   myCont;  //mysql ����
	MYSQL_RES  *result;  //�洢���

	int res;
	string  str_code;//��֤��
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1, f2, num_row, num_col;
	
	Json::Value  mjson_card , mjson_user_data , mjson_value;
	mjson_card["token"]= weixin_openID;
	mjson_card["ToUserName"] = mToUserName;
	mjson_card["FromUserName"] = mFromUserName ;
	mjson_user_data = WXGetUserBindData(sClient ,mjson_card);//�����ݿ��ȡ�û�����
	if(mjson_user_data.size() <= 0 )
		return -1;
	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
	mysql_init(&myCont);
	if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
	{
		//mysql_query(&myCont, "SET NAMES utf8"); //���ñ����ʽ
	}
	else
	{
		cout << "connect failed!" << endl;
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		return -2;
	}
	string  mutf8_str = "�豸��Ϣ:";
	mutf8_str = GetUTF8FromGBK(mutf8_str);
	string  mitem = "�󶨵��豸��\r\n\r\n";
	mitem = GetUTF8FromGBK(mitem);

	if (mjson_user_data["data"].isArray())//�ж���json����
    {
        int nArraySize = mjson_user_data["data"].size(); //�������� 
		
        for (int i=0; i<nArraySize; i++)
        {       
			str_bikename = mjson_user_data["data"][i]["bikename"].asString(); //������ĳ��keyֵ 
            str_card = mjson_user_data["data"][i]["card"].asString(); //������ĳ��keyֵ 
						
			string  m_checkuser = "SELECT  serialnumber  FROM  cardinfo  WHERE card = '" + str_card + "'";
			//cout << m_checkuser << endl;
			getsucess = false;
			res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //ִ��SQL���,ͨ��token����username
			if (!res)
			{
				//�����ѯ�������ݵ�result
				result = mysql_store_result(&myCont);
				num_row = mysql_num_rows(result); //��ȡ����
				mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����
				if(num_row > 0)
				{
					str_SN = getNullStr(mysql_row[0]); //��ȡ�ֶ����ݣ������жϷ�NULL
					getsucess = true;
					Url = to_string( (long) (i+1)) + "   <a href=\"http://wxweb.shaohaoqi.cn/unbind.html?serial=" + str_SN + "\">";
					//Url += "    " + str_SN + "</a>";
					Url +=  mutf8_str  + "    " + str_bikename + "  " + str_SN + "</a>"  ;
					mitem += Url+ "\r\n\r\n";	
					//cout << mitem << endl;
				}
				mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����
			}
        }
    }
/***
	if (getsucess == true)
	{

	}
	else
	{
		mysql_close(&myCont);
		cout << "check card already existed !\n" << endl;
		return -4;
	}
	***/
	str_code = MakeForgetCode(6);//���������
	string  m_checkuser = "SELECT  *  FROM  unbind_code  WHERE token = '" + weixin_openID + "'";
	//cout<< m_checkuser<<endl;
	getsucess = false;
	res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //ִ��SQL���,ͨ��token����username
	if (!res)
	{
		//�����ѯ�������ݵ�result
		result = mysql_store_result(&myCont);
		num_row = mysql_num_rows(result); //��ȡ����
		num_col = mysql_num_fields(result); //��ȡ����
											//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
		MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if (num_row > 0)
		{
			getsucess = true;
			m_checkuser = "UPDATE unbind_code SET unbind_code = '" + str_code + "',register_time = NOW()  WHERE token = '" + weixin_openID + "'";
			res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //ִ��SQL���,���һ����¼
			if (!res)
			{
				//cout << "//////////update  unique_code  sucess!/////////////\n" << endl;					
			}
			else
			{
				mysql_close(&myCont);//�ر�sql����
				mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

				cout << "update  unbind_code error!\n" << endl;
				return -3;
			}
		}
		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

	}
	if (getsucess == true)
	{

	}
	else
	{
		m_checkuser = "INSERT  INTO  unbind_code ( unbind_code , token  , register_time )   VALUES( '" +
			str_code + "','" + weixin_openID + "', NOW(3) ) ";

		//cout<<m_checkuser<<endl;

		res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //ִ��SQL���,���һ����¼
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

		if (!res)
		{
			cout << "//////////regester unbind_code  sucess!///////////" << endl;
		}
		else
		{
			mysql_close(&myCont);//�ر�sql����
			mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

			cout << "Insert  unbind_code  error!" << endl;
			return -3;

		}
	}
	
	mysql_close(&myCont);//�ر�sql����
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	mstr = part1;
	mstr += mToUserName;
	mstr += part2;
	mstr += mFromUserName;
	mstr += part3;
	mstr += "1492853809";
	mstr += part4;
	mstr += "text";
	mstr += part5;
	mstr += mitem;
	mstr += GetUTF8FromGBK("�����Ҫ�����豸��������֤�뼴�ɽ��\r\n�����֤��" + str_code + "����Ҫй¶������, 5������Ч");
	mstr += part6;
	len = mstr.length();//������Ϣ�峤��Content-Length  Ҫ�ͺ�����Ϣ���ֽ����ϸ�һ��

	char content_header[100];
	sprintf_s(content_header, "Content-Length: %d\r\n", len);
	strcat_s(send_str, content_header);
	strcat_s(send_str, "Connection: close\r\n");
	strcat_s(send_str, "Pragma: no-cache\r\n");
	strcat_s(send_str, "\r\n");
	xml = (char*)mstr.c_str();
	strcat_s(send_str, xml);//ע��http��Ӧĩβ����Ϣ�壬����\r\nҲ��2���ֽڵ���Ϣ�壬��Ҫ�෢�ֽ�
							//cout<<send_str<<endl;
	if (send(sClient, send_str, strlen(send_str), 0) == -1)
	{
		cout << "send failed" << endl;
		closesocket(sClient);
		return -11;
	}
	closesocket(sClient);
	return 0;
}

//////////////////////////�ظ������������Ӧ��Ϣ//////////////////////////////////////////////////////
int PassiveReponseForgetMSG(SOCKET sClient, char *mToUserName, char *mFromUserName,string wx_openid)
{
	char send_str[2048+100] = {0};
    memset(send_str,0,sizeof(send_str));//�������������0��memset���ɸ�ָ����䣬��ֻ�����ָ�����ڵ��ڴ�
    //ͷ��Ϣ
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2018 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	string  mstr;
	int len=0;
	char *xml;
	bool  getsucess=false;

	Json::Value  m_json;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;        

    MYSQL   myCont;  //mysql ����
    MYSQL_RES  *result ;  //�洢���
   // MYSQL_ROW  mysql_row; //������
	my_ulonglong  num_row,num_col; 
    int res;
	string  str_code ;//��֤��
					  //��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
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
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		
		return -2;
    }
	str_code = MakeForgetCode(6);
	string  m_checkuser = "SELECT  *  FROM  forget_code  WHERE token = '" + wx_openid + "'";
	//cout<< m_checkuser<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{			
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if(num_row > 0 )
		{
			getsucess = true;
			m_checkuser = "UPDATE forget_code SET unique_code = '" + str_code + "',register_time = NOW()  WHERE token = '"+ wx_openid + "'";
			res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //ִ��SQL���,���һ����¼
			if(!res  )
			{
				//cout << "//////////update  unique_code  sucess!/////////////\n" << endl;					
			}
			else
			{
				mysql_close(&myCont);//�ر�sql����
				mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

				cout << "update  unique_code error!\n" << endl;
				return -3;
			}
		}
		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����
		
	}
	if(getsucess == true)
	{
		
	}
	else
	{			
		m_checkuser = "INSERT  INTO  forget_code ( unique_code , token  , register_time )   VALUES( '" +
		str_code + "','" + wx_openid + "', NOW(3) ) ";

		//cout<<m_checkuser<<endl;

		res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //ִ��SQL���,���һ����¼
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

		if(!res  )
		{			
			cout << "//////////regester unique_code  sucess!///////////" << endl;	
		}
		else
		{
			mysql_close(&myCont);//�ر�sql����
			mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

			cout << "Register  unique_code error!" << endl;
			return -3;
		
		}
	}
	mysql_close(&myCont);//�ر�sql����
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	mstr=part1;
	mstr+=mToUserName;
	mstr+=part2;
	mstr+=mFromUserName;
	mstr+=part3;
	mstr+="1492853809";
	mstr+=part4;
	mstr+="text";
	mstr+=part5;
	mstr+=GetUTF8FromGBK("�����û��������룬�������������ע�ᣬֱ�����ʹ�ã����˺Żᱻ���\r\n");
	mstr+="<a href=\"http://wxweb.shaohaoqi.cn/forget.html?token=" + wx_openid +"\">";
	mstr += GetUTF8FromGBK("�������ע��,������֤��"+ str_code +"����Ҫй¶������,5������Ч");//����
	mstr += "</a>";
	mstr+=part6;
	len=mstr.length();//������Ϣ�峤��Content-Length  Ҫ�ͺ�����Ϣ���ֽ����ϸ�һ��
	
	char content_header[100];
	sprintf_s(content_header,"Content-Length: %d\r\n", len);
	strcat_s(send_str, content_header);
	strcat_s(send_str, "Connection: close\r\n");
	strcat_s(send_str, "Pragma: no-cache\r\n");
	strcat_s(send_str, "\r\n");
	xml = (char*)mstr.c_str();
	strcat_s(send_str, xml);//ע��http��Ӧĩβ����Ϣ�壬����\r\nҲ��2���ֽڵ���Ϣ�壬��Ҫ�෢�ֽ�
	//cout<<send_str<<endl;
	if (send(sClient, send_str, strlen(send_str),0) == -1)
	{
		cout<<"send failed"<<endl;
		closesocket(sClient);
		return -11;
	}
	closesocket(sClient);
	return 0;
}
////////////////////////////////////�����ظ���Ϣ///////////////////////////////////////////////
int PassiveReplyMSG(SOCKET sClient, char *mToUserName, char *mFromUserName,char* parameters)//
{
	
	char send_str[SEND_LEN+100] = {0};
    memset(send_str,0,sizeof(send_str));//�������������0��memset���ɸ�ָ����䣬��ֻ�����ָ�����ڵ��ڴ�
    //ͷ��Ϣ
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2018 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	string  mstr;//��Ϣ���ַ�������
	int len=0;
	char *xml;
	mstr=part1;
	mstr+=mToUserName;
	mstr+=part2;
	mstr+=mFromUserName;
	mstr+=part3;
	mstr+="1492853809";
	mstr+=part4;
	mstr+="text";
	mstr+=part5;
	//mstr+="<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa70b2a8a84b770fb&redirect_uri=http://gps.shaohaoqi.cn/zhuce.html&response_type=code&scope=snsapi_base&state=suoding123#wechat_redirect\">";
	mstr+="<a href=\"http://mp.weixin.qq.com/s/3O8PHvgeUk8aSg1kFaGFjg\">";
	mstr += parameters;//����
	mstr += "</a>";
	mstr+=part6;
	len=mstr.length();//������Ϣ�峤��Content-Length  Ҫ�ͺ�����Ϣ���ֽ����ϸ�һ��
	
	char content_header[100];
	sprintf_s(content_header,"Content-Length: %d\r\n", len);
	strcat_s(send_str, content_header);
	strcat_s(send_str, "Connection: close\r\n");
	strcat_s(send_str, "Pragma: no-cache\r\n");
	strcat_s(send_str, "\r\n");
	xml = (char*)mstr.c_str();
	strcat_s(send_str, xml);//ע��http��Ӧĩβ����Ϣ�壬����\r\nҲ��2���ֽڵ���Ϣ�壬��Ҫ�෢�ֽ�
	//strcat_s(send_str, "\r\n");//ע�⣬��Ϊ����������ֽڣ����³���content-Length��΢��6��Żظ�
	//printf("----------------------------------------------------\r\n");
	//printf("%s",send_str);
	if (send(sClient, send_str, strlen(send_str),0) == -1)
	{
		cout<<"send failed"<<endl;
		closesocket(sClient);
		return -101;
	}
	closesocket(sClient);
    return 0;
}
/////////////////////΢����ҳ��Ȩ��code��ȡ�û�OPENID��΢�ŷ�GET����/////////////////////////////////////////////
int WX_GetUserOpenID(SOCKET sClient, char *buf, unsigned  long buf_len)//
{
	char *ps ,*pd, *pend,t;
	char *revData = buf;
	char MsgContent[3*1024];
	int ref_len=0;
	char ref_buf[HTTP_BUF_SIZE+5];
	WSADATA wsa_data;
    SOCKET  http_sock = 0;         /* socket ��� */
    struct sockaddr_in serv_addr;  /* ��������ַ */
   // struct hostent *host_ent;
    char  user_openid[200];
    int result = 0, send_len;
    char data_buf[HTTP_BUF_SIZE+5];
    char rev_buf[HTTP_BUF_SIZE+5];
    unsigned short port = HTTP_DEF_PORT; //NGINX�������������8088�˿�תhttps
    //unsigned long addr;
    string  card_name , lock_set;
	WSAStartup(MAKEWORD(2,0), &wsa_data); /* ��ʼ�� WinSock ��Դ */  
    
    /* ��������ַ */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//����nginx�������

    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* ���� socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));//����socket
    if (result == SOCKET_ERROR) /* ����ʧ�� */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
		WSACleanup();
        return -1; 
    }
	cout<<"����gps.shaohaoqi.cn"<<endl;
	pend = strstr(buf,"HTTP/1.1\r\n");
	if(pend == NULL)
	{
		cout<<"û��ѯ��HTTP/1.1"<<endl;
		closesocket(http_sock);  
		WSACleanup();
		return -21;
	}
	*pend='\0';//�ض�HTTP�����壬ֻ��ѯǰ�������ͷ
	ps=strstr(buf,"?code=");
	if(ps)
	{
		if(pend - ps < 0)//�ж����������HTTP/1.1 ֮��˵������
		{
			closesocket(http_sock);  
			WSACleanup();
			//cout<<"΢������icoͼ��"<<endl;
			//http_send_response(sClient, buf , buf_len);// ��ͻ��˷�����Ӧ���� 	
			return -22;
		}
		t=strlen("?code=");
		pd=strstr(ps,"&state=");//��code�������飬����֤����������˳����ȷ
		if(pd)
		{
			if( (pd-ps-t) < 1024 && (pend- pd) >0)//��Ϊ�����ѯ֤�������ﲻ���Ǹ���
			{
				int content_len = pd-ps-t;
				memcpy(MsgContent,(ps+t),content_len);//��ȡ��ҳ��Ȩcode
				MsgContent[pd-ps-t]=0;
				printf("content len:%d\n" , content_len);
				printf("content :%s\n" , MsgContent);
				memset(ref_buf , 0,  sizeof(ref_buf));
				ref_len = sprintf(ref_buf , wx_web_get_user_openid, WX_ID , WX_SECRET, MsgContent);
	            
				ps=strstr(pd,"SET");
		        if(ps)
				{
					int  card_len = strlen("&state=");
					//memcpy(card_name.c_str() ,(pd+card_len),ps-pd-card_len);//��ȡ��ҳ��Ȩcode
				    card_name.insert (0,pd+card_len, ps-pd-card_len);
					//cout<< card_name.c_str()<<endl; //card����
				}
				else
				{
					closesocket(http_sock);  
					WSACleanup();
					return -11;
				}

				pd=strstr(ps," ");
		        if(pd)
				{
					int  card_len = strlen("SET");//�����Զ���������ָ��ַ���SET
					//memcpy(card_name.c_str() ,(pd+card_len),ps-pd-card_len);//��ȡ��ҳ��Ȩcode
				    lock_set.insert (0,ps+card_len, pd-ps-card_len);
					//cout<< lock_set.c_str()<<endl; //������־����
				}
				else
				{
					closesocket(http_sock);  
					WSACleanup();
					return -12;
				
				}	/* �����������ҳ��ȨHTTP ���� */
				memset(data_buf , 0, sizeof(data_buf));
				send_len = sprintf(data_buf, http_req_hdr_tmpl, ref_buf, WX_API_URL);//ƴ��http����
				result = send(http_sock, data_buf, send_len, 0);//����http������GET 
				cout<<"��΢�ŷ�����ҳ��Ȩ�������Ϣ��:\r\n"<<data_buf<<endl;
				if (result == SOCKET_ERROR) /* ����ʧ�� */
				{
					printf("[Web] fail to send, error = %d\n", WSAGetLastError());
					closesocket(http_sock);  
					WSACleanup();
					return -1; 
				}

				do /* ������Ӧ�����浽�ļ��� */
				{
					result = recv(http_sock, rev_buf, HTTP_BUF_SIZE, 0);
					if (result > 0)
					{
						/* ����Ļ����� */
						rev_buf[result] = 0;
						printf("΢�Żظ���ҳ��Ȩ%s\r\n", rev_buf);
						if(ps = strstr(rev_buf , "\"openid\":\""))
						{
							ref_len = strlen("\"openid\":\"");
							if(pd = strstr(ps + ref_len , "\",\""))
							{
								if(pd-ps-ref_len < 10 || pd-ps-ref_len > 80)
								{
									closesocket(http_sock);  
									WSACleanup();
									return - 30;
								}
								memcpy(user_openid , ps+ref_len , pd-ps-ref_len);
								user_openid[ pd-ps-ref_len]=0;
								cout<<"�õ����û�openid�� "<<user_openid<<endl;								
								
								if(0 == lock_set.compare("suoding"))
								{
									if( 0 == WX_SetAlarmLock(card_name  , 1 , user_openid) )
									{
										http_response_webfile(sClient , "suoding.html");// ��ͻ��˷�����Ӧ���� 
									}
									else
									{
										http_response_webfile(sClient , "gongji.html");
									}
								}
								else   if(0 == lock_set.compare("jiesuo"))
								{
									if( 0 == WX_SetAlarmLock(card_name  , 0 , user_openid) )
									{
										http_response_webfile(sClient , "jiesuo.html");// ��ͻ��˷�����Ӧ���� 
									}	
									else
									{
										http_response_webfile(sClient , "gongji.html");
									}
								}
								closesocket(http_sock);  
								WSACleanup();
								return 0;
							}
						
						}
						else
						{
							http_response_webfile(sClient , "wxshibai.html");							
						}
					}

				} while(result > 0);

			}
					
		}		
	}
	else
	{
		*pend='H';//�ָ�֮ǰ�޸ĵĽ�����Ϊ'H'
		cout<<"΢������icoͼ��"<<endl;
		http_send_response(sClient, buf , buf_len);// ��ͻ��˷�����Ӧ���� 
		closesocket(http_sock);  
		WSACleanup();
		return 0;
	}
	
    closesocket(http_sock);  
	WSACleanup();

    return 0;

}
////////////////////////////////////�ظ��豸�б�������λ������//////////////////////////////////////////////
int ResponseDeviceListMsg(SOCKET sClient, char *mToUserName, char *mFromUserName, string weixin_openID)//
{
	
	char send_str[SEND_LEN+100] = {0};
	string   mutf8_str ,mDeviceName, mDevTime, mDescription,Url;
	string  mitem = "";
	string  mgps , mbase_station;
	Json::Value  mjson_card , mjson_user_data , mjson_value;
	mjson_card["token"]= weixin_openID;
	mjson_card["ToUserName"] = mToUserName;
	mjson_card["FromUserName"] = mFromUserName ;
	mjson_user_data = WXGetDeviceData(sClient ,mjson_card);//�����ݿ��ȡ�û�����
	if(mjson_user_data.size() <= 0 )
		return -1;
    memset(send_str,0,sizeof(send_str));
    //ͷ��Ϣ
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2017 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	mutf8_str = "��վ��λ:";
	mutf8_str = GetUTF8FromGBK(mutf8_str);
	mitem = "�󶨵��豸�б�\r\n\r\n";
	mitem = GetUTF8FromGBK(mitem);

	if (mjson_user_data["data"].isArray())//�ж���json����
    {
        int nArraySize = mjson_user_data["data"].size(); //�������� 
		
        for (int i=0; i<nArraySize; i++)
        {       
            mDeviceName = mjson_user_data["data"][i]["bikename"].asString(); //������ĳ��keyֵ 
			
			mDevTime = mjson_user_data["data"][i]["time"].asString(); //������ĳ��keyֵ 
			if(  mjson_user_data["data"][i]["base_station"].isString() )
			{
				mbase_station =   mjson_user_data["data"][i]["base_station"].asString(); //��վ��λ���� 
				int  base_len = mbase_station.find("\r");
				mbase_station = mbase_station.substr(0,base_len-0);
				Url = to_string( (long) (i+1)) + "   <a href=\"http://www.gpsspg.com/bs.htm\">";
				Url +=  mDeviceName + "    " + mutf8_str + mbase_station + "</a>"  ;
				mitem += Url;
				mitem += "  " + mDevTime + "\r\n\r\n";	
			}
			else  if(  mjson_user_data["data"][i]["gps"].isString() )
			{
				mgps = mjson_user_data["data"][i]["gps"].asString(); //gps���� 
				Url = to_string( (long) (i+1)) + "   <a href=\"http://uri.amap.com/marker?position="+ mgps + "&src=weixin&coordinate=wgs84\">";
				Url +=  mDeviceName + "    " +"WGS84: " + mgps + "</a>"  ;
				mitem += Url;
				mitem += "  " + mDevTime + "\r\n\r\n";	
			}
				
        }
    }
	string  mstr;
	int len=0;
	char *xml;
	mstr+=part1;
	mstr+=mToUserName;
	mstr+=part2;
	mstr+=mFromUserName;
	mstr+=part3;
	mstr+="1492853809";
	mstr+=part4;
	mstr+="text";
	mstr+=part5;
	mstr += mitem;//����
	mstr+=part6;
	len=mstr.length();

	char content_header[100];
	sprintf_s(content_header,"Content-Length: %d\r\n", len);
	strcat_s(send_str, content_header);
	strcat_s(send_str, "Connection: close\r\n");
	strcat_s(send_str, "Pragma: no-cache\r\n");
	strcat_s(send_str, "\r\n");
	xml = (char*)mstr.data();
	strcat_s(send_str, xml);//ע��http��Ӧĩβ����Ϣ�壬����\r\nҲ��2���ֽڵ���Ϣ�壬��Ҫ�෢�ֽ�
	//strcat_s(send_str, "\r\n");//ע�⣬��Ϊ����������ֽڣ�����content-Length С��΢��6��Żظ�
	
	//printf("%s",send_str);
	if (send(sClient, send_str, strlen(send_str),0) == -1)
	{
		cout<<"send failed"<<endl;
		closesocket(sClient);
		return -101;
	}
	closesocket(sClient);
    return 0;
}

////////////////////////////////////�ظ��豸״̬//////////////////////////////////////////////
int ResponseDeviceStateMsg(SOCKET sClient, char *mToUserName, char *mFromUserName, string weixin_openID)//
{
	char send_str[SEND_LEN+100] = {0};
	string   mutf8_str ,card_state , mDeviceName, mDevCard , mDevState, mDescription,Url;
	string  mitem = "";
	string  mgps  , mDevTime;
	int  mDevLock =0;
	Json::Value  mjson_card , mjson_user_data , mjson_value , mjson_data;
	mjson_card["token"]= weixin_openID;
	mjson_card["ToUserName"] = mToUserName;
	mjson_card["FromUserName"] = mFromUserName ;
	mjson_user_data = WXGetDeviceData(sClient ,mjson_card);//�����ݿ��ȡ�û�����
	if(mjson_user_data.size() <= 0 )
		return -1;
    memset(send_str,0,sizeof(send_str));
    //ͷ��Ϣ
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2017 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	mutf8_str = "    �����λ";
	mutf8_str = GetUTF8FromGBK(mutf8_str);
	mitem = "���������豸�б�\r\n\r\n";
	mitem = GetUTF8FromGBK(mitem);

	if (mjson_user_data["data"].isArray())//�ж���json����
    {
		int  DataSize = 0;
		int  Result = 0;
		int  SaveFlag=0;
        int nArraySize = mjson_user_data["data"].size(); //�������� 
		for (int i=0; i<nArraySize; i++)
        {       
			SaveFlag = 1;
			for(int j=0;j< DataSize; j++)
			{
				string  str1,str2;
				str1 = mjson_user_data["data"][i]["bikename"].asString();
				str2 = mjson_data["data"][j]["bikename"].asString();
				//str1 = GetUTF8FromGBK(str1);
				//str2 = GetUTF8FromGBK(str2);
				//cout<<str1<<endl;
				//cout <<str2<<endl;
				if( 0 == str1.compare( str2) )
				{
					Result = DateCompare( mjson_user_data["data"][i]["time"].asCString() , mjson_data["data"][j]["time"].asCString());
					switch(Result)
					{
						case 1: //ǰ��ʱ���						
								//mjson_data["data"][j].clear();
								mjson_data["data"][j] =  mjson_user_data["data"][i];
							break;
						case 0://����ʱ���

							break;
						case 2: //ʱ�����
							break;
						default:break;
					}

					j=0;
					SaveFlag = 0;
					break;
				}
			}
			if( SaveFlag )
			{			
				mjson_data["data"].append(  mjson_user_data["data"][i]);
				DataSize ++;
			}

		 }

        for (int i=0; i<DataSize; i++)
        {       
            if(  mjson_data["data"][i]["bikename"].isString())		
				mDeviceName = mjson_data["data"][i]["bikename"].asString(); //������ĳ��keyֵ 
			
			if(  mjson_data["data"][i]["card_lock"].isInt())	
				mDevLock = mjson_data["data"][i]["card_lock"].asInt();
			cout<<"mDevLock value is :  "<<mDevLock <<endl;
			if( mjson_data["data"][i]["card"].isString())		
				mDevCard = mjson_data["data"][i]["card"].asString(); //������ĳ��keyֵ 
			if (mjson_data["data"][i]["card_state"].isString())
				card_state = mjson_data["data"][i]["card_state"].asString();

			mDevState = card_state.substr(0, card_state.find_last_of(","));//����AB·����״̬
			if (mDevLock == 1)
			{
				mDevState += ",�Ѳ���";
				mDevState = GetUTF8FromGBK(mDevState);
			}
			else
			{
				mDevState += ",�ѳ���";
				mDevState = GetUTF8FromGBK(mDevState);
			}
			
			mDevTime  = mjson_data["data"][i]["time"].asString();
			if( mjson_data["data"][i]["gps"].isString())
			{
				mgps = mjson_data["data"][i]["gps"].asString(); //gps��γ������ 
				if(mDevLock)//��ǰ������״̬
					Url =to_string((long) (i+1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/suoding.html&response_type=code&scope=snsapi_base&state=" + mDevCard +"SETjiesuo#wechat_redirect\">";
				else
					Url =to_string((long) (i+1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/suoding.html&response_type=code&scope=snsapi_base&state=" + mDevCard +"SETsuoding#wechat_redirect\">";
	
				//Url = to_string( (long) (i+1)) + "   <a href=\"http://uri.amap.com/marker?position="+ mgps + "&src=weixin&coordinate=wgs84\">";
				Url +=  mDeviceName + "    " + mDevState + mDevTime  + "</a>"  ;
				mitem += Url;
				mitem += "\r\n\r\n";		
			}else
			{
				mgps = mjson_data["data"][i]["base_station"].asString(); //��վ����
				if(mDevLock)//��ǰ������״̬
					Url =to_string((long) (i+1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/suoding.html&response_type=code&scope=snsapi_base&state=" + mDevCard +"SETjiesuo#wechat_redirect\">";
				else
					Url =to_string((long) (i+1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/suoding.html&response_type=code&scope=snsapi_base&state=" + mDevCard +"SETsuoding#wechat_redirect\">";
	
				//Url = to_string( (long) (i+1)) + "   <a href=\"http://www.gpsspg.com/bs.htm\">";
				Url +=  mDeviceName + "    " + mDevState + mDevTime  + "</a>"  ;
				mitem += Url;
				mitem += "\r\n\r\n";	
			}
        }
    }
	string  mstr;
	int len=0;
	char *xml;
	mstr+=part1;
	mstr+=mToUserName;
	mstr+=part2;
	mstr+=mFromUserName;
	mstr+=part3;
	mstr+="1492853809";
	mstr+=part4;
	mstr+="text";
	mstr+=part5;
	mstr += mitem;//����
	mstr+=part6;
	len=mstr.length();

	char content_header[100];
	sprintf_s(content_header,"Content-Length: %d\r\n", len);
	strcat_s(send_str, content_header);
	strcat_s(send_str, "Connection: close\r\n");
	strcat_s(send_str, "Pragma: no-cache\r\n");
	strcat_s(send_str, "\r\n");
	xml = (char*)mstr.data();
	strcat_s(send_str, xml);//ע��http��Ӧĩβ����Ϣ�壬����\r\nҲ��2���ֽڵ���Ϣ�壬��Ҫ�෢�ֽ�
	//strcat_s(send_str, "\r\n");//ע�⣬��Ϊ����������ֽڣ�����content-Length С��΢��6��Żظ�
	//printf("%s",send_str);
	if (send(sClient, send_str, strlen(send_str),0) == -1)
	{
		cout<<"send failed"<<endl;
		closesocket(sClient);
		return -101;
	}
	closesocket(sClient);
    return 0;
}
////////////////////////////////////�ظ��豸������б�//////////////////////////////////////////////
int ResponseMotorStateMsg(SOCKET sClient, char *mToUserName, char *mFromUserName, string weixin_openID)//
{
	
	char send_str[SEND_LEN+100] = {0};
	string   mutf8_str , card_state,mDeviceName, mDevCard , mDevState, mDescription,Url;
	string  mitem = "";
	string  mgps  , mDevTime;
	int  mDevLock =0;
	Json::Value  mjson_card , mjson_user_data , mjson_value , mjson_data;
	mjson_card["token"]= weixin_openID;
	mjson_card["ToUserName"] = mToUserName;
	mjson_card["FromUserName"] = mFromUserName ;
	mjson_user_data = WXGetDeviceData(sClient ,mjson_card);//�����ݿ��ȡ�û�����
	if(mjson_user_data.size() <= 0 )
		return -1;
    memset(send_str,0,sizeof(send_str));
    //ͷ��Ϣ
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2017 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	mutf8_str = "    �����λ";
	mutf8_str = GetUTF8FromGBK(mutf8_str);
	mitem = "A·��������豸�б�\r\n\r\n";
	mitem = GetUTF8FromGBK(mitem);

	if (mjson_user_data["data"].isArray())//�ж���json����
    {
		int  DataSize = 0;
		int  Result = 0;
		int  SaveFlag=0;
        int nArraySize = mjson_user_data["data"].size(); //�������� 
		for (int i=0; i<nArraySize; i++)
        {       
			SaveFlag = 1;
			for(int j=0;j< DataSize; j++)
			{
				string  str1,str2;
				str1 = mjson_user_data["data"][i]["bikename"].asString();
				str2 = mjson_data["data"][j]["bikename"].asString();
				//str1 = GetUTF8FromGBK(str1);
				//str2 = GetUTF8FromGBK(str2);
				//cout<<str1<<endl;
				//cout <<str2<<endl;
				if( 0 == str1.compare( str2) )
				{
					Result = DateCompare( mjson_user_data["data"][i]["time"].asCString() , mjson_data["data"][j]["time"].asCString());
					switch(Result)
					{
						case 1: //ǰ��ʱ���						
								//mjson_data["data"][j].clear();
								mjson_data["data"][j] =  mjson_user_data["data"][i];
							break;
						case 0://����ʱ���

							break;
						case 2: //ʱ�����
							break;
						default:break;
					}

					j=0;
					SaveFlag = 0;
					break;
				}
			}
			if( SaveFlag )
			{			
				mjson_data["data"].append(  mjson_user_data["data"][i]);
				DataSize ++;
			}

		 }

        for (int i=0; i<DataSize; i++)
        {       
            if(  mjson_data["data"][i]["bikename"].isString())		
				mDeviceName = mjson_data["data"][i]["bikename"].asString(); //������ĳ��keyֵ 
			
			if(  mjson_data["data"][i]["card_lock"].isInt())	
				mDevLock = mjson_data["data"][i]["card_lock"].asInt();
			cout<<"mDevLock value is :  "<<mDevLock <<endl;
			if( mjson_data["data"][i]["card"].isString())		
				mDevCard = mjson_data["data"][i]["card"].asString(); //������ĳ��keyֵ 
			if (mjson_data["data"][i]["card_state"].isString())
				card_state = mjson_data["data"][i]["card_state"].asString();

			mDevState = card_state.substr( 0,  card_state.find_last_of(","));//����AB·����״̬
	
			mDevTime  = mjson_data["data"][i]["time"].asString();
			if (mDevLock == 1)
			{
				mDevState += ",�Ѳ���";
				
			}
			else
			{
				mDevState += ",�ѳ���";
				
			}
			if (mjson_data["data"][i]["gps"].isString())
			{
				mgps = mjson_data["data"][i]["gps"].asString(); //gps��γ������ 

				string::size_type  pos = mDevState.find("AOPEN");

				if (pos != string::npos)
				{
					//cout << "2222222" << endl;				
					cout << "GPS����Ӧ��close" << endl;
					Url = to_string((long)(i + 1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/motorlock.html&response_type=code&scope=snsapi_base&state=" + mDevCard + "SETmotorclose#wechat_redirect\">";
				}
				else
				{
					cout << "GPS����Ӧ��OPEN" << endl;
					Url = to_string((long)(i + 1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/motorlock.html&response_type=code&scope=snsapi_base&state=" + mDevCard + "SETmotoropen#wechat_redirect\">";
				}
				//Url = to_string( (long) (i+1)) + "   <a href=\"http://uri.amap.com/marker?position="+ mgps + "&src=weixin&coordinate=wgs84\">";
				mDevState = GetUTF8FromGBK(mDevState);
				Url +=  mDeviceName + "    " + mDevState + mDevTime  + "</a>"  ;
				mitem += Url;
				mitem += "\r\n\r\n";		
			}
			else
			{
				mgps = mjson_data["data"][i]["base_station"].asString(); //��վ����			
				string::size_type  pos = mDevState.find("AOPEN");
				if (pos != string::npos)
				{
					cout << "��վ����Ӧ��close" << endl;
					Url = to_string((long)(i + 1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/motorlock.html&response_type=code&scope=snsapi_base&state=" + mDevCard + "SETmotorclose#wechat_redirect\">";
				}
				else
				{
					cout << "��վ����Ӧ��OPEN" << endl;
					Url = to_string((long)(i + 1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/motorlock.html&response_type=code&scope=snsapi_base&state=" + mDevCard + "SETmotoropen#wechat_redirect\">";
				}
				mDevState = GetUTF8FromGBK(mDevState);
				//Url = to_string( (long) (i+1)) + "   <a href=\"http://www.gpsspg.com/bs.htm\">";
				Url +=  mDeviceName + "    " + mDevState + mDevTime  + "</a>"  ;
				mitem += Url;
				mitem += "\r\n\r\n";	
			}
			
        }
    }
	string  mstr;
	int len=0;
	char *xml;
	mstr+=part1;
	mstr+=mToUserName;
	mstr+=part2;
	mstr+=mFromUserName;
	mstr+=part3;
	mstr+="1492853809";
	mstr+=part4;
	mstr+="text";
	mstr+=part5;
	mstr += mitem;//����
	mstr+=part6;
	len=mstr.length();

	char content_header[100];
	sprintf_s(content_header,"Content-Length: %d\r\n", len);
	strcat_s(send_str, content_header);
	strcat_s(send_str, "Connection: close\r\n");
	strcat_s(send_str, "Pragma: no-cache\r\n");
	strcat_s(send_str, "\r\n");
	xml = (char*)mstr.data();
	strcat_s(send_str, xml);//ע��http��Ӧĩβ����Ϣ�壬����\r\nҲ��2���ֽڵ���Ϣ�壬��Ҫ�෢�ֽ�
	//strcat_s(send_str, "\r\n");//ע�⣬��Ϊ����������ֽڣ�����content-Length С��΢��6��Żظ�
	//printf("%s",send_str);
	if (send(sClient, send_str, strlen(send_str),0) == -1)
	{
		cout<<"send failed"<<endl;
		closesocket(sClient);
		return -101;
	}
	closesocket(sClient);
    return 0;
}
////////////////////////////////////�ظ��������豸�����б�//////////////////////////////////////////////
int ResponseDeviceOpenMsg(SOCKET sClient, char *mToUserName, char *mFromUserName, string weixin_openID)//
{

	char send_str[SEND_LEN + 100] = { 0 };
	string   mutf8_str,card_state, mDeviceName, mDevCard, mDevState, mDescription, Url;
	string  mitem = "";
	string  mgps, mDevTime;
	int  mDevLock = 0;
	Json::Value  mjson_card, mjson_user_data, mjson_value, mjson_data;
	mjson_card["token"] = weixin_openID;
	mjson_card["ToUserName"] = mToUserName;
	mjson_card["FromUserName"] = mFromUserName;
	mjson_user_data = WXGetDeviceData(sClient, mjson_card);//�����ݿ��ȡ�û�����
	if (mjson_user_data.size() <= 0)
		return -1;
	memset(send_str, 0, sizeof(send_str));
	//ͷ��Ϣ
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2017 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	mutf8_str = "    �����λ";
	mutf8_str = GetUTF8FromGBK(mutf8_str);
	mitem = "B·��������豸�б�\r\n\r\n";
	mitem = GetUTF8FromGBK(mitem);

	if (mjson_user_data["data"].isArray())//�ж���json����
	{
		int  DataSize = 0;
		int  Result = 0;
		int  SaveFlag = 0;
		int nArraySize = mjson_user_data["data"].size(); //�������� 
		for (int i = 0; i<nArraySize; i++)
		{
			SaveFlag = 1;
			for (int j = 0; j< DataSize; j++)
			{
				string  str1, str2;
				str1 = mjson_user_data["data"][i]["bikename"].asString();
				str2 = mjson_data["data"][j]["bikename"].asString();
				//str1 = GetUTF8FromGBK(str1);
				//str2 = GetUTF8FromGBK(str2);
				//cout<<str1<<endl;
				//cout <<str2<<endl;
				if (0 == str1.compare(str2))
				{
					Result = DateCompare(mjson_user_data["data"][i]["time"].asCString(), mjson_data["data"][j]["time"].asCString());
					switch (Result)
					{
					case 1: //ǰ��ʱ���						
							//mjson_data["data"][j].clear();
						mjson_data["data"][j] = mjson_user_data["data"][i];
						break;
					case 0://����ʱ���

						break;
					case 2: //ʱ�����
						break;
					default:break;
					}

					j = 0;
					SaveFlag = 0;
					break;
				}
			}
			if (SaveFlag)
			{
				mjson_data["data"].append(mjson_user_data["data"][i]);
				DataSize++;
			}

		}

		for (int i = 0; i<DataSize; i++)
		{
			if (mjson_data["data"][i]["bikename"].isString())
				mDeviceName = mjson_data["data"][i]["bikename"].asString(); //������ĳ��keyֵ 

			if (mjson_data["data"][i]["card_lock"].isInt())
				mDevLock = mjson_data["data"][i]["card_lock"].asInt();
			cout << "mDevLock value is :  " << mDevLock << endl;
			if (mjson_data["data"][i]["card"].isString())
				mDevCard = mjson_data["data"][i]["card"].asString(); //������ĳ��keyֵ 
			if (mjson_data["data"][i]["card_state"].isString())
				card_state = mjson_data["data"][i]["card_state"].asString();

			mDevState = card_state.substr(0, card_state.find_last_of(","));//����AB·����״̬
			if (mDevLock == 1)
			{
				mDevState += ",�Ѳ���";
				
			}
			else
			{
				mDevState += ",�ѳ���";
				
			}
			
			mDevTime = mjson_data["data"][i]["time"].asString();
			if (mjson_data["data"][i]["gps"].isString())
			{
				mgps = mjson_data["data"][i]["gps"].asString(); //gps��γ������ 
				
				if (mDevState.find("BOPEN") != std::string::npos)//��ǰ������״̬
					Url = to_string((long)(i + 1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/deviceopen.html&response_type=code&scope=snsapi_base&state=" + mDevCard + "SETdeviceclose#wechat_redirect\">";
				else
					Url = to_string((long)(i + 1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/deviceopen.html&response_type=code&scope=snsapi_base&state=" + mDevCard + "SETdeviceopen#wechat_redirect\">";

				//Url = to_string( (long) (i+1)) + "   <a href=\"http://uri.amap.com/marker?position="+ mgps + "&src=weixin&coordinate=wgs84\">";
				mDevState = GetUTF8FromGBK(mDevState);
				Url += mDeviceName + "    " + mDevState + mDevTime + "</a>";
				mitem += Url;
				mitem += "\r\n\r\n";
			}
			else
			{
				mgps = mjson_data["data"][i]["base_station"].asString(); //��վ����
				if (mDevState.find("BOPEN") != std::string::npos)//��ǰ������״̬
					Url = to_string((long)(i + 1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/deviceopen.html&response_type=code&scope=snsapi_base&state=" + mDevCard + "SETdeviceclose#wechat_redirect\">";
				else
					Url = to_string((long)(i + 1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/deviceopen.html&response_type=code&scope=snsapi_base&state=" + mDevCard + "SETdeviceopen#wechat_redirect\">";

				//Url = to_string( (long) (i+1)) + "   <a href=\"http://www.gpsspg.com/bs.htm\">";
				mDevState = GetUTF8FromGBK(mDevState);
				Url += mDeviceName + "    " + mDevState + mDevTime + "</a>";
				mitem += Url;
				mitem += "\r\n\r\n";
			}
		}
	}
	string  mstr;
	int len = 0;
	char *xml;
	mstr += part1;
	mstr += mToUserName;
	mstr += part2;
	mstr += mFromUserName;
	mstr += part3;
	mstr += "1492853809";
	mstr += part4;
	mstr += "text";
	mstr += part5;
	mstr += mitem;//����
	mstr += part6;
	len = mstr.length();

	char content_header[100];
	sprintf_s(content_header, "Content-Length: %d\r\n", len);
	strcat_s(send_str, content_header);
	strcat_s(send_str, "Connection: close\r\n");
	strcat_s(send_str, "Pragma: no-cache\r\n");
	strcat_s(send_str, "\r\n");
	xml = (char*)mstr.data();
	strcat_s(send_str, xml);//ע��http��Ӧĩβ����Ϣ�壬����\r\nҲ��2���ֽڵ���Ϣ�壬��Ҫ�෢�ֽ�
							//strcat_s(send_str, "\r\n");//ע�⣬��Ϊ����������ֽڣ�����content-Length С��΢��6��Żظ�
							//printf("%s",send_str);
	if (send(sClient, send_str, strlen(send_str), 0) == -1)
	{
		cout << "send failed" << endl;
		closesocket(sClient);
		return -101;
	}
	closesocket(sClient);
	return 0;
}
//////////////////////////////�ظ�ͼ����Ϣ///////////////////////////////////////////////////
int ResponseNewsMsg(SOCKET sClient, char *mToUserName, char *mFromUserName,char* weixin_openID)//
{
	char send_str[SEND_LEN+100] = {0};
	string  mArticleCount ,mutf8_str ,mTitle, mDescription, PicUrl,Url;
    memset(send_str,0,sizeof(send_str));
    //ͷ��Ϣ
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2017 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	string  mstr;
	string  mitem;
	string  mgps;
	int len=0;
	char *xml;
	Json::Value  mjson_card , mjson_user_data , mjson_value;

	mjson_card["token"]= weixin_openID ;
	mjson_card["ToUserName"] = mToUserName;
	mjson_card["FromUserName"] = mFromUserName ;
	mjson_user_data = WXGetDeviceData(sClient ,mjson_card);//��ȡ�û�����
	if(mjson_user_data.size() <= 0 )
		return -1;

	mstr+=part1;
	mstr+=mToUserName;
	mstr+=part2;
	mstr+=mFromUserName;
	mstr+=part3;
	mstr+="1492853809";
	mstr+=part4;
	mstr+="news";
	mstr+=news_part1;
	
	mutf8_str = "    �����λ";
	mutf8_str = GetUTF8FromGBK(mutf8_str);	
			
	PicUrl = "https://mmbiz.qpic.cn/mmbiz_png/I1ABleEnu624wT691lbibCicib7C6K4UiabWHYhXldFQR0glGibv14Kyica3rxpJqQqoUic3zsDw8oUhUXsibiazLhUcW3w/0?wx_fmt=png";
	if (mjson_user_data["data"].isArray())//�ж���json����
    {
        int nArraySize = mjson_user_data["data"].size(); //�������� 
		mArticleCount = to_string( (long )nArraySize);
		mstr+=mArticleCount;
		mstr+=news_part2;
	
        for (int i=0; i<nArraySize; i++)
        {       
            mTitle = mjson_user_data["data"][i]["bikename"].asString(); //������ĳ��keyֵ 
            mgps = mjson_user_data["data"][i]["gps"].asString(); //gps���� 
            mitem += news_part3;
			mitem +=  mTitle + mutf8_str;
			mitem += news_part4;
			mitem +=mTitle;
			mitem +=news_part5;
			mitem += PicUrl;
			mitem +=news_part6;
			Url = "http://uri.amap.com/marker?position="+mgps+ "&src=weixin&coordinate=wgs84";
			mitem += Url;
			mitem += news_part7;		
        }
    }
	mitem += news_part8;

#if  0
	mstr+=mitem;
	mstr+=news_part4;
	
	//cout<<mDescription.c_str()<<endl;
	mstr+=mDescription;
	mstr+=news_part5;
	PicUrl = "https://mmbiz.qpic.cn/mmbiz_png/I1ABleEnu624wT691lbibCicib7C6K4UiabWHYhXldFQR0glGibv14Kyica3rxpJqQqoUic3zsDw8oUhUXsibiazLhUcW3w/0?wx_fmt=png";
	mstr+=PicUrl;
	mstr+=news_part6;
	//Url = "http://api.map.baidu.com/direction?origin=latlng:3800.3858,11427.8776|name:�ҵ�λ��&destination=latlng:3802.1879,11428.6355&mode=driving&region=ʯ��ׯ&output=html&coord_type=wgs84&src=weixin";
	//Url = "http://api.map.baidu.com/direction?origin=latlng:34.264642646862,108.95108518068|name:�ҵ�λ��&destination=������&mode=driving&region=����&output=html&src=weixin";
	Url = "http://uri.amap.com/marker?position="+mgps+ "&src=weixin&coordinate=wgs84";
	mstr+=Url;
	mstr+=news_part7;
#endif

	mstr+=mitem;
	len=mstr.length();
	//cout<< mstr.c_str()<<endl;

	char content_header[100];
	sprintf_s(content_header,"Content-Length: %d\r\n", len);
	strcat_s(send_str, content_header);
	strcat_s(send_str, "Connection: close\r\n");
	strcat_s(send_str, "Pragma: no-cache\r\n");
	strcat_s(send_str, "\r\n");
	xml = (char*)mstr.data();
	strcat_s(send_str, xml);//ע��http��Ӧĩβ����Ϣ�壬����\r\nҲ��2���ֽڵ���Ϣ�壬��Ҫ�෢�ֽ�
	//strcat_s(send_str, "\r\n");//ע�⣬��Ϊ����������ֽڣ�����content-Length С��΢��6��Żظ�
	//printf("%s",send_str);
	if (send(sClient, send_str, strlen(send_str),0) == -1)
	{
		cout<<"send failed"<<endl;
		closesocket(sClient);
		return -101;
	}
	closesocket(sClient);
    return 0;
}
/////////////////////////////////////////////////////////////////////////////////
///////////////////////��ȡ�û�����Ϣ//////////////////////////////////////////
Json::Value  WXGetUserBindData(SOCKET   ClientS, Json::Value  mJsonValue)
{
	bool  getsucess = false;
	const char user[] = "root";
	const char pswd[] = "123456";
	const char host[] = "localhost";
	char table[] = "bike";
	unsigned int port = 3306;
	string  ToUserName;
	string  FromUserName;
	char send_str[2048 + 100] = { 0 };

	Json::Value mjson_cardstate;

	MYSQL myCont;
	MYSQL_RES *result;
	int res;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1, f2, num_row, num_col;
	my_ulonglong  card_row = 0, card_line = 0;
	my_ulonglong  base_station_row = 0, base_station_line = 0;
	string   str_token, str_username, str_card, str_phone, str_bikename, str_bikecolor, str_biketype;

	memset(send_str, 0, sizeof(send_str));//�������������0��memset���ɸ�ָ����䣬��ֻ�����ָ�����ڵ��ڴ�
										  //ͷ��Ϣ
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2018 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	string  mstr;//��Ϣ���ַ�������

	if (GetVaule_CheckStr(&str_token, mJsonValue, "token") == 0)
	{
		cout << "json�������ʹ���tokenӦ��Ϊstring" << endl;
		return  -5;
	}
	if (GetVaule_CheckStr(&ToUserName, mJsonValue, "ToUserName") == 0)
	{
		cout << "json�������ʹ���ToUserNameӦ��Ϊstring" << endl;
		return  -5;
	}
	if (GetVaule_CheckStr(&FromUserName, mJsonValue, "FromUserName") == 0)
	{
		cout << "json�������ʹ���FromUserNameӦ��Ϊstring" << endl;
		return  -5;
	}
	
	string  m_strname = "SELECT  username  FROM  weixin_user  WHERE token = '" + str_token + "'";
	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
	mysql_init(&myCont);//��ʼ��mysql

	if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
	{
	}
	else
	{
		cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

		return -2;
	}

	//cout<<m_strname<<endl;
	getsucess = false;
	res = mysql_query(&myCont, (const  char *)m_strname.c_str()); //ִ��SQL���,ͨ��token����username
	if (!res)
	{
		//�����ѯ�������ݵ�result
		result = mysql_store_result(&myCont);
		num_row = mysql_num_rows(result); //��ȡ����
		num_col = mysql_num_fields(result); //��ȡ����
											//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
		MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������

		for (f1 = 0; f1<num_row; f1++) //ѭ����
		{
			mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

			for (f2 = 0; f2<num_col; f2++) //ѭ����
			{
				if (fields[f2].name != NULL)
					if (!strcmp(fields[f2].name, "username")) //�жϵ�ǰ�е��ֶ�����
					{
						str_username = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL
																  //cout<<str_username.data()<<endl;
						getsucess = true;
						break;
						//cout<<mstr_pwd.data()<<endl;d
					}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������

			}
		}

		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

	}
	if (getsucess == true)
	{

	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

		cout << "select wx_username error!\n" << endl;
		int len = 0;
		char *xml;
		mstr = part1;
		mstr += ToUserName;
		mstr += part2;
		mstr += FromUserName;
		mstr += part3;
		mstr += "1492853809";
		mstr += part4;
		mstr += "text";
		mstr += part5;
		mstr += GetUTF8FromGBK("����û��ע�ᣬ��ע���˺ţ���ʹ��\r\n");
		//mstr+="<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa70b2a8a84b770fb&redirect_uri=http://gps.shaohaoqi.cn/zhuce.html&response_type=code&scope=snsapi_base&state=suoding123#wechat_redirect\">";
		mstr += "<a href=\"http://wxweb.shaohaoqi.cn/zhuce.html?token=" + str_token + "\">";
		mstr += GetUTF8FromGBK("���ע��");//����
		mstr += "</a>";
		mstr += part6;
		len = mstr.length();//������Ϣ�峤��Content-Length  Ҫ�ͺ�����Ϣ���ֽ����ϸ�һ��

		char content_header[100];
		sprintf_s(content_header, "Content-Length: %d\r\n", len);
		strcat_s(send_str, content_header);
		strcat_s(send_str, "Connection: close\r\n");
		strcat_s(send_str, "Pragma: no-cache\r\n");
		strcat_s(send_str, "\r\n");
		xml = (char*)mstr.data();
		strcat_s(send_str, xml);//ע��http��Ӧĩβ����Ϣ�壬����\r\nҲ��2���ֽڵ���Ϣ�壬��Ҫ�෢�ֽ�
		//strcat_s(send_str, "\r\n");//ע�⣬��Ϊ����������ֽڣ����³���content-Length��΢��6��Żظ�
		//printf("----------------------------------------------------\r\n");
		//printf("%s",send_str);
		if (send(ClientS, send_str, strlen(send_str), 0) == -1)
		{
			cout << "send failed" << endl;
			closesocket(ClientS);
			return -101;
		}
		return -4;
	}

	string  m_strcard = "SELECT  *  FROM  user_bike  WHERE username = '" + str_username + "'";
	//cout<< m_strcard<<endl;
	getsucess = false;
	res = mysql_query(&myCont, (const  char *)m_strcard.c_str()); //ִ��SQL���,ͨ��token����username
	if (!res)
	{
		//�����ѯ�������ݵ�result
		result = mysql_store_result(&myCont);
		num_row = mysql_num_rows(result); //��ȡ����
		num_col = mysql_num_fields(result); //��ȡ����
											//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
		MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if (num_row>0)
		{
			mjson_cardstate["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
			mjson_cardstate["error"] = Json::Value("sucess");
			for (f1 = 0; f1<num_row; f1++) //ѭ����
			{
				str_bikename.clear();
				str_card.clear();
				mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����				
								
				for (f2 = 0; f2<num_col; f2++) //ѭ����
				{
					if (!strcmp(fields[f2].name, "bikename"))
					{
						str_bikename = getNullStr(mysql_row[f2]);						
					}

					if (!strcmp(fields[f2].name, "card")) //�жϵ�ǰ�е��ֶ�����
					{
						//cout<<str_username.data()<<endl;
						str_card = getNullStr(mysql_row[f2]);
						getsucess = true;						
					}//endif card
					 //printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������
				}//endfor ��
				Json::Value  mjson_card;  // ��ʾ���� json ����
				mjson_card["bikename"] = Json::Value(str_bikename);
				mjson_card["card"] = Json::Value(str_card);
				mjson_cardstate["data"].append(Json::Value(mjson_card));//�����
			}//endfor ��
			mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

		}
		else
		{
			mysql_close(&myCont);
			mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

			cout << "��ǰ�û�û�а��豸��" << endl;

			int len = 0;
			char *xml;
			mstr = part1;
			mstr += ToUserName;
			mstr += part2;
			mstr += FromUserName;
			mstr += part3;
			mstr += "1492853809";
			mstr += part4;
			mstr += "text";
			mstr += part5;
			mstr += GetUTF8FromGBK("����û�а��豸������豸��ʹ��,���԰󶨶��!\r\n");
			//mstr+="<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa70b2a8a84b770fb&redirect_uri=http://gps.shaohaoqi.cn/zhuce.html&response_type=code&scope=snsapi_base&state=suoding123#wechat_redirect\">";
			mstr += "<a href=\"http://wxweb.shaohaoqi.cn/bind.html?token=" + str_token + "\">";
			mstr += GetUTF8FromGBK("�����");//����
			mstr += "</a>";
			mstr += part6;
			len = mstr.length();//������Ϣ�峤��Content-Length  Ҫ�ͺ�����Ϣ���ֽ����ϸ�һ��

			char content_header[100];
			sprintf_s(content_header, "Content-Length: %d\r\n", len);
			strcat_s(send_str, content_header);
			strcat_s(send_str, "Connection: close\r\n");
			strcat_s(send_str, "Pragma: no-cache\r\n");
			strcat_s(send_str, "\r\n");
			xml = (char*)mstr.data();
			strcat_s(send_str, xml);//ע��http��Ӧĩβ����Ϣ�壬����\r\nҲ��2���ֽڵ���Ϣ�壬��Ҫ�෢�ֽ�

			if (send(ClientS, send_str, strlen(send_str), 0) == -1)
			{
				cout << "send failed" << endl;
				closesocket(ClientS);
				return -101;
			}
			return -6;
		}

	}
	mysql_close(&myCont);//��ʱ�ر�sql���ӣ�����ռ��mysql������
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	if (getsucess == true)
	{
		//Json::FastWriter  fast_writer;//�鿴json���ݶ���
		//string str = fast_writer.write(mjson_cardstate); //jsonתstring
		return  mjson_cardstate;
		//send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
	}
	else  if (getsucess == false)
	{
		//Json::Value root;             // ��ʾ���� json ����
		//root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		//root["error"] = Json::Value("get_userbind error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��

		//Json::FastWriter  fast_writer;//�鿴json���ݶ���
		//string str = fast_writer.write(root); //jsonתstring
		//send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		cout << "get userbind data error!\n" << endl;
		return -4;
	}
	///////////////////////////////////////////////////////////////////////	

	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	return -1;

}
///////////////////////��ȡ�û���������//////////////////////////////////////////
Json::Value  WXGetDeviceData(SOCKET   ClientS ,Json::Value  mJsonValue)
{
	bool  getsucess =false;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;   
	string  ToUserName;
	string  FromUserName;
	char send_str[SEND_LEN+100] = {0};
    
	Json::Value mjson_cardstate ;       
	
	MYSQL myCont;
    MYSQL_RES *result;
    int res;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1,f2,num_row,num_col; 
	my_ulonglong  card_row=0,card_line=0;
	my_ulonglong  base_station_row = 0, base_station_line = 0;
	string   str_token,str_username ,str_card,str_phone,str_bikename,str_bikecolor,str_biketype;  

	memset(send_str,0,sizeof(send_str));//�������������0��memset���ɸ�ָ����䣬��ֻ�����ָ�����ڵ��ڴ�
    //ͷ��Ϣ
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2018 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	string  mstr;//��Ϣ���ַ�������
	
	if( GetVaule_CheckStr(&str_token , mJsonValue , "token") == 0 )
	{
		cout<< "json�������ʹ���tokenӦ��Ϊstring"<<endl;
		return  -5;
	}
	if( GetVaule_CheckStr(&ToUserName , mJsonValue , "ToUserName") == 0 )
	{
		cout<< "json�������ʹ���ToUserNameӦ��Ϊstring"<<endl;
		return  -5;
	}
	if( GetVaule_CheckStr(&FromUserName , mJsonValue , "FromUserName") == 0 )
	{
		cout<< "json�������ʹ���FromUserNameӦ��Ϊstring"<<endl;
		return  -5;
	}
	//string  mstr_ziduan = mJsonValue["token"].asString();
	string  m_strname = "SELECT  username  FROM  weixin_user  WHERE token = '" + str_token + "'";
	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//��ʼ��mysql//��ʼ��mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {    
    }
    else
    {
        cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

		return -2;
    }

	//cout<<m_strname<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strname.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{			
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������

		for(f1=0;f1<num_row;f1++) //ѭ����
		{		
			mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

			for(f2=0;f2<num_col;f2++) //ѭ����
			{	
				if (fields[f2].name!=NULL)  
				if (!strcmp( fields[f2].name , "username")) //�жϵ�ǰ�е��ֶ�����
				{
					str_username = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL
					//cout<<str_username.data()<<endl;
					getsucess = true;
					break;
					//cout<<mstr_pwd.data()<<endl;d
				}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������
			
			}
		}

		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����
		
	}
	if(getsucess == true)
	{
		
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

		cout << "select wx_username error!\n" << endl;
		int len=0;
		char *xml;
		mstr=part1;
		mstr+=ToUserName;
		mstr+=part2;
		mstr+=FromUserName;
		mstr+=part3;
		mstr+="1492853809";
		mstr+=part4;
		mstr+="text";
		mstr+=part5;
		mstr+=GetUTF8FromGBK("����û��ע�ᣬ��ע���˺ţ���ʹ��\r\n");
		//mstr+="<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa70b2a8a84b770fb&redirect_uri=http://gps.shaohaoqi.cn/zhuce.html&response_type=code&scope=snsapi_base&state=suoding123#wechat_redirect\">";
		mstr+="<a href=\"http://wxweb.shaohaoqi.cn/zhuce.html?token=" + str_token +"\">";
		mstr += GetUTF8FromGBK("���ע��");//����
		mstr += "</a>";
		mstr+=part6;
		len=mstr.length();//������Ϣ�峤��Content-Length  Ҫ�ͺ�����Ϣ���ֽ����ϸ�һ��
	
		char content_header[100];
		sprintf_s(content_header,"Content-Length: %d\r\n", len);
		strcat_s(send_str, content_header);
		strcat_s(send_str, "Connection: close\r\n");
		strcat_s(send_str, "Pragma: no-cache\r\n");
		strcat_s(send_str, "\r\n");
		xml = (char*)mstr.data();
		strcat_s(send_str, xml);//ע��http��Ӧĩβ����Ϣ�壬����\r\nҲ��2���ֽڵ���Ϣ�壬��Ҫ�෢�ֽ�
		//strcat_s(send_str, "\r\n");//ע�⣬��Ϊ����������ֽڣ����³���content-Length��΢��6��Żظ�
		//printf("----------------------------------------------------\r\n");
		//printf("%s",send_str);
		if (send(ClientS, send_str, strlen(send_str),0) == -1)
		{
			cout<<"send failed"<<endl;
			closesocket(ClientS);
			return -101;
		}
		return -4;
	}

	string  m_strcard = "SELECT  *  FROM  user_bike  WHERE username = '" + str_username +"'";
	//cout<< m_strcard<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strcard.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res )
	{			
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if(num_row>0)
		{
			mjson_cardstate["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
			mjson_cardstate["error"] = Json::Value("sucess");
			for(f1=0;f1<num_row;f1++) //ѭ����
			{		
				str_bikename.clear();
				str_card.clear();
				mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����
				
				for(f2=0;f2<num_col;f2++) //ѭ����
				{	
					if(!strcmp(fields[f2].name , "bikename"))
					{
						str_bikename =  getNullStr(mysql_row[f2]);		
			
					}
					
					if (fields[f2].name!=NULL)  
					if (!strcmp( fields[f2].name , "card")) //�жϵ�ǰ�е��ֶ�����
					{				
						//cout<<str_username.data()<<endl;
						str_card =  getNullStr(mysql_row[f2]);										
					
					}//endif card
					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������
			
				}//endfor ��
			
				if(str_card.length()>0   )
				{
					string  m_strToken = "SELECT  *  FROM  card_data  WHERE card = '" + str_card + "' ORDER BY card_id DESC LIMIT 0,1";
					//cout<<m_strToken<<endl;
					res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //ִ��SQL���,ͨ��token����username
					if(!res  )
					{				
						//�����ѯ�������ݵ�result
						MYSQL_RES *card_result = mysql_store_result(&myCont);
						card_row=mysql_num_rows(card_result); //��ȡ����
						card_line=mysql_num_fields(card_result); //��ȡ����
						MYSQL_FIELD*  card_files = mysql_fetch_fields(card_result); //���������ֶνṹ������
						if(card_row > 0)
						{	
							getsucess = true;
							Json::Value  mjson_card;  // ��ʾ���� json ����
							mjson_card["card"] = Json::Value(str_card);	
							mjson_card["bikename"] = Json::Value(str_bikename);	
							mysql_row = mysql_fetch_row(card_result); //��ȡÿ�е�����

							for(int card_f2 =0;card_f2<card_line;card_f2++) //ѭ����
							{	
								//if (fields[f2].name!=NULL)  
								if (!strcmp( card_files[card_f2].name , "card_state")) //�жϵ�ǰ�е��ֶ�����
								{										
									mjson_card[card_files[card_f2].name] = Json::Value( getNullStr(mysql_row[card_f2]) );
				
									getsucess = true ;
						
								}
								else  if(!strcmp( card_files[card_f2].name , "card_lock"))
								{
									int  lockset = *mysql_row[card_f2] - '0' ;
									mjson_card[card_files[card_f2].name] = Json::Value( lockset);
				
								}
								else  if(!strcmp( card_files[card_f2].name , "gps"))
								{
									mjson_card[card_files[card_f2].name]  = GetGpsDataFromGPSItem("","",getNullStr(mysql_row[card_f2]));
									//mjson_card[card_files[card_f2].name] = Json::Value( getNullStr(mysql_row[card_f2]) );
				
								}
								else if (!strcmp( card_files[card_f2].name , "time"))
								{
									mjson_card[card_files[card_f2].name] = Json::Value( getNullStr(mysql_row[card_f2]) );
				
								}

								//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������
	
							}//endfor ��

							mysql_free_result(card_result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����	
							//mjson_cardstate["data"] = Json::Value( mjson_card );
							mjson_cardstate["data"].append( Json::Value(mjson_card) );//�����
						
						}//endif
					}
					//////////////////////////////��ѯ��վ�ϴ����ݱ�///////////////////////////
#if   1
					string  m_sql_base_station = "SELECT  *  FROM  card_base_station_data  WHERE card = '" + str_card + "' ORDER BY card_id DESC LIMIT 0,1";
					//cout<<m_strToken<<endl;
					res = mysql_query(&myCont, (const  char *)m_sql_base_station.c_str()); //ִ��SQL���,ͨ��token����username
					if(!res  )
					{				
						//�����ѯ�������ݵ�result
						MYSQL_RES *base_station_result = mysql_store_result(&myCont);
						base_station_row=mysql_num_rows(base_station_result); //��ȡ����
						base_station_line=mysql_num_fields(base_station_result); //��ȡ����
						MYSQL_FIELD*  card_files = mysql_fetch_fields(base_station_result); //���������ֶνṹ������
						if(base_station_row > 0)
						{	
							getsucess = true;
							Json::Value  mbase_json_card;  // ��ʾ���� json ����
							mbase_json_card["card"] = Json::Value(str_card);	
							mbase_json_card["bikename"] = Json::Value(str_bikename);	
							mysql_row = mysql_fetch_row(base_station_result); //��ȡÿ�е�����

							for(int card_f2 =0;card_f2<card_line;card_f2++) //ѭ����
							{	
								//if (fields[f2].name!=NULL)  
								if (!strcmp( card_files[card_f2].name , "card_state")) //�жϵ�ǰ�е��ֶ�����
								{										
									mbase_json_card[card_files[card_f2].name] = Json::Value( getNullStr(mysql_row[card_f2]) );
				
									getsucess = true ;
						
								}
								else  if(!strcmp( card_files[card_f2].name , "card_lock"))
								{
									int  lockset = *mysql_row[card_f2] - '0';
									mbase_json_card[card_files[card_f2].name] = Json::Value(lockset );
				
								}
								else  if(!strcmp( card_files[card_f2].name , "base_station"))
								{
									//mbase_json_card[card_files[card_f2].name]  = GetGpsDataFromGPSItem("","",getNullStr(mysql_row[card_f2]));									
									mbase_json_card[card_files[card_f2].name] = Json::Value( getNullStr(mysql_row[card_f2]) );				
								}
								else if (!strcmp( card_files[card_f2].name , "time"))
								{
									mbase_json_card[card_files[card_f2].name] = Json::Value( getNullStr(mysql_row[card_f2]) );
				
								}

								//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������
	
							}//endfor ��

							mysql_free_result(base_station_result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����	
							//mjson_cardstate["data"] = Json::Value( mbase_json_card );
							mjson_cardstate["data"].append( Json::Value(mbase_json_card) );//�����
						
						}//endif

					}//endif
					#endif

				}//endif ���ų��Ⱥͳ�������
			}//endfor ��
			mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����
			
		}
		else
		{
			mysql_close(&myCont);
			mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

			cout<<"��ǰ�û�û�а��豸��"<<endl;
				
			int len=0;
			char *xml;
			mstr=part1;
			mstr+=ToUserName;
			mstr+=part2;
			mstr+=FromUserName;
			mstr+=part3;
			mstr+="1492853809";
			mstr+=part4;
			mstr+="text";
			mstr+=part5;
			mstr+=GetUTF8FromGBK("����û�а��豸������豸��ʹ��,���԰󶨶��!\r\n");
			//mstr+="<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa70b2a8a84b770fb&redirect_uri=http://gps.shaohaoqi.cn/zhuce.html&response_type=code&scope=snsapi_base&state=suoding123#wechat_redirect\">";
			mstr+="<a href=\"http://wxweb.shaohaoqi.cn/bind.html?token=" + str_token +"\">";
			mstr += GetUTF8FromGBK("�����");//����
			mstr += "</a>";
			mstr+=part6;
			len=mstr.length();//������Ϣ�峤��Content-Length  Ҫ�ͺ�����Ϣ���ֽ����ϸ�һ��
	
			char content_header[100];
			sprintf_s(content_header,"Content-Length: %d\r\n", len);
			strcat_s(send_str, content_header);
			strcat_s(send_str, "Connection: close\r\n");
			strcat_s(send_str, "Pragma: no-cache\r\n");
			strcat_s(send_str, "\r\n");
			xml = (char*)mstr.data();
			strcat_s(send_str, xml);//ע��http��Ӧĩβ����Ϣ�壬����\r\nҲ��2���ֽڵ���Ϣ�壬��Ҫ�෢�ֽ�
	
			if (send(ClientS, send_str, strlen(send_str),0) == -1)
			{
				cout<<"send failed"<<endl;
				closesocket(ClientS);
				return -101;
			}		
			return -6;
		}

	}
	mysql_close(&myCont);//��ʱ�ر�sql���ӣ�����ռ��mysql������
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	if(getsucess == true )
	{
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(mjson_cardstate); //jsonתstring
		return  mjson_cardstate;
		//send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
	}
	else  if(getsucess == false)
	{
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("get_state error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring
		//send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		cout << "get CARD state error!\n" << endl;
		return -4;
	}
///////////////////////////////////////////////////////////////////////	
	
    mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	return -1;

}
///////////////////////΢�����������豸/////////////////////////////////////////////////
int  WX_SetAlarmLock(string card  , int  alarm_lock , string openid )
{
	bool getsucess =false;
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
	struct tm  ;  
	string   tos ,str_token,str_card ,str_gps, str_username;  
	string  str_lock;
	string   radius;

	unsigned int port = 3306;        
        
    MYSQL myCont;
    MYSQL_RES *result;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1,f2,num_row,num_col; 
    int res;

	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	
	str_card = card;
	if(alarm_lock)
		str_lock = "1";
	else
		str_lock = "0";

	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
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
        cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

		return -2;
    }
	
	str_token = openid;
	string  m_strname = "SELECT  username  FROM  weixin_user  WHERE token = '" + str_token + "'";
	//cout<<m_strname<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strname.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{			
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������

		for(f1=0;f1<num_row;f1++) //ѭ����
		{		
			mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

			for(f2=0;f2<num_col;f2++) //ѭ����
			{	
				if (fields[f2].name!=NULL)  
				if (!strcmp( fields[f2].name , "username")) //�жϵ�ǰ�е��ֶ�����
				{
					str_username = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL
					//cout<<str_username.data()<<endl;
					getsucess = true;
					break;
					//cout<<mstr_pwd.data()<<endl;
				}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������
			
			}
		}

		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����
		
	}
	if(getsucess == true)
	{
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

		cout << "select username error!\n" << endl;
		return -4;
	}
	str_card = card;
	string  mSQLStr = "SELECT  *  FROM  user_bike  WHERE username = '" + str_username +"'";
	
	//cout<< mSQLStr <<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{			
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if(num_row>0)
		{
			for(f1=0;f1<num_row;f1++) //ѭ����
			{		
				mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����
				
				for(f2=0;f2<num_col;f2++) //ѭ����
				{	
					if(!strcmp(fields[f2].name , "card"))
					{
						if( 0 == card.compare(  getNullStr(mysql_row[f2])) )					
							getsucess = true;
					}
		
				}//endfor ��
			}//endfor ��
			mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����
			
		}
		else
		{
			mysql_close(&myCont);
			mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

			return -6;
		}

	}//sqlִ�гɹ�

	if(getsucess == true )
	{
		
	}
	else  if(getsucess == false)
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

		cout << "check user card not exist !\n" << endl;
		return -4;
	}
	str_card = card;
///////////////////////////////////////////////////////////////////////
	//INSERT ������һ��
	//on duplicate key update  ��Ҫ����һ�����������Զ�����������������ͻ��ִ�к����updata����
	//�����card����Ϊ����
	mSQLStr = "INSERT  INTO  set_card_alarm( card_id , card, card_socket,allow_alarm,radius,time)   VALUES( 12345, '"
		+ str_card + "',888 , '"  + str_lock + "'  ,200  , NOW(3)) on duplicate key update  card = '" 
		+ str_card + "' , allow_alarm = '"+ str_lock + "' , time = NOW(3) " ;  

	//cout<<mSQLStr<<endl;
	
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���
	mysql_close(&myCont);//�ر�sql����
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	if(!res  )
	{					
		GetLocalTime( &sys ); 
		printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
		return 0;  		
	}
	else
	{	
		cout << "update set_card_alarm error" <<endl;
		return -3;
	}

    mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	return -1;

}
///////////////////////΢�����������������/////////////////////////////////////////////////
int  WX_SetTableMotorLock(string card, int  alarm_lock, string openid)
{
	bool getsucess = false;
	const char user[] = "root";
	const char pswd[] = "123456";
	const char host[] = "localhost";
	char table[] = "bike";
	char bike_username[32] = "";
	char bike_userpwd[32] = "";
	char bike_token[66] = "";
	int  bike_app_socket = 0;
	int  bike_setlock = 0;
	int  bike_update_card = 0;
	char bike_card[32] = "";
	int  bike_card_socket = 1;
	char bike_card_state = 'I';
	int  bike_card_lock = 0;
	char bike_bike_name[64] = "";
	struct tm;
	string   tos, str_token, str_card, str_gps, str_username;
	string  str_lock;
	string   radius;

	unsigned int port = 3306;

	MYSQL myCont;
	MYSQL_RES *result;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1, f2, num_row, num_col;
	int res;

	SYSTEMTIME sys;
	GetLocalTime(&sys);
	printf("%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);

	str_card = card;
	if (alarm_lock)
		str_lock = "1";
	else
		str_lock = "0";
	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
	mysql_init(&myCont);

	if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
	{
		//mysql_query(&myCont, "SET NAMES utf8"); //���ñ����ʽ
	}
	else
	{
		cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

		return -2;
	}

	str_token = openid;
	string  m_strname = "SELECT  username  FROM  weixin_user  WHERE token = '" + str_token + "'";
	//cout<<m_strname<<endl;
	getsucess = false;
	res = mysql_query(&myCont, (const  char *)m_strname.c_str()); //ִ��SQL���,ͨ��token����username
	if (!res)
	{
		//�����ѯ�������ݵ�result
		result = mysql_store_result(&myCont);
		num_row = mysql_num_rows(result); //��ȡ����
		num_col = mysql_num_fields(result); //��ȡ����
											//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
		MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������

		for (f1 = 0; f1<num_row; f1++) //ѭ����
		{
			mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

			for (f2 = 0; f2<num_col; f2++) //ѭ����
			{
				if (fields[f2].name != NULL)
					if (!strcmp(fields[f2].name, "username")) //�жϵ�ǰ�е��ֶ�����
					{
						str_username = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL
																  //cout<<str_username.data()<<endl;
						getsucess = true;
						break;
						//cout<<mstr_pwd.data()<<endl;
					}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������

			}
		}

		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

	}
	if (getsucess == true)
	{
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

		cout << "select username error!\n" << endl;
		return -4;
	}
	str_card = card;
	string  mSQLStr = "SELECT  *  FROM  user_bike  WHERE username = '" + str_username + "'";

	//cout<< mSQLStr <<endl;
	getsucess = false;
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���,ͨ��token����username
	if (!res)
	{
		//�����ѯ�������ݵ�result
		result = mysql_store_result(&myCont);
		num_row = mysql_num_rows(result); //��ȡ����
		num_col = mysql_num_fields(result); //��ȡ����
											//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
		MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if (num_row>0)
		{
			for (f1 = 0; f1<num_row; f1++) //ѭ����
			{
				mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

				for (f2 = 0; f2<num_col; f2++) //ѭ����
				{
					if (!strcmp(fields[f2].name, "card"))
					{
						if (0 == card.compare(getNullStr(mysql_row[f2])))
							getsucess = true;
					}

				}//endfor ��
			}//endfor ��
			mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

		}
		else
		{
			mysql_close(&myCont);
			mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

			return -6;
		}

	}//sqlִ�гɹ�

	if (getsucess == true)
	{

	}
	else  if (getsucess == false)
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

		cout << "check user card not exist !\n" << endl;
		return -4;
	}
	str_card = card;
	///////////////////////////////////////////////////////////////////////
	//INSERT ������һ��
	//on duplicate key update  ��Ҫ����һ�����������Զ�����������������ͻ��ִ�к����updata����
	//�����card����Ϊ����
	mSQLStr = "INSERT  INTO  set_motor_lock( card_id , card, card_socket,motor_lock,radius,time)   VALUES( 12345, '"
		+ str_card + "',888 , '" + str_lock + "'  ,200  , NOW(3)) on duplicate key update  card = '"
		+ str_card + "' , motor_lock = '" + str_lock + "' , time = NOW(3) ";

	//cout<<mSQLStr<<endl;

	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���
	mysql_close(&myCont);//�ر�sql����
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	if (!res)
	{
		GetLocalTime(&sys);
		printf("%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);
		return 0;
	}
	else
	{
		cout << "update set_card_alarm error" << endl;
		return -3;
	}

	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	return -1;

}

///////////////////////΢�����ÿ�����/////////////////////////////////////////////////
int  WX_SetTableDeviceOpen(string card, int  alarm_lock, string openid)
{
	bool getsucess = false;
	const char user[] = "root";
	const char pswd[] = "123456";
	const char host[] = "localhost";
	char table[] = "bike";
	char bike_username[32] = "";
	char bike_userpwd[32] = "";
	char bike_token[66] = "";
	int  bike_app_socket = 0;
	int  bike_setlock = 0;
	int  bike_update_card = 0;
	char bike_card[32] = "";
	int  bike_card_socket = 1;
	char bike_card_state = 'I';
	int  bike_card_lock = 0;
	char bike_bike_name[64] = "";
	struct tm;
	string   tos, str_token, str_card, str_gps, str_username;
	string  str_lock;
	string   radius;

	unsigned int port = 3306;

	MYSQL myCont;
	MYSQL_RES *result;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1, f2, num_row, num_col;
	int res;

	SYSTEMTIME sys;
	GetLocalTime(&sys);
	printf("%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);

	str_card = card;
	if (alarm_lock)
		str_lock = "1";
	else
		str_lock = "0";

	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
	mysql_init(&myCont);

	if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
	{
		//mysql_query(&myCont, "SET NAMES utf8"); //���ñ����ʽ
	}
	else
	{
		cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

		return -2;
	}

	str_token = openid;
	string  m_strname = "SELECT  username  FROM  weixin_user  WHERE token = '" + str_token + "'";
	//cout<<m_strname<<endl;
	getsucess = false;
	res = mysql_query(&myCont, (const  char *)m_strname.c_str()); //ִ��SQL���,ͨ��token����username
	if (!res)
	{
		//�����ѯ�������ݵ�result
		result = mysql_store_result(&myCont);
		num_row = mysql_num_rows(result); //��ȡ����
		num_col = mysql_num_fields(result); //��ȡ����
											//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
		MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������

		for (f1 = 0; f1<num_row; f1++) //ѭ����
		{
			mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

			for (f2 = 0; f2<num_col; f2++) //ѭ����
			{
				if (fields[f2].name != NULL)
					if (!strcmp(fields[f2].name, "username")) //�жϵ�ǰ�е��ֶ�����
					{
						str_username = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL
																  //cout<<str_username.data()<<endl;
						getsucess = true;
						break;
						//cout<<mstr_pwd.data()<<endl;
					}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������

			}
		}

		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

	}
	if (getsucess == true)
	{
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		cout << "select username error!\n" << endl;
		return -4;
	}
	str_card = card;
	string  mSQLStr = "SELECT  *  FROM  user_bike  WHERE username = '" + str_username + "'";

	//cout<< mSQLStr <<endl;
	getsucess = false;
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���,ͨ��token����username
	if (!res)
	{
		//�����ѯ�������ݵ�result
		result = mysql_store_result(&myCont);
		num_row = mysql_num_rows(result); //��ȡ����
		num_col = mysql_num_fields(result); //��ȡ����
											//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
		MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if (num_row>0)
		{
			for (f1 = 0; f1<num_row; f1++) //ѭ����
			{
				mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

				for (f2 = 0; f2<num_col; f2++) //ѭ����
				{
					if (!strcmp(fields[f2].name, "card"))
					{
						if (0 == card.compare(getNullStr(mysql_row[f2])))
							getsucess = true;
					}

				}//endfor ��
			}//endfor ��
			mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

		}
		else
		{
			mysql_close(&myCont);
			mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
			return -6;
		}

	}//sqlִ�гɹ�

	if (getsucess == true)
	{

	}
	else  if (getsucess == false)
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		cout << "check user card not exist !\n" << endl;
		return -4;
	}
	str_card = card;
	///////////////////////////////////////////////////////////////////////
	//INSERT ������һ��
	//on duplicate key update  ��Ҫ����һ�����������Զ�����������������ͻ��ִ�к����updata����
	//�����card����Ϊ����
	mSQLStr = "INSERT  INTO  set_device_open( card_id , card, card_socket,device_open,radius,time)   VALUES( 12345, '"
		+ str_card + "',888 , '" + str_lock + "'  ,200  , NOW(3)) on duplicate key update  card = '"
		+ str_card + "' , device_open = '" + str_lock + "' , time = NOW(3) ";

	//cout<<mSQLStr<<endl;

	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���
	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��//�ر�sql����
	if (!res)
	{
		GetLocalTime(&sys);
		printf("%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);
		return 0;
	}
	else
	{
		cout << "update set_card_alarm error" << endl;
		return -3;
	}

	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
	return -1;

}
///////////////////////��ȡGPS����ת�ȸ�ʽ���string///////////////////////////////////
string  GetGpsDataFromGPSItem(string  weilan_radius , string  weilan_gps  , string  gps)
{
	double  weilan_lon , weilan_lat , gps_lon, gps_lat;
	string  str_weilan_lon , str_weilan_lat, str_gps_lon , str_gps_lat;
	int  pos= -1;
	int  edpos = -1;
	int len=0;
	int sub_len=0;
	double  alarm_radius = atof(weilan_radius.c_str());
	//double  distance;
/*************
	38.01348959752763,114.47717175856118

	,A,3800.3858,N,11427.8776,E,10.05
***************/

	edpos = weilan_gps.find(",");
	if(edpos>=0)
	{
		len = edpos-0;
		
		str_weilan_lat =  weilan_gps.substr(0,len);
		weilan_lat = atof(str_weilan_lat.c_str());
		
		len = weilan_gps.length() - edpos -1;		
		str_weilan_lon = weilan_gps.substr(edpos+1,len);			
		weilan_lon = atof(str_weilan_lon.c_str());
	}		
		
//////////////////////////����Ӳ��GPS����////////////////////////////////////////////////	
	pos = gps.find("A,");
	
	if(pos>=0)
	{
		edpos = gps.find(",N,");
		if(edpos>=0)
		{
			sub_len = strlen("A,");
			len = edpos-pos-sub_len;
			
			str_gps_lat =  gps.substr(pos+sub_len,len);
			gps_lat = atof(str_gps_lat.c_str());
			
			sub_len = strlen(",N,");
		
			str_gps_lon = gps.substr(edpos+sub_len);			
			gps_lon = atof(str_gps_lon.c_str());
			gps_lon = GPSdufen_to_du(gps_lon);//��γ��ת���ɶ�Ϊ��λ
			gps_lat = GPSdufen_to_du(gps_lat);//��γ��ת���ɶ�Ϊ��λ
			//cout << "WEILAN_GPS_LON:"<< weilan_lon << endl;
			//cout << "WEILAN_GPS_LAT:"<<weilan_lat << endl;
			string str_gps = std::to_string((long double)gps_lon) +","+ std::to_string((long double)gps_lat);//doubleתstring
			return str_gps;
		}		
		
	}
	return  "";
	
}
///////////////////////ʱ��ת��stringתtime_t////////////////////////////////////////////////
time_t convert_string_to_time_t(const std::string & time_string)  
{  
    struct tm tm1;  
    time_t time1;  
    int i = sscanf(time_string.c_str(), "%d-%d-%d %d:%d:%d" ,       
                &(tm1.tm_year),   
                &(tm1.tm_mon),   
                &(tm1.tm_mday),  
                &(tm1.tm_hour),  
                &(tm1.tm_min),  
                &(tm1.tm_sec),  
                &(tm1.tm_wday),  
                &(tm1.tm_yday));  
             
    tm1.tm_year -= 1900;  
    tm1.tm_mon --;  
    tm1.tm_isdst=-1;  
    time1 = mktime(&tm1);  
  
    return time1;  
}  
/////////////////////////////�Ƚ�ʱ���С///////////////////////////////////////////
int DateCompare(const char* time1,const char* time2)
{
    int year1,month1,day1,hour1,min1,sec1;
    int year2,month2,day2,hour2,min2,sec2;
    sscanf(time1,"%d-%d-%d %d:%d:%d",&year1,&month1,&day1,&hour1,&min1,&sec1);
    sscanf(time2,"%d-%d-%d %d:%d:%d",&year2,&month2,&day2,&hour2,&min2,&sec2);
    int tm1 = year1*10000+month1*100+day1;
    int tm2 = year2*10000+month2*100+day2;
    if(tm1!=tm2) return (tm1>tm2)?1:0;//�����ȣ��󷵻�1��С����0
    tm1 = hour1*3600+min1*60+sec1;
    tm2 = hour2*3600+min2*60+sec2;//��ʱ����ת��Ϊ����
    if(tm1!=tm2) return (tm1>tm2)?1:0;//�����ȣ��󷵻�1��С����0
    return 2;//�������Ȼ�������
}
///////////////////////gbkתutf8///////////////////////////////////////////
string  GetUTF8FromGBK(string gbk_src)
{
	string  mutf8_str = gbk_src;
	int  nRetLen = GBKToUTF8((unsigned char *) mutf8_str.data(),NULL,NULL);
	//printf("ת������ַ�����Ҫ�Ŀռ䳤��Ϊ��%d ",nRetLen);			
	char *lpUTF8Str = new char[nRetLen + 1];
	nRetLen = GBKToUTF8((unsigned char *)mutf8_str.data(),(unsigned char *)lpUTF8Str,nRetLen);
	if(nRetLen)
	{
		//printf_s("GBKToUTF8ת���ɹ���\n");
		mutf8_str = lpUTF8Str;
		delete  []lpUTF8Str;
		return mutf8_str;		
	}
	else
	{
		printf("GBKToUTF8ת��ʧ�ܣ�");
		delete  []lpUTF8Str;
		return "";				
	}
}
//GBK����ת����UTF8����
int GBKToUTF8(unsigned char * lpGBKStr,unsigned char * lpUTF8Str,int nUTF8StrLen)
{
    wchar_t * lpUnicodeStr = NULL;
    int nRetLen = 0;

    if(!lpGBKStr)  //���GBK�ַ���ΪNULL������˳�
        return 0;

    nRetLen = ::MultiByteToWideChar(CP_ACP,0,(char *)lpGBKStr,-1,NULL,NULL);  //��ȡת����Unicode���������Ҫ���ַ��ռ䳤��
    lpUnicodeStr = new WCHAR[nRetLen + 1];  //ΪUnicode�ַ����ռ�
    nRetLen = ::MultiByteToWideChar(CP_ACP,0,(char *)lpGBKStr,-1,lpUnicodeStr,nRetLen);  //ת����Unicode����
    if(!nRetLen)  //ת��ʧ��������˳�
        return 0;

    nRetLen = ::WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,NULL,0,NULL,NULL);  //��ȡת����UTF8���������Ҫ���ַ��ռ䳤��
    
    if(!lpUTF8Str)  //���������Ϊ���򷵻�ת������Ҫ�Ŀռ��С
    {
        if(lpUnicodeStr)       
			delete []lpUnicodeStr;
        return nRetLen;
    }
    
    if(nUTF8StrLen < nRetLen)  //���������������Ȳ������˳�
    {
        if(lpUnicodeStr)
            delete []lpUnicodeStr;
        return 0;
    }

    nRetLen = ::WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,(char *)lpUTF8Str,nUTF8StrLen,NULL,NULL);  //ת����UTF8����
    
    if(lpUnicodeStr)
        delete []lpUnicodeStr;
    
    return nRetLen;
}
/////////////////////////////////////////////////////////////////
string UTF8ToGBK(const char* strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}
//////////////////////////////////////////////////////////////////////
CTestTask::CTestTask(int id)
	:CTask(id)
{

}
CTestTask::~CTestTask(void)
{
}
////////////////////Ӳ���������ݽ��///////////////////////////////
void CTestTask::taskRecClientProc()
{
	RecClientData(m_SocReUnit.SocketNum ,  m_SocReUnit.RecData ,m_SocReUnit.DataLen);
}

////////////////////����APP���ݽ��///////////////////////////////
void CTestTask::taskAPPRecProc()
{
	RecAPPData(mAPP_RevUnit.SocketNum ,  mAPP_RevUnit.RecData ,mAPP_RevUnit.DataLen);
}
///////////////////Ӳ�����������ݵĴ洢///////////////////////////
void CTestTask::taskSaveSQL()
{
	HardData    *pmdata;

	EnterCriticalSection(&mSaveDataLock);//���洢���ݼ���

	if(!m_SaveQueueSQL.isEmpty())//�ж϶��зǿ�
	{
		HardData  *pd = m_SaveQueueSQL.pop();//������Ķ������ݳ���һ��
		if(pd==NULL)
		{
			LeaveCriticalSection(&mSaveDataLock);//����
			std::cout<<"���ݶ��г���ʧ��!!!"<<std::endl;
			
		}else
		{
			//memcpy(mdata.RecData , pd->RecData ,  pd->DataLen);
			//mdata.DataLen =  pd->DataLen;	
			//mdata.cmd =  pd->cmd;
			//mdata.SocketNum =  pd->SocketNum;
			pmdata = pd;
			
			LeaveCriticalSection(&mSaveDataLock);//����
			//SaveDataSQL(pd->SocketNum, pd->cmd, pd->RecData, pd->DataLen);//�ɼ����ݰ��е�ʵ���û�������Ϣ
			JieXiWeiXin(pd->SocketNum, pd->cmd, pd->RecData, pd->DataLen);
			
			
		}
	}
	else
	{
		LeaveCriticalSection(&mSaveDataLock);//����
	}
	

}
/************************���ݱȽ�*****************************************/
unsigned  char    DataS_Compare(unsigned  char * p1  ,unsigned  char  *p2, unsigned  short  len)
{
 	unsigned 	short i=0;
	for(i=0;i<len;i++)
	{
		if(*(p1++) == *(p2++) )
		{

		}
		else
		return 0 ; //�Ƚϲ�ͬΪ0
	 	
	}

	return  1 ;
}
/**************************�����ƶ��������ֽ�*************************************/
unsigned  char  * FindByteBuff(unsigned  char  *src , unsigned  long srclen ,  unsigned char *dec , unsigned  short datalen )
{
	unsigned  char *pt = dec;
	unsigned	char *ps = src;
	unsigned	char *pd = dec;
	int c = 0;

	while( (unsigned  long )(ps-src) < srclen )
	{
			if(*ps == *pd) //���ҵ��׸���ͬ
			{
					
					while(  (datalen >c) && (srclen-(ps-src)) >0 )	//���Ų���ʣ��ĸ���
					{
							if(*(ps++) == *(pd++) )
							{
								c++;
							}
							else //����ͬ����;����
							{
								c=0;
								break;
							}
					}
			}

			else	//����ͬ�������Ʋ���
			{
					ps++;
			}

			if( c == datalen)	 //������ҵ���β���ﵽ��������ͬ�ĸ���
			{						
				return (unsigned  char *)(ps - c);  //�������ַ�λ��
			}
			
			if( (ps -src)== srclen )
			return 0;  //����ʧ��
			
			c = 0;	 //
			pd = pt; //
			
	}

	return 0;
	
}

/////////////////////////////����������Ӳ������ѹ����У�����Ӵ洢�����������////////////////////////////////////////////////
char  SaveClientData(SOCKET   ClientS ,int cmd , unsigned  char * src, unsigned  int  len)
{
	HardData  mdata , *pmdata ;

	EnterCriticalSection(&mSaveDataLock);//���洢���ݶ��в��������������ȼ���
	
	memcpy(mdata.RecData , src , len);
	mdata.DataLen = len;	
	mdata.cmd = cmd;
	mdata.SocketNum = ClientS;
	pmdata= &mdata;
	
	//��������ѹ����洢����
	m_SaveQueueSQL.push(pmdata);//���洢��������ӣ�ȫ�ֹ��ñ���Ҫ������������Ϊ�ú����ڶ��߳��е�����
	//��������������ʵ���Ǵ�����Ҫִ�е��������������һ�Σ������һ�Ρ�ʵ������û�д洢����ȫ�ֱ���						
	SaveDatathreadpool.addTask(pTask,NORMAL);//��������̳߳��в�ִ�����񣬻ỽ�ѹ�����̣߳��Ӷ�ִ��������ľ������
	//saveCNT++;
	//printf("������ӳɹ� %d\n",saveCNT);//
	LeaveCriticalSection(&mSaveDataLock);//����

	return 1;
	
}
/***********************************************************************
char   RecClientData
���ս���Ӳ���ϴ�����
************************************************************************/
char   RecClientData(SOCKET ClientS, unsigned char * src, unsigned  int  len )
{
	unsigned  char decdata[6]={ 0x96,0x69,0x96,0x69};
	char  loop=1;
	//unsigned  char *pstr;
	unsigned  char *srcdat=src;
	unsigned  char *pdata = src;
	long  DataLen=0;
	long  length=0;

	SocketRecUnit   mRECdataS; //���ݻ���ṹ�壬����socket����

	EnterCriticalSection(&m_RECdataLock);//Ӳ������̼߳���������ȫ�ֱ���
	memcpy( mRECdataS.RecData , src  ,len );
	mRECdataS.DataLen = len;
	
	srcdat=mRECdataS.RecData;
	pdata = srcdat;
	DataLen = mRECdataS.DataLen;
	LeaveCriticalSection(&m_RECdataLock);//����

	if(DataLen >0 && DataLen < REC_SIZE)
	{	
		SaveClientData( ClientS , *(src+0) ,src, len );//������������
		return  1;
	}
	else
	{
		
			return 0;
	}
	
}
//////////////////////////////////�����Ӳ�����ݴ洢���ݿ⺯��////////////////////////////////////////////
char  SaveDataSQL(SOCKET   ClientS ,int Command, unsigned char * src, unsigned  int  len)
{
	//JieXiWeiXin(ClientS ,Command , src, len );
	return 0;

}
//////////////////////////////////////////////////////////////////////////////
int  SaveHardStateInfo(SOCKET   ClientS , Json::Value  mJsonValue)
{
	//time_t now_time; 
	bool  getsucess =false;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;   
	
	MYSQL myCont;
    MYSQL_RES *result;
    int res;
	my_ulonglong  num_row,num_col; 

	string   str_soc,str_username ,str_card,str_state,str_lock,str_gps,str_time; 

	if( GetVaule_CheckStr(&str_card , mJsonValue , "card") == 0 )
	{
		cout<< "json�������ʹ���cardӦ��Ϊstring"<<endl;
		return  -5;
	}
	
	str_soc = to_string((long)ClientS);//socket���Ӻ�,����תstring
	
	if( GetVaule_CheckStr(&str_state , mJsonValue , "card_state") == 0 )
	{
		cout<< "json�������ʹ���card_stateӦ��Ϊstring"<<endl;
		return  -5;
	}

	if(mJsonValue["card_lock"].isInt())
		str_lock = mJsonValue["card_lock"].asInt() + '0';
	else
	{		
		cout<< "json����value���ʹ���card_lockӦ��Ϊint"<<endl;
		return  -5;

	}

	if( GetVaule_CheckStr(&str_gps , mJsonValue , "gps") == 0 )
	{
		cout<< "json�������ʹ���gpsӦ��Ϊstring"<<endl;
		return  -5;
	}

	string  m_strToken = "SELECT  card  FROM  cardinfo  WHERE card = '" + str_card + "'";
	
	//now_time = time(NULL); 
	//cout<<now_time<<endl; 

	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//��ʼ��mysql//��ʼ��mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {
        //mysql_query(&myCont, "SET NAMES utf8"); //���ñ����ʽ       
    }
    else
    {
        cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		return -2;
    }
///////////////////////////////////////////////////////////////////////////////////
	//cout<<m_strToken<<endl;
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{			
		
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if(num_row >0 )
		{
			getsucess =true;
	
		}
		else
			getsucess = false;

		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

	}
	if(getsucess == true)
	{

	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		cout << "select username error!\n" << endl;
		return -4;
	}
	
///////////////////////////////////////////////////////////////////////	
	//INSERT ������һ��
	//on duplicate key update  ��Ҫ����һ�����������Զ�����������������ͻ��ִ�к����updata����
	string  mSQLStr = "INSERT  INTO  card_data(  card , card_socket, card_state , card_lock , gps ,time )   VALUES( '" +
		str_card + "'," + str_soc + ",'"+ str_state + "',"+ str_lock + ",'"+ str_gps+ "', NOW() ) ";

	//UPDATE��������,�Ǹ���,�м����͸��Ǽ�����
	
	//string  mSQLStr="UPDATE  user_bike  SET  username = '" + str_username + "', card = '" + str_card + "', phone = '" + str_phone +"', bikename = '" + str_bikename +
	//	"', bikecolor = '" + str_bikecolor + "', biketype = '"+str_biketype + "', register_time ='2017-5-10-10:02:05' ";

	//cout<<mSQLStr<<endl;

	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���,���һ����¼
	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��//��ʱ�ر�mysql���ӣ�����ռ��������
	if(!res  )
	{	
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("sucess"); // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��	
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring
			
		send(ClientS , (char *)str.data(), (int)str.length() , 0);  // ������Ϣ 
		//_sleep(500);
		//GetLocalTime( &sys ); 
		//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 

		//cout << "///////sucess!\n" << endl;
		return 0;  		
	}
	else
	{
	
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("update_error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring

		send(ClientS , (char *)str.data(),(int)str.length(), 0);  // ������Ϣ 
		cout << "add  SQL error!\n" << endl;
		return -3;
		
	}
	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
	
	return -1;

}
////////////////////////////����GPS����////////////////////////////////////////
int   SaveGPSData(SOCKET   ClientS ,  unsigned  char * src ,unsigned  int  len)
{
	//time_t now_time; 
	int i=0;
	bool  getsucess =false;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;   

	MYSQL myCont;
    MYSQL_RES *result;
    int res;
	int  num_row,num_col; 

	string   str_soc,str_username ,str_card,str_state,str_lock,str_gps,str_time; 

	char  temp[200];
	for(i=0;i<4;i++)
		sprintf(&temp[2*i], "%02x", *(src+2+i)); //Сд16 ���ƣ����ռ2��λ�ã��Ҷ��룬���㲹0
	
	temp[8]='\0';
	//str_card = getNullStr(temp);
	str_card = temp;
	//if( *(src+2+3)< 50 )
	//printf("%s\n",str_card);//��ӡ����

	//EnterCriticalSection(&card_list_Lock);//���洢���ݼ���

	//card_list  +=str_card;
	//card_list += "\r\n";
	//LeaveCriticalSection(&card_list_Lock);//����
	//send(ClientS , (char *)(src+2), 8, 0);  // ������Ϣ 
	//return 0;
	
	str_soc = to_string((long)ClientS);//socket���Ӻ�,����תstring
	
	if('O'==*(src+6))
	str_state="Out";
	else
	str_state="In";

	if(1 ==*(src+7))
	str_lock= "1";
	else
	str_lock= "0";
	if(len-8 > 200)
		len=200;
	memcpy(temp,src+8,len-8);
	temp[len-8]='\0';
	str_gps = getNullStr(temp);

	string  m_strToken = "SELECT  card  FROM  cardinfo  WHERE card = '" + str_card + "'";

//	SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//��ʼ��mysql//��ʼ��mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {
		//mysql_query(&myCont, "SET NAMES utf8"); //���ñ����ʽ       
    }
    else
    {
        cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		return -2;
    }
///////////////////////////////////////////////////////////////////////////////////
	//cout<<m_strToken<<endl;
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{				
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if(num_row >0 )
		{
			getsucess =true;
		}
		else
			getsucess = false;

		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

	}
	if(getsucess == true)
	{

	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		cout << "GPS---��ѯcardʧ��!\n" << endl;
		return -4;
	}
	
///////////////////////////////////////////////////////////////////////	
	//INSERT ������һ��
	//on duplicate key update  ��Ҫ����һ�����������Զ�����������������ͻ��ִ�к����updata����
	string  mSQLStr = "INSERT  INTO  card_data(  card , card_socket, card_state , card_lock , gps ,time )   VALUES( '" +
		str_card + "'," + str_soc + ",'"+ str_state + "',"+ str_lock + ",'"+ str_gps+ "', NOW() ) ";

	//UPDATE��������,�Ǹ���,�м����͸��Ǽ�����

	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���,���һ����¼
	
	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��//��ʱ�ر�mysql���ӣ�����ռ��������
	if(!res  )
	{		
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("sucess"); // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��	
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring			
		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 	

		//GetLocalTime( &sys ); 
		//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
		cout << "SaveGPS-----sucess!\n" << endl;
		return 0;  		
	}
	else
	{
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("update_error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		cout << "add GPS SQL error!\n" << endl;
		return -3;
		
	}
	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
	
	return -1;

}
////////////////////////////�����վ����////////////////////////////////////////
int   SaveBaseStationData(SOCKET   ClientS ,  unsigned  char * src ,unsigned  int  len)
{
	//time_t now_time; 
	int i=0;
	bool  getsucess =false;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;   

	MYSQL myCont;
    MYSQL_RES *result;
    int res;
	int  num_row,num_col; 

	string   str_soc,str_username ,str_card,str_state,str_lock,str_base_station,str_time; 

	char  temp[200];
	for(i=0;i<4;i++)
		sprintf(&temp[2*i], "%02x", *(src+2+i)); //Сд16 ���ƣ����ռ2��λ�ã��Ҷ��룬���㲹0
	
	temp[8]='\0';
	//str_card = getNullStr(temp);
	str_card = temp;
	//if( *(src+2+3)< 50 )
	//printf("%s\n",str_card);//��ӡ����

	//EnterCriticalSection(&card_list_Lock);//���洢���ݼ���

	//card_list  +=str_card;
	//card_list += "\r\n";
	//LeaveCriticalSection(&card_list_Lock);//����
	//send(ClientS , (char *)(src+2), 8, 0);  // ������Ϣ 
	//return 0;
	
	str_soc = to_string((long)ClientS);//socket���Ӻ�,����תstring
	
	if('O'==*(src+6))
	str_state="Out";
	else
	str_state="In";

	if(1 ==*(src+7))
	str_lock= "1";
	else
	str_lock= "0";
	if(len-8 > 200)
		len=200;
	memcpy(temp,src+8,len-8);
	temp[len-8]='\0';
	str_base_station = getNullStr(temp);

	string  m_strToken = "SELECT  card  FROM  cardinfo  WHERE card = '" + str_card + "'";

//	SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
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
        cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		return -2;
    }
///////////////////////////////////////////////////////////////////////////////////
	//cout<<m_strToken<<endl;
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{				
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if(num_row >0 )
		{
			getsucess =true;
		}
		else
			getsucess = false;

		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

	}
	if(getsucess == true)
	{

	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		cout << "��վ��λ---��ѯcardʧ��!\n" << endl;
		return -4;
	}
	
///////////////////////////////////////////////////////////////////////	
	//INSERT ������һ��
	//on duplicate key update  ��Ҫ����һ�����������Զ�����������������ͻ��ִ�к����updata����
	string  mSQLStr = "INSERT  INTO  card_base_station_data(  card , card_socket, card_state , card_lock , base_station ,time )   VALUES( '" +
		str_card + "'," + str_soc + ",'"+ str_state + "',"+ str_lock + ",'"+ str_base_station+ "', NOW() ) ";

	//UPDATE��������,�Ǹ���,�м����͸��Ǽ�����

	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���,���һ����¼
	
	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��//��ʱ�ر�mysql���ӣ�����ռ��������
	if(!res  )
	{		
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("sucess"); // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��	
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring			
		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 	

		//GetLocalTime( &sys ); 
		//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
		cout << "SaveBaseStation-----sucess!\n" << endl;
		return 0;  		
	}
	else
	{
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("update_error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring

		//send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		cout << "Insert ��վ���� SQL error!\n" << endl;
		return -3;
		
	}
	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
	
	return -1;

}

////////////////////////////��֤�û����////////////////////////////////////////
int   WX_check_userID(string  openid)
{
	bool  getsucess =false;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;   
	
	MYSQL myCont;
    MYSQL_RES *result;
    int res;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1,f2,num_row,num_col; 
	string   str_token,str_username ;  
	str_token = openid;

	//string  mstr_ziduan = mJsonValue["token"].asString();
	string  m_strname = "SELECT  username  FROM  weixin_user  WHERE token = '" + str_token + "'";
	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//��ʼ��mysql//��ʼ��mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {    
    }
    else
    {
        cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		return -2;
    }

	//cout<<m_strname<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strname.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{			
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������

		for(f1=0;f1<num_row;f1++) //ѭ����
		{		
			mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

			for(f2=0;f2<num_col;f2++) //ѭ����
			{	
				if (fields[f2].name!=NULL)  
				if (!strcmp( fields[f2].name , "username")) //�жϵ�ǰ�е��ֶ�����
				{
					str_username = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL
					//cout<<str_username.data()<<endl;
					getsucess = true;
					break;
					//cout<<mstr_pwd.data()<<endl;
				}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������
			
			}
		}

		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����
		
	}
	if(getsucess == true)
	{
		return 0;
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		cout << "select username error!\n" << endl;
		return -4;
	}
	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
	return -1;

}

//////////////////////���ҵȴ��趨���豸�������͸�Ӳ��//////////////////////////////////////////
int   WX_Send_SetCardAlarm(SOCKET ClientS , string  DevCard)
{
	bool getsucess =false;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";  
	unsigned int port = 3306;   
    char table[] = "bike";    
	char bike_username[32]="";
	char bike_userpwd[32]="";
	char bike_token[66]="";
	int  bike_app_socket=0;
	int  bike_setlock=0;
	int  bike_update_card =0;
	
	int  bike_card_socket =1;
	char bike_card_state='I';
	int  bike_card_lock=0;
	char bike_bike_name[64]="";
	char bike_gps[200]=",A,3158.4608,N,11848.3737,E,10.05";
	struct tm  ;  

	string   tos ,str_token,str_card ,str_gps, str_username;  
	string   str_lock;
	string   radius;
	string   weilan_gps;
	string   weilan_radius;
	string   allow_alarm;
	string  m_strToken = "SELECT  *  FROM  set_card_alarm  WHERE card = '" + DevCard + "' ORDER BY card_id ASC LIMIT 1 ";
	            
    MYSQL myCont;
    MYSQL_RES *result;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1,f2,num_row,num_col; 
    int res;

//	SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//��ʼ��mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {  
    }
    else
    {
        cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		return -2;
    }
	//cout<<m_strToken<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{			
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if(num_row > 0)
		{
			for(f1=0;f1<1;f1++) //ѭ����ֻȡ�����µ�һ��
			{		
				mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

				for(f2=0;f2<num_col;f2++) //ѭ����
				{	
					if (fields[f2].name!=NULL)  
					{						
						if (!strcmp( fields[f2].name , "allow_alarm")) //�жϵ�ǰ�е��ֶ�����
						{
							allow_alarm = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL			
							getsucess =true;							
						}
					}					
					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������			
				}
			}
		}
		
		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

	}
	
	if(getsucess ==true)
	{

	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		cout << "select alarm_weilan error!\n" << endl;
		return -4;
	}

	if(allow_alarm.compare("1"))
		send(ClientS , "bufang123", strlen("bufang123") , 0);  // ������Ϣ
	else
		send(ClientS , "chefang123", strlen("chefang123") , 0);  // ������Ϣ

	m_strToken = "DELETE    FROM  set_card_alarm   WHERE card = '" + DevCard + "' ORDER BY time ASC LIMIT 1 ";
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{			
	}
	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	return -1;

}

/*****************΢����ҳpost���ݽ���*************************

***************************************************************/
int  WX_PostDataHandle(SOCKET sClient,char *rec_buf , unsigned  long  rec_len )
{	
	char *ps ,*pd, *pend;
	char *revData = rec_buf;
    int result = 0;
	char file_name[HTTP_FILENAME_LEN+10];//�ļ�����
	char ref_context[HTTP_REF_LEN+5];//�������
	char suffix[50]="";//�ļ���׺
	int  data_len =0;
	int  cr_len=0;
	string  str_len;
	memset(file_name ,0 ,sizeof(file_name));//һ��Ҫ����ջ���
	memset(ref_context ,0 ,sizeof(ref_context));//һ��Ҫ����ջ���
	//return 0;
    /* �õ��ļ����ͺ�׺ */
    http_parse_request_cmd(rec_buf, rec_len, file_name, suffix , ref_context);
	cout<<"������ļ�����"<<file_name<<endl;
	cout<<"���������"<<ref_context<<endl;
	
	if(strlen(file_name) > 1)
	{		
		if( ps = strstr(rec_buf , "Content-Length:"))
		{
			if( pd = strstr(ps , "\r\n"))
			{
				cr_len = strlen( "Content-Length:");
				str_len.insert(0,ps+cr_len , pd-ps-cr_len );
				data_len = atoi(str_len.c_str());
				cout<<"POST���ݳ��ȣ� "<<data_len<<endl;
				pend = strstr(pd , "\r\n\r\n");
				if(pend)
				{					
					WX_ResponsePostDataHandle(sClient , pend+4 ,data_len , file_name , ref_context);						
					return  0;
				}
			}
		}
	}	
	
	WX_ResponsePostDataHandle(sClient, rec_buf, rec_len, file_name, ref_context);
    return 0;
	
}
/*****************POST����ֵ����*************************

***************************************************************/
string  GetPostVauleFromKey(char * key  , char *buf )
{
	char *ps,*pd;
	int len =0 ;
	string value="";
	ps = strstr(buf , key);
	if(ps)
	{
		if(pd = strstr(ps , "&"))
		{
			len = pd-ps-strlen(key);
			if(len>0)
			{
				value.insert(0 , ps+strlen(key) , len );
				return value;
			}
		}
		else
		{
			len = buf+strlen(buf)-ps-strlen(key);
			if(len >0)
			{
				value.insert(0 , ps+strlen(key) , len );
				return value;
			}
		}
	}
	return value;
}
/*****************΢�Ŵ���ظ���ҳpost����*************************
***************************************************************/
int  WX_ResponsePostDataHandle(SOCKET sClient,char *p_data , unsigned  long  p_len , char *file_name ,char* ref_context)
{
	//cout<<p_data<<endl;
	if(0==strcmp(file_name ,"motorlock.html"))
	{
		WX_SetMotorLock(sClient , ref_context , 100);
		
	}
	if (0 == strcmp(file_name, "deviceopen.html"))
	{
		WX_SetDeviceOpen(sClient, ref_context, 100);

	}
	else  if (0 == strcmp(file_name, "suoding.html"))
	{

		WX_GetUserOpenID(sClient, p_data, p_len);

	}
	else  if(0==strcmp(file_name ,"login.html"))
	{
		string  account = GetPostVauleFromKey("userfiled=" , p_data );
		string  pwd = GetPostVauleFromKey("pwdfiled=" , p_data );
		string  telnum = GetPostVauleFromKey("telfiled=" , p_data );
		if(account.length()>0 && pwd.length()>0)
			http_response_webfile(sClient , "chenggong.html");// ��ͻ��˷�����Ӧ���� 
	}
	else  if(0==strcmp(file_name ,"zhuce.html"))
	{		
		int ret = WX_ResponseWebRegister( sClient , p_data ,p_len , ref_context );	
		if(0 == ret)
		{
			http_response_webfile(sClient , "zhuce_success.html");// ��ͻ��˷�����Ӧ���� 
		}
		
	}
	else  if(0==strcmp(file_name ,"forget.html"))
	{		
		int ret = WX_ResponseWebForgetRegister( sClient , p_data ,p_len , ref_context );	
		if(0 == ret)
		{
			http_response_webfile(sClient , "forget_success.html");// ��ͻ��˷�����Ӧ���� 
		}

		//WX_AddCustom("xiaolan@gh_f161a8e27ff9" , "С��");
	}
	else  if(0==strcmp(file_name ,"bind.html"))
	{
		cout<<"����bind.html"<<endl;
		string  account = GetPostVauleFromKey("userfiled=" , p_data );
		string  pwd = GetPostVauleFromKey("pwdfiled=" , p_data );
		string  telnum = GetPostVauleFromKey("telfiled=" , p_data );
		//cout<<account<<endl;
		//cout<<pwd<<endl;
		//cout<<telnum<<endl;
		if(account.length()>0 && pwd.length()>0)
		{
			int ret = WX_ResponseWebBind( sClient , p_data ,p_len , ref_context );	
			if(0 == ret)
			{
				http_response_webfile(sClient , "bind_success.html");// ��ͻ��˷�����Ӧ���� 
			}
			
		}
	}
	else  if(0==strcmp(file_name ,"gaimi.html"))
	{
		string  account = GetPostVauleFromKey("userfiled=" , p_data );
		string  pwd = GetPostVauleFromKey("pwdfiled=" , p_data );
		string  telnum = GetPostVauleFromKey("telfiled=" , p_data );
		cout<<account<<endl;
		cout<<pwd<<endl;
		cout<<telnum<<endl;
		if(account.length()>0 && pwd.length()>0)
			http_response_webfile(sClient , "gaimi_success.html");// ��ͻ��˷�����Ӧ���� 
	}
	else  if(0==strcmp(file_name ,"help.html"))
	{
		
	}
	else  if(0==strcmp(file_name ,"buy.html"))
	{
		
	}
	else if(0==strcmp(file_name ,"favicon.ico"))
	{
		http_response_webfile(sClient , "favicon.ico");
	}
	return -1;
}
/////////////////////΢����ҳ��Ȩ��code��ȡ�û�OPENID֮��������������////////////////////////////////////////////
int WX_SetMotorLock(SOCKET sClient, char *buf, unsigned  long buf_len)//
{
	char *ps ,*pd, *pend,t;
	char *revData = buf;
	char MsgContent[3*1024];
	int ref_len=0;
	char ref_buf[HTTP_BUF_SIZE+5];
	WSADATA wsa_data;
    SOCKET  http_sock = 0;         /* socket ��� */
    struct sockaddr_in serv_addr;  /* ��������ַ */
   // struct hostent *host_ent;
    char  user_openid[200];
    int result = 0, send_len;
    char data_buf[HTTP_BUF_SIZE+5];
    char rev_buf[HTTP_BUF_SIZE+5];
    unsigned short port = HTTP_DEF_PORT; //NGINX�������������8088�˿�תhttps
    //unsigned long addr;
    string  card_name ;
	WSAStartup(MAKEWORD(2,0), &wsa_data); /* ��ʼ�� WinSock ��Դ */  
    
    /* ��������ַ */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//����nginx�������

    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* ���� socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));//����socket
    if (result == SOCKET_ERROR) /* ����ʧ�� */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
		WSACleanup();
        return -1; 
    }
	cout<<"����gps.shaohaoqi.cn"<<endl;
	string  wx_code = GetPostVauleFromKey("code=" , buf );
	string  wx_state = GetPostVauleFromKey("state=" , buf );
	string  lock_set = GetPostVauleFromKey("SET" , ( char *)wx_state.c_str() );
	ps = (char*)wx_state.c_str();
	pd = strstr(ps , "SET");
	if (pd)
	{
		card_name.insert(0 ,ps , pd-ps);
	}
	cout << "΢��code��  " << wx_code.c_str() << endl;
	cout << "������豸����  " << wx_state.c_str() << endl;
	if(wx_code.length()>0 && wx_state.length() > 0 && lock_set.length()>0)
	{
		/* �����������ҳ��ȨHTTP ���� */
			memset(data_buf , 0, sizeof(data_buf));
			memset(ref_buf, 0, sizeof(ref_buf));
			ref_len = sprintf(ref_buf, wx_web_get_user_openid, WX_ID, WX_SECRET, wx_code.c_str());
			send_len = sprintf(data_buf, http_req_hdr_tmpl, ref_buf, WX_API_URL);//ƴ��http����
			result = send(http_sock, data_buf, send_len, 0);//����http������GET 
			cout<<"��΢�ŷ�����ҳ��Ȩ�������Ϣ��:\r\n"<<data_buf<<endl;
			if (result == SOCKET_ERROR) /* ����ʧ�� */
			{
				printf("[Web] fail to send, error = %d\n", WSAGetLastError());
				closesocket(http_sock);  
				WSACleanup();
				return -1; 
			}

			do /* ������Ӧ�����浽�ļ��� */
			{
				result = recv(http_sock, rev_buf, HTTP_BUF_SIZE, 0);
				if (result > 0)
				{
					/* ����Ļ����� */
					rev_buf[result] = 0;
					printf("΢�Żظ���ҳ��Ȩ%s\r\n", rev_buf);
					if(ps = strstr(rev_buf , "\"openid\":\""))
					{
						ref_len = strlen("\"openid\":\"");
						if(pd = strstr(ps + ref_len , "\",\""))
						{
							if(pd-ps-ref_len < 10 || pd-ps-ref_len > 80)
							{
								closesocket(http_sock);  
								WSACleanup();
								return - 30;
							}
							memcpy(user_openid , ps+ref_len , pd-ps-ref_len);
							user_openid[ pd-ps-ref_len]=0;
							cout<<"�õ����û�openid�� "<<user_openid<<endl;								
								
							if(0 == lock_set.compare("motoropen"))
							{
								if( 0 == WX_SetTableMotorLock(card_name  , 1 , user_openid) )
								{
									http_response_webfile(sClient , "MotorLock.html");// ��ͻ��˷�����Ӧ���� 
								}
								else
								{
									http_response_webfile(sClient , "gongji.html");
								}
							}
							else   if(0 == lock_set.compare("motorclose"))
							{
								if( 0 == WX_SetTableMotorLock(card_name  , 0 , user_openid) )
								{
									http_response_webfile(sClient , "MotorUnLock.html");// ��ͻ��˷�����Ӧ���� 
								}	
								else
								{
									http_response_webfile(sClient , "gongji.html");
								}
							}
							closesocket(http_sock);  
							WSACleanup();
							return 0;
						}
						
					}
					else
					{
						http_response_webfile(sClient , "wxshibai.html");							
					}
				}

			} while(result > 0);		
	}
	
    closesocket(http_sock);  
	WSACleanup();

    return 0;

}

/////////////////////΢����ҳ��Ȩ��code��ȡ�û�OPENID֮��������������////////////////////////////////////////////
int WX_SetDeviceOpen(SOCKET sClient, char *buf, unsigned  long buf_len)//
{
	char *ps, *pd, *pend, t;
	char *revData = buf;
	char MsgContent[3 * 1024];
	int ref_len = 0;
	char ref_buf[HTTP_BUF_SIZE + 5];
	WSADATA wsa_data;
	SOCKET  http_sock = 0;         /* socket ��� */
	struct sockaddr_in serv_addr;  /* ��������ַ */
								   // struct hostent *host_ent;
	char  user_openid[200];
	int result = 0, send_len;
	char data_buf[HTTP_BUF_SIZE + 5];
	char rev_buf[HTTP_BUF_SIZE + 5];
	unsigned short port = HTTP_DEF_PORT; //NGINX�������������8088�˿�תhttps
										 //unsigned long addr;
	string  card_name;
	WSAStartup(MAKEWORD(2, 0), &wsa_data); /* ��ʼ�� WinSock ��Դ */

										   /* ��������ַ */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//����nginx�������

	http_sock = socket(AF_INET, SOCK_STREAM, 0); /* ���� socket */
	result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));//����socket
	if (result == SOCKET_ERROR) /* ����ʧ�� */
	{
		closesocket(http_sock);
		printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	cout << "����gps.shaohaoqi.cn" << endl;
	string  wx_code = GetPostVauleFromKey("code=", buf);
	string  wx_state = GetPostVauleFromKey("state=", buf);
	string  lock_set = GetPostVauleFromKey("SET", (char *)wx_state.c_str());
	ps = (char*)wx_state.c_str();
	pd = strstr(ps, "SET");
	if (pd)
	{
		card_name.insert(0, ps, pd - ps);
	}
	cout << "΢��code��  " << wx_code.c_str() << endl;
	cout << "������豸����  " << wx_state.c_str() << endl;
	if (wx_code.length()>0 && wx_state.length() > 0 && lock_set.length()>0)
	{
		/* �����������ҳ��ȨHTTP ���� */
		memset(data_buf, 0, sizeof(data_buf));
		memset(ref_buf, 0, sizeof(ref_buf));
		ref_len = sprintf(ref_buf, wx_web_get_user_openid, WX_ID, WX_SECRET, wx_code.c_str());
		send_len = sprintf(data_buf, http_req_hdr_tmpl, ref_buf, WX_API_URL);//ƴ��http����
		result = send(http_sock, data_buf, send_len, 0);//����http������GET 
		cout << "��΢�ŷ�����ҳ��Ȩ�������Ϣ��:\r\n" << data_buf << endl;
		if (result == SOCKET_ERROR) /* ����ʧ�� */
		{
			printf("[Web] fail to send, error = %d\n", WSAGetLastError());
			closesocket(http_sock);
			WSACleanup();
			return -1;
		}

		do /* ������Ӧ�����浽�ļ��� */
		{
			result = recv(http_sock, rev_buf, HTTP_BUF_SIZE, 0);
			if (result > 0)
			{
				/* ����Ļ����� */
				rev_buf[result] = 0;
				printf("΢�Żظ���ҳ��Ȩ%s\r\n", rev_buf);
				if (ps = strstr(rev_buf, "\"openid\":\""))
				{
					ref_len = strlen("\"openid\":\"");
					if (pd = strstr(ps + ref_len, "\",\""))
					{
						if (pd - ps - ref_len < 10 || pd - ps - ref_len > 80)
						{
							closesocket(http_sock);
							WSACleanup();
							return -30;
						}
						memcpy(user_openid, ps + ref_len, pd - ps - ref_len);
						user_openid[pd - ps - ref_len] = 0;
						cout << "�õ����û�openid�� " << user_openid << endl;

						if (0 == lock_set.compare("deviceclose"))
						{
							if (0 == WX_SetTableDeviceOpen(card_name, 0, user_openid))
							{
								http_response_webfile(sClient, "PowClose.html");// ��ͻ��˷�����Ӧ���� 
							}
							else
							{
								http_response_webfile(sClient, "gongji.html");
							}
						}
						else   if (0 == lock_set.compare("deviceopen"))
						{
							if (0 == WX_SetTableDeviceOpen(card_name, 1, user_openid))
							{
								http_response_webfile(sClient, "PowOpen.html");// ��ͻ��˷�����Ӧ���� 
							}
							else
							{
								http_response_webfile(sClient, "gongji.html");
							}
						}
						closesocket(http_sock);
						WSACleanup();
						return 0;
					}

				}
				else
				{
					http_response_webfile(sClient, "wxshibai.html");
				}
			}

		} while (result > 0);
	}

	closesocket(http_sock);
	WSACleanup();

	return 0;

}
////////////////////////////////////�ظ��û�ע��///////////////////////////////////////////////////
int  WX_ResponseWebRegister(SOCKET sClient,char *p_data , unsigned  long  p_len ,char* ref_context)
{
	bool  getsucess=false;

	Json::Value  m_json;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;        

    MYSQL   myCont;  //mysql ����
    MYSQL_RES  *result ;  //�洢���
   // MYSQL_ROW  mysql_row; //������
	my_ulonglong  num_row,num_col; 
	string  mstr_pwd=""; //�洢�û�����
    int res;

	string  m_card ;
	string  m_username = GetPostVauleFromKey("userfiled=" , p_data );
	string  m_userpwd = GetPostVauleFromKey("pwdfiled=" , p_data );
	string  m_phone = GetPostVauleFromKey("telfiled=" , p_data );
	//string  m_serialnumber = GetPostVauleFromKey("devicefiled=" , p_data );
	string  m_token = GetPostVauleFromKey("token=" , p_data );
	if(m_username.length()>0 && m_userpwd.length()>0&&m_token.length()>0)
	{

	}
	else
		return -10;

	urldecode((char*)m_username.c_str());//���ı���ת��%BFת����0XBF�洢
	urldecode((char*)m_userpwd.c_str());
	urldecode((char*)m_token.c_str());

	cout<<"�û�����"<<m_username.c_str()<<endl;
	cout<<"���룺"<<m_userpwd.c_str()<<endl;
	cout<<"΢��openid��"<<m_token.c_str()<<endl;
	m_username = GetUTF8FromGBK(m_username);
	m_userpwd = GetUTF8FromGBK(m_userpwd);
	m_token = GetUTF8FromGBK(m_token);
	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
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
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		return -2;
    }

	string  m_checkuser = "SELECT  *  FROM  weixin_user  WHERE username = '" + m_username + "'";
	cout<< m_checkuser<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{			
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if(num_row > 0 )
		{
			getsucess = false;
		}
		else
		{
			getsucess = true;
		}

		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����
		
	}
	if(getsucess == true)
	{
		
	}
	else
	{	
		return -4;
	}

	string  m_reguser = "INSERT  INTO  weixin_user ( username , userpwd, token ,phone , register_time )   VALUES( '" +
		m_username + "','" + m_userpwd + "','" + m_token + "','"+ m_phone + "', NOW(3) ) ";

	cout<<m_reguser<<endl;

	res = mysql_query(&myCont, (const  char *)m_reguser.c_str()); //ִ��SQL���,���һ����¼
	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
	if(!res  )
	{	
		/****
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��	
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("sucess"); // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��	
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring
			
		//send(sClient , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		 ****/
		cout << "//////////regester user  sucess!/////////////\n" << endl;
		return 0;  		
	}
	else
	{
		/*************
		mysql_close(&myCont);
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("regester_error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring
		****************/
		//send(sClient , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		cout << "Register User error!\n" << endl;
		return -3;
		
	}
    
	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	return -1;
}
////////////////////////////////////�����˺����븲��ע��///////////////////////////////////////////////////
int  WX_ResponseWebForgetRegister(SOCKET sClient,char *p_data , unsigned  long  p_len ,char* ref_context)
{
	bool  getsucess=false;

	Json::Value  m_json;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;        

    MYSQL   myCont;  //mysql ����
    MYSQL_RES  *result ;  //�洢���
    MYSQL_ROW  mysql_row; //������
	my_ulonglong  num_row,num_col , f1,f2;
	string  m_unique_time;
	string  mold_str_name="";//�ɵ��û��˺�
    int res;

	string  m_username = GetPostVauleFromKey("userfiled=" , p_data );
	string  m_userpwd = GetPostVauleFromKey("pwdfiled=" , p_data );
	string  m_phone = GetPostVauleFromKey("telfiled=" , p_data );
	string  m_token = GetPostVauleFromKey("token=" , p_data );
	string  m_unique_code =  GetPostVauleFromKey("unique_code_filed=" , p_data );//��֤��
	if(m_username.length()>0 && m_userpwd.length()>0&&m_token.length()>0 && m_unique_code.length()>0)
	{

	}
	else
		return -10;

	urldecode((char*)m_username.c_str());//���ı���ת��%BFת����0XBF�洢
	urldecode((char*)m_userpwd.c_str());
	urldecode((char*)m_token.c_str());

	cout<<"�û�����"<<m_username.c_str()<<endl;
	cout<<"���룺"<<m_userpwd.c_str()<<endl;
	cout<<"΢��openid��"<<m_token.c_str()<<endl;
	m_username = GetUTF8FromGBK(m_username);
	m_userpwd = GetUTF8FromGBK(m_userpwd);
	m_token = GetUTF8FromGBK(m_token);
	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
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
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		return -2;
    }

	string m_checkuser = "SELECT  *  FROM  forget_code  WHERE token = '" + m_token + "'";
	//cout<< m_checkuser<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		
		if(num_row > 0)
		{
			//getsucess = true;
			
			for(f1=0;f1<1;f1++) //ѭ����ֻȡ�����µ�һ��
			{		
				mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

				for(f2=0;f2<num_col;f2++) //ѭ����
				{	
					if (fields[f2].name!=NULL)  
					{						
						if (!strcmp( fields[f2].name , "register_time")) //�жϵ�ǰ�е��ֶ�����
						{
							m_unique_time = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL			
							cout<<"��֤������ʱ�䣺"<<m_unique_time.c_str()<<endl;
														
							time_t tm_now_time ,tm_unique;  
							tm_unique = convert_string_to_time_t(m_unique_time);//stringתʱ����
							double dec_value;//ʱ���ֵ 
							time(&tm_now_time);//��ȡ��ǰʱ��  
							dec_value = difftime(tm_now_time ,tm_unique);//����ʱ���ֵ���뼶  
							printf("%f\n",dec_value);//
							if(dec_value>0 && dec_value < 300)
							{
								cout<<"��֤����Ч"<<endl;
								getsucess =true;
							}
							else
							{
								cout<<"��֤�����"<<endl;
								getsucess =false;
							}
						}
					}						
					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������			
				}
			}
		}
		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����
	}
	
	if(getsucess == true)
	{
		
	}
	else
	{	
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		return -2;
	}
	m_checkuser = "SELECT  *  FROM  weixin_user  WHERE token = '" + m_token + "'";
	//cout<< m_checkuser<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{			
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		
		if(num_row > 0)
		{			
			for(f1=0;f1<1;f1++) //ѭ����ֻȡ�����µ�һ��
			{		
				mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

				for(f2=0;f2<num_col;f2++) //ѭ����
				{	
					if (fields[f2].name!=NULL)  
					{						
						if (!strcmp( fields[f2].name , "username")) //�жϵ�ǰ�е��ֶ�����
						{
							mold_str_name = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL			
							cout<<"���û�����"<<mold_str_name.c_str()<<endl;
							getsucess =true;							
						}
					}						
					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������			
				}
			}
		}
		else
		{
			getsucess = false;
		}

		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����
		
	}
	if(getsucess == true)
	{
		
	}
	else
	{	
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		return -3;
	}
	
		
	string  m_reguser = "UPDATE  weixin_user SET username ='" + m_username + 
		"', userpwd ='"+ m_userpwd + "',phone = '"+m_phone
		+"' , register_time = NOW(3)  WHERE token =  '"+ m_token +"'";

	//cout<<m_reguser<<endl;

	res = mysql_query(&myCont, (const  char *)m_reguser.c_str()); //ִ��SQL���,���һ����¼

	if(!res  )
	{	
		//cout << "//////////update weixin_user  sucess!///////////" << endl;
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		cout << "Register User error!\n" << endl;
		return -4;
		
	}
    string  m_up_sql = "UPDATE  user_bike SET username ='" + m_username + 
		"',phone = '"+ m_phone
		+"' , register_time = NOW(3)  WHERE username =  '"+ mold_str_name +"'";
	//cout<<m_up_sql<<endl;
	res = mysql_query(&myCont, (const  char *)m_up_sql.c_str()); //ִ��SQL���,���һ����¼
	
	if(!res  )
	{
		m_checkuser = "DELETE   FROM  forget_code  WHERE token = '" + m_token + "'";
		//cout<< m_checkuser<<endl;		
		res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //ִ��SQL���,ͨ��token����username
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��//�ر�sql����
		
		if(!res  )
		{
			return 0;
		}
		else
		{
			return -12;
		}
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		cout << "update user_bike error!\n" << endl;
		return -5;		
	}
	
	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	return -1;
}
///////////////////////////////���豸/////////////////////////////////////
int  WX_ResponseWebBind (SOCKET sClient,char *p_data , unsigned  long  p_len ,char* ref_context)
{
	bool  getsucess =false;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;   

	MYSQL myCont;
    MYSQL_RES *result;
    int res;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1,f2,num_row,num_col; 
	string   sql_userpwd;
	string   str_pwd,str_username ,str_card, str_email , str_phone,str_bikename,str_bikecolor,str_biketype;  

    str_username = GetPostVauleFromKey("userfiled=" , p_data );//ȡ��form������
    str_pwd = GetPostVauleFromKey("pwdfiled=" , p_data );
	str_email = GetPostVauleFromKey("emailfiled=", p_data);
	str_phone = GetPostVauleFromKey("telfiled=" , p_data );
	string  m_serialnumber = GetPostVauleFromKey("devicefiled=" , p_data );
	string  m_token = GetPostVauleFromKey("token=" , p_data );
	str_bikename = GetPostVauleFromKey("carnamefiled=" , p_data );
	str_bikecolor = GetPostVauleFromKey("carcolorfiled=" , p_data );
	str_biketype = GetPostVauleFromKey("carmodelfiled=" , p_data );
	if(str_username.length()>0 && str_pwd.length()>0&&m_serialnumber.length()>0&& str_email.length()>0 &&
		str_bikename.length()>0 && str_bikecolor.length() && str_biketype.length()>0)
	{

	}
	else
		return -9;
	urldecode((char *)str_bikename.c_str());//���ı���ת��%BFת����0XBF�洢,html�����ı�����
	urldecode((char *)str_bikecolor.c_str());
	urldecode((char *)str_biketype.c_str());
	
	cout<<"�������ƣ�"<<str_bikename.c_str()<<endl;
	cout<<"������ɫ��"<<str_bikecolor.c_str()<<endl;
	cout<<"����Ʒ�ƣ�"<<str_biketype.c_str()<<endl;
	str_bikename = GetUTF8FromGBK(str_bikename);
	str_bikecolor = GetUTF8FromGBK(str_bikecolor);
	str_biketype = GetUTF8FromGBK(str_biketype);

	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
	mysql_init(&myCont);//��ʼ��mysql

    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {
        //mysql_query(&myCont, "SET NAMES utf8"); //���ñ����ʽ   
    }
    else
    {
        cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		return -2;
    }

	string  mSQLStr="SELECT *  FROM  cardinfo  WHERE  serialnumber = '" + m_serialnumber + "'";
	
	cout<< mSQLStr<<endl;
	//return -4;
	///////////////////////////////////////////////////////////////////////
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���
	
	if(!res  )
	{	
        //�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);

		num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if(num_row >=1)
		{
			getsucess = true ;
			mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

			for(int f2=0;f2<num_col;f2++) //ѭ����
			{	
				if (fields[f2].name!=NULL)  
				if (!strcmp( fields[f2].name , "card")) //�жϵ�ǰ�е��ֶ�����
				{
					str_card = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL
					//cout<<mstr_pwd.data()<<endl;
				}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������
			
			}
	
		}
		else
		{
			getsucess = false;
		}
		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

	}
	if(getsucess == true)
	{
		
	}
	else
	{
		cout << "check card is not exist!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		return -4;
	}
	//string  mstr_ziduan = mJsonValue["token"].asString();

	string  m_strToken = "SELECT  userpwd  FROM  weixin_user  WHERE username = '" + str_username + "'";
	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
///////////////////////////////////////////////////////////////////////////////////
	//cout<<m_strToken<<endl;
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{			
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������

		for(f1=0;f1<num_row;f1++) //ѭ����
		{		
			mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

			for(f2=0;f2<num_col;f2++) //ѭ����
			{	
				if (fields[f2].name!=NULL)  
				if (!strcmp( fields[f2].name , "userpwd")) //�жϵ�ǰ�е��ֶ�����
				{
					sql_userpwd = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL
					//cout<<str_username.data()<<endl;
					if(0==str_pwd.compare( sql_userpwd))
					{
						getsucess = true;
						//cout<<sql_userpwd.data()<<endl;
						break;
					}
					//
				}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������
			
			}
		}

		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����
		if(getsucess == false)
		{
			mysql_close(&myCont);
			mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
			cout << "select userpwd error!\n" << endl;
			return -4;
		}
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		cout << "select userpwd error!\n" << endl;
		return -4;
	}
	
	string  m_checkuser = "SELECT  *  FROM  user_bike  WHERE card = '" + str_card + "'";
	cout<< m_checkuser<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{			
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if(num_row >=1)
		{
			getsucess = false;//���б��Ѿ����ڸ��豸
		}
		else
		{
			getsucess = true;
		}

		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����
		
	}
	if(getsucess == true)
	{
		
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		cout << "check card already existed !\n" << endl;
		return -4;
	}
///////////////////////////////////////////////////////////////////////	
	//INSERT ������һ��
	//on duplicate key update  ��Ҫ����һ�����������Զ�����������������ͻ��ִ�к����updata����
	 mSQLStr = "INSERT  INTO  user_bike(  username, card , email , phone,bikename,bikecolor,biketype,register_time)   VALUES( '"
		+ str_username + "','"+ str_card + "','"+str_email +"','" + str_phone + "','"+ str_bikename + "','"+ str_bikecolor+ "','"+ str_biketype
		+ "', NOW(3))" ;
	cout<<mSQLStr<<endl;
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���,���һ����¼
	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
	if(!res  )
	{			
		
		//GetLocalTime( &sys ); 
		//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
		cout << "//////////bind device sucess!/////////////\n" << endl;
		return 0;  		
	}
	else
	{
		cout << "bind user_bike error!\n" << endl;
		return -3;
		
	}
	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	return -1;

}
///////////////////////////��ӿͷ��˺�////////////////////////////////////////////
int  WX_AddCustom(string  kf_account , string  nickname)
{
	Json::Value  m_json;
	//char *revData = buf;
	//char MsgContent[3*1024];
	int ref_len=0;
	//char ref_buf[HTTP_BUF_SIZE+5];
	WSADATA wsa_data;
    SOCKET  http_sock = 0;         /* socket ��� */
    struct sockaddr_in serv_addr;  /* ��������ַ */
   // struct hostent *host_ent;
   // char  user_openid[200];
    int result = 0, send_len;
    char data_buf[HTTP_BUF_SIZE+5];
    char rev_buf[HTTP_BUF_SIZE+5];
    unsigned short port = HTTP_DEF_PORT; //NGINX�������������8088�˿�תhttps
    //unsigned long addr;
    string  card_name , lock_set;
	WSAStartup(MAKEWORD(2,0), &wsa_data); /* ��ʼ�� WinSock ��Դ */  

    /* ��������ַ */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//����nginx�������

    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* ���� socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));//����socket
    if (result == SOCKET_ERROR) /* ����ʧ�� */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
		WSACleanup();
        return -1; 
    }
	
	m_json["kf_account"] = kf_account;
	m_json["nickname"] = nickname;
	Json::FastWriter  fast_writer;//�鿴json���ݶ���
	string json_str = fast_writer.write(m_json); //jsonתstring
	/* �����������ҳ��ȨHTTP ���� */
	memset(data_buf , 0, sizeof(data_buf));
	
	send_len = sprintf(data_buf, http_post_req_hdr, wx_add_custom , WX_API_URL , json_str.length());//ƴ��http����
	string req_str = data_buf;
	req_str+=json_str;
	send_len+=json_str.length();
	result = send(http_sock, req_str.c_str(), send_len, 0);//����http������GET 
	cout<<"��΢�ŷ�����ӿͷ��˺�����:\r\n"<<req_str.c_str()<<endl;
	if (result == SOCKET_ERROR) /* ����ʧ�� */
	{
		printf("[Web] fail to send, error = %d\n", WSAGetLastError());
		closesocket(http_sock);  
		WSACleanup();
		return -1; 
	}

	do /* ������Ӧ�����浽�ļ��� */
	{
		result = recv(http_sock, rev_buf, HTTP_BUF_SIZE, 0);
		if (result > 0)
		{
			/* ����Ļ����� */
			rev_buf[result] = 0;
			printf("΢����ӿͷ��˺���Ӧ:%s\r\n", rev_buf);
			
		}

	} while(result > 0);
			
	
	
    closesocket(http_sock);  
	WSACleanup();

    return 0;
	
}
//////////////////////////����󶨿ͷ��˺�/////////////////////////////////////
int  WX_BindCustom(string  kf_account , string  invite_wx)
{
	
	return 0;
}
/////////////////////////΢�Ż�ȡACCESS_TOKEN//////////////////////////////////////////
int  WX_GetAccessTokenMsg()
{
	Json::Value  m_json;
	Json::Value  m_template;
	char *ps ,*pd;
	//char *revData = buf;
	char wx_token_ref[HTTP_BUF_SIZE];
	//char MsgContent[3*1024];
	int ref_len=0;
	//char ref_buf[HTTP_BUF_SIZE+5];
	WSADATA wsa_data;
    SOCKET  http_sock = 0;         /* socket ��� */
    struct sockaddr_in serv_addr;  /* ��������ַ */
   // struct hostent *host_ent;
    //char  user_openid[200];
    int result = 0, send_len;
    char data_buf[HTTP_BUF_SIZE+5];
    char rev_buf[HTTP_BUF_SIZE+5];
    unsigned short port = HTTP_DEF_PORT; //NGINX�������������8088�˿�תhttps
    //unsigned long addr;
    string  card_name , lock_set;
	WSAStartup(MAKEWORD(2,0), &wsa_data); /* ��ʼ�� WinSock ��Դ */  

    /* ��������ַ */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//����nginx�������

    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* ���� socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));//����socket
    if (result == SOCKET_ERROR) /* ����ʧ�� */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
		WSACleanup();
        return -1; 
    }
	//string  str_content="�����豸�����ˣ���ȥ�鿴�ɣ�";
	//str_content = GetUTF8FromGBK(str_content);
	
	/* �����������ҳ��ȨHTTP ���� */
	memset(data_buf , 0, sizeof(data_buf));
	memset(wx_token_ref, 0, sizeof(wx_token_ref));
	sprintf( wx_token_ref ,wx_get_accesstoken_ref , WX_ID ,WX_SECRET);//���access_token
	send_len = sprintf(data_buf, http_req_hdr_tmpl, wx_token_ref , WX_API_URL );//ƴ��http����
	
	result = send(http_sock, data_buf, send_len, 0);//����http������GET 
	
	if (result == SOCKET_ERROR) /* ����ʧ�� */
	{
		printf("[Web] fail to send, error = %d\n", WSAGetLastError());
		closesocket(http_sock);  
		WSACleanup();
		return -1; 
	}

	do /* ������Ӧ�����浽�ļ��� */
	{
		result = recv(http_sock, rev_buf, HTTP_BUF_SIZE, 0);
		if (result > 0)
		{
			/* ����Ļ����� */
			rev_buf[result] = 0;
			printf("΢��access_token��Ӧ:%s\r\n", rev_buf);
			if(ps = strstr( rev_buf , "\"access_token\":\"") )
			{
				pd=strstr(ps , "\",\"");
				if(pd)
				{
					int head_len = strlen("\"access_token\":\"");
					memcpy(WX_access_token ,ps+head_len , pd-ps-head_len );//�洢΢��ACCESS_TOKEN
				}
			}
		}

	} while(result > 0);
			
		
    closesocket(http_sock);  
	WSACleanup();

    return 0;
}
/////////////////////////΢��ģ����Ϣ����//////////////////////////////////////////
int  WX_SendAlarmTemplateMsg(string touser ,string  template_id , string url,Json::Value  mjson_content )
{
	Json::Value  m_json;
	Json::Value  m_template;
	//char *ps ,*pd, *pend,t;
	//char *revData = buf;
	char wx_latemsg_ref[1024];
	//char MsgContent[3*1024];
	int ref_len=0;
	//char ref_buf[HTTP_BUF_SIZE+5];
	WSADATA wsa_data;
    SOCKET  http_sock = 0;         /* socket ��� */
    struct sockaddr_in serv_addr;  /* ��������ַ */
    //struct hostent *host_ent;
    //char  user_openid[200];
    int result = 0, send_len;
    char data_buf[HTTP_BUF_SIZE+5];
    char rev_buf[HTTP_BUF_SIZE+5];
    unsigned short port = HTTP_DEF_PORT; //NGINX�������������8088�˿�תhttps
    //unsigned long addr;
    string  card_name , lock_set;
	WSAStartup(MAKEWORD(2,0), &wsa_data); /* ��ʼ�� WinSock ��Դ */  

    /* ��������ַ */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//����nginx�������

    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* ���� socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));//����socket
    if (result == SOCKET_ERROR) /* ����ʧ�� */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
		WSACleanup();
        return -1; 
    }
	//string  str_content="�����豸�����ˣ���ȥ�鿴�ɣ�";
	//str_content = GetUTF8FromGBK(str_content);
	m_json["touser"] = touser;
	m_json["template_id"] = template_id;
	m_json["url"] = url;
	m_json["data"]= mjson_content;
	Json::FastWriter  fast_writer;//�鿴json���ݶ���
	string json_str = fast_writer.write(m_json); //jsonתstring
	/* �����������ҳ��ȨHTTP ���� */
	memset(data_buf , 0, sizeof(data_buf));
	sprintf( wx_latemsg_ref ,wx_send_template_ref , WX_access_token);//���access_token
	send_len = sprintf(data_buf, http_post_req_hdr, wx_latemsg_ref , WX_API_URL , json_str.length());//ƴ��http����
	string req_str = data_buf;
	req_str+=json_str;
	send_len+=json_str.length();
	result = send(http_sock, req_str.c_str(), send_len, 0);//����http������GET 
	cout<<"��΢�ŷ�����ӿͷ��˺�����:\r\n"<<req_str.c_str()<<endl;
	if (result == SOCKET_ERROR) /* ����ʧ�� */
	{
		printf("[Web] fail to send, error = %d\n", WSAGetLastError());
		closesocket(http_sock);  
		WSACleanup();
		return -1; 
	}

	do /* ������Ӧ�����浽�ļ��� */
	{
		result = recv(http_sock, rev_buf, HTTP_BUF_SIZE, 0);
		if (result > 0)
		{
			/* ����Ļ����� */
			rev_buf[result] = 0;
			printf("΢����ӿͷ��˺���Ӧ:%s\r\n", rev_buf);
			closesocket(http_sock);
			WSACleanup();
			return 0;
		}

	} while(result > 0);
			
	
	
    closesocket(http_sock);  
	WSACleanup();

    return 0;
}

//////////////////////���ҵȴ�Ӳ�������б����͸�΢��//////////////////////////////////////////
int   WX_Send_CardAlarmToUser(SOCKET ClientS , string  DevCard)
{
	bool getsucess =false;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";  
	unsigned int port = 3306;   
    char table[] = "bike";    

	struct tm ;  
	Json::Value  m_wx_content;
	string   tos ,str_token,str_card , str_username , bikename;  
	string   str_lock;
	string   alarm_time;
	string   alarm_state;
	string   wx_openid;
	string   m_strToken = "SELECT  *  FROM  card_alarm  WHERE  send = 1  ORDER BY card_id ASC  ";
	            
    MYSQL myCont;
    MYSQL_RES *result;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1,f2,num_row,num_col; 
    int res;

	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//��ʼ��mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {  
    }
    else
    {
        cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		return -2;
    }
	cout<<m_strToken<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{			
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if(num_row > 0)
		{
			string str_title= GetUTF8FromGBK("�����豸������");
			Json::Value  m_json;
			m_json["value"]=str_title;
			m_json["color"]="#0000FF";
			m_wx_content["first"] = m_json;

			for(f1=0;f1<num_row;f1++) //ѭ����ֻȡ�����µ�һ��
			{		
				mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

				for(f2=0;f2<num_col;f2++) //ѭ����
				{	
					if (fields[f2].name!=NULL)  
					{	
						if (!strcmp( fields[f2].name , "card")) //�жϵ�ǰ�е��ֶ�����
						{
							str_card = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL			
							
						}
						if (!strcmp( fields[f2].name , "time")) //�жϵ�ǰ�е��ֶ�����
						{
							alarm_time = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL			
							getsucess =true;
							Json::Value  m_json;
							m_json["value"]=alarm_time;
							m_json["color"]="#0000FF";
							m_wx_content["alarm_time"] = m_json;
						}
						if (!strcmp( fields[f2].name , "card_state")) //�жϵ�ǰ�е��ֶ�����
						{
							alarm_state = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL			
							getsucess =true;
							Json::Value  m_json;
							m_json["value"]=alarm_state;
							m_json["color"]="#0000FF";
							m_wx_content["alarm_state"] = m_json;
						}
					}					
					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������			
				}//for��ѭ��
				m_strToken = "SELECT  *  FROM  user_bike  WHERE card = '" + str_card + "' ";
				cout<<m_strToken<<endl;
				res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //ִ��SQL���,ͨ��token����username
				if(!res  )
				{
					//�����ѯ�������ݵ�result
					MYSQL_RES * user_result = mysql_store_result(&myCont);
					int  num_row=mysql_num_rows(user_result); //��ȡ����
					int  num_col=mysql_num_fields(user_result); //��ȡ����
					//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
					MYSQL_FIELD* fields = mysql_fetch_fields(user_result); //���������ֶνṹ������
					if(num_row > 0)
					{
						MYSQL_ROW  userbike_row = mysql_fetch_row(user_result); //��ȡÿ�е�����
						for(int f2=0;f2<num_col;f2++) //ѭ����
						{	
							if (!strcmp( fields[f2].name , "username")) //�жϵ�ǰ�е��ֶ�����
							{
								str_username = getNullStr(userbike_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL			
							}
							if (!strcmp( fields[f2].name , "bikename")) //�жϵ�ǰ�е��ֶ�����
							{
								bikename = getNullStr(userbike_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL			
								Json::Value  m_json;
								m_json["value"]=bikename;
								m_json["color"]="#0000FF";
								m_wx_content["bikename"] = m_json;
							}

						}	
						Json::Value  m_json;
						m_json["value"]=GetUTF8FromGBK("�ⲻ����ϰ������ʵ��Ӳ��������Ϣ���ͣ�");
						m_json["color"]="#FF0000";
						m_wx_content["alarm_msg"] = m_json;
						
						m_strToken = "SELECT  token  FROM  weixin_user  WHERE username = '" + str_username + "' ";
						
						res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //ִ��SQL���,ͨ��token����username
						if(!res  )
						{
							cout<<m_strToken<<endl;
							//�����ѯ�������ݵ�result
							MYSQL_RES * token_result = mysql_store_result(&myCont);
							int  num_row=mysql_num_rows(token_result); //��ȡ����
							int  num_col=mysql_num_fields(token_result); //��ȡ����
							MYSQL_ROW  token_row = mysql_fetch_row(token_result); //��ȡÿ�е�����
						
							//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
							MYSQL_FIELD* fields = mysql_fetch_fields(token_result); //���������ֶνṹ������
							if(num_row > 0)
							{
								wx_openid = getNullStr(token_row[0]); //��ȡ�ֶ����ݣ������жϷ�NULL									
								WX_SendAlarmTemplateMsg(wx_openid , WX_TEMPLATE_ID , "http://wxweb.shaohaoqi.cn/login.html" ,m_wx_content);
							}
							mysql_free_result(token_result);
						}
					}
					mysql_free_result(user_result);
				}//��ѯuser_bike
			#if  1
				//ɾ���Ѿ����͵ı����б�
				m_strToken = "DELETE   FROM  card_alarm   WHERE card = '" + str_card + "' ORDER BY time ASC LIMIT 1 ";
				res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //ִ��SQL���,ͨ��token����username
				if(!res  )
				{		

				}
			#endif

			}//��ѭ��
		}
		
		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

	}
	
	if(getsucess ==true)
	{
	
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		cout << "select alarm_weilan error!\n" << endl;
		return -4;
	}

	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	return -1;

}

//////////////////////���ҵȴ�Ӳ�������б����ͱ����ʼ�/////////////////////////////////////////
int   Email_Send_CardAlarmToUser(SOCKET ClientS , string  DevCard)
{
	bool getsucess =false;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";  
	unsigned int port = 3306;   
    char table[] = "bike";    

	struct tm ;  
	Json::Value  m_wx_content;
	string   tos ,str_token,str_card , str_username , str_bikename;  
	string   str_lock;
	string   alarm_time;
	string   alarm_state;
	string   wx_openid;
	string   m_strToken = "SELECT  *  FROM  card_alarm  WHERE  send = 1  ORDER BY card_id ASC  ";
	            
    MYSQL myCont;
    MYSQL_RES *result;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1,f2,num_row,num_col; 
    int res;

	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//��ʼ�����ݿ�  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//��ʼ��mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {  
    }
    else
    {
        cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��
		return -2;
    }
	cout<<m_strToken<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{			
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if(num_row > 0)
		{
			string str_title= GetUTF8FromGBK("�����豸������");
			Json::Value  m_json;
			m_json["value"]=str_title;
			m_json["color"]="#0000FF";
			m_wx_content["first"] = m_json;

			for(f1=0;f1<num_row;f1++) //ѭ����ֻȡ�����µ�һ��
			{		
				mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

				for(f2=0;f2<num_col;f2++) //ѭ����
				{	
					if (fields[f2].name!=NULL)  
					{	
						if (!strcmp( fields[f2].name , "card")) //�жϵ�ǰ�е��ֶ�����
						{
							str_card = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL			
							
						}
						if (!strcmp( fields[f2].name , "time")) //�жϵ�ǰ�е��ֶ�����
						{
							alarm_time = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL			
							getsucess =true;
							Json::Value  m_json;
							m_json["value"]=alarm_time;
							m_json["color"]="#0000FF";
							m_wx_content["alarm_time"] = m_json;
						}
						if (!strcmp( fields[f2].name , "card_state")) //�жϵ�ǰ�е��ֶ�����
						{
							alarm_state = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL			
							getsucess =true;
							Json::Value  m_json;
							m_json["value"]=alarm_state;
							m_json["color"]="#0000FF";
							m_wx_content["alarm_state"] = m_json;
						}
					}					
					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������			
				}//for��ѭ��
				m_strToken = "SELECT  *  FROM  user_bike  WHERE card = '" + str_card + "' ";
				cout<<m_strToken<<endl;
				res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //ִ��SQL���,ͨ��token����username
				if(!res  )
				{
					//�����ѯ�������ݵ�result
					MYSQL_RES * user_result = mysql_store_result(&myCont);
					int  num_row=mysql_num_rows(user_result); //��ȡ����
					int  num_col=mysql_num_fields(user_result); //��ȡ����
					//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
					MYSQL_FIELD* fields = mysql_fetch_fields(user_result); //���������ֶνṹ������
					if(num_row > 0)
					{
						MYSQL_ROW  userbike_row = mysql_fetch_row(user_result); //��ȡÿ�е�����
						for(int f2=0;f2<num_col;f2++) //ѭ����
						{	
							if (!strcmp( fields[f2].name , "username")) //�жϵ�ǰ�е��ֶ�����
							{
								str_username = getNullStr(userbike_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL			
							}
							if (!strcmp( fields[f2].name , "bikename")) //�жϵ�ǰ�е��ֶ�����
							{
								str_bikename = getNullStr(userbike_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL			
								Json::Value  m_json;

								m_json["value"]= str_bikename;
								m_json["color"]="#0000FF";
								m_wx_content["bikename"] = m_json;
							}
							if (!strcmp(fields[f2].name, "email")) //�жϵ�ǰ�е��ֶ�����
							{
								m_wx_content["email"]  = getNullStr(userbike_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL			
							}

						}	
						Json::Value  m_json;
						m_json["value"]=GetUTF8FromGBK("�ⲻ����ϰ������ʵ��Ӳ��������Ϣ���ͣ�");
						m_json["color"]="#FF0000";
						m_wx_content["alarm_msg"] = m_json;

						Sleep(1000);
						
						string  str_email ;
						if (GetVaule_CheckStr(&str_email , m_wx_content, "email") == 0)
						{
							cout << "json�������ʹ���FromUserNameӦ��Ϊstring" << endl;
							return  -5;
						}
						
						Json::Value  m_json_email;
						m_json_email["bikename"] = str_bikename;
						m_json_email["state"] = alarm_state;
						m_json_email["alarm_time"] = alarm_time;
						//���ͱ����ʼ�
						Email_SMTP_send_simple(mEmailSender.smtp_server, mEmailSender.Email, mEmailSender.EmailAccount, 
							mEmailSender.EmailPwd , str_email, m_json_email);
						
						m_strToken = "SELECT  token  FROM  weixin_user  WHERE username = '" + str_username + "' ";
						
						res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //ִ��SQL���,ͨ��token����username
						if(!res  )
						{
							cout<<m_strToken<<endl;
							//�����ѯ�������ݵ�result
							MYSQL_RES * token_result = mysql_store_result(&myCont);
							int  num_row=mysql_num_rows(token_result); //��ȡ����
							int  num_col=mysql_num_fields(token_result); //��ȡ����
							MYSQL_ROW  token_row = mysql_fetch_row(token_result); //��ȡÿ�е�����
						
							//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
							MYSQL_FIELD* fields = mysql_fetch_fields(token_result); //���������ֶνṹ������
							if(num_row > 0)
							{
								wx_openid = getNullStr(token_row[0]); //��ȡ�ֶ����ݣ������жϷ�NULL									
								WX_SendAlarmTemplateMsg(wx_openid , WX_TEMPLATE_ID , "http://wxweb.shaohaoqi.cn/login.html" ,m_wx_content);
							}
							mysql_free_result(token_result);
						}
					}
					mysql_free_result(user_result);
				}//��ѯuser_bike
			#if  1
				//ɾ���Ѿ����͵ı����б�
				m_strToken = "DELETE   FROM  card_alarm   WHERE card = '" + str_card + "' ORDER BY time ASC LIMIT 1 ";
				res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //ִ��SQL���,ͨ��token����username
				if(!res  )
				{		

				}
			#endif

			}//��ѭ��
		}
		
		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

	}
	
	if(getsucess ==true)
	{
	
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

		cout << "�����б�û����Ϣ!\n" << endl;
		return -4;
	}

	mysql_close(&myCont);
	mysql_library_end();//���ǵ��� mysql_close ֮����� mysql_library_end() ���ͷ�δ���ͷŵ��ڴ�.mysql_library_initҪ���ʹ��

	return -1;

}

//////////////////////////////////////////////////////////////////


