
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
CMySaveQueue   m_SaveQueueSQL;//创建对象，实例化一个结构体队列
CTestTask *pTask =new CTestTask(10);//创建任务
int  saveCNT=0;
///////////////////////////////////////////////////////////////////////
#define   CONTENT_LEN        1024*100*1
#define   REC_LEN            1024*100*1
#define   SEND_LEN           1024*100*1
//////////////////////////////////////////////////////////////////////
char  WX_access_token[700]="7_wVJ4KaDc9lC1BCI6j6aHpNPh_naiQ1Oswp0xQQK5EACRbSBdY8N8HXrCgCTwd1as5YwetLBbOjsO1JOkGl1MXWG4qpjj7bKR-cVF4jgvGigiP2EQpmbuERykONnoY9LvxqDKSWWgaQpgNkaAHJDbAGAACW";//微信公众号access_token
///////////////////////////////////////////////////////////////////////
#define  WX_API_URL             "api.weixin.qq.com"
#define  WX_ID                  "wxa70b2a8a84b770fb"
#define  WX_SECRET              "bd54c7d4cf1fd5f89e4e09bb3e2f342e"
#define  WX_MSG_REQ_URL         "wx.shaohaoqi.cn"
#define  WX_WEB_REQ_URL         "gps.shaohaoqi.cn"
#define  WX_WEB_POST_URL        "wxwebpost.shaohaoqi.cn"
#define  WX_TEMPLATE_ID         "lnJxTDRunbYDKzT_B_8s_RyUiuDbJGb4KQQsdQM0hf0"   
///////////////////////微信公众号xml格式/////////////////////////////////////////////////
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
EMAIL_ServerInfo    mEmailSender;//服务器邮箱配置
/////////////////////////////////////////////////////////////////////////////////////
string  MakeForgetCode( int n );
//////////////////////////////////////////////////////////////////////////////
//函数声明
int EmailConfig();
//函数声明
int request(char* hostname, char* api, char* parameters);

time_t convert_string_to_time_t(const std::string & time_string);
int DateCompare(const char* time1,const char* time2);
int PassiveReplyMSG(SOCKET sClient, char *mToUserName, char *mFromUserName,char* parameters);//
int PassiveReponseForgetMSG(SOCKET sClient, char *mToUserName, char *mFromUserName,string wx_openid);
/*******************微信网页授权用code请求用户的opeid*******************************/
int  WX_GetAccessTokenMsg();
int WX_GetUserOpenID(SOCKET sClient , char *buf,  unsigned  long buf_len);
int WX_GetMsgContent(SOCKET sClient,char *rec_buf , unsigned  long  rec_len);
int ResponseNewsMsg(SOCKET sClient, char *mToUserName, char *mFromUserName,char* parameters);
char  JieXiWeiXin(SOCKET   ClientS ,int cmd ,  unsigned char * src, unsigned  long  length);
int  WX_AddCustom(string  kf_account , string  nickname);
int  WX_BindCustom(string  kf_account , string  invite_wx);
int  WX_SendAlarmTemplateMsg(string touser ,string  template_id , string url,Json::Value  mjson_content);
///////////////////////http请求//////////////////////////////////////////
void http_parse_request_url( char *buf, char *host, unsigned short *port, char *file_name);
char  http_req_send(char *dst_host ,long dst_port ,char *ref);
///////////////////////http响应/////////////////////////////////////////////////////
int  http_send_response(SOCKET soc, char *buf, unsigned long buf_len);
char *http_get_type_by_suffix(const char *suffix);
int  http_parse_request_cmd(char *buf, int buflen, char *file_name, char *suffix , char  *ref_context);
int http_response_webfile(SOCKET soc, char *file_name  );
///////////////////////////////////////////////////////////////////////////////////////
int   WX_SetAlarmLock(string card  , int  alarm_lock , string openid );//写入数据库锁定操作
int   WX_Send_CardAlarmToUser(SOCKET ClientS , string  DevCard);//发送报警信息给微信
//////////////////////数据库操作/////////////////////////////////////////////////////
int   WX_check_userID(string  openid);
string  GetGpsDataFromGPSItem(string  weilan_radius , string  weilan_gps  , string  gps);
Json::Value  WXGetDeviceData(SOCKET   ClientS ,Json::Value  mJsonValue);
int   ResponseDeviceListMsg(SOCKET sClient, char *mToUserName, char *mFromUserName, string  weixin_openID);
int   ResponseDeviceStateMsg(SOCKET sClient, char *mToUserName, char *mFromUserName,string  weixin_openID);
int ResponseMotorStateMsg(SOCKET sClient, char *mToUserName, char *mFromUserName, string weixin_openID);//
int ResponseDeviceOpenMsg(SOCKET sClient, char *mToUserName, char *mFromUserName, string weixin_openID);
Json::Value  WXGetUserBindData(SOCKET   ClientS, Json::Value  mJsonValue);
int  PassiveReponseUnbindMSG(SOCKET sClient, char *mToUserName, char *mFromUserName, string weixin_openID);
////////////////////////////////提取网页请求的post数据////////////////////////////////////////////////////
int  WX_PostDataHandle(SOCKET sClient,char *rec_buf , unsigned  long  rec_len );
int  WX_ResponsePostDataHandle(SOCKET sClient,char *p_data , unsigned  long  p_len 
								, char *file_name ,char* ref_context);
string  GetPostVauleFromKey(char * key  , char *buf );
int  WX_SetMotorLock(SOCKET sClient, char *buf, unsigned  long buf_len);//锁电机
int  WX_SetTableMotorLock(string card, int  alarm_lock, string openid);
int  WX_SetDeviceOpen(SOCKET sClient, char *buf, unsigned  long buf_len);
int  WX_SetTableDeviceOpen(string card, int  alarm_lock, string openid);
int  WX_ResponseWebRegister(SOCKET sClient,char *p_data , unsigned  long  p_len ,char* ref_context);
int  WX_ResponseWebForgetRegister(SOCKET sClient,char *p_data , unsigned  long  p_len ,char* ref_context);
int  WX_ResponseWebBind (SOCKET sClient,char *p_data , unsigned  long  p_len ,char* ref_context);
void urldecode(char *p);//url中文编码转换解码
////////////////////////////////////////////////////////////////////////////////////
int   Email_Send_CardAlarmToUser(SOCKET ClientS , string  DevCard);
/////////////////////////////////////////////////////////////////////////////////////////////
/* 定义常量 */
#define    HTTP_BUF_SIZE       1024*6     /* 缓冲区的大小 */
#define    HTTP_FILENAME_LEN   256     /* 文件名长度 */
#define    HTTP_REF_LEN        1024  /***请求参数最大长度***/
//////////////////////////////////////////////////////////////////////////////////////////////
/* 定义文件类型对应的 Content-Type */
struct doc_type
{
    char *suffix; /* 文件后缀 */
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
//////////////////////////http客户端//////////////////////////////
#define HTTP_HOST_LEN    256  /* 主机名长度 */
#define HTTP_DEF_PORT    9010   /* 连接的缺省端口 */

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


////////////////初始化服务器email配置/////////////////////////////////////////////
int EmailConfig()
{
	mEmailSender.smtp_server = "smtp.163.com";
	mEmailSender.Email = "ssou_1985@163.com";
	mEmailSender.EmailAccount = "ssou_1985@163.com";
	mEmailSender.EmailPwd = "qinaide,7758521";
	return 0;
}
////////////////////////定时查询报警表数据线程////////////////////////////////////
DWORD  WINAPI  WX_CheckAlarmThread (LPVOID lpParameter)
{  
	EmailConfig();//配置服务器email

	while(true)  
	{  
		//每次等2000毫秒   
		int nIndex = WaitForMultipleObjects(1, WX_CheckAlarmEvent, FALSE,5000);     

		if (nIndex == WAIT_OBJECT_0 + 1)   
		{  
			
		}   
		else if (nIndex == WAIT_OBJECT_0) //第一个事件发生    
		{   			
			
		}    
		else if (nIndex == WAIT_TIMEOUT) //超时    
		{   //超时可作定时用
			printf("开始查询微信报警表，推送报警模板消息!\n");//
			//WX_Send_CardAlarmToUser(0 , "");//查询报警表，发送微信报警模板消息
			Email_Send_CardAlarmToUser(0, "");//查询报警表，发送报警邮件

			//SetEvent(WX_CheckAlarmEvent[0]);//触发事件唤醒线程
		}   
	}  
	 printf("线程结束\n");  

	 return 0;
	
}

////////////////////////定时获取微信ACCESS_TOKEN线程////////////////////////////////////
DWORD  WINAPI  WX_GetTokenThread (LPVOID lpParameter)
{  
	WX_GetAccessTokenMsg();
	while(true)  
	{  
		//每次间隔20分钟  
		int nIndex = WaitForMultipleObjects(1, WX_GetTokenEvent, FALSE,60000*30);     

		if (nIndex == WAIT_OBJECT_0) //第一个事件发生    
		{   			
			printf("开始更新微信ACCESS_TOKEN!\n");//
			WX_GetAccessTokenMsg();
		}    
		else if (nIndex == WAIT_TIMEOUT) //超时    
		{   //超时可作定时用    
			SetEvent(WX_GetTokenEvent[0]);//触发事件唤醒线程
		}   
	}  
	 printf("线程结束\n");  

	 return 0;
	
}
/**************************************************
网页中文解码
%BD转换成单字节值存储
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
 * 函数功能: 解析命令行参数, 分别得到主机名, 端口号和文件名. 命令行格式:
 *           [http://www.baidu.com:8080/index.html]
 *
 * 参数说明: [IN]  buf, 字符串指针数组;
 *           [OUT] host, 保存主机;
 *           [OUT] port, 端口;
 *           [OUT] file_name, 文件名;
 *
 * 返 回 值: void.
 *
 **************************************************************************/
void http_parse_request_url( char *buf, char *host, 
                            unsigned short *port, char *file_name)
{
    int length = 0;
    char port_buf[8];
    char *buf_end = (char *)(buf + strlen(buf));
    char *begin, *host_end, *colon, *file;

    /* 查找主机的开始位置 */
    begin = strstr(buf, "//");
    begin = (begin ? begin + 2 : buf);
    
    colon = strchr(begin, ':');
    host_end = strchr(begin, '/');

    if (host_end == NULL)
    {
        host_end = buf_end;
    }
    else
    {   /* 得到文件名 */
        file = strrchr(host_end, '/');
        if (file && (file + 1) != buf_end)
            strcpy(file_name, file + 1);
    }

    if (colon) /* 得到端口号 */
    {
        colon++;

        length = host_end - colon;
        memcpy(port_buf, colon, length);
        port_buf[length] = 0;
        *port = atoi(port_buf);

        host_end = colon - 1;
    }

    /* 得到主机信息 */
    length = host_end - begin;
    memcpy(host, begin, length);
    host[length] = 0;
}
//////////////////////////////////////////////////////////////////
/**************************************************************************
 *
 * 函数功能: 发送http请求
 *           [http://www.baidu.com:8080/index.html]
 *
 * 参数说明: [IN]  dst_host, 目的服务器主机;
 *           [IN] dst_port, 端口;
 *           [IN] ref, 请求参数字符串;
 *
 * 返 回 值: char.
 *
 **************************************************************************/
char  http_req_send(char *dst_host ,long dst_port ,char *ref)
{
	WSADATA wsa_data;
    SOCKET  http_sock = 0;         /* socket 句柄 */
    struct sockaddr_in serv_addr;  /* 服务器地址 */
    struct hostent *host_ent;
    
    int result = 0, send_len;
    char data_buf[HTTP_BUF_SIZE];
    
    unsigned short port = HTTP_DEF_PORT;
    unsigned long addr;
   
    WSAStartup(MAKEWORD(2,0), &wsa_data); /* 初始化 WinSock 资源 */

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

    /* 服务器地址 */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = addr;

    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* 创建 socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (result == SOCKET_ERROR) /* 连接失败 */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
        return -1; 
    }

    /* 发送 HTTP 请求 */
    send_len = sprintf(data_buf, http_req_hdr_tmpl, ref, dst_host);//拼接http参数
    result = send(http_sock, data_buf, send_len, 0);//发送http请求体

    if (result == SOCKET_ERROR) /* 发送失败 */
    {
        printf("[Web] fail to send, error = %d\n", WSAGetLastError());
        return -1; 
    }

    do /* 接收响应并保存到文件中 */
    {
        result = recv(http_sock, data_buf, HTTP_BUF_SIZE, 0);
        if (result > 0)
        {
            /* 在屏幕上输出 */
            data_buf[result] = 0;
            printf("%s", data_buf);
        }
    } while(result > 0);

    closesocket(http_sock);
    WSACleanup();

    return 0;
	
}
//////////////////////////http客户端//////////////////////////////
/**************************************************************************
 *
 * 函数功能: 根据文件后缀查找对应的 Content-Type.
 *
 * 参数说明: [IN] suffix, 文件名后缀;
 *
 * 返 回 值: 成功返回文件对应的 Content-Type, 失败返回 NULL.
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
 * 函数功能: 向客户端发送 HTTP 响应.
 *
 * 参数说明: [IN]  buf, 字符串指针数组;
 *           [IN]  buf_len, buf 的长度;
 *
 * 返 回 值: 成功返回非0, 失败返回0.
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
	memset(file_name ,0 ,sizeof(file_name));//一定要先清空缓存
	memset(ref_context ,0 ,sizeof(ref_context));//一定要先清空缓存
    /* 得到文件名和后缀 */
    http_parse_request_cmd(buf, buf_len, file_name, suffix , ref_context);
	if(strlen(file_name) ==0)
		return  -1;
	printf("file name is %s--------------------------\n",file_name);
    res_file = fopen( file_name, "rb+"); /* 用二进制格式打开文件 */
	//工程所在目录内的文件夹文件表示方法："pic\\sr\\zhuce.html" 双斜杠实际是单个斜杠，前面不能带
    if (res_file == NULL)
    {
        printf("[Web] The file [%s] is not existed\n", file_name);
        return 0;
    }

    fseek(res_file, 0, SEEK_END);
    file_len = ftell(res_file);
    fseek(res_file, 0, SEEK_SET);
    
    type = http_get_type_by_suffix(suffix); /* 文件对应的 Content-Type */
    if (type == NULL)
    {
        printf("[Web] There is not the related content type\n");
        return 0;
    }

    /* 构造 HTTP 首部，并发送 */
    hdr_len = sprintf(http_header, http_res_hdr_tmpl, file_len, type);
    send_len = send(soc, http_header, hdr_len, 0);
    //printf("head send--------------\n%s\n", http_header);
    if (send_len == SOCKET_ERROR)
    {
        fclose(res_file);
        printf("[Web] Fail to send, error = %d\n", WSAGetLastError());
        return 0;
    }

    do /* 发送文件, HTTP 的消息体 */
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
 * 函数功能: 解析请求行, 得到文件名及其后缀. 请求行格式:
 *           [GET /ZHUCE.HTML?STATE=abc HTTP/1.1\r\n]
 *	      [GET /?STATE=abc HTTP/1.1\r\n]
*            [GET /ZHUCE.HTML HTTP/1.1\r\n]
*            [GET / HTTP/1.1\r\n]
 * 参数说明: [IN]  buf, 字符串指针数组;
 *           [IN]  buflen, buf 的长度;
 *           [OUT] file_name, 文件名;
 *           [OUT] suffix, 文件名后缀;
 *
 * 返 回 值: void.
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
    /* 查找 URL 的开始位置 */
    begin = strchr(buf, ' ');//查找第一个空格
    if(begin)
	{
		if(pend - begin < 0)
			return -11;
		begin++;//跳过第一个空格位置
		req_end = strchr(begin, ' ');//查找第二个空格
		if(req_end)
		{
			if(pend - req_end < 0)
			return -12;

			bias = strchr(begin, '/');//查找第一个斜杠/，是文件名起始位置
			if(bias)
			{
				if(req_end - bias > 1)//文件名存在
				{
					length = req_end - bias;//得到请求的长度，可能包含文件名和参数，也可能只有二者之一		
					/* 查找 URL 的结束位置 */
					end = strchr(bias, '?');
					if(end)//如果在请求内容中找到?说明后面有请求参数
					{
						if(end - bias > 1)//判断文件名是否存在，可能是/?参数内容，没有文件名
						{
							file_len = end-bias-1;//去掉了第一个斜杠/
							
							ref_len = req_end - end - 1;
							if(ref_len >= HTTP_REF_LEN )
								return -9;
							if( ref_len > 0)//确定参数在HTTP/1.1之前
							{
								cout<<"有参数存在！"<<endl;
								memcpy(ref_context , end+1 , ref_len);//取出请求参数
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
							begin = strrchr(file_name, '.');//查找最后一个点。
							if (begin)
								strcpy(suffix, begin + 1);
						}							
					}
					else //没有请求参数
					{	
						
						if(req_end - bias > 1)//判断文件名是否存在，可能是GET / HTTP/1.1\r\n
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
							begin = strrchr(file_name, '.');//查找最后一个点。
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
	
    res_file = fopen( file_name, "rb+"); /* 用二进制格式打开文件 */
	//工程所在目录内的文件夹文件表示方法："pic\\sr\\zhuce.html" 双斜杠实际是单个斜杠，前面不能带
    if (res_file == NULL)
    {
        printf("[Web] The file [%s] is not existed\n", file_name);
        return 0;
    }
	type = http_get_type_by_suffix(suffix); /* 文件对应的 Content-Type */
    if (type == NULL)
    {
        printf("[Web] There is not the related content type\n");
        return 0;
    }
    fseek(res_file, 0, SEEK_END);
    file_len = ftell(res_file);
    fseek(res_file, 0, SEEK_SET);
    /* 构造 HTTP 首部，并发送 */
    hdr_len = sprintf(http_header, http_res_hdr_tmpl, file_len, type);
    send_len = send(soc, http_header, hdr_len, 0);
    //printf("head send--------------\n%s\n", http_header);
    if (send_len == SOCKET_ERROR)
    {
        fclose(res_file);
        printf("[Web] Fail to send, error = %d\n", WSAGetLastError());
        return 0;
    }
	cout<<"发送文件"<<endl;
    do /* 发送文件, HTTP 的消息体 */
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
/////////////////////////////////解析微信post数据并响应http 1.1 20 OK.../////////////////////////////////////////////////
char  JieXiWeiXin(SOCKET   sClient ,int cmd ,  unsigned char * src, unsigned  long  length)
{
	int t;
	char *pst = NULL ,*p2= NULL,*ps= NULL , *pd= NULL;
	int ret;
	int len = 0;

	char   WebData[REC_LEN+10]; //接收缓存
	ret = length;
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	printf("开始解析:%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);

	memset(WebData , 0 , sizeof(WebData));
	//EnterCriticalSection(&mSaveDataLock);//待存储数据加锁
	//接收数据
	memcpy(WebData, src, length);

	//LeaveCriticalSection(&mSaveDataLock);//解锁

	WebData[ret] = '\0';
    printf(WebData);
	printf("\r\n");
	//printf("APP2222222222\n");
	if(ps = strstr( WebData, "\r\nHost:"))
	{	
		t=strlen("\r\nHost:");
		    
		if(strstr(ps+t,WX_MSG_REQ_URL))//判断是微信POST消息请求
		{
			printf("wx_msg_req-------------------------------\r\n");
			WX_GetMsgContent(sClient ,  WebData , length);//解析微信消息xml	
		}
		else  if(strstr(ps+t,WX_WEB_REQ_URL))//判断是微信网页授权用户信息请求
		{
			printf("wx_web_req-------------------------------\r\n");
			//WX_GetUserOpenID(sClient,WebData ,ret);//向微信服务器发获取用户openID请求		
			WX_PostDataHandle(sClient,WebData ,ret);//网页提交的POST请求
			
		}
		else  if(strstr(ps+t,WX_WEB_POST_URL))//判断是微信网页POST过来的数据请求
		{
			if(strstr( WebData, "GET"))//浏览器请求
			{			
				http_send_response(sClient, WebData, ret);// 向客户端发送响应数据 
			}
			else  if(strstr( WebData, "POST"))//浏览器发送
			{
				printf("web post data req-------------------------------\r\n");
				WX_PostDataHandle(sClient,WebData ,ret);//网页提交的POST请求
			}
						
		}	
        /////////////////////处理其他域名网页请求///////////////////////////
		else
		{			
			if(strstr( WebData, "GET"))//浏览器请求
			{			
				http_send_response(sClient, WebData, ret);// 向客户端发送响应数据 
			}
			else  if(strstr( WebData, "POST"))//浏览器发送
			{
			
			}
		/////////////////////////////////////////////////////////////////////////////
		}//end 网页请求处理
	}
	printf("关闭该socket连接！\r\n");
	closesocket(sClient);
	
	GetLocalTime(&sys);
	printf("解析回复完毕:%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);

	return  0; 

}

///////////////////////解析微信消息xml///////////////////////////////
int WX_GetMsgContent(SOCKET sClient  ,  char *rec_buf , unsigned  long  rec_len)
{
	char content_header[100];
	char  ToUserName[1024];
	char  FromUserName[1024];
	char  MsgContent[CONTENT_LEN+10];//提取内容缓存
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
	//初始化发送信息	
	memset(send_str,0,sizeof(send_str));
	//头信息
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

		pst = strstr(revData , "echostr=");//查找echo_str					
		if(pst && p2)
		{
			if(pend-pst < 0 )
				return -22;
			num=p2-pst-strlen("echostr=");
			if(num < 1 || num > sizeof(m_echostr) - 2)
				return -32;
			//查找到echo_str				
			memcpy(m_echostr,pst+8,num);//拷贝出echostr
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
				
				ps=strstr(pd,"<MsgType><![CDATA[event]]></MsgType>");//点击菜单拉取消息时的事件推送
				if(ps)
				{						
					ps=strstr(ps,"<Event><![CDATA[CLICK]]></Event>");//点击菜单拉取消息时的事件推送
					if(ps)
					{
						
						msgType=5;
						break;
					}
				}
			
				ps=strstr(pd,"<MsgType><![CDATA[event]]></MsgType>");//点击菜单跳转链接事件
				if(ps)
				{
					ps=strstr(ps,"<Event><![CDATA[VIEW]]></Event>");//点击菜单拉取消息时的事件推送
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
				//cout<<"响应消息类型"<<endl;
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
					if(strstr(MsgContent , "GPSbtn_list1_1"))//第一组菜单第一个被点击
					{
						//cout<<"event news "<<endl;							
						ResponseDeviceStateMsg(sClient, ToUserName,FromUserName,ToUserName  );//回复设备列表文字消息
						
					}
					else if(strstr(MsgContent , "GPSbtn_list1_2"))//第一组菜单第二个被点击
					{
						//cout<<"event news "<<endl;							
						ResponseMotorStateMsg(sClient, ToUserName,FromUserName,ToUserName  );//回复设备列表文字消息
						
					}
					else if (strstr(MsgContent , "GPSbtn_list1_3"))//第一组菜单第三个被点击
					{
						//cout<<"event news "<<endl;							
						ResponseDeviceOpenMsg(sClient, ToUserName,FromUserName,ToUserName  );//回复设备列表文字消息
						
					}
					else if(strstr(MsgContent , "GPSbtn_list2_1"))//第二组菜单第一个被点击
					{
						ResponseDeviceListMsg(sClient, ToUserName,FromUserName,ToUserName);//回复图文消息
					}
					else if(strstr(MsgContent , "GPSbtn_list3_1"))//第二组菜单第一个被点击
					{
						//PassiveReponseUnbindMSG(sClient, ToUserName, FromUserName, ToUserName);//回复图文消息
						PassiveReponseForgetMSG(sClient, ToUserName,FromUserName,ToUserName);//回复图文消息
					}
					else if (strstr(MsgContent, "GPSbtn_list3_2"))//第二组菜单第一个被点击
					{
						PassiveReponseUnbindMSG(sClient, ToUserName, FromUserName, ToUserName);//回复图文消息
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
					cout<<"不认识的消息"<<endl;
					len=strlen("success");
					memset(m_echostr,0,sizeof(m_echostr));
					strcat_s(m_echostr,"success" );
				
					sprintf_s(content_header,"Content-Length: %d\r\n", len);
					strcat_s(send_str, content_header);
					strcat_s(send_str, "Connection: close\r\n");
					strcat_s(send_str, "Pragma: no-cache\r\n");
					strcat_s(send_str, "\r\n");
					strcat_s(send_str, m_echostr);//消息长度和Content-Length定义要一致,否则微信回复慢
			
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
///////////////////////////////生成随机码//////////////////////////////////////////
string  MakeForgetCode( int n )
{
	string  str_code ="";
	int i,j,len;
    char pstr[] = "0123456789";
    len = strlen(pstr);         //求字符串pstr的长度
    srand(time(0));
    for (i = 0;i < n; i++) 
	{
        j = rand()%len;        //生成0~len-1的随机数
		str_code+=pstr[j];

    }
	return str_code;
}

/////////////////////////回复设备号和解绑验证码//////////////////////////////////////////////////////
int PassiveReponseUnbindMSG(SOCKET sClient, char *mToUserName, char *mFromUserName, string weixin_openID)
{
	char send_str[2048 + 100] = { 0 };
	memset(send_str, 0, sizeof(send_str));//这里是数组填充0，memset不可给指针填充，那只会填充指针所在的内存
										  //头信息
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

	MYSQL   myCont;  //mysql 连接
	MYSQL_RES  *result;  //存储结果

	int res;
	string  str_code;//验证码
	MYSQL_ROW  mysql_row; //行内容
	my_ulonglong  f1, f2, num_row, num_col;
	
	Json::Value  mjson_card , mjson_user_data , mjson_value;
	mjson_card["token"]= weixin_openID;
	mjson_card["ToUserName"] = mToUserName;
	mjson_card["FromUserName"] = mFromUserName ;
	mjson_user_data = WXGetUserBindData(sClient ,mjson_card);//从数据库获取用户数据
	if(mjson_user_data.size() <= 0 )
		return -1;
	//初始化数据库  
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
		//mysql_query(&myCont, "SET NAMES utf8"); //设置编码格式
	}
	else
	{
		cout << "connect failed!" << endl;
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		return -2;
	}
	string  mutf8_str = "设备信息:";
	mutf8_str = GetUTF8FromGBK(mutf8_str);
	string  mitem = "绑定的设备号\r\n\r\n";
	mitem = GetUTF8FromGBK(mitem);

	if (mjson_user_data["data"].isArray())//判断是json数组
    {
        int nArraySize = mjson_user_data["data"].size(); //数组条数 
		
        for (int i=0; i<nArraySize; i++)
        {       
			str_bikename = mjson_user_data["data"][i]["bikename"].asString(); //数组中某个key值 
            str_card = mjson_user_data["data"][i]["card"].asString(); //数组中某个key值 
						
			string  m_checkuser = "SELECT  serialnumber  FROM  cardinfo  WHERE card = '" + str_card + "'";
			//cout << m_checkuser << endl;
			getsucess = false;
			res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //执行SQL语句,通过token查找username
			if (!res)
			{
				//保存查询到的数据到result
				result = mysql_store_result(&myCont);
				num_row = mysql_num_rows(result); //读取行数
				mysql_row = mysql_fetch_row(result); //获取每行的内容
				if(num_row > 0)
				{
					str_SN = getNullStr(mysql_row[0]); //获取字段内容，里面判断非NULL
					getsucess = true;
					Url = to_string( (long) (i+1)) + "   <a href=\"http://wxweb.shaohaoqi.cn/unbind.html?serial=" + str_SN + "\">";
					//Url += "    " + str_SN + "</a>";
					Url +=  mutf8_str  + "    " + str_bikename + "  " + str_SN + "</a>"  ;
					mitem += Url+ "\r\n\r\n";	
					//cout << mitem << endl;
				}
				mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长
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
	str_code = MakeForgetCode(6);//生成随机码
	string  m_checkuser = "SELECT  *  FROM  unbind_code  WHERE token = '" + weixin_openID + "'";
	//cout<< m_checkuser<<endl;
	getsucess = false;
	res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //执行SQL语句,通过token查找username
	if (!res)
	{
		//保存查询到的数据到result
		result = mysql_store_result(&myCont);
		num_row = mysql_num_rows(result); //读取行数
		num_col = mysql_num_fields(result); //读取列数
											//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
		MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if (num_row > 0)
		{
			getsucess = true;
			m_checkuser = "UPDATE unbind_code SET unbind_code = '" + str_code + "',register_time = NOW()  WHERE token = '" + weixin_openID + "'";
			res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //执行SQL语句,添加一条记录
			if (!res)
			{
				//cout << "//////////update  unique_code  sucess!/////////////\n" << endl;					
			}
			else
			{
				mysql_close(&myCont);//关闭sql连接
				mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

				cout << "update  unbind_code error!\n" << endl;
				return -3;
			}
		}
		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长

	}
	if (getsucess == true)
	{

	}
	else
	{
		m_checkuser = "INSERT  INTO  unbind_code ( unbind_code , token  , register_time )   VALUES( '" +
			str_code + "','" + weixin_openID + "', NOW(3) ) ";

		//cout<<m_checkuser<<endl;

		res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //执行SQL语句,添加一条记录
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

		if (!res)
		{
			cout << "//////////regester unbind_code  sucess!///////////" << endl;
		}
		else
		{
			mysql_close(&myCont);//关闭sql连接
			mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

			cout << "Insert  unbind_code  error!" << endl;
			return -3;

		}
	}
	
	mysql_close(&myCont);//关闭sql连接
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

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
	mstr += GetUTF8FromGBK("点击需要解绑的设备，输入验证码即可解绑\r\n解绑验证码" + str_code + "。不要泄露给别人, 5分钟有效");
	mstr += part6;
	len = mstr.length();//整个消息体长度Content-Length  要和后面消息体字节数严格一致

	char content_header[100];
	sprintf_s(content_header, "Content-Length: %d\r\n", len);
	strcat_s(send_str, content_header);
	strcat_s(send_str, "Connection: close\r\n");
	strcat_s(send_str, "Pragma: no-cache\r\n");
	strcat_s(send_str, "\r\n");
	xml = (char*)mstr.c_str();
	strcat_s(send_str, xml);//注意http响应末尾是消息体，就是\r\n也算2个字节的消息体，不要多发字节
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

//////////////////////////回复忘记密码的响应消息//////////////////////////////////////////////////////
int PassiveReponseForgetMSG(SOCKET sClient, char *mToUserName, char *mFromUserName,string wx_openid)
{
	char send_str[2048+100] = {0};
    memset(send_str,0,sizeof(send_str));//这里是数组填充0，memset不可给指针填充，那只会填充指针所在的内存
    //头信息
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

    MYSQL   myCont;  //mysql 连接
    MYSQL_RES  *result ;  //存储结果
   // MYSQL_ROW  mysql_row; //行内容
	my_ulonglong  num_row,num_col; 
    int res;
	string  str_code ;//验证码
					  //初始化数据库  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//初始化mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {
        //mysql_query(&myCont, "SET NAMES utf8"); //设置编码格式
    }
    else
    {
        cout << "connect failed!" << endl;
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		
		return -2;
    }
	str_code = MakeForgetCode(6);
	string  m_checkuser = "SELECT  *  FROM  forget_code  WHERE token = '" + wx_openid + "'";
	//cout<< m_checkuser<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{			
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if(num_row > 0 )
		{
			getsucess = true;
			m_checkuser = "UPDATE forget_code SET unique_code = '" + str_code + "',register_time = NOW()  WHERE token = '"+ wx_openid + "'";
			res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //执行SQL语句,添加一条记录
			if(!res  )
			{
				//cout << "//////////update  unique_code  sucess!/////////////\n" << endl;					
			}
			else
			{
				mysql_close(&myCont);//关闭sql连接
				mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

				cout << "update  unique_code error!\n" << endl;
				return -3;
			}
		}
		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长
		
	}
	if(getsucess == true)
	{
		
	}
	else
	{			
		m_checkuser = "INSERT  INTO  forget_code ( unique_code , token  , register_time )   VALUES( '" +
		str_code + "','" + wx_openid + "', NOW(3) ) ";

		//cout<<m_checkuser<<endl;

		res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //执行SQL语句,添加一条记录
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

		if(!res  )
		{			
			cout << "//////////regester unique_code  sucess!///////////" << endl;	
		}
		else
		{
			mysql_close(&myCont);//关闭sql连接
			mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

			cout << "Register  unique_code error!" << endl;
			return -3;
		
		}
	}
	mysql_close(&myCont);//关闭sql连接
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

	mstr=part1;
	mstr+=mToUserName;
	mstr+=part2;
	mstr+=mFromUserName;
	mstr+=part3;
	mstr+="1492853809";
	mstr+=part4;
	mstr+="text";
	mstr+=part5;
	mstr+=GetUTF8FromGBK("忘记用户名或密码，点击这里重新新注册，直接替代使用，旧账号会被清除\r\n");
	mstr+="<a href=\"http://wxweb.shaohaoqi.cn/forget.html?token=" + wx_openid +"\">";
	mstr += GetUTF8FromGBK("点击重新注册,输入验证码"+ str_code +"。不要泄露给别人,5分钟有效");//内容
	mstr += "</a>";
	mstr+=part6;
	len=mstr.length();//整个消息体长度Content-Length  要和后面消息体字节数严格一致
	
	char content_header[100];
	sprintf_s(content_header,"Content-Length: %d\r\n", len);
	strcat_s(send_str, content_header);
	strcat_s(send_str, "Connection: close\r\n");
	strcat_s(send_str, "Pragma: no-cache\r\n");
	strcat_s(send_str, "\r\n");
	xml = (char*)mstr.c_str();
	strcat_s(send_str, xml);//注意http响应末尾是消息体，就是\r\n也算2个字节的消息体，不要多发字节
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
////////////////////////////////////被动回复消息///////////////////////////////////////////////
int PassiveReplyMSG(SOCKET sClient, char *mToUserName, char *mFromUserName,char* parameters)//
{
	
	char send_str[SEND_LEN+100] = {0};
    memset(send_str,0,sizeof(send_str));//这里是数组填充0，memset不可给指针填充，那只会填充指针所在的内存
    //头信息
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2018 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	string  mstr;//消息体字符串缓存
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
	mstr += parameters;//内容
	mstr += "</a>";
	mstr+=part6;
	len=mstr.length();//整个消息体长度Content-Length  要和后面消息体字节数严格一致
	
	char content_header[100];
	sprintf_s(content_header,"Content-Length: %d\r\n", len);
	strcat_s(send_str, content_header);
	strcat_s(send_str, "Connection: close\r\n");
	strcat_s(send_str, "Pragma: no-cache\r\n");
	strcat_s(send_str, "\r\n");
	xml = (char*)mstr.c_str();
	strcat_s(send_str, xml);//注意http响应末尾是消息体，就是\r\n也算2个字节的消息体，不要多发字节
	//strcat_s(send_str, "\r\n");//注意，因为多加了两个字节，导致超过content-Length，微信6秒才回复
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
/////////////////////微信网页授权用code获取用户OPENID向微信发GET请求/////////////////////////////////////////////
int WX_GetUserOpenID(SOCKET sClient, char *buf, unsigned  long buf_len)//
{
	char *ps ,*pd, *pend,t;
	char *revData = buf;
	char MsgContent[3*1024];
	int ref_len=0;
	char ref_buf[HTTP_BUF_SIZE+5];
	WSADATA wsa_data;
    SOCKET  http_sock = 0;         /* socket 句柄 */
    struct sockaddr_in serv_addr;  /* 服务器地址 */
   // struct hostent *host_ent;
    char  user_openid[200];
    int result = 0, send_len;
    char data_buf[HTTP_BUF_SIZE+5];
    char rev_buf[HTTP_BUF_SIZE+5];
    unsigned short port = HTTP_DEF_PORT; //NGINX正向代理设置了8088端口转https
    //unsigned long addr;
    string  card_name , lock_set;
	WSAStartup(MAKEWORD(2,0), &wsa_data); /* 初始化 WinSock 资源 */  
    
    /* 服务器地址 */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//本机nginx正向代理

    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* 创建 socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));//建立socket
    if (result == SOCKET_ERROR) /* 连接失败 */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
		WSACleanup();
        return -1; 
    }
	cout<<"解析gps.shaohaoqi.cn"<<endl;
	pend = strstr(buf,"HTTP/1.1\r\n");
	if(pend == NULL)
	{
		cout<<"没查询到HTTP/1.1"<<endl;
		closesocket(http_sock);  
		WSACleanup();
		return -21;
	}
	*pend='\0';//截断HTTP请求体，只查询前面的请求头
	ps=strstr(buf,"?code=");
	if(ps)
	{
		if(pend - ps < 0)//判断请求参数在HTTP/1.1 之后，说明不对
		{
			closesocket(http_sock);  
			WSACleanup();
			//cout<<"微信请求ico图标"<<endl;
			//http_send_response(sClient, buf , buf_len);// 向客户端发送响应数据 	
			return -22;
		}
		t=strlen("?code=");
		pd=strstr(ps,"&state=");//从code参数向后查，这能证明两个参数顺序正确
		if(pd)
		{
			if( (pd-ps-t) < 1024 && (pend- pd) >0)//因为上面查询证明，这里不会是负数
			{
				int content_len = pd-ps-t;
				memcpy(MsgContent,(ps+t),content_len);//获取网页授权code
				MsgContent[pd-ps-t]=0;
				printf("content len:%d\n" , content_len);
				printf("content :%s\n" , MsgContent);
				memset(ref_buf , 0,  sizeof(ref_buf));
				ref_len = sprintf(ref_buf , wx_web_get_user_openid, WX_ID , WX_SECRET, MsgContent);
	            
				ps=strstr(pd,"SET");
		        if(ps)
				{
					int  card_len = strlen("&state=");
					//memcpy(card_name.c_str() ,(pd+card_len),ps-pd-card_len);//获取网页授权code
				    card_name.insert (0,pd+card_len, ps-pd-card_len);
					//cout<< card_name.c_str()<<endl; //card参数
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
					int  card_len = strlen("SET");//查找自定义参数区分割字符串SET
					//memcpy(card_name.c_str() ,(pd+card_len),ps-pd-card_len);//获取网页授权code
				    lock_set.insert (0,ps+card_len, pd-ps-card_len);
					//cout<< lock_set.c_str()<<endl; //锁定标志参数
				}
				else
				{
					closesocket(http_sock);  
					WSACleanup();
					return -12;
				
				}	/* 向服务器发网页授权HTTP 请求 */
				memset(data_buf , 0, sizeof(data_buf));
				send_len = sprintf(data_buf, http_req_hdr_tmpl, ref_buf, WX_API_URL);//拼接http参数
				result = send(http_sock, data_buf, send_len, 0);//发送http请求体GET 
				cout<<"向微信发送网页授权请求的消息体:\r\n"<<data_buf<<endl;
				if (result == SOCKET_ERROR) /* 发送失败 */
				{
					printf("[Web] fail to send, error = %d\n", WSAGetLastError());
					closesocket(http_sock);  
					WSACleanup();
					return -1; 
				}

				do /* 接收响应并保存到文件中 */
				{
					result = recv(http_sock, rev_buf, HTTP_BUF_SIZE, 0);
					if (result > 0)
					{
						/* 在屏幕上输出 */
						rev_buf[result] = 0;
						printf("微信回复网页授权%s\r\n", rev_buf);
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
								cout<<"得到的用户openid： "<<user_openid<<endl;								
								
								if(0 == lock_set.compare("suoding"))
								{
									if( 0 == WX_SetAlarmLock(card_name  , 1 , user_openid) )
									{
										http_response_webfile(sClient , "suoding.html");// 向客户端发送响应数据 
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
										http_response_webfile(sClient , "jiesuo.html");// 向客户端发送响应数据 
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
		*pend='H';//恢复之前修改的结束符为'H'
		cout<<"微信请求ico图标"<<endl;
		http_send_response(sClient, buf , buf_len);// 向客户端发送响应数据 
		closesocket(http_sock);  
		WSACleanup();
		return 0;
	}
	
    closesocket(http_sock);  
	WSACleanup();

    return 0;

}
////////////////////////////////////回复设备列表，并带定位超链接//////////////////////////////////////////////
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
	mjson_user_data = WXGetDeviceData(sClient ,mjson_card);//从数据库获取用户数据
	if(mjson_user_data.size() <= 0 )
		return -1;
    memset(send_str,0,sizeof(send_str));
    //头信息
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2017 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	mutf8_str = "基站定位:";
	mutf8_str = GetUTF8FromGBK(mutf8_str);
	mitem = "绑定的设备列表\r\n\r\n";
	mitem = GetUTF8FromGBK(mitem);

	if (mjson_user_data["data"].isArray())//判断是json数组
    {
        int nArraySize = mjson_user_data["data"].size(); //数组条数 
		
        for (int i=0; i<nArraySize; i++)
        {       
            mDeviceName = mjson_user_data["data"][i]["bikename"].asString(); //数组中某个key值 
			
			mDevTime = mjson_user_data["data"][i]["time"].asString(); //数组中某个key值 
			if(  mjson_user_data["data"][i]["base_station"].isString() )
			{
				mbase_station =   mjson_user_data["data"][i]["base_station"].asString(); //基站定位数据 
				int  base_len = mbase_station.find("\r");
				mbase_station = mbase_station.substr(0,base_len-0);
				Url = to_string( (long) (i+1)) + "   <a href=\"http://www.gpsspg.com/bs.htm\">";
				Url +=  mDeviceName + "    " + mutf8_str + mbase_station + "</a>"  ;
				mitem += Url;
				mitem += "  " + mDevTime + "\r\n\r\n";	
			}
			else  if(  mjson_user_data["data"][i]["gps"].isString() )
			{
				mgps = mjson_user_data["data"][i]["gps"].asString(); //gps数据 
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
	mstr += mitem;//内容
	mstr+=part6;
	len=mstr.length();

	char content_header[100];
	sprintf_s(content_header,"Content-Length: %d\r\n", len);
	strcat_s(send_str, content_header);
	strcat_s(send_str, "Connection: close\r\n");
	strcat_s(send_str, "Pragma: no-cache\r\n");
	strcat_s(send_str, "\r\n");
	xml = (char*)mstr.data();
	strcat_s(send_str, xml);//注意http响应末尾是消息体，就是\r\n也算2个字节的消息体，不要多发字节
	//strcat_s(send_str, "\r\n");//注意，因为多加了两个字节，导致content-Length 小，微信6秒才回复
	
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

////////////////////////////////////回复设备状态//////////////////////////////////////////////
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
	mjson_user_data = WXGetDeviceData(sClient ,mjson_card);//从数据库获取用户数据
	if(mjson_user_data.size() <= 0 )
		return -1;
    memset(send_str,0,sizeof(send_str));
    //头信息
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2017 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	mutf8_str = "    点击定位";
	mutf8_str = GetUTF8FromGBK(mutf8_str);
	mitem = "布防操作设备列表\r\n\r\n";
	mitem = GetUTF8FromGBK(mitem);

	if (mjson_user_data["data"].isArray())//判断是json数组
    {
		int  DataSize = 0;
		int  Result = 0;
		int  SaveFlag=0;
        int nArraySize = mjson_user_data["data"].size(); //数组条数 
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
						case 1: //前者时间大						
								//mjson_data["data"][j].clear();
								mjson_data["data"][j] =  mjson_user_data["data"][i];
							break;
						case 0://后者时间大

							break;
						case 2: //时间相等
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
				mDeviceName = mjson_data["data"][i]["bikename"].asString(); //数组中某个key值 
			
			if(  mjson_data["data"][i]["card_lock"].isInt())	
				mDevLock = mjson_data["data"][i]["card_lock"].asInt();
			cout<<"mDevLock value is :  "<<mDevLock <<endl;
			if( mjson_data["data"][i]["card"].isString())		
				mDevCard = mjson_data["data"][i]["card"].asString(); //数组中某个key值 
			if (mjson_data["data"][i]["card_state"].isString())
				card_state = mjson_data["data"][i]["card_state"].asString();

			mDevState = card_state.substr(0, card_state.find_last_of(","));//保存AB路开关状态
			if (mDevLock == 1)
			{
				mDevState += ",已布防";
				mDevState = GetUTF8FromGBK(mDevState);
			}
			else
			{
				mDevState += ",已撤防";
				mDevState = GetUTF8FromGBK(mDevState);
			}
			
			mDevTime  = mjson_data["data"][i]["time"].asString();
			if( mjson_data["data"][i]["gps"].isString())
			{
				mgps = mjson_data["data"][i]["gps"].asString(); //gps经纬度数据 
				if(mDevLock)//当前是锁定状态
					Url =to_string((long) (i+1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/suoding.html&response_type=code&scope=snsapi_base&state=" + mDevCard +"SETjiesuo#wechat_redirect\">";
				else
					Url =to_string((long) (i+1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/suoding.html&response_type=code&scope=snsapi_base&state=" + mDevCard +"SETsuoding#wechat_redirect\">";
	
				//Url = to_string( (long) (i+1)) + "   <a href=\"http://uri.amap.com/marker?position="+ mgps + "&src=weixin&coordinate=wgs84\">";
				Url +=  mDeviceName + "    " + mDevState + mDevTime  + "</a>"  ;
				mitem += Url;
				mitem += "\r\n\r\n";		
			}else
			{
				mgps = mjson_data["data"][i]["base_station"].asString(); //基站数据
				if(mDevLock)//当前是锁定状态
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
	mstr += mitem;//内容
	mstr+=part6;
	len=mstr.length();

	char content_header[100];
	sprintf_s(content_header,"Content-Length: %d\r\n", len);
	strcat_s(send_str, content_header);
	strcat_s(send_str, "Connection: close\r\n");
	strcat_s(send_str, "Pragma: no-cache\r\n");
	strcat_s(send_str, "\r\n");
	xml = (char*)mstr.data();
	strcat_s(send_str, xml);//注意http响应末尾是消息体，就是\r\n也算2个字节的消息体，不要多发字节
	//strcat_s(send_str, "\r\n");//注意，因为多加了两个字节，导致content-Length 小，微信6秒才回复
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
////////////////////////////////////回复设备锁电机列表//////////////////////////////////////////////
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
	mjson_user_data = WXGetDeviceData(sClient ,mjson_card);//从数据库获取用户数据
	if(mjson_user_data.size() <= 0 )
		return -1;
    memset(send_str,0,sizeof(send_str));
    //头信息
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2017 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	mutf8_str = "    点击定位";
	mutf8_str = GetUTF8FromGBK(mutf8_str);
	mitem = "A路输出操作设备列表\r\n\r\n";
	mitem = GetUTF8FromGBK(mitem);

	if (mjson_user_data["data"].isArray())//判断是json数组
    {
		int  DataSize = 0;
		int  Result = 0;
		int  SaveFlag=0;
        int nArraySize = mjson_user_data["data"].size(); //数组条数 
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
						case 1: //前者时间大						
								//mjson_data["data"][j].clear();
								mjson_data["data"][j] =  mjson_user_data["data"][i];
							break;
						case 0://后者时间大

							break;
						case 2: //时间相等
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
				mDeviceName = mjson_data["data"][i]["bikename"].asString(); //数组中某个key值 
			
			if(  mjson_data["data"][i]["card_lock"].isInt())	
				mDevLock = mjson_data["data"][i]["card_lock"].asInt();
			cout<<"mDevLock value is :  "<<mDevLock <<endl;
			if( mjson_data["data"][i]["card"].isString())		
				mDevCard = mjson_data["data"][i]["card"].asString(); //数组中某个key值 
			if (mjson_data["data"][i]["card_state"].isString())
				card_state = mjson_data["data"][i]["card_state"].asString();

			mDevState = card_state.substr( 0,  card_state.find_last_of(","));//保存AB路开关状态
	
			mDevTime  = mjson_data["data"][i]["time"].asString();
			if (mDevLock == 1)
			{
				mDevState += ",已布防";
				
			}
			else
			{
				mDevState += ",已撤防";
				
			}
			if (mjson_data["data"][i]["gps"].isString())
			{
				mgps = mjson_data["data"][i]["gps"].asString(); //gps经纬度数据 

				string::size_type  pos = mDevState.find("AOPEN");

				if (pos != string::npos)
				{
					//cout << "2222222" << endl;				
					cout << "GPS命令应该close" << endl;
					Url = to_string((long)(i + 1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/motorlock.html&response_type=code&scope=snsapi_base&state=" + mDevCard + "SETmotorclose#wechat_redirect\">";
				}
				else
				{
					cout << "GPS命令应该OPEN" << endl;
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
				mgps = mjson_data["data"][i]["base_station"].asString(); //基站数据			
				string::size_type  pos = mDevState.find("AOPEN");
				if (pos != string::npos)
				{
					cout << "基站命令应该close" << endl;
					Url = to_string((long)(i + 1)) + "<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=" + WX_ID + "&redirect_uri=http://gps.shaohaoqi.cn/motorlock.html&response_type=code&scope=snsapi_base&state=" + mDevCard + "SETmotorclose#wechat_redirect\">";
				}
				else
				{
					cout << "基站命令应该OPEN" << endl;
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
	mstr += mitem;//内容
	mstr+=part6;
	len=mstr.length();

	char content_header[100];
	sprintf_s(content_header,"Content-Length: %d\r\n", len);
	strcat_s(send_str, content_header);
	strcat_s(send_str, "Connection: close\r\n");
	strcat_s(send_str, "Pragma: no-cache\r\n");
	strcat_s(send_str, "\r\n");
	xml = (char*)mstr.data();
	strcat_s(send_str, xml);//注意http响应末尾是消息体，就是\r\n也算2个字节的消息体，不要多发字节
	//strcat_s(send_str, "\r\n");//注意，因为多加了两个字节，导致content-Length 小，微信6秒才回复
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
////////////////////////////////////回复开电门设备操作列表//////////////////////////////////////////////
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
	mjson_user_data = WXGetDeviceData(sClient, mjson_card);//从数据库获取用户数据
	if (mjson_user_data.size() <= 0)
		return -1;
	memset(send_str, 0, sizeof(send_str));
	//头信息
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2017 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	mutf8_str = "    点击定位";
	mutf8_str = GetUTF8FromGBK(mutf8_str);
	mitem = "B路输出操作设备列表\r\n\r\n";
	mitem = GetUTF8FromGBK(mitem);

	if (mjson_user_data["data"].isArray())//判断是json数组
	{
		int  DataSize = 0;
		int  Result = 0;
		int  SaveFlag = 0;
		int nArraySize = mjson_user_data["data"].size(); //数组条数 
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
					case 1: //前者时间大						
							//mjson_data["data"][j].clear();
						mjson_data["data"][j] = mjson_user_data["data"][i];
						break;
					case 0://后者时间大

						break;
					case 2: //时间相等
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
				mDeviceName = mjson_data["data"][i]["bikename"].asString(); //数组中某个key值 

			if (mjson_data["data"][i]["card_lock"].isInt())
				mDevLock = mjson_data["data"][i]["card_lock"].asInt();
			cout << "mDevLock value is :  " << mDevLock << endl;
			if (mjson_data["data"][i]["card"].isString())
				mDevCard = mjson_data["data"][i]["card"].asString(); //数组中某个key值 
			if (mjson_data["data"][i]["card_state"].isString())
				card_state = mjson_data["data"][i]["card_state"].asString();

			mDevState = card_state.substr(0, card_state.find_last_of(","));//保存AB路开关状态
			if (mDevLock == 1)
			{
				mDevState += ",已布防";
				
			}
			else
			{
				mDevState += ",已撤防";
				
			}
			
			mDevTime = mjson_data["data"][i]["time"].asString();
			if (mjson_data["data"][i]["gps"].isString())
			{
				mgps = mjson_data["data"][i]["gps"].asString(); //gps经纬度数据 
				
				if (mDevState.find("BOPEN") != std::string::npos)//当前是锁定状态
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
				mgps = mjson_data["data"][i]["base_station"].asString(); //基站数据
				if (mDevState.find("BOPEN") != std::string::npos)//当前是锁定状态
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
	mstr += mitem;//内容
	mstr += part6;
	len = mstr.length();

	char content_header[100];
	sprintf_s(content_header, "Content-Length: %d\r\n", len);
	strcat_s(send_str, content_header);
	strcat_s(send_str, "Connection: close\r\n");
	strcat_s(send_str, "Pragma: no-cache\r\n");
	strcat_s(send_str, "\r\n");
	xml = (char*)mstr.data();
	strcat_s(send_str, xml);//注意http响应末尾是消息体，就是\r\n也算2个字节的消息体，不要多发字节
							//strcat_s(send_str, "\r\n");//注意，因为多加了两个字节，导致content-Length 小，微信6秒才回复
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
//////////////////////////////回复图文消息///////////////////////////////////////////////////
int ResponseNewsMsg(SOCKET sClient, char *mToUserName, char *mFromUserName,char* weixin_openID)//
{
	char send_str[SEND_LEN+100] = {0};
	string  mArticleCount ,mutf8_str ,mTitle, mDescription, PicUrl,Url;
    memset(send_str,0,sizeof(send_str));
    //头信息
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
	mjson_user_data = WXGetDeviceData(sClient ,mjson_card);//获取用户数据
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
	
	mutf8_str = "    点击定位";
	mutf8_str = GetUTF8FromGBK(mutf8_str);	
			
	PicUrl = "https://mmbiz.qpic.cn/mmbiz_png/I1ABleEnu624wT691lbibCicib7C6K4UiabWHYhXldFQR0glGibv14Kyica3rxpJqQqoUic3zsDw8oUhUXsibiazLhUcW3w/0?wx_fmt=png";
	if (mjson_user_data["data"].isArray())//判断是json数组
    {
        int nArraySize = mjson_user_data["data"].size(); //数组条数 
		mArticleCount = to_string( (long )nArraySize);
		mstr+=mArticleCount;
		mstr+=news_part2;
	
        for (int i=0; i<nArraySize; i++)
        {       
            mTitle = mjson_user_data["data"][i]["bikename"].asString(); //数组中某个key值 
            mgps = mjson_user_data["data"][i]["gps"].asString(); //gps数据 
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
	//Url = "http://api.map.baidu.com/direction?origin=latlng:3800.3858,11427.8776|name:我的位置&destination=latlng:3802.1879,11428.6355&mode=driving&region=石家庄&output=html&coord_type=wgs84&src=weixin";
	//Url = "http://api.map.baidu.com/direction?origin=latlng:34.264642646862,108.95108518068|name:我的位置&destination=大雁塔&mode=driving&region=西安&output=html&src=weixin";
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
	strcat_s(send_str, xml);//注意http响应末尾是消息体，就是\r\n也算2个字节的消息体，不要多发字节
	//strcat_s(send_str, "\r\n");//注意，因为多加了两个字节，导致content-Length 小，微信6秒才回复
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
///////////////////////获取用户绑定信息//////////////////////////////////////////
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
	MYSQL_ROW  mysql_row; //行内容
	my_ulonglong  f1, f2, num_row, num_col;
	my_ulonglong  card_row = 0, card_line = 0;
	my_ulonglong  base_station_row = 0, base_station_line = 0;
	string   str_token, str_username, str_card, str_phone, str_bikename, str_bikecolor, str_biketype;

	memset(send_str, 0, sizeof(send_str));//这里是数组填充0，memset不可给指针填充，那只会填充指针所在的内存
										  //头信息
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2018 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	string  mstr;//消息体字符串缓存

	if (GetVaule_CheckStr(&str_token, mJsonValue, "token") == 0)
	{
		cout << "json数据类型错误token应该为string" << endl;
		return  -5;
	}
	if (GetVaule_CheckStr(&ToUserName, mJsonValue, "ToUserName") == 0)
	{
		cout << "json数据类型错误ToUserName应该为string" << endl;
		return  -5;
	}
	if (GetVaule_CheckStr(&FromUserName, mJsonValue, "FromUserName") == 0)
	{
		cout << "json数据类型错误FromUserName应该为string" << endl;
		return  -5;
	}
	
	string  m_strname = "SELECT  username  FROM  weixin_user  WHERE token = '" + str_token + "'";
	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//初始化数据库  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
	mysql_init(&myCont);//初始化mysql

	if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
	{
	}
	else
	{
		cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

		return -2;
	}

	//cout<<m_strname<<endl;
	getsucess = false;
	res = mysql_query(&myCont, (const  char *)m_strname.c_str()); //执行SQL语句,通过token查找username
	if (!res)
	{
		//保存查询到的数据到result
		result = mysql_store_result(&myCont);
		num_row = mysql_num_rows(result); //读取行数
		num_col = mysql_num_fields(result); //读取列数
											//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
		MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组

		for (f1 = 0; f1<num_row; f1++) //循环行
		{
			mysql_row = mysql_fetch_row(result); //获取每行的内容

			for (f2 = 0; f2<num_col; f2++) //循环列
			{
				if (fields[f2].name != NULL)
					if (!strcmp(fields[f2].name, "username")) //判断当前列的字段名称
					{
						str_username = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL
																  //cout<<str_username.data()<<endl;
						getsucess = true;
						break;
						//cout<<mstr_pwd.data()<<endl;d
					}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据

			}
		}

		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长

	}
	if (getsucess == true)
	{

	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

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
		mstr += GetUTF8FromGBK("您还没有注册，先注册账号，再使用\r\n");
		//mstr+="<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa70b2a8a84b770fb&redirect_uri=http://gps.shaohaoqi.cn/zhuce.html&response_type=code&scope=snsapi_base&state=suoding123#wechat_redirect\">";
		mstr += "<a href=\"http://wxweb.shaohaoqi.cn/zhuce.html?token=" + str_token + "\">";
		mstr += GetUTF8FromGBK("点击注册");//内容
		mstr += "</a>";
		mstr += part6;
		len = mstr.length();//整个消息体长度Content-Length  要和后面消息体字节数严格一致

		char content_header[100];
		sprintf_s(content_header, "Content-Length: %d\r\n", len);
		strcat_s(send_str, content_header);
		strcat_s(send_str, "Connection: close\r\n");
		strcat_s(send_str, "Pragma: no-cache\r\n");
		strcat_s(send_str, "\r\n");
		xml = (char*)mstr.data();
		strcat_s(send_str, xml);//注意http响应末尾是消息体，就是\r\n也算2个字节的消息体，不要多发字节
		//strcat_s(send_str, "\r\n");//注意，因为多加了两个字节，导致超过content-Length，微信6秒才回复
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
	res = mysql_query(&myCont, (const  char *)m_strcard.c_str()); //执行SQL语句,通过token查找username
	if (!res)
	{
		//保存查询到的数据到result
		result = mysql_store_result(&myCont);
		num_row = mysql_num_rows(result); //读取行数
		num_col = mysql_num_fields(result); //读取列数
											//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
		MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if (num_row>0)
		{
			mjson_cardstate["errno"] = Json::Value(0);     // 新建一个 Key（名为：key_string），赋予字符串值："value_string"。
			mjson_cardstate["error"] = Json::Value("sucess");
			for (f1 = 0; f1<num_row; f1++) //循环行
			{
				str_bikename.clear();
				str_card.clear();
				mysql_row = mysql_fetch_row(result); //获取每行的内容				
								
				for (f2 = 0; f2<num_col; f2++) //循环列
				{
					if (!strcmp(fields[f2].name, "bikename"))
					{
						str_bikename = getNullStr(mysql_row[f2]);						
					}

					if (!strcmp(fields[f2].name, "card")) //判断当前列的字段名称
					{
						//cout<<str_username.data()<<endl;
						str_card = getNullStr(mysql_row[f2]);
						getsucess = true;						
					}//endif card
					 //printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据
				}//endfor 列
				Json::Value  mjson_card;  // 表示整个 json 对象
				mjson_card["bikename"] = Json::Value(str_bikename);
				mjson_card["card"] = Json::Value(str_card);
				mjson_cardstate["data"].append(Json::Value(mjson_card));//添加子
			}//endfor 行
			mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长

		}
		else
		{
			mysql_close(&myCont);
			mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

			cout << "当前用户没有绑定设备！" << endl;

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
			mstr += GetUTF8FromGBK("您还没有绑定设备，请绑定设备再使用,可以绑定多个!\r\n");
			//mstr+="<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa70b2a8a84b770fb&redirect_uri=http://gps.shaohaoqi.cn/zhuce.html&response_type=code&scope=snsapi_base&state=suoding123#wechat_redirect\">";
			mstr += "<a href=\"http://wxweb.shaohaoqi.cn/bind.html?token=" + str_token + "\">";
			mstr += GetUTF8FromGBK("点击绑定");//内容
			mstr += "</a>";
			mstr += part6;
			len = mstr.length();//整个消息体长度Content-Length  要和后面消息体字节数严格一致

			char content_header[100];
			sprintf_s(content_header, "Content-Length: %d\r\n", len);
			strcat_s(send_str, content_header);
			strcat_s(send_str, "Connection: close\r\n");
			strcat_s(send_str, "Pragma: no-cache\r\n");
			strcat_s(send_str, "\r\n");
			xml = (char*)mstr.data();
			strcat_s(send_str, xml);//注意http响应末尾是消息体，就是\r\n也算2个字节的消息体，不要多发字节

			if (send(ClientS, send_str, strlen(send_str), 0) == -1)
			{
				cout << "send failed" << endl;
				closesocket(ClientS);
				return -101;
			}
			return -6;
		}

	}
	mysql_close(&myCont);//及时关闭sql连接，否则占用mysql连接数
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

	if (getsucess == true)
	{
		//Json::FastWriter  fast_writer;//查看json内容对象
		//string str = fast_writer.write(mjson_cardstate); //json转string
		return  mjson_cardstate;
		//send(ClientS , (char *)str.data(), (int)str.length(), 0);  // 发送信息 
	}
	else  if (getsucess == false)
	{
		//Json::Value root;             // 表示整个 json 对象
		//root["errno"] = Json::Value(1);     // 新建一个 Key（名为：key_string），赋予字符串值："value_string"。
		//root["error"] = Json::Value("get_userbind error");             // 新建一个 Key（名为：key_number），赋予数值：12345。

		//Json::FastWriter  fast_writer;//查看json内容对象
		//string str = fast_writer.write(root); //json转string
		//send(ClientS , (char *)str.data(), (int)str.length(), 0);  // 发送信息 
		cout << "get userbind data error!\n" << endl;
		return -4;
	}
	///////////////////////////////////////////////////////////////////////	

	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

	return -1;

}
///////////////////////获取用户最新数据//////////////////////////////////////////
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
	MYSQL_ROW  mysql_row; //行内容
	my_ulonglong  f1,f2,num_row,num_col; 
	my_ulonglong  card_row=0,card_line=0;
	my_ulonglong  base_station_row = 0, base_station_line = 0;
	string   str_token,str_username ,str_card,str_phone,str_bikename,str_bikecolor,str_biketype;  

	memset(send_str,0,sizeof(send_str));//这里是数组填充0，memset不可给指针填充，那只会填充指针所在的内存
    //头信息
	strcat_s(send_str, "HTTP/1.1 200 OK\r\n");
	strcat_s(send_str, "Date: Tue, 08 Mar 2018 09:05:25 GMT\r\n");
	strcat_s(send_str, "Content-Type: text/xml\r\n");

	string  mstr;//消息体字符串缓存
	
	if( GetVaule_CheckStr(&str_token , mJsonValue , "token") == 0 )
	{
		cout<< "json数据类型错误token应该为string"<<endl;
		return  -5;
	}
	if( GetVaule_CheckStr(&ToUserName , mJsonValue , "ToUserName") == 0 )
	{
		cout<< "json数据类型错误ToUserName应该为string"<<endl;
		return  -5;
	}
	if( GetVaule_CheckStr(&FromUserName , mJsonValue , "FromUserName") == 0 )
	{
		cout<< "json数据类型错误FromUserName应该为string"<<endl;
		return  -5;
	}
	//string  mstr_ziduan = mJsonValue["token"].asString();
	string  m_strname = "SELECT  username  FROM  weixin_user  WHERE token = '" + str_token + "'";
	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//初始化数据库  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//初始化mysql//初始化mysql
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
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

		return -2;
    }

	//cout<<m_strname<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strname.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{			
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组

		for(f1=0;f1<num_row;f1++) //循环行
		{		
			mysql_row = mysql_fetch_row(result); //获取每行的内容

			for(f2=0;f2<num_col;f2++) //循环列
			{	
				if (fields[f2].name!=NULL)  
				if (!strcmp( fields[f2].name , "username")) //判断当前列的字段名称
				{
					str_username = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL
					//cout<<str_username.data()<<endl;
					getsucess = true;
					break;
					//cout<<mstr_pwd.data()<<endl;d
				}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据
			
			}
		}

		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长
		
	}
	if(getsucess == true)
	{
		
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

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
		mstr+=GetUTF8FromGBK("您还没有注册，先注册账号，再使用\r\n");
		//mstr+="<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa70b2a8a84b770fb&redirect_uri=http://gps.shaohaoqi.cn/zhuce.html&response_type=code&scope=snsapi_base&state=suoding123#wechat_redirect\">";
		mstr+="<a href=\"http://wxweb.shaohaoqi.cn/zhuce.html?token=" + str_token +"\">";
		mstr += GetUTF8FromGBK("点击注册");//内容
		mstr += "</a>";
		mstr+=part6;
		len=mstr.length();//整个消息体长度Content-Length  要和后面消息体字节数严格一致
	
		char content_header[100];
		sprintf_s(content_header,"Content-Length: %d\r\n", len);
		strcat_s(send_str, content_header);
		strcat_s(send_str, "Connection: close\r\n");
		strcat_s(send_str, "Pragma: no-cache\r\n");
		strcat_s(send_str, "\r\n");
		xml = (char*)mstr.data();
		strcat_s(send_str, xml);//注意http响应末尾是消息体，就是\r\n也算2个字节的消息体，不要多发字节
		//strcat_s(send_str, "\r\n");//注意，因为多加了两个字节，导致超过content-Length，微信6秒才回复
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
	res = mysql_query(&myCont, (const  char *)m_strcard.c_str()); //执行SQL语句,通过token查找username
	if(!res )
	{			
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if(num_row>0)
		{
			mjson_cardstate["errno"] = Json::Value(0);     // 新建一个 Key（名为：key_string），赋予字符串值："value_string"。
			mjson_cardstate["error"] = Json::Value("sucess");
			for(f1=0;f1<num_row;f1++) //循环行
			{		
				str_bikename.clear();
				str_card.clear();
				mysql_row = mysql_fetch_row(result); //获取每行的内容
				
				for(f2=0;f2<num_col;f2++) //循环列
				{	
					if(!strcmp(fields[f2].name , "bikename"))
					{
						str_bikename =  getNullStr(mysql_row[f2]);		
			
					}
					
					if (fields[f2].name!=NULL)  
					if (!strcmp( fields[f2].name , "card")) //判断当前列的字段名称
					{				
						//cout<<str_username.data()<<endl;
						str_card =  getNullStr(mysql_row[f2]);										
					
					}//endif card
					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据
			
				}//endfor 列
			
				if(str_card.length()>0   )
				{
					string  m_strToken = "SELECT  *  FROM  card_data  WHERE card = '" + str_card + "' ORDER BY card_id DESC LIMIT 0,1";
					//cout<<m_strToken<<endl;
					res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //执行SQL语句,通过token查找username
					if(!res  )
					{				
						//保存查询到的数据到result
						MYSQL_RES *card_result = mysql_store_result(&myCont);
						card_row=mysql_num_rows(card_result); //读取行数
						card_line=mysql_num_fields(card_result); //读取列数
						MYSQL_FIELD*  card_files = mysql_fetch_fields(card_result); //返回所有字段结构的数组
						if(card_row > 0)
						{	
							getsucess = true;
							Json::Value  mjson_card;  // 表示整个 json 对象
							mjson_card["card"] = Json::Value(str_card);	
							mjson_card["bikename"] = Json::Value(str_bikename);	
							mysql_row = mysql_fetch_row(card_result); //获取每行的内容

							for(int card_f2 =0;card_f2<card_line;card_f2++) //循环列
							{	
								//if (fields[f2].name!=NULL)  
								if (!strcmp( card_files[card_f2].name , "card_state")) //判断当前列的字段名称
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

								//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据
	
							}//endfor 列

							mysql_free_result(card_result); //释放缓存，特别注意，不释放会导致内存增长	
							//mjson_cardstate["data"] = Json::Value( mjson_card );
							mjson_cardstate["data"].append( Json::Value(mjson_card) );//添加子
						
						}//endif
					}
					//////////////////////////////查询基站上传数据表///////////////////////////
#if   1
					string  m_sql_base_station = "SELECT  *  FROM  card_base_station_data  WHERE card = '" + str_card + "' ORDER BY card_id DESC LIMIT 0,1";
					//cout<<m_strToken<<endl;
					res = mysql_query(&myCont, (const  char *)m_sql_base_station.c_str()); //执行SQL语句,通过token查找username
					if(!res  )
					{				
						//保存查询到的数据到result
						MYSQL_RES *base_station_result = mysql_store_result(&myCont);
						base_station_row=mysql_num_rows(base_station_result); //读取行数
						base_station_line=mysql_num_fields(base_station_result); //读取列数
						MYSQL_FIELD*  card_files = mysql_fetch_fields(base_station_result); //返回所有字段结构的数组
						if(base_station_row > 0)
						{	
							getsucess = true;
							Json::Value  mbase_json_card;  // 表示整个 json 对象
							mbase_json_card["card"] = Json::Value(str_card);	
							mbase_json_card["bikename"] = Json::Value(str_bikename);	
							mysql_row = mysql_fetch_row(base_station_result); //获取每行的内容

							for(int card_f2 =0;card_f2<card_line;card_f2++) //循环列
							{	
								//if (fields[f2].name!=NULL)  
								if (!strcmp( card_files[card_f2].name , "card_state")) //判断当前列的字段名称
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

								//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据
	
							}//endfor 列

							mysql_free_result(base_station_result); //释放缓存，特别注意，不释放会导致内存增长	
							//mjson_cardstate["data"] = Json::Value( mbase_json_card );
							mjson_cardstate["data"].append( Json::Value(mbase_json_card) );//添加子
						
						}//endif

					}//endif
					#endif

				}//endif 卡号长度和车辆名称
			}//endfor 行
			mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长
			
		}
		else
		{
			mysql_close(&myCont);
			mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

			cout<<"当前用户没有绑定设备！"<<endl;
				
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
			mstr+=GetUTF8FromGBK("您还没有绑定设备，请绑定设备再使用,可以绑定多个!\r\n");
			//mstr+="<a href=\"https://open.weixin.qq.com/connect/oauth2/authorize?appid=wxa70b2a8a84b770fb&redirect_uri=http://gps.shaohaoqi.cn/zhuce.html&response_type=code&scope=snsapi_base&state=suoding123#wechat_redirect\">";
			mstr+="<a href=\"http://wxweb.shaohaoqi.cn/bind.html?token=" + str_token +"\">";
			mstr += GetUTF8FromGBK("点击绑定");//内容
			mstr += "</a>";
			mstr+=part6;
			len=mstr.length();//整个消息体长度Content-Length  要和后面消息体字节数严格一致
	
			char content_header[100];
			sprintf_s(content_header,"Content-Length: %d\r\n", len);
			strcat_s(send_str, content_header);
			strcat_s(send_str, "Connection: close\r\n");
			strcat_s(send_str, "Pragma: no-cache\r\n");
			strcat_s(send_str, "\r\n");
			xml = (char*)mstr.data();
			strcat_s(send_str, xml);//注意http响应末尾是消息体，就是\r\n也算2个字节的消息体，不要多发字节
	
			if (send(ClientS, send_str, strlen(send_str),0) == -1)
			{
				cout<<"send failed"<<endl;
				closesocket(ClientS);
				return -101;
			}		
			return -6;
		}

	}
	mysql_close(&myCont);//及时关闭sql连接，否则占用mysql连接数
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

	if(getsucess == true )
	{
		Json::FastWriter  fast_writer;//查看json内容对象
		string str = fast_writer.write(mjson_cardstate); //json转string
		return  mjson_cardstate;
		//send(ClientS , (char *)str.data(), (int)str.length(), 0);  // 发送信息 
	}
	else  if(getsucess == false)
	{
		Json::Value root;             // 表示整个 json 对象
		root["errno"] = Json::Value(1);     // 新建一个 Key（名为：key_string），赋予字符串值："value_string"。
		root["error"] = Json::Value("get_state error");             // 新建一个 Key（名为：key_number），赋予数值：12345。
		
		Json::FastWriter  fast_writer;//查看json内容对象
		string str = fast_writer.write(root); //json转string
		//send(ClientS , (char *)str.data(), (int)str.length(), 0);  // 发送信息 
		cout << "get CARD state error!\n" << endl;
		return -4;
	}
///////////////////////////////////////////////////////////////////////	
	
    mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

	return -1;

}
///////////////////////微信设置锁定设备/////////////////////////////////////////////////
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
	MYSQL_ROW  mysql_row; //行内容
	my_ulonglong  f1,f2,num_row,num_col; 
    int res;

	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	
	str_card = card;
	if(alarm_lock)
		str_lock = "1";
	else
		str_lock = "0";

	//初始化数据库  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//初始化mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {
        //mysql_query(&myCont, "SET NAMES utf8"); //设置编码格式
    }
    else
    {
        cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

		return -2;
    }
	
	str_token = openid;
	string  m_strname = "SELECT  username  FROM  weixin_user  WHERE token = '" + str_token + "'";
	//cout<<m_strname<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strname.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{			
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组

		for(f1=0;f1<num_row;f1++) //循环行
		{		
			mysql_row = mysql_fetch_row(result); //获取每行的内容

			for(f2=0;f2<num_col;f2++) //循环列
			{	
				if (fields[f2].name!=NULL)  
				if (!strcmp( fields[f2].name , "username")) //判断当前列的字段名称
				{
					str_username = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL
					//cout<<str_username.data()<<endl;
					getsucess = true;
					break;
					//cout<<mstr_pwd.data()<<endl;
				}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据
			
			}
		}

		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长
		
	}
	if(getsucess == true)
	{
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

		cout << "select username error!\n" << endl;
		return -4;
	}
	str_card = card;
	string  mSQLStr = "SELECT  *  FROM  user_bike  WHERE username = '" + str_username +"'";
	
	//cout<< mSQLStr <<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{			
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if(num_row>0)
		{
			for(f1=0;f1<num_row;f1++) //循环行
			{		
				mysql_row = mysql_fetch_row(result); //获取每行的内容
				
				for(f2=0;f2<num_col;f2++) //循环列
				{	
					if(!strcmp(fields[f2].name , "card"))
					{
						if( 0 == card.compare(  getNullStr(mysql_row[f2])) )					
							getsucess = true;
					}
		
				}//endfor 列
			}//endfor 行
			mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长
			
		}
		else
		{
			mysql_close(&myCont);
			mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

			return -6;
		}

	}//sql执行成功

	if(getsucess == true )
	{
		
	}
	else  if(getsucess == false)
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

		cout << "check user card not exist !\n" << endl;
		return -4;
	}
	str_card = card;
///////////////////////////////////////////////////////////////////////
	//INSERT 会新增一条
	//on duplicate key update  需要设置一个主键，不自动增长，遇到主键冲突，执行后面的updata内容
	//这里把card设置为主键
	mSQLStr = "INSERT  INTO  set_card_alarm( card_id , card, card_socket,allow_alarm,radius,time)   VALUES( 12345, '"
		+ str_card + "',888 , '"  + str_lock + "'  ,200  , NOW(3)) on duplicate key update  card = '" 
		+ str_card + "' , allow_alarm = '"+ str_lock + "' , time = NOW(3) " ;  

	//cout<<mSQLStr<<endl;
	
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //执行SQL语句
	mysql_close(&myCont);//关闭sql连接
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

	if(!res  )
	{					
		GetLocalTime( &sys ); 
		printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
		return 0;  		
	}
	else
	{	
		cout << "update set_card_alarm error" <<endl;
		return -3;
	}

    mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

	return -1;

}
///////////////////////微信设置锁电机表内容/////////////////////////////////////////////////
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
	MYSQL_ROW  mysql_row; //行内容
	my_ulonglong  f1, f2, num_row, num_col;
	int res;

	SYSTEMTIME sys;
	GetLocalTime(&sys);
	printf("%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);

	str_card = card;
	if (alarm_lock)
		str_lock = "1";
	else
		str_lock = "0";
	//初始化数据库  
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
		//mysql_query(&myCont, "SET NAMES utf8"); //设置编码格式
	}
	else
	{
		cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

		return -2;
	}

	str_token = openid;
	string  m_strname = "SELECT  username  FROM  weixin_user  WHERE token = '" + str_token + "'";
	//cout<<m_strname<<endl;
	getsucess = false;
	res = mysql_query(&myCont, (const  char *)m_strname.c_str()); //执行SQL语句,通过token查找username
	if (!res)
	{
		//保存查询到的数据到result
		result = mysql_store_result(&myCont);
		num_row = mysql_num_rows(result); //读取行数
		num_col = mysql_num_fields(result); //读取列数
											//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
		MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组

		for (f1 = 0; f1<num_row; f1++) //循环行
		{
			mysql_row = mysql_fetch_row(result); //获取每行的内容

			for (f2 = 0; f2<num_col; f2++) //循环列
			{
				if (fields[f2].name != NULL)
					if (!strcmp(fields[f2].name, "username")) //判断当前列的字段名称
					{
						str_username = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL
																  //cout<<str_username.data()<<endl;
						getsucess = true;
						break;
						//cout<<mstr_pwd.data()<<endl;
					}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据

			}
		}

		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长

	}
	if (getsucess == true)
	{
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

		cout << "select username error!\n" << endl;
		return -4;
	}
	str_card = card;
	string  mSQLStr = "SELECT  *  FROM  user_bike  WHERE username = '" + str_username + "'";

	//cout<< mSQLStr <<endl;
	getsucess = false;
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //执行SQL语句,通过token查找username
	if (!res)
	{
		//保存查询到的数据到result
		result = mysql_store_result(&myCont);
		num_row = mysql_num_rows(result); //读取行数
		num_col = mysql_num_fields(result); //读取列数
											//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
		MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if (num_row>0)
		{
			for (f1 = 0; f1<num_row; f1++) //循环行
			{
				mysql_row = mysql_fetch_row(result); //获取每行的内容

				for (f2 = 0; f2<num_col; f2++) //循环列
				{
					if (!strcmp(fields[f2].name, "card"))
					{
						if (0 == card.compare(getNullStr(mysql_row[f2])))
							getsucess = true;
					}

				}//endfor 列
			}//endfor 行
			mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长

		}
		else
		{
			mysql_close(&myCont);
			mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

			return -6;
		}

	}//sql执行成功

	if (getsucess == true)
	{

	}
	else  if (getsucess == false)
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

		cout << "check user card not exist !\n" << endl;
		return -4;
	}
	str_card = card;
	///////////////////////////////////////////////////////////////////////
	//INSERT 会新增一条
	//on duplicate key update  需要设置一个主键，不自动增长，遇到主键冲突，执行后面的updata内容
	//这里把card设置为主键
	mSQLStr = "INSERT  INTO  set_motor_lock( card_id , card, card_socket,motor_lock,radius,time)   VALUES( 12345, '"
		+ str_card + "',888 , '" + str_lock + "'  ,200  , NOW(3)) on duplicate key update  card = '"
		+ str_card + "' , motor_lock = '" + str_lock + "' , time = NOW(3) ";

	//cout<<mSQLStr<<endl;

	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //执行SQL语句
	mysql_close(&myCont);//关闭sql连接
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

	if (!res)
	{
		GetLocalTime(&sys);
		printf("%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);
		return 0;
	}
	else
	{
		cout << "update set_card_alarm error" << endl;
		return -3;
	}

	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

	return -1;

}

///////////////////////微信设置开电门/////////////////////////////////////////////////
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
	MYSQL_ROW  mysql_row; //行内容
	my_ulonglong  f1, f2, num_row, num_col;
	int res;

	SYSTEMTIME sys;
	GetLocalTime(&sys);
	printf("%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);

	str_card = card;
	if (alarm_lock)
		str_lock = "1";
	else
		str_lock = "0";

	//初始化数据库  
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
		//mysql_query(&myCont, "SET NAMES utf8"); //设置编码格式
	}
	else
	{
		cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

		return -2;
	}

	str_token = openid;
	string  m_strname = "SELECT  username  FROM  weixin_user  WHERE token = '" + str_token + "'";
	//cout<<m_strname<<endl;
	getsucess = false;
	res = mysql_query(&myCont, (const  char *)m_strname.c_str()); //执行SQL语句,通过token查找username
	if (!res)
	{
		//保存查询到的数据到result
		result = mysql_store_result(&myCont);
		num_row = mysql_num_rows(result); //读取行数
		num_col = mysql_num_fields(result); //读取列数
											//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
		MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组

		for (f1 = 0; f1<num_row; f1++) //循环行
		{
			mysql_row = mysql_fetch_row(result); //获取每行的内容

			for (f2 = 0; f2<num_col; f2++) //循环列
			{
				if (fields[f2].name != NULL)
					if (!strcmp(fields[f2].name, "username")) //判断当前列的字段名称
					{
						str_username = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL
																  //cout<<str_username.data()<<endl;
						getsucess = true;
						break;
						//cout<<mstr_pwd.data()<<endl;
					}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据

			}
		}

		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长

	}
	if (getsucess == true)
	{
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		cout << "select username error!\n" << endl;
		return -4;
	}
	str_card = card;
	string  mSQLStr = "SELECT  *  FROM  user_bike  WHERE username = '" + str_username + "'";

	//cout<< mSQLStr <<endl;
	getsucess = false;
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //执行SQL语句,通过token查找username
	if (!res)
	{
		//保存查询到的数据到result
		result = mysql_store_result(&myCont);
		num_row = mysql_num_rows(result); //读取行数
		num_col = mysql_num_fields(result); //读取列数
											//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
		MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if (num_row>0)
		{
			for (f1 = 0; f1<num_row; f1++) //循环行
			{
				mysql_row = mysql_fetch_row(result); //获取每行的内容

				for (f2 = 0; f2<num_col; f2++) //循环列
				{
					if (!strcmp(fields[f2].name, "card"))
					{
						if (0 == card.compare(getNullStr(mysql_row[f2])))
							getsucess = true;
					}

				}//endfor 列
			}//endfor 行
			mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长

		}
		else
		{
			mysql_close(&myCont);
			mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
			return -6;
		}

	}//sql执行成功

	if (getsucess == true)
	{

	}
	else  if (getsucess == false)
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		cout << "check user card not exist !\n" << endl;
		return -4;
	}
	str_card = card;
	///////////////////////////////////////////////////////////////////////
	//INSERT 会新增一条
	//on duplicate key update  需要设置一个主键，不自动增长，遇到主键冲突，执行后面的updata内容
	//这里把card设置为主键
	mSQLStr = "INSERT  INTO  set_device_open( card_id , card, card_socket,device_open,radius,time)   VALUES( 12345, '"
		+ str_card + "',888 , '" + str_lock + "'  ,200  , NOW(3)) on duplicate key update  card = '"
		+ str_card + "' , device_open = '" + str_lock + "' , time = NOW(3) ";

	//cout<<mSQLStr<<endl;

	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //执行SQL语句
	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用//关闭sql连接
	if (!res)
	{
		GetLocalTime(&sys);
		printf("%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds, sys.wDayOfWeek);
		return 0;
	}
	else
	{
		cout << "update set_card_alarm error" << endl;
		return -3;
	}

	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
	return -1;

}
///////////////////////截取GPS数据转度格式输出string///////////////////////////////////
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
		
//////////////////////////解析硬件GPS数据////////////////////////////////////////////////	
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
			gps_lon = GPSdufen_to_du(gps_lon);//经纬度转换成度为单位
			gps_lat = GPSdufen_to_du(gps_lat);//经纬度转换成度为单位
			//cout << "WEILAN_GPS_LON:"<< weilan_lon << endl;
			//cout << "WEILAN_GPS_LAT:"<<weilan_lat << endl;
			string str_gps = std::to_string((long double)gps_lon) +","+ std::to_string((long double)gps_lat);//double转string
			return str_gps;
		}		
		
	}
	return  "";
	
}
///////////////////////时间转换string转time_t////////////////////////////////////////////////
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
/////////////////////////////比较时间大小///////////////////////////////////////////
int DateCompare(const char* time1,const char* time2)
{
    int year1,month1,day1,hour1,min1,sec1;
    int year2,month2,day2,hour2,min2,sec2;
    sscanf(time1,"%d-%d-%d %d:%d:%d",&year1,&month1,&day1,&hour1,&min1,&sec1);
    sscanf(time2,"%d-%d-%d %d:%d:%d",&year2,&month2,&day2,&hour2,&min2,&sec2);
    int tm1 = year1*10000+month1*100+day1;
    int tm2 = year2*10000+month2*100+day2;
    if(tm1!=tm2) return (tm1>tm2)?1:0;//如果相等，大返回1，小返回0
    tm1 = hour1*3600+min1*60+sec1;
    tm2 = hour2*3600+min2*60+sec2;//将时分秒转换为秒数
    if(tm1!=tm2) return (tm1>tm2)?1:0;//如果相等，大返回1，小返回0
    return 2;//到这里必然是相等了
}
///////////////////////gbk转utf8///////////////////////////////////////////
string  GetUTF8FromGBK(string gbk_src)
{
	string  mutf8_str = gbk_src;
	int  nRetLen = GBKToUTF8((unsigned char *) mutf8_str.data(),NULL,NULL);
	//printf("转换后的字符串需要的空间长度为：%d ",nRetLen);			
	char *lpUTF8Str = new char[nRetLen + 1];
	nRetLen = GBKToUTF8((unsigned char *)mutf8_str.data(),(unsigned char *)lpUTF8Str,nRetLen);
	if(nRetLen)
	{
		//printf_s("GBKToUTF8转换成功！\n");
		mutf8_str = lpUTF8Str;
		delete  []lpUTF8Str;
		return mutf8_str;		
	}
	else
	{
		printf("GBKToUTF8转换失败！");
		delete  []lpUTF8Str;
		return "";				
	}
}
//GBK编码转换到UTF8编码
int GBKToUTF8(unsigned char * lpGBKStr,unsigned char * lpUTF8Str,int nUTF8StrLen)
{
    wchar_t * lpUnicodeStr = NULL;
    int nRetLen = 0;

    if(!lpGBKStr)  //如果GBK字符串为NULL则出错退出
        return 0;

    nRetLen = ::MultiByteToWideChar(CP_ACP,0,(char *)lpGBKStr,-1,NULL,NULL);  //获取转换到Unicode编码后所需要的字符空间长度
    lpUnicodeStr = new WCHAR[nRetLen + 1];  //为Unicode字符串空间
    nRetLen = ::MultiByteToWideChar(CP_ACP,0,(char *)lpGBKStr,-1,lpUnicodeStr,nRetLen);  //转换到Unicode编码
    if(!nRetLen)  //转换失败则出错退出
        return 0;

    nRetLen = ::WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,NULL,0,NULL,NULL);  //获取转换到UTF8编码后所需要的字符空间长度
    
    if(!lpUTF8Str)  //输出缓冲区为空则返回转换后需要的空间大小
    {
        if(lpUnicodeStr)       
			delete []lpUnicodeStr;
        return nRetLen;
    }
    
    if(nUTF8StrLen < nRetLen)  //如果输出缓冲区长度不够则退出
    {
        if(lpUnicodeStr)
            delete []lpUnicodeStr;
        return 0;
    }

    nRetLen = ::WideCharToMultiByte(CP_UTF8,0,lpUnicodeStr,-1,(char *)lpUTF8Str,nUTF8StrLen,NULL,NULL);  //转换到UTF8编码
    
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
////////////////////硬件接收数据解包///////////////////////////////
void CTestTask::taskRecClientProc()
{
	RecClientData(m_SocReUnit.SocketNum ,  m_SocReUnit.RecData ,m_SocReUnit.DataLen);
}

////////////////////接收APP数据解包///////////////////////////////
void CTestTask::taskAPPRecProc()
{
	RecAPPData(mAPP_RevUnit.SocketNum ,  mAPP_RevUnit.RecData ,mAPP_RevUnit.DataLen);
}
///////////////////硬件解析后数据的存储///////////////////////////
void CTestTask::taskSaveSQL()
{
	HardData    *pmdata;

	EnterCriticalSection(&mSaveDataLock);//待存储数据加锁

	if(!m_SaveQueueSQL.isEmpty())//判断队列非空
	{
		HardData  *pd = m_SaveQueueSQL.pop();//解析后的队列数据出队一组
		if(pd==NULL)
		{
			LeaveCriticalSection(&mSaveDataLock);//解锁
			std::cout<<"数据队列出队失败!!!"<<std::endl;
			
		}else
		{
			//memcpy(mdata.RecData , pd->RecData ,  pd->DataLen);
			//mdata.DataLen =  pd->DataLen;	
			//mdata.cmd =  pd->cmd;
			//mdata.SocketNum =  pd->SocketNum;
			pmdata = pd;
			
			LeaveCriticalSection(&mSaveDataLock);//解锁
			//SaveDataSQL(pd->SocketNum, pd->cmd, pd->RecData, pd->DataLen);//采集数据包中的实际用户数据信息
			JieXiWeiXin(pd->SocketNum, pd->cmd, pd->RecData, pd->DataLen);
			
			
		}
	}
	else
	{
		LeaveCriticalSection(&mSaveDataLock);//解锁
	}
	

}
/************************数据比较*****************************************/
unsigned  char    DataS_Compare(unsigned  char * p1  ,unsigned  char  *p2, unsigned  short  len)
{
 	unsigned 	short i=0;
	for(i=0;i<len;i++)
	{
		if(*(p1++) == *(p2++) )
		{

		}
		else
		return 0 ; //比较不同为0
	 	
	}

	return  1 ;
}
/**************************查找制定连续个字节*************************************/
unsigned  char  * FindByteBuff(unsigned  char  *src , unsigned  long srclen ,  unsigned char *dec , unsigned  short datalen )
{
	unsigned  char *pt = dec;
	unsigned	char *ps = src;
	unsigned	char *pd = dec;
	int c = 0;

	while( (unsigned  long )(ps-src) < srclen )
	{
			if(*ps == *pd) //查找到首个相同
			{
					
					while(  (datalen >c) && (srclen-(ps-src)) >0 )	//接着查找剩余的个数
					{
							if(*(ps++) == *(pd++) )
							{
								c++;
							}
							else //不相同，半途而废
							{
								c=0;
								break;
							}
					}
			}

			else	//不相同继续下移查找
			{
					ps++;
			}

			if( c == datalen)	 //如果查找到结尾，达到了所有相同的个数
			{						
				return (unsigned  char *)(ps - c);  //返回首字符位置
			}
			
			if( (ps -src)== srclen )
			return 0;  //返回失败
			
			c = 0;	 //
			pd = pt; //
			
	}

	return 0;
	
}

/////////////////////////////解析出来的硬件数据压入队列，并添加存储数据任务队列////////////////////////////////////////////////
char  SaveClientData(SOCKET   ClientS ,int cmd , unsigned  char * src, unsigned  int  len)
{
	HardData  mdata , *pmdata ;

	EnterCriticalSection(&mSaveDataLock);//待存储数据队列操作，公共对象先加锁
	
	memcpy(mdata.RecData , src , len);
	mdata.DataLen = len;	
	mdata.cmd = cmd;
	mdata.SocketNum = ClientS;
	pmdata= &mdata;
	
	//具体数据压入待存储队列
	m_SaveQueueSQL.push(pmdata);//待存储的数据入队，全局共用变量要加锁操作，因为该函数在多线程中调用了
	//这里的任务队列其实就是存了需要执行的任务次数，触发一次，就入队一次。实际数据没有存储，是全局变量						
	SaveDatathreadpool.addTask(pTask,NORMAL);//任务放入线程池中并执行任务，会唤醒挂起的线程，从而执行任务类的具体代码
	//saveCNT++;
	//printf("数据入队成功 %d\n",saveCNT);//
	LeaveCriticalSection(&mSaveDataLock);//解锁

	return 1;
	
}
/***********************************************************************
char   RecClientData
接收解析硬件上传数据
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

	SocketRecUnit   mRECdataS; //数据缓存结构体，包括socket连接

	EnterCriticalSection(&m_RECdataLock);//硬件解包线程加锁，保护全局变量
	memcpy( mRECdataS.RecData , src  ,len );
	mRECdataS.DataLen = len;
	
	srcdat=mRECdataS.RecData;
	pdata = srcdat;
	DataLen = mRECdataS.DataLen;
	LeaveCriticalSection(&m_RECdataLock);//解锁

	if(DataLen >0 && DataLen < REC_SIZE)
	{	
		SaveClientData( ClientS , *(src+0) ,src, len );//解析网络数据
		return  1;
	}
	else
	{
		
			return 0;
	}
	
}
//////////////////////////////////具体的硬件数据存储数据库函数////////////////////////////////////////////
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
		cout<< "json数据类型错误card应该为string"<<endl;
		return  -5;
	}
	
	str_soc = to_string((long)ClientS);//socket连接号,整型转string
	
	if( GetVaule_CheckStr(&str_state , mJsonValue , "card_state") == 0 )
	{
		cout<< "json数据类型错误card_state应该为string"<<endl;
		return  -5;
	}

	if(mJsonValue["card_lock"].isInt())
		str_lock = mJsonValue["card_lock"].asInt() + '0';
	else
	{		
		cout<< "json数据value类型错误card_lock应该为int"<<endl;
		return  -5;

	}

	if( GetVaule_CheckStr(&str_gps , mJsonValue , "gps") == 0 )
	{
		cout<< "json数据类型错误gps应该为string"<<endl;
		return  -5;
	}

	string  m_strToken = "SELECT  card  FROM  cardinfo  WHERE card = '" + str_card + "'";
	
	//now_time = time(NULL); 
	//cout<<now_time<<endl; 

	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//初始化数据库  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//初始化mysql//初始化mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {
        //mysql_query(&myCont, "SET NAMES utf8"); //设置编码格式       
    }
    else
    {
        cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		return -2;
    }
///////////////////////////////////////////////////////////////////////////////////
	//cout<<m_strToken<<endl;
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{			
		
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if(num_row >0 )
		{
			getsucess =true;
	
		}
		else
			getsucess = false;

		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长

	}
	if(getsucess == true)
	{

	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		cout << "select username error!\n" << endl;
		return -4;
	}
	
///////////////////////////////////////////////////////////////////////	
	//INSERT 会新增一条
	//on duplicate key update  需要设置一个主键，不自动增长，遇到主键冲突，执行后面的updata内容
	string  mSQLStr = "INSERT  INTO  card_data(  card , card_socket, card_state , card_lock , gps ,time )   VALUES( '" +
		str_card + "'," + str_soc + ",'"+ str_state + "',"+ str_lock + ",'"+ str_gps+ "', NOW() ) ";

	//UPDATE不会新增,是覆盖,有几条就覆盖几条。
	
	//string  mSQLStr="UPDATE  user_bike  SET  username = '" + str_username + "', card = '" + str_card + "', phone = '" + str_phone +"', bikename = '" + str_bikename +
	//	"', bikecolor = '" + str_bikecolor + "', biketype = '"+str_biketype + "', register_time ='2017-5-10-10:02:05' ";

	//cout<<mSQLStr<<endl;

	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //执行SQL语句,添加一条记录
	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用//及时关闭mysql连接，否则占用连接数
	if(!res  )
	{	
		Json::Value root;             // 表示整个 json 对象
		root["errno"] = Json::Value(0);     // 新建一个 Key（名为：key_string），赋予字符串值："value_string"。
		root["error"] = Json::Value("sucess"); // 新建一个 Key（名为：key_number），赋予数值：12345。	
		Json::FastWriter  fast_writer;//查看json内容对象
		string str = fast_writer.write(root); //json转string
			
		send(ClientS , (char *)str.data(), (int)str.length() , 0);  // 发送信息 
		//_sleep(500);
		//GetLocalTime( &sys ); 
		//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 

		//cout << "///////sucess!\n" << endl;
		return 0;  		
	}
	else
	{
	
		Json::Value root;             // 表示整个 json 对象
		root["errno"] = Json::Value(1);     // 新建一个 Key（名为：key_string），赋予字符串值："value_string"。
		root["error"] = Json::Value("update_error");             // 新建一个 Key（名为：key_number），赋予数值：12345。
		
		Json::FastWriter  fast_writer;//查看json内容对象
		string str = fast_writer.write(root); //json转string

		send(ClientS , (char *)str.data(),(int)str.length(), 0);  // 发送信息 
		cout << "add  SQL error!\n" << endl;
		return -3;
		
	}
	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
	
	return -1;

}
////////////////////////////保存GPS数据////////////////////////////////////////
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
		sprintf(&temp[2*i], "%02x", *(src+2+i)); //小写16 进制，宽度占2个位置，右对齐，不足补0
	
	temp[8]='\0';
	//str_card = getNullStr(temp);
	str_card = temp;
	//if( *(src+2+3)< 50 )
	//printf("%s\n",str_card);//打印卡号

	//EnterCriticalSection(&card_list_Lock);//待存储数据加锁

	//card_list  +=str_card;
	//card_list += "\r\n";
	//LeaveCriticalSection(&card_list_Lock);//解锁
	//send(ClientS , (char *)(src+2), 8, 0);  // 发送信息 
	//return 0;
	
	str_soc = to_string((long)ClientS);//socket连接号,整型转string
	
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
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//初始化数据库  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//初始化mysql//初始化mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {
		//mysql_query(&myCont, "SET NAMES utf8"); //设置编码格式       
    }
    else
    {
        cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		return -2;
    }
///////////////////////////////////////////////////////////////////////////////////
	//cout<<m_strToken<<endl;
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{				
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if(num_row >0 )
		{
			getsucess =true;
		}
		else
			getsucess = false;

		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长

	}
	if(getsucess == true)
	{

	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		cout << "GPS---查询card失败!\n" << endl;
		return -4;
	}
	
///////////////////////////////////////////////////////////////////////	
	//INSERT 会新增一条
	//on duplicate key update  需要设置一个主键，不自动增长，遇到主键冲突，执行后面的updata内容
	string  mSQLStr = "INSERT  INTO  card_data(  card , card_socket, card_state , card_lock , gps ,time )   VALUES( '" +
		str_card + "'," + str_soc + ",'"+ str_state + "',"+ str_lock + ",'"+ str_gps+ "', NOW() ) ";

	//UPDATE不会新增,是覆盖,有几条就覆盖几条。

	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //执行SQL语句,添加一条记录
	
	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用//及时关闭mysql连接，否则占用连接数
	if(!res  )
	{		
		Json::Value root;             // 表示整个 json 对象
		root["errno"] = Json::Value(0);     // 新建一个 Key（名为：key_string），赋予字符串值："value_string"。
		root["error"] = Json::Value("sucess"); // 新建一个 Key（名为：key_number），赋予数值：12345。	
		Json::FastWriter  fast_writer;//查看json内容对象
		string str = fast_writer.write(root); //json转string			
		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // 发送信息 	

		//GetLocalTime( &sys ); 
		//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
		cout << "SaveGPS-----sucess!\n" << endl;
		return 0;  		
	}
	else
	{
		Json::Value root;             // 表示整个 json 对象
		root["errno"] = Json::Value(1);     // 新建一个 Key（名为：key_string），赋予字符串值："value_string"。
		root["error"] = Json::Value("update_error");             // 新建一个 Key（名为：key_number），赋予数值：12345。
		
		Json::FastWriter  fast_writer;//查看json内容对象
		string str = fast_writer.write(root); //json转string

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // 发送信息 
		cout << "add GPS SQL error!\n" << endl;
		return -3;
		
	}
	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
	
	return -1;

}
////////////////////////////保存基站数据////////////////////////////////////////
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
		sprintf(&temp[2*i], "%02x", *(src+2+i)); //小写16 进制，宽度占2个位置，右对齐，不足补0
	
	temp[8]='\0';
	//str_card = getNullStr(temp);
	str_card = temp;
	//if( *(src+2+3)< 50 )
	//printf("%s\n",str_card);//打印卡号

	//EnterCriticalSection(&card_list_Lock);//待存储数据加锁

	//card_list  +=str_card;
	//card_list += "\r\n";
	//LeaveCriticalSection(&card_list_Lock);//解锁
	//send(ClientS , (char *)(src+2), 8, 0);  // 发送信息 
	//return 0;
	
	str_soc = to_string((long)ClientS);//socket连接号,整型转string
	
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
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//初始化数据库  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
	if (mysql_init(&myCont) == NULL)//初始化mysql
	{
		return -11;
    }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {
		//mysql_query(&myCont, "SET NAMES utf8"); //设置编码格式       
    }
    else
    {
        cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		return -2;
    }
///////////////////////////////////////////////////////////////////////////////////
	//cout<<m_strToken<<endl;
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{				
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if(num_row >0 )
		{
			getsucess =true;
		}
		else
			getsucess = false;

		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长

	}
	if(getsucess == true)
	{

	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		cout << "基站定位---查询card失败!\n" << endl;
		return -4;
	}
	
///////////////////////////////////////////////////////////////////////	
	//INSERT 会新增一条
	//on duplicate key update  需要设置一个主键，不自动增长，遇到主键冲突，执行后面的updata内容
	string  mSQLStr = "INSERT  INTO  card_base_station_data(  card , card_socket, card_state , card_lock , base_station ,time )   VALUES( '" +
		str_card + "'," + str_soc + ",'"+ str_state + "',"+ str_lock + ",'"+ str_base_station+ "', NOW() ) ";

	//UPDATE不会新增,是覆盖,有几条就覆盖几条。

	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //执行SQL语句,添加一条记录
	
	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用//及时关闭mysql连接，否则占用连接数
	if(!res  )
	{		
		Json::Value root;             // 表示整个 json 对象
		root["errno"] = Json::Value(0);     // 新建一个 Key（名为：key_string），赋予字符串值："value_string"。
		root["error"] = Json::Value("sucess"); // 新建一个 Key（名为：key_number），赋予数值：12345。	
		Json::FastWriter  fast_writer;//查看json内容对象
		string str = fast_writer.write(root); //json转string			
		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // 发送信息 	

		//GetLocalTime( &sys ); 
		//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
		cout << "SaveBaseStation-----sucess!\n" << endl;
		return 0;  		
	}
	else
	{
		Json::Value root;             // 表示整个 json 对象
		root["errno"] = Json::Value(1);     // 新建一个 Key（名为：key_string），赋予字符串值："value_string"。
		root["error"] = Json::Value("update_error");             // 新建一个 Key（名为：key_number），赋予数值：12345。
		
		Json::FastWriter  fast_writer;//查看json内容对象
		string str = fast_writer.write(root); //json转string

		//send(ClientS , (char *)str.data(), (int)str.length(), 0);  // 发送信息 
		cout << "Insert 基站数据 SQL error!\n" << endl;
		return -3;
		
	}
	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
	
	return -1;

}

////////////////////////////验证用户身份////////////////////////////////////////
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
	MYSQL_ROW  mysql_row; //行内容
	my_ulonglong  f1,f2,num_row,num_col; 
	string   str_token,str_username ;  
	str_token = openid;

	//string  mstr_ziduan = mJsonValue["token"].asString();
	string  m_strname = "SELECT  username  FROM  weixin_user  WHERE token = '" + str_token + "'";
	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//初始化数据库  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//初始化mysql//初始化mysql
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
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		return -2;
    }

	//cout<<m_strname<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strname.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{			
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组

		for(f1=0;f1<num_row;f1++) //循环行
		{		
			mysql_row = mysql_fetch_row(result); //获取每行的内容

			for(f2=0;f2<num_col;f2++) //循环列
			{	
				if (fields[f2].name!=NULL)  
				if (!strcmp( fields[f2].name , "username")) //判断当前列的字段名称
				{
					str_username = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL
					//cout<<str_username.data()<<endl;
					getsucess = true;
					break;
					//cout<<mstr_pwd.data()<<endl;
				}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据
			
			}
		}

		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长
		
	}
	if(getsucess == true)
	{
		return 0;
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		cout << "select username error!\n" << endl;
		return -4;
	}
	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
	return -1;

}

//////////////////////查找等待设定的设备表，并发送给硬件//////////////////////////////////////////
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
	MYSQL_ROW  mysql_row; //行内容
	my_ulonglong  f1,f2,num_row,num_col; 
    int res;

//	SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//初始化数据库  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//初始化mysql
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
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		return -2;
    }
	//cout<<m_strToken<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{			
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if(num_row > 0)
		{
			for(f1=0;f1<1;f1++) //循环行只取了最新的一行
			{		
				mysql_row = mysql_fetch_row(result); //获取每行的内容

				for(f2=0;f2<num_col;f2++) //循环列
				{	
					if (fields[f2].name!=NULL)  
					{						
						if (!strcmp( fields[f2].name , "allow_alarm")) //判断当前列的字段名称
						{
							allow_alarm = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL			
							getsucess =true;							
						}
					}					
					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据			
				}
			}
		}
		
		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长

	}
	
	if(getsucess ==true)
	{

	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		cout << "select alarm_weilan error!\n" << endl;
		return -4;
	}

	if(allow_alarm.compare("1"))
		send(ClientS , "bufang123", strlen("bufang123") , 0);  // 发送信息
	else
		send(ClientS , "chefang123", strlen("chefang123") , 0);  // 发送信息

	m_strToken = "DELETE    FROM  set_card_alarm   WHERE card = '" + DevCard + "' ORDER BY time ASC LIMIT 1 ";
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{			
	}
	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

	return -1;

}

/*****************微信网页post数据解析*************************

***************************************************************/
int  WX_PostDataHandle(SOCKET sClient,char *rec_buf , unsigned  long  rec_len )
{	
	char *ps ,*pd, *pend;
	char *revData = rec_buf;
    int result = 0;
	char file_name[HTTP_FILENAME_LEN+10];//文件名称
	char ref_context[HTTP_REF_LEN+5];//请求参数
	char suffix[50]="";//文件后缀
	int  data_len =0;
	int  cr_len=0;
	string  str_len;
	memset(file_name ,0 ,sizeof(file_name));//一定要先清空缓存
	memset(ref_context ,0 ,sizeof(ref_context));//一定要先清空缓存
	//return 0;
    /* 得到文件名和后缀 */
    http_parse_request_cmd(rec_buf, rec_len, file_name, suffix , ref_context);
	cout<<"请求的文件名："<<file_name<<endl;
	cout<<"请求参数："<<ref_context<<endl;
	
	if(strlen(file_name) > 1)
	{		
		if( ps = strstr(rec_buf , "Content-Length:"))
		{
			if( pd = strstr(ps , "\r\n"))
			{
				cr_len = strlen( "Content-Length:");
				str_len.insert(0,ps+cr_len , pd-ps-cr_len );
				data_len = atoi(str_len.c_str());
				cout<<"POST数据长度： "<<data_len<<endl;
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
/*****************POST数据值解析*************************

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
/*****************微信处理回复网页post请求*************************
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
			http_response_webfile(sClient , "chenggong.html");// 向客户端发送响应数据 
	}
	else  if(0==strcmp(file_name ,"zhuce.html"))
	{		
		int ret = WX_ResponseWebRegister( sClient , p_data ,p_len , ref_context );	
		if(0 == ret)
		{
			http_response_webfile(sClient , "zhuce_success.html");// 向客户端发送响应数据 
		}
		
	}
	else  if(0==strcmp(file_name ,"forget.html"))
	{		
		int ret = WX_ResponseWebForgetRegister( sClient , p_data ,p_len , ref_context );	
		if(0 == ret)
		{
			http_response_webfile(sClient , "forget_success.html");// 向客户端发送响应数据 
		}

		//WX_AddCustom("xiaolan@gh_f161a8e27ff9" , "小兰");
	}
	else  if(0==strcmp(file_name ,"bind.html"))
	{
		cout<<"解析bind.html"<<endl;
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
				http_response_webfile(sClient , "bind_success.html");// 向客户端发送响应数据 
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
			http_response_webfile(sClient , "gaimi_success.html");// 向客户端发送响应数据 
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
/////////////////////微信网页授权用code获取用户OPENID之后操作锁电机功能////////////////////////////////////////////
int WX_SetMotorLock(SOCKET sClient, char *buf, unsigned  long buf_len)//
{
	char *ps ,*pd, *pend,t;
	char *revData = buf;
	char MsgContent[3*1024];
	int ref_len=0;
	char ref_buf[HTTP_BUF_SIZE+5];
	WSADATA wsa_data;
    SOCKET  http_sock = 0;         /* socket 句柄 */
    struct sockaddr_in serv_addr;  /* 服务器地址 */
   // struct hostent *host_ent;
    char  user_openid[200];
    int result = 0, send_len;
    char data_buf[HTTP_BUF_SIZE+5];
    char rev_buf[HTTP_BUF_SIZE+5];
    unsigned short port = HTTP_DEF_PORT; //NGINX正向代理设置了8088端口转https
    //unsigned long addr;
    string  card_name ;
	WSAStartup(MAKEWORD(2,0), &wsa_data); /* 初始化 WinSock 资源 */  
    
    /* 服务器地址 */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//本机nginx正向代理

    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* 创建 socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));//建立socket
    if (result == SOCKET_ERROR) /* 连接失败 */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
		WSACleanup();
        return -1; 
    }
	cout<<"解析gps.shaohaoqi.cn"<<endl;
	string  wx_code = GetPostVauleFromKey("code=" , buf );
	string  wx_state = GetPostVauleFromKey("state=" , buf );
	string  lock_set = GetPostVauleFromKey("SET" , ( char *)wx_state.c_str() );
	ps = (char*)wx_state.c_str();
	pd = strstr(ps , "SET");
	if (pd)
	{
		card_name.insert(0 ,ps , pd-ps);
	}
	cout << "微信code：  " << wx_code.c_str() << endl;
	cout << "锁电机设备名：  " << wx_state.c_str() << endl;
	if(wx_code.length()>0 && wx_state.length() > 0 && lock_set.length()>0)
	{
		/* 向服务器发网页授权HTTP 请求 */
			memset(data_buf , 0, sizeof(data_buf));
			memset(ref_buf, 0, sizeof(ref_buf));
			ref_len = sprintf(ref_buf, wx_web_get_user_openid, WX_ID, WX_SECRET, wx_code.c_str());
			send_len = sprintf(data_buf, http_req_hdr_tmpl, ref_buf, WX_API_URL);//拼接http参数
			result = send(http_sock, data_buf, send_len, 0);//发送http请求体GET 
			cout<<"向微信发送网页授权请求的消息体:\r\n"<<data_buf<<endl;
			if (result == SOCKET_ERROR) /* 发送失败 */
			{
				printf("[Web] fail to send, error = %d\n", WSAGetLastError());
				closesocket(http_sock);  
				WSACleanup();
				return -1; 
			}

			do /* 接收响应并保存到文件中 */
			{
				result = recv(http_sock, rev_buf, HTTP_BUF_SIZE, 0);
				if (result > 0)
				{
					/* 在屏幕上输出 */
					rev_buf[result] = 0;
					printf("微信回复网页授权%s\r\n", rev_buf);
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
							cout<<"得到的用户openid： "<<user_openid<<endl;								
								
							if(0 == lock_set.compare("motoropen"))
							{
								if( 0 == WX_SetTableMotorLock(card_name  , 1 , user_openid) )
								{
									http_response_webfile(sClient , "MotorLock.html");// 向客户端发送响应数据 
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
									http_response_webfile(sClient , "MotorUnLock.html");// 向客户端发送响应数据 
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

/////////////////////微信网页授权用code获取用户OPENID之后操作锁电机功能////////////////////////////////////////////
int WX_SetDeviceOpen(SOCKET sClient, char *buf, unsigned  long buf_len)//
{
	char *ps, *pd, *pend, t;
	char *revData = buf;
	char MsgContent[3 * 1024];
	int ref_len = 0;
	char ref_buf[HTTP_BUF_SIZE + 5];
	WSADATA wsa_data;
	SOCKET  http_sock = 0;         /* socket 句柄 */
	struct sockaddr_in serv_addr;  /* 服务器地址 */
								   // struct hostent *host_ent;
	char  user_openid[200];
	int result = 0, send_len;
	char data_buf[HTTP_BUF_SIZE + 5];
	char rev_buf[HTTP_BUF_SIZE + 5];
	unsigned short port = HTTP_DEF_PORT; //NGINX正向代理设置了8088端口转https
										 //unsigned long addr;
	string  card_name;
	WSAStartup(MAKEWORD(2, 0), &wsa_data); /* 初始化 WinSock 资源 */

										   /* 服务器地址 */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//本机nginx正向代理

	http_sock = socket(AF_INET, SOCK_STREAM, 0); /* 创建 socket */
	result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));//建立socket
	if (result == SOCKET_ERROR) /* 连接失败 */
	{
		closesocket(http_sock);
		printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	cout << "解析gps.shaohaoqi.cn" << endl;
	string  wx_code = GetPostVauleFromKey("code=", buf);
	string  wx_state = GetPostVauleFromKey("state=", buf);
	string  lock_set = GetPostVauleFromKey("SET", (char *)wx_state.c_str());
	ps = (char*)wx_state.c_str();
	pd = strstr(ps, "SET");
	if (pd)
	{
		card_name.insert(0, ps, pd - ps);
	}
	cout << "微信code：  " << wx_code.c_str() << endl;
	cout << "锁电机设备名：  " << wx_state.c_str() << endl;
	if (wx_code.length()>0 && wx_state.length() > 0 && lock_set.length()>0)
	{
		/* 向服务器发网页授权HTTP 请求 */
		memset(data_buf, 0, sizeof(data_buf));
		memset(ref_buf, 0, sizeof(ref_buf));
		ref_len = sprintf(ref_buf, wx_web_get_user_openid, WX_ID, WX_SECRET, wx_code.c_str());
		send_len = sprintf(data_buf, http_req_hdr_tmpl, ref_buf, WX_API_URL);//拼接http参数
		result = send(http_sock, data_buf, send_len, 0);//发送http请求体GET 
		cout << "向微信发送网页授权请求的消息体:\r\n" << data_buf << endl;
		if (result == SOCKET_ERROR) /* 发送失败 */
		{
			printf("[Web] fail to send, error = %d\n", WSAGetLastError());
			closesocket(http_sock);
			WSACleanup();
			return -1;
		}

		do /* 接收响应并保存到文件中 */
		{
			result = recv(http_sock, rev_buf, HTTP_BUF_SIZE, 0);
			if (result > 0)
			{
				/* 在屏幕上输出 */
				rev_buf[result] = 0;
				printf("微信回复网页授权%s\r\n", rev_buf);
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
						cout << "得到的用户openid： " << user_openid << endl;

						if (0 == lock_set.compare("deviceclose"))
						{
							if (0 == WX_SetTableDeviceOpen(card_name, 0, user_openid))
							{
								http_response_webfile(sClient, "PowClose.html");// 向客户端发送响应数据 
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
								http_response_webfile(sClient, "PowOpen.html");// 向客户端发送响应数据 
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
////////////////////////////////////回复用户注册///////////////////////////////////////////////////
int  WX_ResponseWebRegister(SOCKET sClient,char *p_data , unsigned  long  p_len ,char* ref_context)
{
	bool  getsucess=false;

	Json::Value  m_json;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;        

    MYSQL   myCont;  //mysql 连接
    MYSQL_RES  *result ;  //存储结果
   // MYSQL_ROW  mysql_row; //行内容
	my_ulonglong  num_row,num_col; 
	string  mstr_pwd=""; //存储用户密码
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

	urldecode((char*)m_username.c_str());//中文编码转换%BF转换成0XBF存储
	urldecode((char*)m_userpwd.c_str());
	urldecode((char*)m_token.c_str());

	cout<<"用户名："<<m_username.c_str()<<endl;
	cout<<"密码："<<m_userpwd.c_str()<<endl;
	cout<<"微信openid："<<m_token.c_str()<<endl;
	m_username = GetUTF8FromGBK(m_username);
	m_userpwd = GetUTF8FromGBK(m_userpwd);
	m_token = GetUTF8FromGBK(m_token);
	//初始化数据库  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//初始化mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {
        //mysql_query(&myCont, "SET NAMES utf8"); //设置编码格式
    }
    else
    {
        cout << "connect failed!" << endl;
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		return -2;
    }

	string  m_checkuser = "SELECT  *  FROM  weixin_user  WHERE username = '" + m_username + "'";
	cout<< m_checkuser<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{			
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if(num_row > 0 )
		{
			getsucess = false;
		}
		else
		{
			getsucess = true;
		}

		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长
		
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

	res = mysql_query(&myCont, (const  char *)m_reguser.c_str()); //执行SQL语句,添加一条记录
	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
	if(!res  )
	{	
		/****
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用	
		Json::Value root;             // 表示整个 json 对象
		root["errno"] = Json::Value(0);     // 新建一个 Key（名为：key_string），赋予字符串值："value_string"。
		root["error"] = Json::Value("sucess"); // 新建一个 Key（名为：key_number），赋予数值：12345。	
		Json::FastWriter  fast_writer;//查看json内容对象
		string str = fast_writer.write(root); //json转string
			
		//send(sClient , (char *)str.data(), (int)str.length(), 0);  // 发送信息 
		 ****/
		cout << "//////////regester user  sucess!/////////////\n" << endl;
		return 0;  		
	}
	else
	{
		/*************
		mysql_close(&myCont);
		Json::Value root;             // 表示整个 json 对象
		root["errno"] = Json::Value(1);     // 新建一个 Key（名为：key_string），赋予字符串值："value_string"。
		root["error"] = Json::Value("regester_error");             // 新建一个 Key（名为：key_number），赋予数值：12345。
		
		Json::FastWriter  fast_writer;//查看json内容对象
		string str = fast_writer.write(root); //json转string
		****************/
		//send(sClient , (char *)str.data(), (int)str.length(), 0);  // 发送信息 
		cout << "Register User error!\n" << endl;
		return -3;
		
	}
    
	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

	return -1;
}
////////////////////////////////////忘记账号密码覆盖注册///////////////////////////////////////////////////
int  WX_ResponseWebForgetRegister(SOCKET sClient,char *p_data , unsigned  long  p_len ,char* ref_context)
{
	bool  getsucess=false;

	Json::Value  m_json;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;        

    MYSQL   myCont;  //mysql 连接
    MYSQL_RES  *result ;  //存储结果
    MYSQL_ROW  mysql_row; //行内容
	my_ulonglong  num_row,num_col , f1,f2;
	string  m_unique_time;
	string  mold_str_name="";//旧的用户账号
    int res;

	string  m_username = GetPostVauleFromKey("userfiled=" , p_data );
	string  m_userpwd = GetPostVauleFromKey("pwdfiled=" , p_data );
	string  m_phone = GetPostVauleFromKey("telfiled=" , p_data );
	string  m_token = GetPostVauleFromKey("token=" , p_data );
	string  m_unique_code =  GetPostVauleFromKey("unique_code_filed=" , p_data );//验证码
	if(m_username.length()>0 && m_userpwd.length()>0&&m_token.length()>0 && m_unique_code.length()>0)
	{

	}
	else
		return -10;

	urldecode((char*)m_username.c_str());//中文编码转换%BF转换成0XBF存储
	urldecode((char*)m_userpwd.c_str());
	urldecode((char*)m_token.c_str());

	cout<<"用户名："<<m_username.c_str()<<endl;
	cout<<"密码："<<m_userpwd.c_str()<<endl;
	cout<<"微信openid："<<m_token.c_str()<<endl;
	m_username = GetUTF8FromGBK(m_username);
	m_userpwd = GetUTF8FromGBK(m_userpwd);
	m_token = GetUTF8FromGBK(m_token);
	//初始化数据库  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//初始化mysql
   {
	   return -11;
   }
    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {
        //mysql_query(&myCont, "SET NAMES utf8"); //设置编码格式
    }
    else
    {
        cout << "connect failed!" << endl;
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		return -2;
    }

	string m_checkuser = "SELECT  *  FROM  forget_code  WHERE token = '" + m_token + "'";
	//cout<< m_checkuser<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		
		if(num_row > 0)
		{
			//getsucess = true;
			
			for(f1=0;f1<1;f1++) //循环行只取了最新的一行
			{		
				mysql_row = mysql_fetch_row(result); //获取每行的内容

				for(f2=0;f2<num_col;f2++) //循环列
				{	
					if (fields[f2].name!=NULL)  
					{						
						if (!strcmp( fields[f2].name , "register_time")) //判断当前列的字段名称
						{
							m_unique_time = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL			
							cout<<"验证码生成时间："<<m_unique_time.c_str()<<endl;
														
							time_t tm_now_time ,tm_unique;  
							tm_unique = convert_string_to_time_t(m_unique_time);//string转时间类
							double dec_value;//时间差值 
							time(&tm_now_time);//获取当前时间  
							dec_value = difftime(tm_now_time ,tm_unique);//计算时间差值，秒级  
							printf("%f\n",dec_value);//
							if(dec_value>0 && dec_value < 300)
							{
								cout<<"验证码有效"<<endl;
								getsucess =true;
							}
							else
							{
								cout<<"验证码过期"<<endl;
								getsucess =false;
							}
						}
					}						
					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据			
				}
			}
		}
		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长
	}
	
	if(getsucess == true)
	{
		
	}
	else
	{	
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		return -2;
	}
	m_checkuser = "SELECT  *  FROM  weixin_user  WHERE token = '" + m_token + "'";
	//cout<< m_checkuser<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{			
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		
		if(num_row > 0)
		{			
			for(f1=0;f1<1;f1++) //循环行只取了最新的一行
			{		
				mysql_row = mysql_fetch_row(result); //获取每行的内容

				for(f2=0;f2<num_col;f2++) //循环列
				{	
					if (fields[f2].name!=NULL)  
					{						
						if (!strcmp( fields[f2].name , "username")) //判断当前列的字段名称
						{
							mold_str_name = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL			
							cout<<"旧用户名："<<mold_str_name.c_str()<<endl;
							getsucess =true;							
						}
					}						
					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据			
				}
			}
		}
		else
		{
			getsucess = false;
		}

		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长
		
	}
	if(getsucess == true)
	{
		
	}
	else
	{	
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		return -3;
	}
	
		
	string  m_reguser = "UPDATE  weixin_user SET username ='" + m_username + 
		"', userpwd ='"+ m_userpwd + "',phone = '"+m_phone
		+"' , register_time = NOW(3)  WHERE token =  '"+ m_token +"'";

	//cout<<m_reguser<<endl;

	res = mysql_query(&myCont, (const  char *)m_reguser.c_str()); //执行SQL语句,添加一条记录

	if(!res  )
	{	
		//cout << "//////////update weixin_user  sucess!///////////" << endl;
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		cout << "Register User error!\n" << endl;
		return -4;
		
	}
    string  m_up_sql = "UPDATE  user_bike SET username ='" + m_username + 
		"',phone = '"+ m_phone
		+"' , register_time = NOW(3)  WHERE username =  '"+ mold_str_name +"'";
	//cout<<m_up_sql<<endl;
	res = mysql_query(&myCont, (const  char *)m_up_sql.c_str()); //执行SQL语句,添加一条记录
	
	if(!res  )
	{
		m_checkuser = "DELETE   FROM  forget_code  WHERE token = '" + m_token + "'";
		//cout<< m_checkuser<<endl;		
		res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //执行SQL语句,通过token查找username
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用//关闭sql连接
		
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
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		cout << "update user_bike error!\n" << endl;
		return -5;		
	}
	
	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

	return -1;
}
///////////////////////////////绑定设备/////////////////////////////////////
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
	MYSQL_ROW  mysql_row; //行内容
	my_ulonglong  f1,f2,num_row,num_col; 
	string   sql_userpwd;
	string   str_pwd,str_username ,str_card, str_email , str_phone,str_bikename,str_bikecolor,str_biketype;  

    str_username = GetPostVauleFromKey("userfiled=" , p_data );//取出form表单内容
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
	urldecode((char *)str_bikename.c_str());//中文编码转换%BF转换成0XBF存储,html对中文编了码
	urldecode((char *)str_bikecolor.c_str());
	urldecode((char *)str_biketype.c_str());
	
	cout<<"车辆名称："<<str_bikename.c_str()<<endl;
	cout<<"车辆颜色："<<str_bikecolor.c_str()<<endl;
	cout<<"车辆品牌："<<str_biketype.c_str()<<endl;
	str_bikename = GetUTF8FromGBK(str_bikename);
	str_bikecolor = GetUTF8FromGBK(str_bikecolor);
	str_biketype = GetUTF8FromGBK(str_biketype);

	//初始化数据库  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
	mysql_init(&myCont);//初始化mysql

    if (mysql_real_connect(&myCont, host, user, pswd, table, port, NULL, 0))
    {
        //mysql_query(&myCont, "SET NAMES utf8"); //设置编码格式   
    }
    else
    {
        cout << "connect failed!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		return -2;
    }

	string  mSQLStr="SELECT *  FROM  cardinfo  WHERE  serialnumber = '" + m_serialnumber + "'";
	
	cout<< mSQLStr<<endl;
	//return -4;
	///////////////////////////////////////////////////////////////////////
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //执行SQL语句
	
	if(!res  )
	{	
        //保存查询到的数据到result
        result = mysql_store_result(&myCont);

		num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if(num_row >=1)
		{
			getsucess = true ;
			mysql_row = mysql_fetch_row(result); //获取每行的内容

			for(int f2=0;f2<num_col;f2++) //循环列
			{	
				if (fields[f2].name!=NULL)  
				if (!strcmp( fields[f2].name , "card")) //判断当前列的字段名称
				{
					str_card = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL
					//cout<<mstr_pwd.data()<<endl;
				}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据
			
			}
	
		}
		else
		{
			getsucess = false;
		}
		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长

	}
	if(getsucess == true)
	{
		
	}
	else
	{
		cout << "check card is not exist!\n" << endl;
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		return -4;
	}
	//string  mstr_ziduan = mJsonValue["token"].asString();

	string  m_strToken = "SELECT  userpwd  FROM  weixin_user  WHERE username = '" + str_username + "'";
	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
///////////////////////////////////////////////////////////////////////////////////
	//cout<<m_strToken<<endl;
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{			
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组

		for(f1=0;f1<num_row;f1++) //循环行
		{		
			mysql_row = mysql_fetch_row(result); //获取每行的内容

			for(f2=0;f2<num_col;f2++) //循环列
			{	
				if (fields[f2].name!=NULL)  
				if (!strcmp( fields[f2].name , "userpwd")) //判断当前列的字段名称
				{
					sql_userpwd = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL
					//cout<<str_username.data()<<endl;
					if(0==str_pwd.compare( sql_userpwd))
					{
						getsucess = true;
						//cout<<sql_userpwd.data()<<endl;
						break;
					}
					//
				}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据
			
			}
		}

		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长
		if(getsucess == false)
		{
			mysql_close(&myCont);
			mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
			cout << "select userpwd error!\n" << endl;
			return -4;
		}
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		cout << "select userpwd error!\n" << endl;
		return -4;
	}
	
	string  m_checkuser = "SELECT  *  FROM  user_bike  WHERE card = '" + str_card + "'";
	cout<< m_checkuser<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_checkuser.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{			
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if(num_row >=1)
		{
			getsucess = false;//绑定列表已经存在该设备
		}
		else
		{
			getsucess = true;
		}

		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长
		
	}
	if(getsucess == true)
	{
		
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		cout << "check card already existed !\n" << endl;
		return -4;
	}
///////////////////////////////////////////////////////////////////////	
	//INSERT 会新增一条
	//on duplicate key update  需要设置一个主键，不自动增长，遇到主键冲突，执行后面的updata内容
	 mSQLStr = "INSERT  INTO  user_bike(  username, card , email , phone,bikename,bikecolor,biketype,register_time)   VALUES( '"
		+ str_username + "','"+ str_card + "','"+str_email +"','" + str_phone + "','"+ str_bikename + "','"+ str_bikecolor+ "','"+ str_biketype
		+ "', NOW(3))" ;
	cout<<mSQLStr<<endl;
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //执行SQL语句,添加一条记录
	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
	if(!res  )
	{			
		
		//GetLocalTime( &sys ); 
		//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
		cout << "//////////bind device sucess!/////////////\n" << endl;
		return 0;  		
	}
	else
	{
		cout << "bind user_bike error!\n" << endl;
		return -3;
		
	}
	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

	return -1;

}
///////////////////////////添加客服账号////////////////////////////////////////////
int  WX_AddCustom(string  kf_account , string  nickname)
{
	Json::Value  m_json;
	//char *revData = buf;
	//char MsgContent[3*1024];
	int ref_len=0;
	//char ref_buf[HTTP_BUF_SIZE+5];
	WSADATA wsa_data;
    SOCKET  http_sock = 0;         /* socket 句柄 */
    struct sockaddr_in serv_addr;  /* 服务器地址 */
   // struct hostent *host_ent;
   // char  user_openid[200];
    int result = 0, send_len;
    char data_buf[HTTP_BUF_SIZE+5];
    char rev_buf[HTTP_BUF_SIZE+5];
    unsigned short port = HTTP_DEF_PORT; //NGINX正向代理设置了8088端口转https
    //unsigned long addr;
    string  card_name , lock_set;
	WSAStartup(MAKEWORD(2,0), &wsa_data); /* 初始化 WinSock 资源 */  

    /* 服务器地址 */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//本机nginx正向代理

    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* 创建 socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));//建立socket
    if (result == SOCKET_ERROR) /* 连接失败 */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
		WSACleanup();
        return -1; 
    }
	
	m_json["kf_account"] = kf_account;
	m_json["nickname"] = nickname;
	Json::FastWriter  fast_writer;//查看json内容对象
	string json_str = fast_writer.write(m_json); //json转string
	/* 向服务器发网页授权HTTP 请求 */
	memset(data_buf , 0, sizeof(data_buf));
	
	send_len = sprintf(data_buf, http_post_req_hdr, wx_add_custom , WX_API_URL , json_str.length());//拼接http参数
	string req_str = data_buf;
	req_str+=json_str;
	send_len+=json_str.length();
	result = send(http_sock, req_str.c_str(), send_len, 0);//发送http请求体GET 
	cout<<"向微信发送添加客服账号请求:\r\n"<<req_str.c_str()<<endl;
	if (result == SOCKET_ERROR) /* 发送失败 */
	{
		printf("[Web] fail to send, error = %d\n", WSAGetLastError());
		closesocket(http_sock);  
		WSACleanup();
		return -1; 
	}

	do /* 接收响应并保存到文件中 */
	{
		result = recv(http_sock, rev_buf, HTTP_BUF_SIZE, 0);
		if (result > 0)
		{
			/* 在屏幕上输出 */
			rev_buf[result] = 0;
			printf("微信添加客服账号响应:%s\r\n", rev_buf);
			
		}

	} while(result > 0);
			
	
	
    closesocket(http_sock);  
	WSACleanup();

    return 0;
	
}
//////////////////////////邀请绑定客服账号/////////////////////////////////////
int  WX_BindCustom(string  kf_account , string  invite_wx)
{
	
	return 0;
}
/////////////////////////微信获取ACCESS_TOKEN//////////////////////////////////////////
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
    SOCKET  http_sock = 0;         /* socket 句柄 */
    struct sockaddr_in serv_addr;  /* 服务器地址 */
   // struct hostent *host_ent;
    //char  user_openid[200];
    int result = 0, send_len;
    char data_buf[HTTP_BUF_SIZE+5];
    char rev_buf[HTTP_BUF_SIZE+5];
    unsigned short port = HTTP_DEF_PORT; //NGINX正向代理设置了8088端口转https
    //unsigned long addr;
    string  card_name , lock_set;
	WSAStartup(MAKEWORD(2,0), &wsa_data); /* 初始化 WinSock 资源 */  

    /* 服务器地址 */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//本机nginx正向代理

    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* 创建 socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));//建立socket
    if (result == SOCKET_ERROR) /* 连接失败 */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
		WSACleanup();
        return -1; 
    }
	//string  str_content="您的设备报警了，快去查看吧！";
	//str_content = GetUTF8FromGBK(str_content);
	
	/* 向服务器发网页授权HTTP 请求 */
	memset(data_buf , 0, sizeof(data_buf));
	memset(wx_token_ref, 0, sizeof(wx_token_ref));
	sprintf( wx_token_ref ,wx_get_accesstoken_ref , WX_ID ,WX_SECRET);//填充access_token
	send_len = sprintf(data_buf, http_req_hdr_tmpl, wx_token_ref , WX_API_URL );//拼接http参数
	
	result = send(http_sock, data_buf, send_len, 0);//发送http请求体GET 
	
	if (result == SOCKET_ERROR) /* 发送失败 */
	{
		printf("[Web] fail to send, error = %d\n", WSAGetLastError());
		closesocket(http_sock);  
		WSACleanup();
		return -1; 
	}

	do /* 接收响应并保存到文件中 */
	{
		result = recv(http_sock, rev_buf, HTTP_BUF_SIZE, 0);
		if (result > 0)
		{
			/* 在屏幕上输出 */
			rev_buf[result] = 0;
			printf("微信access_token响应:%s\r\n", rev_buf);
			if(ps = strstr( rev_buf , "\"access_token\":\"") )
			{
				pd=strstr(ps , "\",\"");
				if(pd)
				{
					int head_len = strlen("\"access_token\":\"");
					memcpy(WX_access_token ,ps+head_len , pd-ps-head_len );//存储微信ACCESS_TOKEN
				}
			}
		}

	} while(result > 0);
			
		
    closesocket(http_sock);  
	WSACleanup();

    return 0;
}
/////////////////////////微信模板消息发送//////////////////////////////////////////
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
    SOCKET  http_sock = 0;         /* socket 句柄 */
    struct sockaddr_in serv_addr;  /* 服务器地址 */
    //struct hostent *host_ent;
    //char  user_openid[200];
    int result = 0, send_len;
    char data_buf[HTTP_BUF_SIZE+5];
    char rev_buf[HTTP_BUF_SIZE+5];
    unsigned short port = HTTP_DEF_PORT; //NGINX正向代理设置了8088端口转https
    //unsigned long addr;
    string  card_name , lock_set;
	WSAStartup(MAKEWORD(2,0), &wsa_data); /* 初始化 WinSock 资源 */  

    /* 服务器地址 */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//本机nginx正向代理

    http_sock = socket(AF_INET, SOCK_STREAM, 0); /* 创建 socket */
    result = connect(http_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));//建立socket
    if (result == SOCKET_ERROR) /* 连接失败 */
    {
        closesocket(http_sock);
        printf("[Web] fail to connect, error = %d\n", WSAGetLastError());
		WSACleanup();
        return -1; 
    }
	//string  str_content="您的设备报警了，快去查看吧！";
	//str_content = GetUTF8FromGBK(str_content);
	m_json["touser"] = touser;
	m_json["template_id"] = template_id;
	m_json["url"] = url;
	m_json["data"]= mjson_content;
	Json::FastWriter  fast_writer;//查看json内容对象
	string json_str = fast_writer.write(m_json); //json转string
	/* 向服务器发网页授权HTTP 请求 */
	memset(data_buf , 0, sizeof(data_buf));
	sprintf( wx_latemsg_ref ,wx_send_template_ref , WX_access_token);//填充access_token
	send_len = sprintf(data_buf, http_post_req_hdr, wx_latemsg_ref , WX_API_URL , json_str.length());//拼接http参数
	string req_str = data_buf;
	req_str+=json_str;
	send_len+=json_str.length();
	result = send(http_sock, req_str.c_str(), send_len, 0);//发送http请求体GET 
	cout<<"向微信发送添加客服账号请求:\r\n"<<req_str.c_str()<<endl;
	if (result == SOCKET_ERROR) /* 发送失败 */
	{
		printf("[Web] fail to send, error = %d\n", WSAGetLastError());
		closesocket(http_sock);  
		WSACleanup();
		return -1; 
	}

	do /* 接收响应并保存到文件中 */
	{
		result = recv(http_sock, rev_buf, HTTP_BUF_SIZE, 0);
		if (result > 0)
		{
			/* 在屏幕上输出 */
			rev_buf[result] = 0;
			printf("微信添加客服账号响应:%s\r\n", rev_buf);
			closesocket(http_sock);
			WSACleanup();
			return 0;
		}

	} while(result > 0);
			
	
	
    closesocket(http_sock);  
	WSACleanup();

    return 0;
}

//////////////////////查找等待硬件报警列表，推送给微信//////////////////////////////////////////
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
	MYSQL_ROW  mysql_row; //行内容
	my_ulonglong  f1,f2,num_row,num_col; 
    int res;

	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//初始化数据库  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//初始化mysql
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
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		return -2;
    }
	cout<<m_strToken<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{			
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if(num_row > 0)
		{
			string str_title= GetUTF8FromGBK("您的设备报警了");
			Json::Value  m_json;
			m_json["value"]=str_title;
			m_json["color"]="#0000FF";
			m_wx_content["first"] = m_json;

			for(f1=0;f1<num_row;f1++) //循环行只取了最新的一行
			{		
				mysql_row = mysql_fetch_row(result); //获取每行的内容

				for(f2=0;f2<num_col;f2++) //循环列
				{	
					if (fields[f2].name!=NULL)  
					{	
						if (!strcmp( fields[f2].name , "card")) //判断当前列的字段名称
						{
							str_card = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL			
							
						}
						if (!strcmp( fields[f2].name , "time")) //判断当前列的字段名称
						{
							alarm_time = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL			
							getsucess =true;
							Json::Value  m_json;
							m_json["value"]=alarm_time;
							m_json["color"]="#0000FF";
							m_wx_content["alarm_time"] = m_json;
						}
						if (!strcmp( fields[f2].name , "card_state")) //判断当前列的字段名称
						{
							alarm_state = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL			
							getsucess =true;
							Json::Value  m_json;
							m_json["value"]=alarm_state;
							m_json["color"]="#0000FF";
							m_wx_content["alarm_state"] = m_json;
						}
					}					
					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据			
				}//for列循环
				m_strToken = "SELECT  *  FROM  user_bike  WHERE card = '" + str_card + "' ";
				cout<<m_strToken<<endl;
				res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //执行SQL语句,通过token查找username
				if(!res  )
				{
					//保存查询到的数据到result
					MYSQL_RES * user_result = mysql_store_result(&myCont);
					int  num_row=mysql_num_rows(user_result); //读取行数
					int  num_col=mysql_num_fields(user_result); //读取列数
					//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
					MYSQL_FIELD* fields = mysql_fetch_fields(user_result); //返回所有字段结构的数组
					if(num_row > 0)
					{
						MYSQL_ROW  userbike_row = mysql_fetch_row(user_result); //获取每行的内容
						for(int f2=0;f2<num_col;f2++) //循环列
						{	
							if (!strcmp( fields[f2].name , "username")) //判断当前列的字段名称
							{
								str_username = getNullStr(userbike_row[f2]); //获取字段内容，里面判断非NULL			
							}
							if (!strcmp( fields[f2].name , "bikename")) //判断当前列的字段名称
							{
								bikename = getNullStr(userbike_row[f2]); //获取字段内容，里面判断非NULL			
								Json::Value  m_json;
								m_json["value"]=bikename;
								m_json["color"]="#0000FF";
								m_wx_content["bikename"] = m_json;
							}

						}	
						Json::Value  m_json;
						m_json["value"]=GetUTF8FromGBK("这不是演习，是真实的硬件报警信息推送！");
						m_json["color"]="#FF0000";
						m_wx_content["alarm_msg"] = m_json;
						
						m_strToken = "SELECT  token  FROM  weixin_user  WHERE username = '" + str_username + "' ";
						
						res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //执行SQL语句,通过token查找username
						if(!res  )
						{
							cout<<m_strToken<<endl;
							//保存查询到的数据到result
							MYSQL_RES * token_result = mysql_store_result(&myCont);
							int  num_row=mysql_num_rows(token_result); //读取行数
							int  num_col=mysql_num_fields(token_result); //读取列数
							MYSQL_ROW  token_row = mysql_fetch_row(token_result); //获取每行的内容
						
							//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
							MYSQL_FIELD* fields = mysql_fetch_fields(token_result); //返回所有字段结构的数组
							if(num_row > 0)
							{
								wx_openid = getNullStr(token_row[0]); //获取字段内容，里面判断非NULL									
								WX_SendAlarmTemplateMsg(wx_openid , WX_TEMPLATE_ID , "http://wxweb.shaohaoqi.cn/login.html" ,m_wx_content);
							}
							mysql_free_result(token_result);
						}
					}
					mysql_free_result(user_result);
				}//查询user_bike
			#if  1
				//删除已经推送的报警列表
				m_strToken = "DELETE   FROM  card_alarm   WHERE card = '" + str_card + "' ORDER BY time ASC LIMIT 1 ";
				res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //执行SQL语句,通过token查找username
				if(!res  )
				{		

				}
			#endif

			}//行循环
		}
		
		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长

	}
	
	if(getsucess ==true)
	{
	
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		cout << "select alarm_weilan error!\n" << endl;
		return -4;
	}

	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

	return -1;

}

//////////////////////查找等待硬件报警列表，发送报警邮件/////////////////////////////////////////
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
	MYSQL_ROW  mysql_row; //行内容
	my_ulonglong  f1,f2,num_row,num_col; 
    int res;

	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 
	//初始化数据库  
	if (0 == mysql_library_init(0, NULL, NULL))
	{
		cout << "mysql_library_init() succeed" << endl;
	}
	else
	{
		cout << "mysql_library_init() failed" << endl;
		return -1;
	}
   if (mysql_init(&myCont) == NULL)//初始化mysql
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
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用
		return -2;
    }
	cout<<m_strToken<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //执行SQL语句,通过token查找username
	if(!res  )
	{			
		//保存查询到的数据到result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //读取行数
		num_col=mysql_num_fields(result); //读取列数
		//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //返回所有字段结构的数组
		if(num_row > 0)
		{
			string str_title= GetUTF8FromGBK("您的设备报警了");
			Json::Value  m_json;
			m_json["value"]=str_title;
			m_json["color"]="#0000FF";
			m_wx_content["first"] = m_json;

			for(f1=0;f1<num_row;f1++) //循环行只取了最新的一行
			{		
				mysql_row = mysql_fetch_row(result); //获取每行的内容

				for(f2=0;f2<num_col;f2++) //循环列
				{	
					if (fields[f2].name!=NULL)  
					{	
						if (!strcmp( fields[f2].name , "card")) //判断当前列的字段名称
						{
							str_card = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL			
							
						}
						if (!strcmp( fields[f2].name , "time")) //判断当前列的字段名称
						{
							alarm_time = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL			
							getsucess =true;
							Json::Value  m_json;
							m_json["value"]=alarm_time;
							m_json["color"]="#0000FF";
							m_wx_content["alarm_time"] = m_json;
						}
						if (!strcmp( fields[f2].name , "card_state")) //判断当前列的字段名称
						{
							alarm_state = getNullStr(mysql_row[f2]); //获取字段内容，里面判断非NULL			
							getsucess =true;
							Json::Value  m_json;
							m_json["value"]=alarm_state;
							m_json["color"]="#0000FF";
							m_wx_content["alarm_state"] = m_json;
						}
					}					
					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //打印每行的各个列数据			
				}//for列循环
				m_strToken = "SELECT  *  FROM  user_bike  WHERE card = '" + str_card + "' ";
				cout<<m_strToken<<endl;
				res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //执行SQL语句,通过token查找username
				if(!res  )
				{
					//保存查询到的数据到result
					MYSQL_RES * user_result = mysql_store_result(&myCont);
					int  num_row=mysql_num_rows(user_result); //读取行数
					int  num_col=mysql_num_fields(user_result); //读取列数
					//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
					MYSQL_FIELD* fields = mysql_fetch_fields(user_result); //返回所有字段结构的数组
					if(num_row > 0)
					{
						MYSQL_ROW  userbike_row = mysql_fetch_row(user_result); //获取每行的内容
						for(int f2=0;f2<num_col;f2++) //循环列
						{	
							if (!strcmp( fields[f2].name , "username")) //判断当前列的字段名称
							{
								str_username = getNullStr(userbike_row[f2]); //获取字段内容，里面判断非NULL			
							}
							if (!strcmp( fields[f2].name , "bikename")) //判断当前列的字段名称
							{
								str_bikename = getNullStr(userbike_row[f2]); //获取字段内容，里面判断非NULL			
								Json::Value  m_json;

								m_json["value"]= str_bikename;
								m_json["color"]="#0000FF";
								m_wx_content["bikename"] = m_json;
							}
							if (!strcmp(fields[f2].name, "email")) //判断当前列的字段名称
							{
								m_wx_content["email"]  = getNullStr(userbike_row[f2]); //获取字段内容，里面判断非NULL			
							}

						}	
						Json::Value  m_json;
						m_json["value"]=GetUTF8FromGBK("这不是演习，是真实的硬件报警信息推送！");
						m_json["color"]="#FF0000";
						m_wx_content["alarm_msg"] = m_json;

						Sleep(1000);
						
						string  str_email ;
						if (GetVaule_CheckStr(&str_email , m_wx_content, "email") == 0)
						{
							cout << "json数据类型错误FromUserName应该为string" << endl;
							return  -5;
						}
						
						Json::Value  m_json_email;
						m_json_email["bikename"] = str_bikename;
						m_json_email["state"] = alarm_state;
						m_json_email["alarm_time"] = alarm_time;
						//发送报警邮件
						Email_SMTP_send_simple(mEmailSender.smtp_server, mEmailSender.Email, mEmailSender.EmailAccount, 
							mEmailSender.EmailPwd , str_email, m_json_email);
						
						m_strToken = "SELECT  token  FROM  weixin_user  WHERE username = '" + str_username + "' ";
						
						res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //执行SQL语句,通过token查找username
						if(!res  )
						{
							cout<<m_strToken<<endl;
							//保存查询到的数据到result
							MYSQL_RES * token_result = mysql_store_result(&myCont);
							int  num_row=mysql_num_rows(token_result); //读取行数
							int  num_col=mysql_num_fields(token_result); //读取列数
							MYSQL_ROW  token_row = mysql_fetch_row(token_result); //获取每行的内容
						
							//printf("row: %d,col: %d\n",num_row,num_col);//打印行列个数
							MYSQL_FIELD* fields = mysql_fetch_fields(token_result); //返回所有字段结构的数组
							if(num_row > 0)
							{
								wx_openid = getNullStr(token_row[0]); //获取字段内容，里面判断非NULL									
								WX_SendAlarmTemplateMsg(wx_openid , WX_TEMPLATE_ID , "http://wxweb.shaohaoqi.cn/login.html" ,m_wx_content);
							}
							mysql_free_result(token_result);
						}
					}
					mysql_free_result(user_result);
				}//查询user_bike
			#if  1
				//删除已经推送的报警列表
				m_strToken = "DELETE   FROM  card_alarm   WHERE card = '" + str_card + "' ORDER BY time ASC LIMIT 1 ";
				res = mysql_query(&myCont, (const  char *)m_strToken.c_str()); //执行SQL语句,通过token查找username
				if(!res  )
				{		

				}
			#endif

			}//行循环
		}
		
		mysql_free_result(result); //释放缓存，特别注意，不释放会导致内存增长

	}
	
	if(getsucess ==true)
	{
	
	}
	else
	{
		mysql_close(&myCont);
		mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

		cout << "报警列表没有信息!\n" << endl;
		return -4;
	}

	mysql_close(&myCont);
	mysql_library_end();//，记得在 mysql_close 之后调用 mysql_library_end() 来释放未被释放的内存.mysql_library_init要配合使用

	return -1;

}

//////////////////////////////////////////////////////////////////


