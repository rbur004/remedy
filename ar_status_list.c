#include "ar_class.h"

static VALUE myClass;  //For this ruby class.

static void ar_status_list_mark(ARStatusListClass *arsl)
{
	//don't need to do anything.  
}

static void ar_status_list_free(ARStatusListClass *arsl)
{
	if(arsl)
	{
		FreeARStatusList(arsl->status_list, TRUE); //ensure we free any status records.
		free(arsl); 
	}
}

static VALUE ar_status_list_alloc(VALUE klass)
{
	ARStatusListClass *arsl = calloc(1, sizeof(ARStatusListClass *));
	
  return Data_Wrap_Struct(klass, ar_status_list_mark, ar_status_list_free, arsl); //wraps a c struct in VALUE
  //recover with Data_Get_Struct()
}

//This is a read only representation of the C ARStatusList held in other classes
//The header for the status list is copied in at a C level, not in Ruby.
//Hence we have no arguments to .new
static VALUE ar_status_list_initialize(VALUE obj)
{
  return obj;
}

static void copy_status_list(ARStatusList *ar_status_list, ARStatusList *status_list)
{
	//we are copying, rather than referencing the status list, as the parent
	//class may have raised an exception, and freed the status list.
	int i;
		
	ar_status_list->numItems = status_list->numItems;
	ar_status_list->statusList = calloc(ar_status_list->numItems, sizeof(ARStatusStruct);
	for(i = 0; i < ar_status_list->numItems; i++)
	{
	  ar_status_list->statusList[i].messageType = status_list->statusList[i].messageType;
	  ar_status_list->statusList[i].messageNum = status_list->statusList[i].messageNum;
		if(status_list->statusList[i].messageText != NULL)
	  	ar_status_list->statusList[i].messageText = strdup(status_list->statusList[i].messageText);
		if(status_list->statusList[i].appendedText != NULL)
	  	ar_status_list->statusList[i].appendedText = strdup(status_list->statusList[i].appendedText);
  }
}

//helper for status exception class creation from other c code.
//This is the actual creation function for this class. .new would not be called.
VALUE ar_status_list_new(ARStatusList *status_list)
{
	VALUE nobj = rb_class_new_instance( argc, argv, myClass); //new instance of this class.
	ARStatusListClass *arsl;

  Data_Get_Struct(nobj, ARStatusListClass, arsl);
	copy_status_list(arsl->status_list,status_list);

	return nobj; 
}

static VALUE ar_status_list_each_message(VALUE obj)
{
	ARStatusListClass *arsl;
	int i;

  Data_Get_Struct(obj, ARStatusListClass, arsl);

	if(rb_block_given_p()) //if we have a block.
	{
		for(i = 0; i < arsl->numItems; i++)
			rb_yield( rb_ary_new3(4, INT2FIX(arsl->statusList[i].messageType), INT2FIX(arsl->statusList[i].messageNum), 
																rb_str_new2(arsl->statusList[i].messageText == NULL ? "" : arsl->statusList[i].messageText), 
																rb_str_new2(arsl->statusList[i].appendedText == NULL ? "" : arsl->statusList[i].appendedText ) ) );
	}
	
	return obj;
}

static VALUE ar_status_list_message(VALUE obj, VALUE index)
{
	ARStatusListClass *arsl;
	int i;

  Data_Get_Struct(obj, ARStatusListClass, arsl);
	if(TYPE(index) != T_FIXNUM)
		rb_raise(rb_eTypeError, "StatusList message index should be FIXNUM");
		
	i = NUM2INT(index);

	if(i >= arsl->numItems || i < 0)
		rb_raise(rb_eIndexError, "StatusList message index out of bounds"); 
		
	return rb_ary_new3(4, INT2FIX(arsl->statusList[i].messageType), INT2FIX(arsl->statusList[i].messageNum), 
												rb_str_new2(arsl->statusList[i].messageText == NULL ? "" : arsl->statusList[i].messageText), 
												rb_str_new2(arsl->statusList[i].appendedText == NULL ? "" : arsl->statusList[i].appendedText ) );
}

static char *AR_RETURN[] = { "OK","WARNING","ERROR","FATAL","BAD_STATUS","PROMPT","ACCESSIBLE"};

static VALUE ar_messageType_to_s(VALUE obj, VALUE messageType)
{
	int i;

	if(TYPE(messageType) != T_FIXNUM)
		rb_raise(rb_eTypeError, "StatusList messageType should be FIXNUM");
		
	i = NUM2INT(messageType);

	if(i >= (sizeof(AR_RETURN)/sizeof(char *)) || i < 0)
		return rb_str_new2( "StatusList messageType unknown"); //could be we linked to a newer AR library.
		

	return rb_str_new2( AR_RETURN[arsl->statusList[i].messageType] ); 
}

//Class level function, as unrelated to a specific instance.
//Retrieves the message text for the specified error from the local catalog (in the 
//local language). The length of the text is limited by AR_MAX_MESSAGE_SIZE 
//(255 bytes). 
static VALUE ar_getTextForErrorMessage(VALUE obj, VALUE messageNumber)
{
	int i;
	
	if(TYPE(messageNumber) != T_FIXNUM)
		rb_raise(rb_eTypeError, "StatusList messageNumber should be FIXNUM");

	if((message = ARGetTextForErrorMessage(NUM2INT(messageNumber)) != NULL)
	{
		VALUE m =	rb_str_new2(message);
		free(message);
		return m;
	}
	return rb_str_new2("Unknown Message Number"); //should we raise this, rather than return it?
}

//Instance level method for ARGetTextForErrorMessage, passing it the messages index.
static VALUE ar_getMessageText(VALUE obj, VALUE index)
{
	ARStatusListClass *arsl;
	int i;

  Data_Get_Struct(obj, ARStatusListClass, arsl);
	if(TYPE(index) != T_FIXNUM)
		rb_raise(rb_eTypeError, "StatusList message index should be FIXNUM");
		
	i = NUM2INT(index);

	if(i >= arsl->numItems || i < 0)
		rb_raise(rb_eIndexError, "StatusList message index out of bounds"); 
		
	return ar_getTextForErrorMessage(obj, INT2FIX(arsl->statusList[i].messageNum) );
}

static VALUE ar_status_list_to_s(VALUE obj)
{
	ARStatusListClass *arsl;
	int i;
	VALUE s = rb_str_new2("");
	char buffer[16];
  
  Data_Get_Struct(obj, ARStatusListClass, arsl);

	for(i = 0; i < arsl->numItems; i++)
	{
		v = arsl->statusList[i].messageText == NULL ? "" : arsl->statusList[i].messageText;
		rb_str_cat(s, v, strlen(v));
		if(arsl->statusList[i].appendedText != NULL && arsl->statusList[i].appendedText[0] != '\0')
		{
			rb_str_cat(s, " : ", 3);
			rb_str_cat(s, arsl->statusList[i].appendedText, strlen(arsl->statusList[i].appendedText));
		}
		rb_str_cat(s, " (", 2);
		if(i >= (sizeof(AR_RETURN)/sizeof(char *)) || i < 0)
			rb_str_cat(s, "StatusList messageType unknown", sizeof("StatusList messageType unknown")); //could be we linked to a newer AR library.
		else
			rb_str_cat(s, AR_RETURN[arsl->statusList[i].messageType], strlen(AR_RETURN[arsl->statusList[i].messageType])); 
		rb_str_cat(s, ":", 1);
		sprintf(buffer, "%d", arsl->statusList[i].messageNum);
		rb_str_cat(s, buffer, sizeof(buffer));
		rb_str_cat(s, ")\n", 2);
	}
	return s;
}

VALUE Init_status_list(VALUE myModule)
{
  myClass = rb_define_class_under(myModule, "ARStatusList", rb_cObject);
  rb_define_alloc_func(myClass, ar_status_list_alloc);
  rb_define_method(myClass, "initialize", ar_status_list_initialize, 0);
	//Class methods
  rb_define_module_function(myClass, "getTextForErrorMessage", ar_getTextForErrorMessage, 1);

  //instance methods.
	rb_define_method(myClass, "each", ar_status_list_each_message, 0);
  rb_define_method(myClass, "[]", ar_status_list_message, 1);
  rb_define_method(myClass, "getTextForErrorMessage", ar_getMessageText, 1);

	rb_define_method(myClass, "messageType_to_s", ar_messageType_to_s, 1);
rb_define_method(myClass, "to_s", ar_status_list_to_s, 0);
  
	return myClass;
}
