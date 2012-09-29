#include <ruby.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "ar_class.h"

static VALUE myModule; //ruby madule
static VALUE myClass;  //For this ruby class.
static VALUE transactionClass;

static void ar_mark(ARCLass *ar)
{
	//don't need to do anything.  
}

static void ar_free(ARClass *ar)
{
	if(ar)
	{
		FreeARStatusList(ar->status, TRUE); //ensure we free any status records.
		if(ar->control) free(ar->control); //free the control structure.
		free(ar);
	}
}

static VALUE ar_alloc(VALUE klass)
{
	ARClass *ar = calloc(1, sizeof(ARClass));

	ar->control = calloc(1, sizeof(ARControlStruct));
	ar->status = calloc(1, sizeof(ARStatusList));
	
  return Data_Wrap_Struct(klass, ar_mark, ar_free, ar); //wraps a c struct in VALUE
  //recover with Data_Get_Struct()
}

int ar_type_check(VALUE obj)
{
	return(  TYPE(obj) == T_DATA && RDATA(obj)->dfree == (RUBY_DATA_FUNC)ar_free);
}

//Ruby
//	ar = Ar.new(username=nil, password=nil, server=nil, authstring=nil, localeinfo=nil)
//    #do stuff
//  ar.close
static VALUE ar_initialize(int argc, VALUE *argv, VALUE obj)
{
	VALUE username, password, localeinfo, server, authstring;
	ARClass *ar;

	//All arguments are optional as we could load them later.
	rb_scan_args(argc, argv, "04", &username, &password, &server, &authstring, &localeinfo);

  if(TYPE(username) != T_STRING && username != Qnil)
		rb_raise(rb_eTypeError, "username should be a String"); 
  if(TYPE(password) != T_STRING && password != Qnil)
		rb_raise(rb_eTypeError, "password should be a String"); 
  if(TYPE(server) != T_STRING &&  server != Qnil)
		rb_raise(rb_eTypeError, "server should be a String"); 
  if(TYPE(authstring) != T_STRING && authstring != Qnil)
		rb_raise(rb_eTypeError, "authstring should be a String"); 
  if(TYPE(localeinfo) != T_STRING && localeinfo != Qnil)
		rb_raise(rb_eTypeError, "localeinfo should be a String"); 
	
  Data_Get_Struct(obj, ARClass, ar);
	
	if(username != Qnil)
	{
		strncpy(ar->control.user, STR2CSTR(username), AR_MAX_ACCESS_NAME_SIZE);
		ar->control.user[AR_MAX_ACCESS_NAME_SIZE] = '\0'; //ensure string is null terminated.
	}
	if(password != Qnil)
	{
		strncpy(ar->control.password, STR2CSTR(password), AR_MAX_PASSWORD_SIZE);
		ar->control.password[AR_MAX_PASSWORD_SIZE] = '\0'; //ensure string is null terminated.
	}
	if(server != Qnil)
	{
		strncpy(ar->control.server, STR2CSTR(server), AR_MAX_SERVER_SIZE);
		ar->control.server[AR_MAX_SERVER_SIZE] = '\0'; //ensure string is null terminated.
	}
	if(authString != Qnil)
	{
		strncpy(ar->control.authString, STR2CSTR(authString), AR_MAX_AUTH_SIZE);
		ar->control.authString[AR_MAX_AUTH_SIZE] = '\0'; //ensure string is null terminated.
	}
	if(localeinfo != Qnil)
	{
		strncpy(ar->control.localeinfo, STR2CSTR(localeinfo), AR_MAX_LOCALE_SIZE);
		ar->control.localeinfo[AR_MAX_LOCALE_SIZE] = '\0'; //ensure string is null terminated.
	}
	
	if (ARInitialization(ar->control, ar->status) >= AR_RETURN_ERROR)
		rb_raise(ar_status_list_new(ar->status), "ARInitialization Failed"); 
	
	ar->connected = 1;
  return obj;
}

static VALUE ar_close(VALUE obj)
{
	ARClass *ar;

  Data_Get_Struct(obj, ARClass, ar);
	if(ar->connected)
	{
		ARTermination(ar->control, ar->status);
		ar->connected = 0;
		memset(ar->control.passwd[0], '\0', AR_MAX_PASSWORD_SIZE);
	}
	return obj;
}

//Class level method, alternate to new.
//Ruby
//	Ar.open(username=nil, password=nil, server=nil, authstring=nil, localeinfo=nil) do |ar|
//    #do stuff
//  end
static VALUE ar_open(int argc, VALUE *argv, VALUE obj)
{
VALUE nobj = rb_class_new_instance( argc, argv, obj); //new instance of this class.

	if(rb_block_given_p()) //if we have a block.
	{	rb_yield(nobj);
		ar_close(nobj);
	}
	return nobj; //if we didn't have a block, then Ar.open === Ar.new
}

////////////////  attr_readers /////////////////////
static VALUE get_user(VALUE obj)
{
	ARClass *ar;

  Data_Get_Struct(obj, ARClass, ar);
	return rb_str_new2(ar->control.user);
}
//No get_password, for obvious reasons.
static VALUE get_server(VALUE obj)
{
	ARClass *ar;

  Data_Get_Struct(obj, ARClass, ar);
	return rb_str_new2(ar->control.server);
}
static VALUE get_authString(VALUE obj)
{
	ARClass *ar;

  Data_Get_Struct(obj, ARClass, ar);
	return rb_str_new2(ar->control.authString);
}
static VALUE get_localeinfo(VALUE obj)
{
	ARClass *ar;

  Data_Get_Struct(obj, ARClass, ar);
	return rb_str_new2(ar->control.localeinfo);
}
static VALUE get_status(VALUE obj)
{
	ARClass *ar;

  Data_Get_Struct(obj, ARClass, ar);
	return ar_status_list_new(ar->status);
}

////////////////  attr_writer /////////////////////
static VALUE set_user(int argc, VALUE *argv, VALUE obj)
{
	VALUE username;
	ARClass *ar;

	//All arguments are optional as we could load them later.
	rb_scan_args(argc, argv, "1", &username);

  if(TYPE(username) != T_STRING && username != Qnil)
		rb_raise(rb_eTypeError, "username should be a String"); 
	
  Data_Get_Struct(obj, ARClass, ar);
	
	if(username != Qnil)
	{
		strncpy(ar->control.user, STR2CSTR(username), AR_MAX_ACCESS_NAME_SIZE);
		ar->control.user[AR_MAX_ACCESS_NAME_SIZE] = '\0'; //ensure string is null terminated.
	}
	else
		ar->control.user[0] = '\0'; 
	
	return obj;
}

static VALUE set_password(int argc, VALUE *argv, VALUE obj)
{
	VALUE password;
	ARClass *ar;

	//All arguments are optional as we could load them later.
	rb_scan_args(argc, argv, "1", &password);

  if(TYPE(password) != T_STRING && password != Qnil)
		rb_raise(rb_eTypeError, "password should be a String"); 
	
  Data_Get_Struct(obj, ARClass, ar);
	
	if(password != Qnil)
	{
		strncpy(ar->control.password, STR2CSTR(password), AR_MAX_PASSWORD_SIZE);
		ar->control.password[AR_MAX_PASSWORD_SIZE] = '\0'; //ensure string is null terminated.
	}
	else //blat the password.
		memset(ar->control.passwd[0], '\0', AR_MAX_PASSWORD_SIZE);
	
	return obj;
}

static VALUE set_server(int argc, VALUE *argv, VALUE obj)
{
	VALUE server;
	ARClass *ar;

	//All arguments are optional as we could load them later.
	rb_scan_args(argc, argv, "1", &server);

  if(TYPE(server) != T_STRING &&  server != Qnil)
		rb_raise(rb_eTypeError, "server should be a String"); 
	
  Data_Get_Struct(obj, ARClass, ar);
	
	if(server != Qnil)
	{
		strncpy(ar->control.server, STR2CSTR(server), AR_MAX_SERVER_SIZE);
		ar->control.server[AR_MAX_SERVER_SIZE] = '\0'; //ensure string is null terminated.
	}
	else
		ar->control.server[0] = '\0'; 
	
	return obj;
}

static VALUE set_authString(int argc, VALUE *argv, VALUE obj)
{
	VALUE authString;
	ARClass *ar;

	//All arguments are optional as we could load them later.
	rb_scan_args(argc, argv, "1", &authString);

  if(TYPE(authstring) != T_STRING && authString != Qnil)
		rb_raise(rb_eTypeError, "authstring should be a String"); 
	
  Data_Get_Struct(obj, ARClass, ar);
	
	if(authString != Qnil)
	{
		strncpy(ar->control.authString, STR2CSTR(authString), AR_MAX_AUTH_SIZE);
		ar->control.authString[AR_MAX_AUTH_SIZE] = '\0'; //ensure string is null terminated.
	}
	else
		ar->control.authString[0] = '\0'; 
	
	return obj;
}

static VALUE set_localeinfo(int argc, VALUE *argv, VALUE obj)
{
	VALUE localeinfo;
	ARClass *ar;

	//All arguments are optional as we could load them later.
	rb_scan_args(argc, argv, "1", &localeinfo);

  if(TYPE(localeinfo) != T_STRING && localeinfo != Qnil)
		rb_raise(rb_eTypeError, "localeinfo should be a String"); 
	
  Data_Get_Struct(obj, ARClass, ar);
	
	if(localeinfo != Qnil)
	{
		strncpy(ar->control.localeinfo, STR2CSTR(localeinfo), AR_MAX_LOCALE_SIZE);
		ar->control.localeinfo[AR_MAX_LOCALE_SIZE] = '\0'; //ensure string is null terminated.
	}
	else
		ar->control.localeinfo[0] = '\0'; 
	
	return obj;
}

//Specifies the port that your program will use to communicate with the 
//ARSystem server and whether to use a private server.
//args: server=nil, port=nil, rpcProgNum=nil

static VALUE ar_setServerValues(VALUE obj, VALUE server, VALUE port, VALUE rpcProgNum)
{
	ARClass *ar;
	ARStatusList status;
	char *server_p;
	int port_n, rpcProgNum_n;
	
  Data_Get_Struct(obj, ARClass, ar);
	
	if(server == Qnil)
		server_p = ar->control.server;
	else if(TYPE(server) != T_STRING && server != Qnil)
		rb_raise(rb_eTypeError, "server should be a String or nil");
	else
		server_p = STR2CSTR(server);
	
	if(port == Qnil)
		port_n = 0; //causes a reset to the default port
	else if(TYPE(port) != T_FIXNUM)
		rb_raise(rb_eTypeError, "port should be a Fixnum");
	else
		port_n = NUM2INT(port);
		
	if(rpcProgNum == Qnil)
		rpcProgNum_n = 0; //Use the default.
	else if(TYPE(rpcProgNum) != T_FIXNUM)
		rb_raise(rb_eTypeError, "rpcProgNum should be a Fixnum");
	else
		rpcProgNum_n = NUM2INT(rpcProgNum);
		
	if(ARSetServerPort(ar->control, server_p, port_n, rpcProgNum_n, &status)  >= AR_RETURN_ERROR)
	{
		VALUE status_v = ar_status_list_new(ar->status);
		FreeARStatusList(&status, FALSE);
		rb_raise(status_v, "ARSetServerPort failed"); 
	}

	FreeARStatusList(&status, FALSE);
	
	return obj;
}

static VALUE ar_setServerPort(int argc, VALUE *argv, VALUE obj)
{
	VALUE server, port, rpcProgNum;
	ARClass *ar;
	ARStatusList status;
	char *server_p;
	int port_n, rpcProgNum_n;

	//All arguments are optional as we could load them later.
	rb_scan_args(argc, argv, "3", &server, &port, &rpcProgNum);
	return ar_setServerValues(obj, server, port, rpcProgNum);
}

static VALUE ar_setPort(int argc, VALUE *argv, VALUE obj)
{
	VALUE port;

	rb_scan_args(argc, argv, "1",  &port);
	return ar_setServerValues(obj, Qnil, port, Qnil);
}

//Create a Ruby string with the main values of Ar.control.
static VALUE ar_to_s(VALUE obj)
{
	ARClass *ar;
  VALUE s = rb_str_new2("User = \'");

  Data_Get_Struct(obj, ARClass, ar);
	rb_str_cat(s, ar->user, strlen(ar->user));
	rb_str_cat(s, "\'\nServer = \'", 12);
	rb_str_cat(s, ar->server, strlen(ar->server));
	rb_str_cat(s, "\'\nauthString = \'", 16);
	rb_str_cat(s, ar->authString, strlen(ar->authString));
	rb_str_cat(s, "\'\nlocaleinfo = \'", 16);
	rb_str_cat(s, ar->localeinfo, strlen(ar->localeinfo));
	rb_str_cat(s, "\'", 1);
	return s;
}

static VALUE ar_startTransaction(VALUE obj)
{
	ARClass *ar;
	ARStatusList status;
	
  Data_Get_Struct(obj, ARClass, ar);
		
	if(ARBeginBulkEntryTransaction(ar->control, ar_bulk->status)  >= AR_RETURN_ERROR)
	{
		VALUE status_v = ar_status_list_new(&status);
		FreeARStatusList(&status, FALSE);
		rb_raise(status_v, "ARBeginBulkEntryTransaction failed"); 
	}

	FreeARStatusList(&status, FALSE);
	return obj;
}

static VALUE ar_create_bulkEntryReturnList(ARBulkEntryReturnList *bulkEntryReturnList)
{
	int i;
	VALUE bulkEntryReturnList_v = rb_class_new_instance(0, 0,  rb_const_get(rb_cObject, rb_intern("ARBulkEntryReturnList")));
	
	for(i = 0; i < bulkEntryReturnList->numItems; i++)
	{
		switch(bulkEntryReturnList->entryReturnList[i].entryCallType)
		{
			case AR_BULK_ENTRY_CREATE :
				rb_ary_push(bulkEntryReturnList_v, rb_class_new_instance(0, rb_ary_new3(2,,  rb_const_get(rb_cObject, rb_intern("ARCreateEntryReturn"))));
				break;
			case AR_BULK_ENTRY_SET :
				rb_ary_push(bulkEntryReturnList_v, rb_class_new_instance(0, 0,  rb_const_get(rb_cObject, rb_intern("ARSetEntryReturn"))));
				break;
			case AR_BULK_ENTRY_DELETE :
				rb_ary_push(bulkEntryReturnList_v, rb_class_new_instance(0, 0,  rb_const_get(rb_cObject, rb_intern("ARDeleteEntryReturn"))));
				break;
			case AR_BULK_ENTRY_MERGE  :
				rb_ary_push(bulkEntryReturnList_v, rb_class_new_instance(0, 0,  rb_const_get(rb_cObject, rb_intern("ARMergeEntryReturn"))));
				break;
			case AR_BULK_ENTRY_XMLCREATE :
				rb_ary_push(bulkEntryReturnList_v, rb_class_new_instance(0, 0,  rb_const_get(rb_cObject, rb_intern("ARXMLCreateEntryReturn"))));
				break;
			case AR_BULK_ENTRY_XMLSET :
				rb_ary_push(bulkEntryReturnList_v, rb_class_new_instance(0, 0,  rb_const_get(rb_cObject, rb_intern("ARXMLSetEntryReturn"))));
				break;
 			case AR_BULK_ENTRY_XMLDELETE :
				rb_ary_push(bulkEntryReturnList_v, rb_class_new_instance(0, 0,  rb_const_get(rb_cObject, rb_intern("ARXMLDeleteEntryReturn"))));
				break;
		}
	}
	
	return bulkEntryReturnList_v;
}

static VALUE ar_endTransaction(VALUE obj, VALUE action)
{
	ARClass *ar;
	ARStatusList status;
	unsigned int actionType;
	ARBulkEntryReturnList bulkEntryReturnList;
	VALUE bulkEntryReturnList_v;

  Data_Get_Struct(obj, ARClass, ar);

	if(rb_intern("send") == SYM2ID(action)) //were we passed a :send or a :cancel?
		actionType = AR_BULK_ENTRY_ACTION_SEND;
	else
		actionType = AR_BULK_ENTRY_ACTION_CANCEL;
		
  if(ar_bulk->parent_connection->connected == 0)
		rb_raise(rb_eRuntimeError, "Not connected?");
		
	if (AREndBulkEntryTransaction(ar->control, actionType, &bulkEntryReturnList, &status) >= AR_RETURN_ERROR)
	{
		VALUE status_v = ar_status_list_new(&status);
		FreeARStatusList(&status, FALSE);
		rb_raise(status_v, "AREndBulkEntryTransaction failed"); 
	}

	FreeARStatusList(&status, FALSE);
	bulkEntryReturnList_v = ar_create_bulkEntryReturnList(&bulkEntryReturnList);
	return obj; //!!! will want to return the bulkEntryReturnList here.
}

/*Enables plug-ins, the mid tier, and other programs (such as the Email 
Engine) to run as an administrator but to perform operations as a specific 
user (with the user’s permissions and licensing in effect). For more 
information, see “Impersonating a user” on page446. 
*/
static VALUE ar_ARSetImpersonatedUser(VALUE obj, VALUE user)
{
	ARClass *ar;
	ARStatusList status;
	char *s;
	
  Data_Get_Struct(obj, ARClass, ar);

	if(user == Qnil)
		s = NULL
	else 	if(TYPE(user) != T_STRING)
		rb_raise(rb_eTypeError, "user should be a String or nil");
	else
	{
		s = STR2CSTR(user);
		if(s[0] == '\0') s = NULL; //moke wild assumption that they meant to use NULL.
	}
		
	if(ARSetImpersonatedUser(ar->control, s, &status)  >= AR_RETURN_ERROR)
	{
		VALUE status_v = ar_status_list_new(&status);
		FreeARStatusList(&status, FALSE);
		rb_raise(status_v, "ARSetImpersonatedUser failed"); 
	}

	FreeARStatusList(&status, FALSE);
	return obj;
}

/*
	Retrieves a list of rows from the underlying SQL database on the specified 
	server. The server executes the SQL command you specify and returns the 
	matching rows. A list with zero items and a warning message are returned if 
	no SQL database resides on the server. The system returns information based 
	on the access privileges of the user who launched the ARSystem server.
*/
//args: sqlCommand, limit=nil, getCount=nil
static VALUE ar_getListSQL(int argc, VALUE *argv, VALUE obj)
{
	VALUE sqlCommand, maxRetrieve, getCount;
	ARClass *ar;
	ARStatusList status;
	char *sql;
	int maxRows;
	unsigned int nrows = -1;
	ARValueListList valueListList;
	VALUE valueListList_value; //who thought this name up?
	
	rb_scan_args(argc, argv, "12", &sqlCommand, &maxRetrieve, &getCount);
  Data_Get_Struct(obj, ARClass, ar);

	if(TYPE(sqlCommand) != T_STRING)
		rb_raise(rb_eTypeError, "sqlCommand should be a String");
	else
		sql = STR2CSTR(sqlCommand);
	
	if(maxRetrieve == Qnil)
		maxRows = 0; //causes a reset to the default port
	else if(TYPE(maxRetrieve) != T_FIXNUM)
		rb_raise(rb_eTypeError, "maxRetrieve should be a Fixnum or nil");
	else
		maxRows = NUM2INT(maxRetrieve);
		
	if(ARGetListSQL(ar->control, sql, maxRows, &valueListList, getCount == Qtrue ? &nrows : NULL, &status)  >= AR_RETURN_ERROR)
	{
		VALUE status_v = ar_status_list_new(ar->status);
		FreeARStatusList(&status, FALSE);
		FreeARValueListList(&valueListList, FALSE);
		rb_raise(status_v, "ARGetListSQL failed");
	}

	FreeARStatusList(&status, FALSE);
	//Note that we have not freed the valueListList.valueListList
	valueListList_value = ar_valueListList_new(&valueListList);
	
	//Nb. not using FreeARValueListList, as we want to retain the ARValueStruct members.
	//Will fix later. For now the ar_valueListList_new() call is retaining pointers that may be in the ARValueStruct.
	for(i=0; i < valueListList.numItems; i++)
		free(valueListList.valueListList[i].valueList); //free the array of values pointed to be this pointer
	free(valueListList->valueListList); //free the array of valueLists.
	
	if(getCount == Qtrue)
		return rb_ary_new3(2, valueListList_value,  INT2FIX(nrows) );  //Returning an array to caller.
	else
		return valueListList_value; 
}

//	Active links 
// -Remove line when processed -  "ar_links.c"
//	Containers 
// -Remove line when processed -  "ar_containers.c"
//	Entries 
// -Remove line when processed -  "ar_entries.c"
//	Escalations 
// -Remove line when processed -  "ar_escalations.c"
//	Fields 
// -Remove line when processed -  "ar_fields.c"
//	Filters 
// -Remove line when processed -  "ar_filters.c"
//	Character menus 
// -Remove line when processed -  "ar_menus.c"
//	Forms (schemas) 
// -Remove line when processed -  "ar_forms.c"
//	Support files 
// -Remove line when processed -  "ar_support.c"
//	VUIs 
// -Remove line when processed -  "ar_vuis.c"
//	Alert functions 
// -Remove line when processed -  "ar_alerts.c"
//	Access control 
// -Remove line when processed -  "ar_access_ctl.c"
//	Currency 
// -Remove line when processed -  "ar_currency.c"
//	Data structure help functions 
// -Remove line when processed -  "ar_helpers.c"
//	Licensing 
// -Remove line when processed -  "ar_licensing.c"
//	Object definitions and external data manipulation 
// -Remove line when processed -  "ar_defs.c"
//	Server processes 
// -Remove line when processed -  "ar_server_proc.c"
//	Text manipulation 
// -Remove line when processed -  "ar_text.c"


void Init_Ar()
{
  // Create a Ruby class in this module.
  // rb_cObject is defined in ruby.h
  
  myModule = rb_define_module("ARMod");
  myClass = rb_define_class_under(myModule, "Ar", rb_cObject);
  rb_define_alloc_func(myClass, ar_alloc); 

	//Ar.new
  rb_define_method(myClass, "initialize", ar_initialize, -1); 
	//Class methods
  rb_define_module_function(myClass, "open", ar_open, -1);

	//instance methods
  rb_define_method(myClass, "close", ar_close, 0);
	rb_define_method(myClass, "setServerPort", ar_setServerPort, -1);//args: server=nil, port=nil, rpcProgNum=nil
	rb_define_method(myClass, "port=", ar_setPort, -1);//args: port=nil
	rb_define_method(myClass, "getListSQL", ar_getListSQL, -1); //args: sqlCommand, limit=nil, getCount=nil
	
	//attr_readers
  rb_define_method(myClass, "user", ar_get_user, 0);
  rb_define_method(myClass, "server", ar_get_server, 0);
  rb_define_method(myClass, "authString", ar_get_authString, 0);
  rb_define_method(myClass, "localeinfo", ar_get_localeinfo, 0);
  rb_define_method(myClass, "status", ar_get_status, 0);
	//attr_writers
	rb_define_method(myClass, "user=", ar_set_user, -1);
  rb_define_method(myClass, "password=", ar_set_status, -1);
  rb_define_method(myClass, "server=", ar_set_server, -1);
  rb_define_method(myClass, "authString=", ar_set_authString, -1);
  rb_define_method(myClass, "localeinfo=", ar_set_localeinfo, -1);
  
	rb_define_method(myClass, "to_s", ar_to_s, 0);
	
	Init_status_list(myModule); //create the ARStatusList class.
	Init_status(myModule); //create the ARStatus class.

//	Active links 
//	ar_links_init()
//	Containers 
//  containers_init()
//	Entries 
//  entries_init()
//	Escalations 
//  escalations_init()
//	Fields 
//  fields_init()
//	Filters 
//  filters_init()
//	Character menus 
//  menus_init()
//	Forms (schemas) 
//  forms_init()
//	Support files 
//  support_init()
//	VUIs 
//  vuis_init()
//	Alert functions 
//  alerts_init()
//	Access control 
//  access_ctl_init()
//	Bulk entry 
  rb_define_method(myClass, "beginBulkEntryTransaction", ar_startTransaction , 0);
  rb_define_method(myClass, "endBulkEntryTransaction", ar_endTransaction , 1);
//	Currency 
//  currency_init()
//	Data structure help functions 
//  helpers_init()
//	Licensing 
//  licensing_init()
//	Object definitions and external data manipulation 
//  defs_init()
//	Server processes 
//  server_proc_init()
//	Text manipulation 
//  text_init()
	
}
