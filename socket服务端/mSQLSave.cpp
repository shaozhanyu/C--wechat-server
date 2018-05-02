
#include "mcom.h"
#include "string.h"


#ifdef  WIN_64_DEF
#include <mysql.h>
#endif

#ifdef  WIN_64_DEF
#pragma comment(lib,"libmysql.lib") 
#endif


#ifdef  WIN_64_DEF
//////////////////////////���¿�״̬/////////////////////////////////////////////
int  APPUpdateUserState(SOCKET   ClientS ,Json::Value  mJsonValue)
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
	char bike_gps[150]=",A,3158.4608,N,11848.3737,E,10.05";
	struct tm  ;  

	string   tos ,str_token,str_card , str_username;  
	string  str_lock;

	tos = to_string((long)ClientS);	

	if(mJsonValue["setlock"] == 1)
		str_lock = "1";
	else if(mJsonValue["setlock"] == 0)
		str_lock = "0";

	if( GetVaule_CheckStr(&str_token , mJsonValue , "token") == 0 )
	{
		cout<< "json�������ʹ���tokenӦ��Ϊstring"<<endl;
		return  -5;
	}	

	if( GetVaule_CheckStr(&str_card , mJsonValue , "card") == 0 )
	{
		cout<< "json�������ʹ���cardӦ��Ϊstring"<<endl;
		return  -5;
	}
	
	string  m_strToken = "SELECT  username  FROM  register_user  WHERE token = '" + str_token + "' ";

	unsigned int port = 3306;        
        
    MYSQL myCont;
    MYSQL_RES *result;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1,f2,num_row,num_col; 
    int res;

	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 

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
		return -2;
    }

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
		if(num_row > 0)
		{
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
						f1=num_row;
						f2=num_col;
						getsucess =true;
						break;
						//cout<<mstr_pwd.data()<<endl;
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
		cout << "select username error!\n" << endl;
		return -4;
	}
///////////////////////////////////////////////////////////////////////
	//INSERT ������һ��
	//string  mSQLStr = "INSERT  INTO user_set(username, token,app_socket,setlock,card,update_card,time)   VALUES('"
	//	+ str_username + "','"+ str_token + "',"+ tos + ","+ str_lock + ",'"+ str_card+ "','1', NOW())" ;

	//on duplicate key update  ��Ҫ����һ�����������Զ�����������������ͻ��ִ�к����updata����
	string  mSQLStr = "INSERT  INTO user_set(user_id , username, token,app_socket,setlock,card,update_card,time)   VALUES( 1 , '"
		+ str_username + "','"+ str_token + "',"+ tos + ","+ str_lock + ",'"+ str_card+ "',1, NOW()) on duplicate key update  user_id = " + 
		"1 ,username = '"  + str_username + "' , token =  '" + str_token + "' , app_socket = " + tos + ", setlock = " + str_lock +", card = '"+ 
		str_card  + "', update_card = 2 , time = NOW() " ;


	//string  mSQLStr="UPDATE user_set  SET username = '" + str_username + "' , token =  '" + str_token + "' , app_socket = " + tos + ", setlock = " + str_lock + ", card = '"+ str_card  +
	//	"', update_card = 2 , time = NOW() " ;

	//cout<<mSQLStr<<endl;

	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���
	mysql_close(&myCont);
	if(!res  )
	{			
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("sucess"); // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��	
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring
		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
	
		GetLocalTime( &sys ); 
		printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 

		return 0;  		
	}
	else
	{
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("lock_error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
			
		return -3;
	}

	cout << "updateSQL error!\n" << endl;

    mysql_close(&myCont);

	return -1;

}

////////////////////APP�����֤-token��֤//////////////////////////////
/////////////////////token��ѯ�ȶ�///////////////////////////////////
int  FindToken(string  token)
{
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";    

	string  mSQLStr="SELECT count(*) FROM register_user WHERE token = '" + token + "'";
	
	//cout<< mSQLStr<<endl;

	unsigned int port = 3306;        
        
    MYSQL myCont;
    MYSQL_RES  *result ;
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
		return -2;
    }
	///////////////////////////////////////////////////////////////////////
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���
	
	if(!res  )
	{	
        //�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        my_ulonglong  rowcount = mysql_num_rows(result);

		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

		if ( rowcount >0 )
		{
			mysql_close(&myCont);		
			return 0;
		}
		else
		{
			mysql_close(&myCont);
			return -3;
		}    
		
	}

	mysql_close(&myCont);

	return -1;

}

#endif
//////////////////////////////////////////////////////////////////
//���ݿ�nullֵת��"""
string  getNullStr(const char* str ) 
{
    string res="";
    if (str!=NULL)
    {    res=str;
     }
    return res;
} 

////////////////////////////////////////////////////////////////////

int  GetVaule_CheckStr(string *str , Json::Value  mJsonValue , string  str_value)
{
	string  str_key =  str_value ;

	if(mJsonValue[str_value].isString())
	{
		*str = mJsonValue[str_value].asString();	
	}
	else
	{		
		return  0;

	}
	return 1;
}





