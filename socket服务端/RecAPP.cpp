#include "SocketAPP.h"
#include "TestTask.h"
#include "stdio.h"
#include "SaveQueue.h"
#include "mcom.h"

#ifdef  WIN_64_DEF
#include <mysql.h>
#endif

#ifdef  WIN_64_DEF
#pragma comment(lib,"libmysql.lib") 
#endif

/**************************�ַ���תJSON����******************************************/
int  ParseJsonFromString(char *mstr , Json::Value  *mJson)    
{    
	const char* str1 = "{\"uploadid\": \"UP000000\",\"code\": 100,\"msg\": \"\",\"files\": \"\"}";    
    
	Json::Reader reader;     
	if (!reader.parse(mstr, *mJson))  // reader��Json�ַ���������root��root������Json��������Ԫ��     
	return  -1;	

	return 0;    
}  

/*************************JSON����ƴװ************************************/
int ParseJsonFromFile(const char* filename)
{
	Json::Value   json_temp;      // ��ʱ���󣬹����´���ʹ��
	json_temp["name"] = Json::Value("huchao");
	json_temp["age"] = Json::Value(26);

	Json::Value root;             // ��ʾ���� json ����
	root["key_string"] = Json::Value("value_string");     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
	root["key_number"] = Json::Value(12345);             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
	root["key_boolean"] = Json::Value(false);             // �½�һ�� Key����Ϊ��key_boolean��������boolֵ��false��
	root["key_double"] = Json::Value(12.345);             // �½�һ�� Key����Ϊ��key_double�������� double ֵ��12.345��
	root["key_object"] = json_temp;                       // �½�һ�� Key����Ϊ��key_object�������� json::Value ����ֵ��
	root["key_array"].append("array_string");             // �½�һ�� Key����Ϊ��key_array��������Ϊ���飬�Ե�һ��Ԫ�ظ�ֵΪ�ַ�����"array_string"��
	root["key_array"].append(1234);                       // Ϊ���� key_array ��ֵ���Եڶ���Ԫ�ظ�ֵΪ��1234��
	
	Json::ValueType type = root.type();                   // ��� root �����ͣ��˴�Ϊ objectValue ���͡�
 
	Json::Reader reader;
	Json::Value json_object;
	const char* json_document = "{\"age\" : 26,\"name\" : \"123\"}";
	if (!reader.parse(json_document, json_object))
	return 0;
	std::cout << json_object["name"] << std::endl;
	std::cout << json_object["age"] << std::endl;

	return 0;
}
////////////////////////////////////////////////////////////////////////
int   TranslateJson(const string strData)
{
    // ����json��Json::Reader
    Json::Reader *readerinfo = new Json::Reader(Json::Features::strictMode());
    // Json::Value��һ�ֺ���Ҫ�����ͣ����Դ����������͡���int, string, object, array...
    Json::Value root;       
    if (readerinfo->parse(strData, root))
    {
        //arrayObj.append(new_item);
        if (root.isArray())
        {
            int nArraySize = root.size();   
            for (int i=0; i<nArraySize; i++)
            {       
                int nID = root[i]["id"].asInt();
                std::string strName = root[i]["name"].asString();   
            }
        }


        // arrayObj["array"].append(new_item);
        if (root["array"].isArray())
        {
            int nArraySize = root["array"].size();  
            for (int i=0; i<nArraySize; i++)
            {       
                int nID = root["array"][i]["id"].asInt();
                std::string strName = root["array"][i]["name"].asString();  
            }
        }
    }

    ::delete readerinfo;
    readerinfo = NULL;
	return  0;
}
/**************
char   RecAPPData
���ս����������·�����
*****/
char   RecAPPData( SOCKET ClientS, unsigned char * src, unsigned  int  len )
{
	unsigned  char decdata[6]={ 0x96,0x69,0x96,0x69};
	char  loop=1;
	unsigned  char *pstr;
	unsigned  char *srcdat=src;
	unsigned  char *pdata = src;
	long  DataLen=0;
	SocketRecUnit   mRECdataS; //���ݻ���ṹ�壬����socket����
	EnterCriticalSection(&mAPP_RECLock);//����
	memcpy( mRECdataS.RecData , src  ,len );
	mRECdataS.DataLen = len;
	LeaveCriticalSection(&mAPP_RECLock);//����
	srcdat=mRECdataS.RecData;
	pdata = srcdat;
	DataLen = mRECdataS.DataLen;

	if(DataLen >0 && DataLen < REC_SIZE)
	{	
		while(loop)
		{
			 //ע��:APP������û���������Ķ������ݸ�����ÿ���������ݵĳ��ȣ�����2���ֽ�
			if( (DataLen-(pdata-srcdat)) >= APP_MIN_DATA_LEN )
			{
				

				pstr = FindByteBuff(pdata , (DataLen-(pdata-srcdat)) , decdata , 4);//��������ͷ
				if(pstr!=0)//���ҵ���ͷ
				{		
					pdata=pstr;
					if( DataLen-(pdata-srcdat)- 
						(DATA_HEAD_LEN+*(pdata+14)*256+ *(pdata+15)+DATA_END_LEN) >= 0 )//�ж�ʣ�����ݳ����Ƿ�һ������
					{
						switch (*(pdata+4))//�ж�����ͷ
						{								
							case  APP_REQUEST_WEILAN_ALARM:
								//cout<<"��ѯΧ������\n"<<endl;
								SaveAPPData( ClientS , *(pdata+4) ,(char*)(pdata+DATA_HEAD_LEN) , *(pdata+14)*256+ *(pdata+15) );//������������

								if( DataLen-(pdata-srcdat)- 
									(DATA_HEAD_LEN+ *(pdata+14)*256 + *(pdata+15)+DATA_END_LEN) >= APP_MIN_DATA_LEN )//�ж�ʣ�����ݳ���
								pdata = pdata+DATA_HEAD_LEN+*(pdata+14)*256+ *(pdata+15)+DATA_END_LEN;
								else
								loop=0;
								break;
							case  APP_REQUEST_WEILAN_DATA:
								//cout<<"��ѯΧ������\n"<<endl;
								SaveAPPData( ClientS , *(pdata+4) ,(char*)(pdata+DATA_HEAD_LEN) , *(pdata+14)*256+ *(pdata+15) );//������������

								if( DataLen-(pdata-srcdat)- 
									(DATA_HEAD_LEN+ *(pdata+14)*256 + *(pdata+15)+DATA_END_LEN) >= APP_MIN_DATA_LEN )//�ж�ʣ�����ݳ���
								pdata = pdata+DATA_HEAD_LEN+*(pdata+14)*256+ *(pdata+15)+DATA_END_LEN;
								else
								loop=0;
								break;
							case  APP_SET_LOCK :	//����Χ��					
								SaveAPPData( ClientS , *(pdata+4) ,(char*)(pdata+DATA_HEAD_LEN) , *(pdata+14)*256+ *(pdata+15) );//������������

								if( DataLen-(pdata-srcdat)- 
									(DATA_HEAD_LEN+ *(pdata+14)*256 + *(pdata+15)+DATA_END_LEN) >= APP_MIN_DATA_LEN )//�ж�ʣ�����ݳ���
								pdata = pdata+DATA_HEAD_LEN+*(pdata+14)*256+ *(pdata+15)+DATA_END_LEN;
								else
								loop=0;					
							break;
							case   APP_CONFIG_WEILAN:  //��ͣΧ��
							case   APP_SET_UNLOCK:								
								SaveAPPData( ClientS , *(pdata+4) ,(char*)(pdata+DATA_HEAD_LEN) , *(pdata+14)*256+ *(pdata+15) );//������������

								if( DataLen-(pdata-srcdat)- 
									(DATA_HEAD_LEN+ *(pdata+14)*256 + *(pdata+15)+DATA_END_LEN) >= APP_MIN_DATA_LEN )//�ж�ʣ�����ݳ���
								pdata = pdata+DATA_HEAD_LEN+*(pdata+14)*256+ *(pdata+15)+DATA_END_LEN;
								else
								loop=0;	
							break;
							
							case  APP_REGESTER_USER:  //APPע���˺�
								responseAPPregester( ClientS , *(pdata+4) ,(char*)(pdata+DATA_HEAD_LEN) , *(pdata+14)*256+ *(pdata+15) );
								
								if( DataLen-(pdata-srcdat)- 
									(DATA_HEAD_LEN+ *(pdata+14)*256 + *(pdata+15)+DATA_END_LEN) >= APP_MIN_DATA_LEN )//�ж�ʣ�����ݳ���
								pdata = pdata+DATA_HEAD_LEN+*(pdata+14)*256+ *(pdata+15)+DATA_END_LEN;
								else
								loop=0;
								break;
								break;
							case  APP_SET_DENG_LU :          //APP��¼����
								responseAPPdenglu( ClientS , *(pdata+4) ,(char*)(pdata+DATA_HEAD_LEN) , *(pdata+14)*256+ *(pdata+15) );
								
								if( DataLen-(pdata-srcdat)- 
									(DATA_HEAD_LEN+ *(pdata+14)*256 + *(pdata+15)+DATA_END_LEN) >= APP_MIN_DATA_LEN )//�ж�ʣ�����ݳ���
								pdata = pdata+DATA_HEAD_LEN+*(pdata+14)*256+ *(pdata+15)+DATA_END_LEN;
								else
								loop=0;
								break;
							case  APP_SET_ZHUXIAO  :         //APPע������

								break;
							case  APP_SET_BANGDING :         //APP���豸����
								responseAPPbind( ClientS , *(pdata+4) ,(char*)(pdata+DATA_HEAD_LEN) , *(pdata+14)*256+ *(pdata+15) );//������������

								if( DataLen-(pdata-srcdat)- 
									(DATA_HEAD_LEN+ *(pdata+14)*256 + *(pdata+15)+DATA_END_LEN) >= APP_MIN_DATA_LEN )//�ж�ʣ�����ݳ���
								pdata = pdata+DATA_HEAD_LEN+*(pdata+14)*256+ *(pdata+15)+DATA_END_LEN;
								else
								loop=0;	
								break;
							case  APP_SET_SHANCHU  :         //APPɾ���豸����
								SaveAPPData( ClientS , *(pdata+4) ,(char*)(pdata+DATA_HEAD_LEN) , *(pdata+14)*256+ *(pdata+15) );//������������

								if( DataLen-(pdata-srcdat)- 
									(DATA_HEAD_LEN+ *(pdata+14)*256 + *(pdata+15)+DATA_END_LEN) >= APP_MIN_DATA_LEN )//�ж�ʣ�����ݳ���
								pdata = pdata+DATA_HEAD_LEN+*(pdata+14)*256+ *(pdata+15)+DATA_END_LEN;
								else
								loop=0;	
								
								break;
							case  APP_SET_GET_STATE :        //APP��ѯ�豸״̬����
								SaveAPPData( ClientS , *(pdata+4) ,(char*)(pdata+DATA_HEAD_LEN) , *(pdata+14)*256+ *(pdata+15) );//������������

								if( DataLen-(pdata-srcdat)- 
									(DATA_HEAD_LEN+ *(pdata+14)*256 + *(pdata+15)+DATA_END_LEN) >= APP_MIN_DATA_LEN )//�ж�ʣ�����ݳ���
								pdata = pdata+DATA_HEAD_LEN+*(pdata+14)*256+ *(pdata+15)+DATA_END_LEN;
								else
								loop=0;	
								
								break;
							case  APP_GET_ALL_DEVICE_STATE:  //APP��ѯ���������豸״̬
								SaveAPPData( ClientS , *(pdata+4) ,(char*)(pdata+DATA_HEAD_LEN) , *(pdata+14)*256+ *(pdata+15) );//������������

								if( DataLen-(pdata-srcdat)- 
									(DATA_HEAD_LEN+ *(pdata+14)*256 + *(pdata+15)+DATA_END_LEN) >= APP_MIN_DATA_LEN )//�ж�ʣ�����ݳ���
								pdata = pdata+DATA_HEAD_LEN+*(pdata+14)*256+ *(pdata+15)+DATA_END_LEN;
								else
								loop=0;	
								break;
							default: 
							loop =0;	
							break;
						}
					}
					else
					{
						loop=0;
					}
				}
				else
				{
						loop=0;
				}
			}
			else
			{
				loop=0;
			}
		}


			return  1;
	}
	else
	{
		
			return 0;
	}
	
}

/////////////////////////////����������APP��������////////////////////////////////////////////////
char  SaveAPPData(SOCKET   ClientS ,int Command, char * src, unsigned  int  len)
{
	unsigned  int  i=0;

	int  ret=0;
	Json::Value  m_json;

	/**����Jsoncpp�����Ƿ�jsonʱ�����Զ��ݴ���ַ����͡����ַ�����ȡ�±�ʱ���ᴥ��assert��ֹ���̡�
��������������ϸ�ģʽ���÷Ƿ���json����ʱֱ�ӷ���false�����Զ��ݴ��������ڵ���parse��ʱ��ͻ᷵��false��
	**/
	Json::Reader *pJsonParser = new Json::Reader(Json::Features::strictMode());
	
	if(!pJsonParser->parse(src, m_json)) //json�Ϸ��Լ��
    {
		cout << "parse error" << endl;
		delete  pJsonParser;
        return -5;
    }
	delete  pJsonParser;
	string   str_token;
	//Json::FastWriter  fast_writer;//�鿴json���ݶ���
	//string str = fast_writer.write( m_json["token"]); //jsonתstring
	//cout<<str<<endl;//�����Ǵ����ŵ�""������string����
	if( GetVaule_CheckStr(&str_token , m_json , "token") == 0 )
	{
		cout<< "json�������ʹ���tokenӦ��Ϊstring"<<endl;
		return  -5;
	}
	
	ret = FindToken(str_token);//�ȶ�token

	//cout<<"�����֤��������:  "<<totalcnt<<endl;

	if(0 == ret)	//�����֤�ɹ�
	{
		cout<<"�����֤�ɹ�"<<endl;

		switch(Command)
		{
			case   APP_SET_LOCK :	//
				#ifdef  WIN_64_DEF
				APPSetWeiLan(ClientS , m_json );
				#endif
				return 0;
			break;
			case   APP_SET_UNLOCK:
				#ifdef  WIN_64_DEF
				APPUnSetWeiLan(ClientS , m_json );
				#endif
				return 0;
			break;

			case   APP_CONFIG_WEILAN:  //��ͣΧ��
				#ifdef  WIN_64_DEF
				//cout<< "��ͣΧ��"<< endl;

				APPConfigWeiLan(ClientS , m_json );
				#endif
				return 0;
			break;
			
			case  APP_SET_ZHUXIAO  :         //APPע������

				break;
			case  APP_SET_BANGDING :         //APP���豸����
			
				break;
			case  APP_SET_SHANCHU  :         //APPɾ���豸����
				responseAPPjiebang(ClientS , m_json);
				break;
			case  APP_SET_GET_STATE :        //APP��ѯ�豸״̬����
				responseAPPgetstate(ClientS , m_json);
				break;
			case  APP_GET_ALL_DEVICE_STATE:  //APP��ѯ���������豸״̬
				responseAPPgetalldevicestate(ClientS , m_json);
				break;
			case  APP_REQUEST_WEILAN_ALARM: //��ѯΧ������״̬
				responseAPPrequest_weilan_alarm(ClientS , m_json);
				break;
			case  APP_REQUEST_WEILAN_DATA: //APP��ѯΧ������
				responseAPPrequest_weilan_data(ClientS , m_json);
				break;
			default: 
				return -4;
			break;
		}
		
	}
	else //�����֤ʧ��
	{
		cout<<"�����֤ʧ�ܣ�"<<endl;
		responseCheckTokenErr(ClientS);
		return -3;
	}

	return 1;
	
}


//////////////////////�ظ�token��֤ʧ��/////////////////////////////////////////
int  responseCheckTokenErr(SOCKET   ClientS)
{
	Json::Value root;             // ��ʾ���� json ����
	root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
	root["error"] = Json::Value("token_error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
	Json::FastWriter  fast_writer;//�鿴json���ݶ���
	string str = fast_writer.write(root); //jsonתstring

	//cout<< str<<endl;

	send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 

	return 0;

}
////////////////////////////////////�ظ��û�ע��///////////////////////////////////////////////////
int  responseAPPregester(SOCKET   ClientS ,int Command, char * src, unsigned  int  len)
{
	bool  getsucess=false;

	Json::Value  m_json;
	Json::Reader *pJsonParser = new Json::Reader(Json::Features::strictMode());
	
	if(!pJsonParser->parse(src, m_json)) //char* תjson�����Ϸ��Լ��
    {
		cout << "parse error" << endl;
		delete  pJsonParser;
        return -5;
    }
	delete  pJsonParser;
	string  m_username , m_userpwd ,m_phone ,m_serialnumber , m_card ;
	if( GetVaule_CheckStr(&m_username , m_json , "user") == 0 )
	{
		cout<< "json�������ʹ���userӦ��Ϊstring"<<endl;
		return  -5;
	}

	if( GetVaule_CheckStr(&m_userpwd , m_json , "pwd") == 0 )
	{
		cout<< "json�������ʹ���pwdӦ��Ϊstring"<<endl;
		return  -5;
	}
	if( GetVaule_CheckStr(&m_phone , m_json , "phone") == 0 )
	{
		cout<< "json�������ʹ���pwdӦ��Ϊstring"<<endl;
		return  -5;
	}
	if( GetVaule_CheckStr(&m_serialnumber , m_json , "card") == 0 )
	{
		cout<< "json�������ʹ���pwdӦ��Ϊstring"<<endl;
		return  -5;
	}
	//cout<<m_userpwd.data()<<endl;

	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";    

	string  mSQLStr="SELECT *  FROM cardinfo WHERE serialnumber = '" + m_serialnumber + "'";
	
	cout<< mSQLStr<<endl;

	unsigned int port = 3306;        
        
    MYSQL   myCont;  //mysql ����
    MYSQL_RES  *result ;  //�洢���
    MYSQL_ROW  mysql_row; //������
	
	my_ulonglong  num_row,num_col; 
	string  mstr_pwd=""; //�洢�û�����
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

		num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if(num_row ==1)
		{
			getsucess = true ;
			mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

			for(int f2=0;f2<num_col;f2++) //ѭ����
			{	
				if (fields[f2].name!=NULL)  
				if (!strcmp( fields[f2].name , "card")) //�жϵ�ǰ�е��ֶ�����
				{
					m_card = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL
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
		return -4;
	}

	string  m_strcard = "SELECT  card  FROM  user_bike  WHERE card = '" + m_card + "'";
	cout<< m_strcard<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strcard.c_str()); //ִ��SQL���,ͨ��token����username
	if(!res  )
	{			
		//�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);
        num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if(num_row ==1)
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
		cout << "check card  is  used !\n" << endl;
		mysql_close(&myCont);
		return -4;
	}
////
	
	string  m_checkuser = "SELECT  *  FROM  register_user  WHERE username = '" + m_username + "'";
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
		if(num_row ==1)
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
		mysql_close(&myCont);
		cout << "check user  already used!\n" << endl;
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("user_already_used");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		
		return -4;
	}
////
	string  m_reguser = "INSERT  INTO  register_user ( username , userpwd, token ,phone , register_time )   VALUES( '" +
		m_username + "','" + m_userpwd + "','9m7u8N7x5AFEE2rvfioikjde215CTT','"+ m_phone + "', NOW() ) ";

	cout<<m_reguser<<endl;

	res = mysql_query(&myCont, (const  char *)m_reguser.c_str()); //ִ��SQL���,���һ����¼
	
	if(!res  )
	{	
		
		mysql_close(&myCont);	
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("sucess"); // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��	
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring
			
		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		 
		cout << "//////////regester user  sucess!/////////////\n" << endl;
		return 0;  		
	}
	else
	{
		mysql_close(&myCont);
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("regester_error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		cout << "add  SQL error!\n" << endl;
		return -3;
		
	}
    
	mysql_close(&myCont);

	return -1;
}
////////////////////////////////////�ظ���¼///////////////////////////////////////////////////
int  responseAPPdenglu(SOCKET   ClientS ,int Command, char * src, unsigned  int  len)
{
	Json::Value  m_json;
	Json::Reader *pJsonParser = new Json::Reader(Json::Features::strictMode());
	
	if(!pJsonParser->parse(src, m_json)) //char* תjson�����Ϸ��Լ��
    {
		cout << "parse error" << endl;
		delete  pJsonParser;
        return -5;
    }
	delete  pJsonParser;
	string  m_username , m_userpwd;
	if( GetVaule_CheckStr(&m_username , m_json , "user") == 0 )
	{
		cout<< "json�������ʹ���userӦ��Ϊstring"<<endl;
		return  -5;
	}

	if( GetVaule_CheckStr(&m_userpwd , m_json , "pwd") == 0 )
	{
		cout<< "json�������ʹ���pwdӦ��Ϊstring"<<endl;
		return  -5;
	}
	//cout<<m_userpwd.data()<<endl;

	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";    

	string  mSQLStr="SELECT userpwd  FROM register_user WHERE username = '" + m_username + "'";
	
	//cout<< mSQLStr<<endl;

	unsigned int port = 3306;        
        
    MYSQL   myCont;  //mysql ����
    MYSQL_RES  *result ;  //�洢���
    MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1,f2;
	my_ulonglong  num_row,num_col; 
	string  mstr_pwd=""; //�洢�û�����
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

		num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������

		//printf("row: %d,col: %d\n",num_row,num_col);//��ӡ���и���

		for(f1=0;f1<num_row;f1++) //ѭ����
		{		
			mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

			for(f2=0;f2<num_col;f2++) //ѭ����
			{	
				if (fields[f2].name!=NULL)  
				if (!strcmp( fields[f2].name , "userpwd")) //�жϵ�ǰ�е��ֶ�����
				{
					mstr_pwd = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL
					//cout<<mstr_pwd.data()<<endl;
				}
				//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������
			
			}
		}

		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

		//if (! strcmp(mstr_pwd.c_str(), m_userpwd.c_str()) )
		if (mstr_pwd== m_userpwd )
		{
			
			MakeUUID();//����UUID
			string  uuid_str  = m_uuid;
			string  UpdateCardStateSQL= "UPDATE register_user SET  token ='" + uuid_str
				+"' WHERE  username='"+m_username+"'";
			///////////////////////////////////////////////////////////////////////
			res = mysql_query(&myCont, (const  char *)UpdateCardStateSQL.c_str()); //ִ��SQL���
			cout<<UpdateCardStateSQL.c_str()<<endl;
			if(!res  )
			{
				mysql_close(&myCont);

				Json::Value root;             // ��ʾ���� json ����
				root["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
				root["error"] = Json::Value("sucess"); // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��	
				root["token"] = uuid_str;
				Json::FastWriter  fast_writer;//�鿴json���ݶ���
				string str = fast_writer.write(root); //jsonתstring
				send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
				return 0;
			}
			
			else
			{
				mysql_close(&myCont);
				Json::Value root;             // ��ʾ���� json ����
				root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
				root["error"] = Json::Value("pwd_error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
				Json::FastWriter  fast_writer;//�鿴json���ݶ���
				string str = fast_writer.write(root); //jsonתstring

				send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
			
				return -6;
			}
		}
		else
		{
			mysql_close(&myCont);
			Json::Value root;             // ��ʾ���� json ����
			root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
			root["error"] = Json::Value("pwd_error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
			Json::FastWriter  fast_writer;//�鿴json���ݶ���
			string str = fast_writer.write(root); //jsonתstring

			send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
			
			return -3;
		}    
		
	}

	mysql_close(&myCont);

	return -1;
}

//////////////////////////////////////////////////////////////////////////////////
int  responseAPPbangding(SOCKET   ClientS ,Json::Value  mJsonValue)
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

	string   str_token,str_username ,str_card,str_phone,str_bikename,str_bikecolor,str_biketype;  

	if( GetVaule_CheckStr(&str_token , mJsonValue , "token") == 0 )
	{
		cout<< "json�������ʹ���tokenӦ��Ϊstring"<<endl;
		return  -5;
	}
	
	if( GetVaule_CheckStr(&str_phone , mJsonValue , "phone") == 0 )
	{
		cout<< "json�������ʹ���phoneӦ��Ϊstring"<<endl;
		return  -5;
	}

	if( GetVaule_CheckStr(&str_card , mJsonValue , "card") == 0 )
	{
		cout<< "json�������ʹ���cardӦ��Ϊstring"<<endl;
		return  -5;
	}

	if( GetVaule_CheckStr(&str_bikename , mJsonValue , "bikename") == 0 )
	{
		cout<< "json�������ʹ���bikenameӦ��Ϊstring"<<endl;
		return  -5;
	}

	if( GetVaule_CheckStr(&str_bikecolor , mJsonValue , "bikecolor") == 0 )
	{
		cout<< "json�������ʹ���bikecolorӦ��Ϊstring"<<endl;
		return  -5;
	}

	if( GetVaule_CheckStr(&str_biketype , mJsonValue , "biketype") == 0 )
	{
		cout<< "json�������ʹ���biketypeӦ��Ϊstring"<<endl;
		return  -5;
	}

	//string  mstr_ziduan = mJsonValue["token"].asString();

	string  m_strToken = "SELECT  username  FROM  register_user  WHERE token = '" + str_token + "'";
	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 

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
		if(getsucess == false)
		{
			mysql_close(&myCont);
			cout << "select username error!\n" << endl;
			return -4;
		}
	}
	else
	{
		mysql_close(&myCont);
		cout << "select username error!\n" << endl;
		return -4;
	}
///////////////////////////////////////////////////////////////////////	
	//INSERT ������һ��
	//on duplicate key update  ��Ҫ����һ�����������Զ�����������������ͻ��ִ�к����updata����
	string  mSQLStr = "INSERT  INTO  user_bike(bike_id , username, card,phone,bikename,bikecolor,biketype,register_time)   VALUES( 1, '"
		+ str_username + "','"+ str_card + "','"+ str_phone + "','"+ str_bikename + "','"+ str_bikecolor+ "','"+ str_biketype
		+ "', NOW()) on duplicate key update  bike_id = 1 , username = '" + str_username + "', card = '" + str_card + "', phone = '" + str_phone +"', bikename = '" + str_bikename +
		"', bikecolor = '" + str_bikecolor + "', biketype = '"+str_biketype + "', register_time =  NOW() " ;
	

	//UPDATE��������,�Ǹ���,�м����͸��Ǽ�����
	
	//string  mSQLStr="UPDATE  user_bike  SET  username = '" + str_username + "', card = '" + str_card + "', phone = '" + str_phone +"', bikename = '" + str_bikename +
	//	"', bikecolor = '" + str_bikecolor + "', biketype = '"+str_biketype + "', register_time ='2017-5-10-10:02:05' ";

	//cout<<mSQLStr<<endl;
	
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���,���һ����¼
	if(!res  )
	{			
		mysql_close(&myCont);	
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
		mysql_close(&myCont);
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("bangding_error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		cout << "add  SQL error!\n" << endl;
		return -3;
		
	}
    mysql_close(&myCont);

	return -1;

}


///////////////////////////////���豸/////////////////////////////////////
int  responseAPPbind (SOCKET   ClientS ,int Command, char * src, unsigned  int  len)
{
	bool  getsucess =false;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;   

	Json::Value  mJsonValue;
	Json::Reader *pJsonParser = new Json::Reader(Json::Features::strictMode());
	
	if(!pJsonParser->parse(src, mJsonValue)) //char* תjson�����Ϸ��Լ��
    {
		cout << "parse error" << endl;
		delete  pJsonParser;
        return -5;
    }
	delete  pJsonParser;


	MYSQL myCont;
    MYSQL_RES *result;
    int res;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1,f2,num_row,num_col; 
	string  sql_userpwd;
	string   str_pwd,str_username ,str_card,str_phone,str_bikename,str_bikecolor,str_biketype;  
	string  m_serialnumber;

	if( GetVaule_CheckStr(&str_username , mJsonValue , "user") == 0 )
	{
		cout<< "json�������ʹ���tokenӦ��Ϊstring"<<endl;
		return  -5;
	}
	if( GetVaule_CheckStr(&str_pwd , mJsonValue , "pwd") == 0 )
	{
		cout<< "json�������ʹ���tokenӦ��Ϊstring"<<endl;
		return  -5;
	}
	
	if( GetVaule_CheckStr(&str_phone , mJsonValue , "phone") == 0 )
	{
		cout<< "json�������ʹ���phoneӦ��Ϊstring"<<endl;
		return  -5;
	}

	if( GetVaule_CheckStr(&m_serialnumber , mJsonValue , "card") == 0 )
	{
		cout<< "json�������ʹ���cardӦ��Ϊstring"<<endl;
		return  -5;
	}

	if( GetVaule_CheckStr(&str_bikename , mJsonValue , "bikename") == 0 )
	{
		cout<< "json�������ʹ���bikenameӦ��Ϊstring"<<endl;
		return  -5;
	}

	if( GetVaule_CheckStr(&str_bikecolor , mJsonValue , "bikecolor") == 0 )
	{
		cout<< "json�������ʹ���bikecolorӦ��Ϊstring"<<endl;
		return  -5;
	}

	if( GetVaule_CheckStr(&str_biketype , mJsonValue , "biketype") == 0 )
	{
		cout<< "json�������ʹ���biketypeӦ��Ϊstring"<<endl;
		return  -5;
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
		return -2;
    }

	string  mSQLStr="SELECT *  FROM  cardinfo  WHERE  serialnumber = '" + m_serialnumber + "'";
	
	cout<< mSQLStr<<endl;

	///////////////////////////////////////////////////////////////////////
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���
	
	if(!res  )
	{	
        //�����ѯ�������ݵ�result
        result = mysql_store_result(&myCont);

		num_row=mysql_num_rows(result); //��ȡ����
		num_col=mysql_num_fields(result); //��ȡ����
	    MYSQL_FIELD* fields = mysql_fetch_fields(result); //���������ֶνṹ������
		if(num_row ==1)
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
		return -4;
	}
	//string  mstr_ziduan = mJsonValue["token"].asString();

	string  m_strToken = "SELECT  userpwd  FROM  register_user  WHERE username = '" + str_username + "'";
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
					if(str_pwd == sql_userpwd)
					{
						getsucess = true;
						cout<<sql_userpwd.data()<<endl;
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
			cout << "select userpwd error!\n" << endl;
			return -4;
		}
	}
	else
	{
		mysql_close(&myCont);
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
		mysql_close(&myCont);
		cout << "check card already existed !\n" << endl;
		return -4;
	}
///////////////////////////////////////////////////////////////////////	
	//INSERT ������һ��
	//on duplicate key update  ��Ҫ����һ�����������Զ�����������������ͻ��ִ�к����updata����
	 mSQLStr = "INSERT  INTO  user_bike(  username, card,phone,bikename,bikecolor,biketype,register_time)   VALUES( '"
		+ str_username + "','"+ str_card + "','"+ str_phone + "','"+ str_bikename + "','"+ str_bikecolor+ "','"+ str_biketype
		+ "', NOW())" ;
	//UPDATE��������,�Ǹ���,�м����͸��Ǽ�����
	
	//string  mSQLStr="UPDATE  user_bike  SET  username = '" + str_username + "', card = '" + str_card + "', phone = '" + str_phone +"', bikename = '" + str_bikename +
	//	"', bikecolor = '" + str_bikecolor + "', biketype = '"+str_biketype + "', register_time ='2017-5-10-10:02:05' ";

	//cout<<mSQLStr<<endl;
	
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���,���һ����¼
	if(!res  )
	{			
		mysql_close(&myCont);	
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("sucess"); // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��	
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring
		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		
		//GetLocalTime( &sys ); 
		//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 

		return 0;  		
	}
	else
	{
		mysql_close(&myCont);
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("bangding_error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		cout << "add  SQL error!\n" << endl;
		return -3;
		
	}
    mysql_close(&myCont);

	return -1;

}

/////////////////////////��ȡ״̬///////////////////////////////////////////
int  responseAPPgetstate(SOCKET   ClientS ,Json::Value  mJsonValue)
{
	bool  getsucess =false;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;   

	Json::Value  mjson_card;  // ��ʾ���� json ����
	MYSQL myCont;
    MYSQL_RES *result;
    int res;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1,f2,num_row,num_col; 

	string   str_token,str_username ,str_card,str_phone,str_bikename,str_bikecolor,str_biketype;  

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

	//string  mstr_ziduan = mJsonValue["token"].asString();
	string  m_strname = "SELECT  username  FROM  register_user  WHERE token = '" + str_token + "'";
	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 

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
		
	}
	else
	{
		mysql_close(&myCont);
		cout << "select username error!\n" << endl;
		return -4;
	}

	string  m_strcard = "SELECT  card  FROM  user_bike  WHERE username = '" + str_username + "'  and  card = '" +str_card +"'";
	//cout<< m_strcard<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strcard.c_str()); //ִ��SQL���,ͨ��token����username
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
				if (!strcmp( fields[f2].name , "card")) //�жϵ�ǰ�е��ֶ�����
				{				
					//cout<<str_username.data()<<endl;
					getsucess = true;
					break;
					
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
		cout << "check card error!\n" << endl;
		return -4;
	}
///////////////////////////////////////////////////////////////////////////////////
	string  m_strToken = "SELECT  *  FROM  card_data  WHERE card = '" + str_card + "' ORDER BY card_id DESC LIMIT 0,1";
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
			mjson_card["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
			mjson_card["error"] = Json::Value("sucess"); 
			mjson_card["card"] = Json::Value(str_card);	

			for(f1=0;f1<1;f1++) //ѭ����
			{		
				mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

				for(f2=0;f2<num_col;f2++) //ѭ����
				{	
					//if (fields[f2].name!=NULL)  
					if (!strcmp( fields[f2].name , "card_state")) //�жϵ�ǰ�е��ֶ�����
					{										
						mjson_card[fields[f2].name] = Json::Value( getNullStr(mysql_row[f2]) );
						
						getsucess = true ;
								
					}
					else  if(!strcmp( fields[f2].name , "card_lock"))
					{
						int  lockset = *mysql_row[f2] - '0';
						mjson_card[fields[f2].name] = Json::Value( lockset );
						
					}
					else  if(!strcmp( fields[f2].name , "gps"))
					{
						mjson_card[fields[f2].name] = Json::Value( getNullStr(mysql_row[f2]) );
						
					}
					else if (!strcmp( fields[f2].name , "time"))
					{
						mjson_card[fields[f2].name] = Json::Value( getNullStr(mysql_row[f2]) );
						
					}

					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������
			
				}
			}
		}
	
		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����	
	}

	mysql_close(&myCont);//�ر����ӣ�����return�󣬺����˳��ˣ�mysql��������ռ��

	if(getsucess == true )
	{
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(mjson_card); //jsonתstring
		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		
		GetLocalTime( &sys ); 
		printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 

		return 0; 
	}
	else  if(getsucess == false)
	{
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("get_state error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		cout << "get CARD state error!\n" << endl;
		return -4;
	}
///////////////////////////////////////////////////////////////////////	
	
    mysql_close(&myCont);

	return -1;

}

//////////////////////////////////////////////////////////////////////////////////
int  responseAPPjiebang(SOCKET   ClientS ,Json::Value  mJsonValue)
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

	string   str_token,str_username ,str_card,str_phone,str_bikename,str_bikecolor,str_biketype;  

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


	string  m_strToken = "SELECT  username  FROM  register_user  WHERE token = '" + str_token + "'";
	
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
	if(getsucess == true )
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
	//on duplicate key update  ��Ҫ����һ�����������Զ�����������������ͻ��ִ�к����updata����
	string  mSQLStr = "DELETE  FROM  user_bike  WHERE  card = '" + str_card+"'  and  username = '" +str_username + "' " ;

	//cout<<mSQLStr<<endl;
	
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���,���һ����¼
	if(!res  )
	{			
		mysql_close(&myCont);	
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("sucess"); // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��	
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring
		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 

		return 0;  		
	}
	else
	{
		mysql_close(&myCont);
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("jiebang_error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		cout << "jie bang error!\n" << endl;
		return -3;
		
	}
    mysql_close(&myCont);

	return -1;

}


/////////////////////////��ȡ�����豸״̬///////////////////////////////////////////
int  responseAPPgetalldevicestate(SOCKET   ClientS ,Json::Value  mJsonValue)
{
	bool  getsucess =false;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;   
	Json::Value mjson_cardstate;       
	
	MYSQL myCont;
    MYSQL_RES *result;
    int res;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1,f2,num_row,num_col; 
	my_ulonglong  card_row=0,card_line=0;
	my_ulonglong  base_station_row = 0, base_station_line = 0;
	string   str_token,str_username ,str_card,str_phone,str_bikename,str_bikecolor,str_biketype;  

	if( GetVaule_CheckStr(&str_token , mJsonValue , "token") == 0 )
	{
		cout<< "json�������ʹ���tokenӦ��Ϊstring"<<endl;
		return  -5;
	}
	
	//string  mstr_ziduan = mJsonValue["token"].asString();
	string  m_strname = "SELECT  username  FROM  register_user  WHERE token = '" + str_token + "'";
	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 

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
		
	}
	else
	{
		mysql_close(&myCont);
		cout << "select username error!\n" << endl;
		return -4;
	}

	string  m_strcard = "SELECT  *  FROM  user_bike  WHERE username = '" + str_username +"'";
	//cout<< m_strcard<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strcard.c_str()); //ִ��SQL���,ͨ��token����username
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
									int  lockset = *mysql_row[card_f2] - '0';
									mjson_card[card_files[card_f2].name] = Json::Value( lockset );
				
								}
								else  if(!strcmp( card_files[card_f2].name , "gps"))
								{
									mjson_card[card_files[card_f2].name] = Json::Value( getNullStr(mysql_row[card_f2]) );
				
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
									mbase_json_card[card_files[card_f2].name] = Json::Value( lockset );
				
								}
								else  if(!strcmp( card_files[card_f2].name , "base_station"))
								{
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
					}
				}//endif ���ų��Ⱥͳ�������
			}//endfor ��
			mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����
		}
		else
		{
			mysql_close(&myCont);
			return -6;
		}
	}
	mysql_close(&myCont);//��ʱ�ر�sql���ӣ�����ռ��mysql������

	if(getsucess == true )
	{
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(mjson_cardstate); //jsonתstring

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		return 0; 
	}
	else  if(getsucess == false)
	{
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("get_state error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		cout << "get CARD state error!\n" << endl;
		return -4;
	}
///////////////////////////////////////////////////////////////////////	
	
    mysql_close(&myCont);

	return -1;

}

///////////////////////����Χ��/////////////////////////////////////////////
int  APPSetWeiLan(SOCKET   ClientS ,Json::Value  mJsonValue)
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
	char bike_gps[200]=",A,3158.4608,N,11848.3737,E,10.05";
	struct tm  ;  

	string   tos ,str_token,str_card ,str_gps, str_username;  
	string  str_lock;
	string   radius;

	tos = to_string((long)ClientS);	
	
	if(mJsonValue["radius"].isInt())
	{
		radius = to_string( (long)mJsonValue["radius"].asInt());
	}
	else
	{		
		cout<< "json����value���ʹ���radiusӦ��Ϊint"<<endl;
		return  -5;

	}

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
	if( GetVaule_CheckStr(&str_gps , mJsonValue , "gps") == 0 )
	{
		cout<< "json�������ʹ���gpsӦ��Ϊstring"<<endl;
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
	//�����card����Ϊ����
	//string  mSQLStr="UPDATE user_set  SET username = '" + str_username + "' , token =  '" + str_token + "' , app_socket = " + tos + ", setlock = " + str_lock + ", card = '"+ str_card  +
	//	"', update_card = 2 , time = NOW() " ;
	string  mSQLStr = "INSERT  INTO  alarm_weilan(  card, card_socket,allow_alarm,radius,gps,time)   VALUES(  '"
		+ str_card + "','"+ tos + "' , 1  ," +  radius + ",'" + str_gps + "' , NOW()) on duplicate key update  " + 
		" card = '"  + str_card + "' , card_socket =  '" + tos + "' , allow_alarm = 1 , radius = " + radius + " , gps = '"+ 
		str_gps +"' , time = NOW() " ;

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

	cout << "set weilan error!\n" << endl;

    mysql_close(&myCont);

	return -1;

}

///////////////////////ȡ��Χ��/////////////////////////////////////////////
int  APPUnSetWeiLan(SOCKET   ClientS ,Json::Value  mJsonValue)
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
	char bike_gps[200]=",A,3158.4608,N,11848.3737,E,10.05";
	struct tm  ;  

	string   tos ,str_token,str_card ,str_gps, str_username;  
	string  str_lock;
	string   radius;

	tos = to_string((long)ClientS);	

	if(mJsonValue["radius"].isInt())
	{
		radius = mJsonValue["radius"].asString();
	}
	else
	{		
		cout<< "json����value���ʹ���radiusӦ��Ϊint"<<endl;
		return  -5;

	}

	
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
	if( GetVaule_CheckStr(&str_gps , mJsonValue , "gps") == 0 )
	{
		cout<< "json�������ʹ���gpsӦ��Ϊstring"<<endl;
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

	//string  mSQLStr="UPDATE user_set  SET username = '" + str_username + "' , token =  '" + str_token + "' , app_socket = " + tos + ", setlock = " + str_lock + ", card = '"+ str_card  +
	//	"', update_card = 2 , time = NOW() " ;
	string  mSQLStr = "INSERT  INTO  alarm_weilan(  card, card_socket,allow_alarm,radius,gps,time)   VALUES(  '"
		+ str_card + "','"+ tos + "' , 0  ," +  radius + ",'" + str_gps + "' , NOW()) on duplicate key update  " + 
		" card = '"  + str_card + "' , card_socket =  '" + tos + "' , allow_alarm = 0 , radius = " + radius + " , gps = '"+ 
		str_gps +"' , time = NOW() " ;

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
		root["error"] = Json::Value("close_weilan_error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
			
		return -3;
	}

	cout << "updateSQL error!\n" << endl;

    mysql_close(&myCont);

	return -1;

}


///////////////////////��ͣΧ��/////////////////////////////////////////////
int  APPConfigWeiLan(SOCKET   ClientS ,Json::Value  mJsonValue)
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
	char bike_gps[200]=",A,3158.4608,N,11848.3737,E,10.05";
	struct tm  ;  

	string   tos ,str_token,str_card ,str_gps, str_username;  
	string  str_lock;
	string   radius , allow_alarm;

	tos = to_string((long)ClientS);	

	//cout<< mJsonValue["allow_alarm"] << endl;

	if( GetVaule_CheckStr(&allow_alarm , mJsonValue , "allow_alarm") == 0 )
	{
		cout<< "json�������ʹ��� allow_alarm Ӧ��Ϊstring"<<endl;
		return  -5;
	}
	
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
	string  mSQLStr="UPDATE alarm_weilan  SET allow_alarm = '" + allow_alarm + "' , time = NOW()  WHERE  card  =  '" + str_card +"' ";

	//cout<<mSQLStr<<endl;
	
	res = mysql_query(&myCont, (const  char *)mSQLStr.c_str()); //ִ��SQL���
	mysql_close(&myCont);
	if(!res  )
	{			
		cout<<"�޸�Χ���ɹ�"<<endl;
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("sucess"); // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��	
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring
		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		//cout<<str.data()<<endl;
		//GetLocalTime( &sys ); 
		//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 

		return 0;  		
	}
	else
	{
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("config_weilan_error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
			
		return -3;
	}

	cout << "updateSQL error!\n" << endl;

    mysql_close(&myCont);

	return -1;

}


/////////////////////////����Χ������״̬///////////////////////////////////////////
int  responseAPPrequest_weilan_alarm(SOCKET   ClientS ,Json::Value  mJsonValue)
{
	bool  getsucess =false;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;   
	Json::Value mjson_cardstate;       
	
	MYSQL myCont;
    MYSQL_RES *result;
    int res;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1,f2,num_row,num_col; 
	my_ulonglong  card_row=0,card_line=0;
	string   str_token,str_username ,str_card,str_phone,str_bikename,str_bikecolor,str_biketype;  
	string  gps_data;
	if( GetVaule_CheckStr(&str_token , mJsonValue , "token") == 0 )
	{
		cout<< "json�������ʹ���tokenӦ��Ϊstring"<<endl;
		return  -5;
	}
	
	//string  mstr_ziduan = mJsonValue["token"].asString();
	string  m_strname = "SELECT  username  FROM  register_user  WHERE token = '" + str_token + "'";
	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 

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
		
	}
	else
	{
		mysql_close(&myCont);
		cout << "select username error!\n" << endl;
		return -4;
	}

	string  m_strcard = "SELECT  card  FROM  user_bike  WHERE username = '" + str_username +"'";
	//cout<< m_strcard<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strcard.c_str()); //ִ��SQL���,ͨ��token����username
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
			mjson_cardstate["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
			mjson_cardstate["error"] = Json::Value("sucess");
			for(f1=0;f1<num_row;f1++) //ѭ����
			{		
				mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

				for(f2=0;f2<num_col;f2++) //ѭ����
				{	
					if (fields[f2].name!=NULL)  
					if (!strcmp( fields[f2].name , "card")) //�жϵ�ǰ�е��ֶ�����
					{				
						//cout<<str_username.data()<<endl;
						str_card =  getNullStr(mysql_row[f2]);										
						
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
								//Json::Value  mjson_card;  // ��ʾ���� json ����
								//mjson_card["card"] = Json::Value(str_card);	
								mysql_row = mysql_fetch_row(card_result); //��ȡÿ�е�����

								for(int card_f2 =0;card_f2<card_line;card_f2++) //ѭ����
								{	
									//if (fields[f2].name!=NULL)  
									if(!strcmp( card_files[card_f2].name , "gps"))
									{
										gps_data = getNullStr( mysql_row[card_f2]);
										APPCheckAlarmTable(ClientS , gps_data , str_card);//����ָ��cardΧ�������
									}
									//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������
			
								}//endfor ��

								mysql_free_result(card_result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����	
								//mjson_cardstate["data"] = Json::Value( mjson_card );
								
							}//endif
						
						}//endif
					
					}//endif card
					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������
			
				}//endfor ��
			}//endfor ��
			mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����
		}
		else
		{
			mysql_close(&myCont);
			return -6;
		}
	}
	
	mysql_close(&myCont);//��ʱ�ر�sql���ӣ�����ռ��mysql������

	if(getsucess == true )
	{
		//Json::FastWriter  fast_writer;//�鿴json���ݶ���
		//string str = fast_writer.write(mjson_cardstate); //jsonתstring

		//send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		return 0; 
	}
	else  if(getsucess == false)
	{
		//Json::Value root;             // ��ʾ���� json ����
		//root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		//root["error"] = Json::Value("get_state error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		//Json::FastWriter  fast_writer;//�鿴json���ݶ���
		//string str = fast_writer.write(root); //jsonתstring

		//send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		cout << "get CARD state error!\n" << endl;
		return -4;
	}
///////////////////////////////////////////////////////////////////////	
	
    mysql_close(&myCont);

	return -1;

}

//////////////////////����ָ��cardΧ�������//////////////////////////////////////////
int  APPCheckAlarmTable(SOCKET ClientS , string gps , string  card)
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
	string  m_strToken = "SELECT  *  FROM  alarm_weilan  WHERE card = '" + card + "' ORDER BY card_id DESC LIMIT 0,1 ";
	            
    MYSQL myCont;
    MYSQL_RES *result;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1,f2,num_row,num_col; 
    int res;

//	SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 

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
	cout<<card <<"��ѯΧ��״̬��"<<endl;
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
			for(f1=0;f1<1;f1++) //ѭ����ֻȡ�����µ�һ��
			{		
				mysql_row = mysql_fetch_row(result); //��ȡÿ�е�����

				for(f2=0;f2<num_col;f2++) //ѭ����
				{	
					if (fields[f2].name!=NULL)  
					{
						if (!strcmp( fields[f2].name , "gps")) //�жϵ�ǰ�е��ֶ�����
						{
							weilan_gps = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL
							//cout<<str_username.data()<<endl;
							
							getsucess =true;
							
							//cout<<mstr_pwd.data()<<endl;
						}
						else  if (!strcmp( fields[f2].name , "radius")) //�жϵ�ǰ�е��ֶ�����
						{
							weilan_radius = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL
							//cout<<str_username.data()<<endl;
							
							getsucess =true;
							
							//cout<<mstr_pwd.data()<<endl;
						}
						if (!strcmp( fields[f2].name , "allow_alarm")) //�жϵ�ǰ�е��ֶ�����
						{
							allow_alarm = getNullStr(mysql_row[f2]); //��ȡ�ֶ����ݣ������жϷ�NULL
							if( 0 == allow_alarm.compare("1"))
							getsucess =true;
							else  if( 0 == allow_alarm.compare("0"))
							{
								getsucess = false;
								cout << "Χ��δ����" << endl;
								cout<<"///////////////////////////////////////"<<endl;
								return  -10;
							}
							//cout<<mstr_pwd.data()<<endl;
						}
					}
					
					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������
			
				}
			}
		}
		
		mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����

	}
	
	mysql_close(&myCont);
	
	if(getsucess ==true)
	{

	}
	else
	{
		cout << "select alarm_weilan error!\n" << endl;
		return -4;
	}
	
	//cout << "panduan weilan !\n" << endl;
	
	//cout << weilan_radius << endl;
	
	//cout << weilan_gps << endl;
	
	//cout << gps << endl;
////////////////////////////////�ж�Χ���Ƿ񳬳�///////////////////////////////////////
	if(PanDuanWeiLan( weilan_radius , weilan_gps , gps ) >0 ) //�ж�Χ������
	{
		
		cout << "alarm weilan !\n" << endl;
		
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("weilan_alarm"); // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��	
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring
		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		return 0;
	}
	else
	{
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("no weilan_alarm");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		return -6;	
	}

	mysql_close(&myCont);
	return -1;

}


#define       PI         3.141592657
#define      EARTH_RADIUS    6378137

/////////////////////////////////////////////////////////
double GPSdufen_to_du(double dufen)
{
	double gps_du;
	int  du = (int) (dufen/100);
	
	double  fen = dufen - du*100;
	gps_du = du + fen /60.00; //ת��Ϊ�ȸ�ʽ
	return  gps_du;
}

static inline double  rad( double degree )
{
    return  PI * degree / 180.0;
}
static inline double haverSin(double x)
{
    double v = sin(x / 2.0);
    return v * v;
}


//�������(��λ : m)
static double getDistance(double lon1, double lat1, double lon2, double lat2)
{
    double radlon1 = rad(lon1);
    double radlat1 = rad(lat1);
    double radlon2 = rad(lon2);
    double radlat2 = rad(lat2);
    
    double a = fabs(radlat1 - radlat2);
    double b = fabs(radlon1 - radlon2);
    
    double h = haverSin(b) + cos(lat1)*cos(lat2)*haverSin(a);
    double distance = 2 * EARTH_RADIUS * asin(sqrt(h));
    return  distance;
}

//////////////////////////////////�ж��ƶ������Χ���Ƿ��н���///////////////////////////////////////////////
int  PanDuanWeiLan(string  weilan_radius , string  weilan_gps  , string  gps)
{
	double  weilan_lon , weilan_lat , gps_lon, gps_lat;
	string  str_weilan_lon , str_weilan_lat, str_gps_lon , str_gps_lat;
	int  pos= -1;
	int  edpos = -1;
	int len=0;
	int sub_len=0;
	double  alarm_radius = atof(weilan_radius.c_str());
	double  distance;
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
		
//////////////////////////////////////////////////////////////////////////	
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
		
		}		
		
	}

	gps_lon = GPSdufen_to_du(gps_lon);//��γ��ת���ɶ�Ϊ��λ
	gps_lat = GPSdufen_to_du(gps_lat);//��γ��ת���ɶ�Ϊ��λ
	cout << "WEILAN_GPS_LON:"<< weilan_lon << endl;
	cout << "WEILAN_GPS_LAT:"<<weilan_lat << endl;
	cout <<"HARD_GPS_LON:"<<  gps_lon <<endl;
	cout <<"HARD_GPS_LAT:"<< gps_lat <<endl;
	cout << "ALARM_RADIUS(meter):"<<alarm_radius <<endl;
	
	//�������㾭γ��֮��ֱ�߾���
	distance = getDistance(weilan_lon , weilan_lat , gps_lon , gps_lat);
	cout<<"ƫ����룺"<< distance <<endl;
	cout << "///////////////////////////////////"<<endl;
	if(distance > alarm_radius) //����Χ��
	{
		return  1;
	}
	
	return  0;
	
	
}

/////////////////////////����Χ����������//////////////////////////////////////////
int  responseAPPrequest_weilan_data(SOCKET   ClientS ,Json::Value  mJsonValue)
{
	bool  getsucess =false;
	const char user[] = "root";         
    const char pswd[] = "123456";        
    const char host[] = "localhost";    
    char table[] = "bike";  
	unsigned int port = 3306;   
	Json::Value mjson_cardstate;       
	
	MYSQL myCont;
    MYSQL_RES *result;
    int res;
	MYSQL_ROW  mysql_row; //������
	my_ulonglong  f1,f2,num_row,num_col; 
	my_ulonglong  card_row=0,card_line=0;
	string   str_token,str_username ,str_card,str_phone,str_bikename,str_bikecolor,str_biketype;  
	string  gps_data;
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
	//string  mstr_ziduan = mJsonValue["token"].asString();
	string  m_strname = "SELECT  username  FROM  register_user  WHERE token = '" + str_token + "'";
	//SYSTEMTIME sys; 
	//GetLocalTime( &sys ); 
	//printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ����%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek); 

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
		
	}
	else
	{
		mysql_close(&myCont);
		cout << "select username error!\n" << endl;
		return -4;
	}

	string  m_strcard = "SELECT  *  FROM  alarm_weilan  WHERE card = '" + str_card + "' ORDER BY time DESC LIMIT 0,1";
	//cout<< m_strcard<<endl;
	getsucess =false;
	res = mysql_query(&myCont, (const  char *)m_strcard.c_str()); //ִ��SQL���,ͨ��token����username
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
			mjson_cardstate["errno"] = Json::Value(0);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
			mjson_cardstate["error"] = Json::Value("sucess");
			Json::Value  mjson_card;  // ��ʾ���� json ����
			mjson_card["card"] = Json::Value(str_card);	
			mysql_row = mysql_fetch_row(result); //��ȡ��ѯ��������
			
			for(f1=0;f1<num_row;f1++) //ѭ����
			{		
				
				for(f2=0;f2<num_col;f2++) //ѭ����
				{	
					if (fields[f2].name!=NULL)  
					{
						if (!strcmp( fields[f2].name , "card")) //�жϵ�ǰ�е��ֶ�����
						{				
							//cout<<str_username.data()<<endl;
							str_card =  getNullStr(mysql_row[f2]);										
						}//endif card
						else  if (!strcmp( fields[f2].name , "radius")) //�жϵ�ǰ�е��ֶ�����
						{				
							string  radius =  getNullStr(mysql_row[f2]);
							mjson_card[fields[f2].name] = Json::Value( radius );
							//cout<<"Χ���뾶��"<<radius<<endl;
						}
						else  if (!strcmp( fields[f2].name , "allow_alarm")) //�жϵ�ǰ�е��ֶ�����
						{				
							string  allow =  getNullStr(mysql_row[f2]);
							mjson_card[fields[f2].name] = Json::Value( allow );
							//cout<<"Χ����������"<<radius<<endl;
						}
						else  if (!strcmp( fields[f2].name , "gps")) //�жϵ�ǰ�е��ֶ�����
						{				
							mjson_card[fields[f2].name] = Json::Value( getNullStr(mysql_row[f2]) );
														
						}
						else  if (!strcmp( fields[f2].name , "time")) //�жϵ�ǰ�е��ֶ�����
						{				
							mjson_card[fields[f2].name] = Json::Value( getNullStr(mysql_row[f2]) );
														
						}
													
					}
										
					//printf("[Row %d,Col %d]==>[%s]\n",f1,f2,mysql_row[f2]); //��ӡÿ�еĸ���������
			
				}//endfor ��
			}//endfor ��
			getsucess =true;
			mysql_free_result(result); //�ͷŻ��棬�ر�ע�⣬���ͷŻᵼ���ڴ�����
			mjson_cardstate["data"].append( Json::Value(mjson_card) );//�������
		}
		else
		{
			mysql_close(&myCont);
			return -6;
		}
	}
	
	mysql_close(&myCont);//��ʱ�ر�sql���ӣ�����ռ��mysql������

	if(getsucess == true )
	{
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(mjson_cardstate); //jsonתstring

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		return 0; 
	}
	else  if(getsucess == false)
	{
		Json::Value root;             // ��ʾ���� json ����
		root["errno"] = Json::Value(1);     // �½�һ�� Key����Ϊ��key_string���������ַ���ֵ��"value_string"��
		root["error"] = Json::Value("get_weilan_state error");             // �½�һ�� Key����Ϊ��key_number����������ֵ��12345��
		
		Json::FastWriter  fast_writer;//�鿴json���ݶ���
		string str = fast_writer.write(root); //jsonתstring

		send(ClientS , (char *)str.data(), (int)str.length(), 0);  // ������Ϣ 
		cout << "get_weilan_state error!\n" << endl;
		return -4;
	}
///////////////////////////////////////////////////////////////////////	
	
    mysql_close(&myCont);

	return -1;

}











