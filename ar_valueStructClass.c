#include "ar_class.h"

static VALUE myClass;  //For this ruby class.

static void ar_value_mark(ARValueStructClass *arvs)
{
	//don't need to do anything.  
}

static void ar_value_free(ARValueStructClass *arvs)
{
	if(arvs)
	{
		FreeARValueStruct(arvs->value, FALSE);
		free(arvs);
	}
}

static VALUE ar_value_alloc(VALUE klass)
{
	ARValueStructClass *arvs = calloc(1, sizeof(ARValueStructClass));
	
  return Data_Wrap_Struct(klass, ar_value_mark, ar_value_free, arvs); //wraps a c struct in VALUE
  //recover with Data_Get_Struct()
}

char * dataTypes[] = {  "NULL","KEYWORD","INTEGER","REAL","CHAR","DIARY","ENUM","TIME","BITMASK","BYTES","DECIMAL","ATTACH,CURRENCY","DATE","TIME_OF_DAY",
												NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
												"JOIN","TRIM","CONTROL","TABLE","COLUMN","PAGE","PAGE_HOLDER","ATTACH_POOL", NULL, NULL, 
												"ULONG","COORDS",
												"VIEW","DISPLAY" 
										 };

static VALUE dataType_Symbol_Map; //Ruby Hash of symbols to AR_DATA_TYPE_XXXX
static void create_dataType_hash(void)
{
	dataType_Symbol_Map = rb_hash_new(); //Create a Ruby hash.
	rb_hash_aset(dataType_Symbol_Map, rb_intern("null"), INT2FIX(AR_DATA_TYPE_NULL));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("keyword"), INT2FIX(AR_DATA_TYPE_KEYWORD));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("integer"), INT2FIX(AR_DATA_TYPE_INTEGER));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("real"), INT2FIX(AR_DATA_TYPE_REAL));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("char"), AR_DATA_TYPE_CHAR));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("diary"), AR_DATA_TYPE_DIARY));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("enum"), INT2FIX(AR_DATA_TYPE_ENUM));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("time"), INT2FIX(AR_DATA_TYPE_TIME));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("bitmask"), INT2FIX(AR_DATA_TYPE_BITMASK));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("bytes"), INT2FIX(AR_DATA_TYPE_BYTES));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("decimal"), INT2FIX(AR_DATA_TYPE_DECIMAL));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("attach"), INT2FIX(AR_DATA_TYPE_ATTACH)); //haven't implementd Ruby === yet.
	rb_hash_aset(dataType_Symbol_Map, rb_intern("currency"), INT2FIX(AR_DATA_TYPE_CURRENCY));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("date"), INT2FIX(AR_DATA_TYPE_DATE));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("time_of_day"), INT2FIX(AR_DATA_TYPE_TIME_OF_DAY));
//End of standard types

//Don't know what these are, and there isn't an associated variable in the ARValueStruct union.
	rb_hash_aset(dataType_Symbol_Map, rb_intern("join"), INT2FIX(AR_DATA_TYPE_JOIN));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("trim"), INT2FIX(AR_DATA_TYPE_TRIM));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("control"), INT2FIX(AR_DATA_TYPE_CONTROL));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("table"), INT2FIX(AR_DATA_TYPE_TABLE));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("column"), INT2FIX(AR_DATA_TYPE_COLUMN));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("page"), INT2FIX(AR_DATA_TYPE_PAGE));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("page_holder"), INT2FIX(AR_DATA_TYPE_PAGE_HOLDER));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("attach_pool"), INT2FIX(AR_DATA_TYPE_ATTACH_POOL));

//The next two have associated definitions in the union.
	rb_hash_aset(dataType_Symbol_Map, rb_intern("ulong"), INT2FIX(AR_DATA_TYPE_ULONG));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("coords"), INT2FIX(AR_DATA_TYPE_COORDS));
//The following don't have storage definitions?
	rb_hash_aset(dataType_Symbol_Map, rb_intern("view"), INT2FIX(AR_DATA_TYPE_VIEW));
	rb_hash_aset(dataType_Symbol_Map, rb_intern("display"), INT2FIX(AR_DATA_TYPE_DISPLAY));

}

static int symbol_to_dataType(VALUE symbol)
{
	VALUE ar_data_type = rb_hash_aref(dataType_Symbol_Map, symbol); // => dataType field from hash
	if(ar_data_type == Qnil) return AR_DATA_TYPE_NULL;
	else return  NUM2INT(ar_data_type);
}

static char dataType_to_char(int dataType)
{ static char buffer[32];
	if(dataType < 0 || dataType > AR_DATA_TYPE_MAX_TYPE || dataTypes[dataType] == NULL)
		sprintf(buffer, "Unknown dataType %d", dataType);
	else
		sprintf(buffer, "AR_DATA_TYPE_%s", dataTypes[dataType]);
	return buffer;
}

static void ar_to_value(int dataType, ARValueStructClass *arvs, VALUE value)
{
	switch(arvs->value.dataType = dataType)
		case AR_DATA_TYPE_NULL:				
			/* code for a NULL value */
			return;
		case AR_DATA_TYPE_KEYWORD: 		/* code indicating a keyword setting */
			//unsigned int       keyNum;
			arvs->keyNum = NUM2UINT(value); 
			return;
		case AR_DATA_TYPE_INTEGER: 		/* codes for the data type of a value */
	  	//ARLong32           intVal;
			arvs->intVal = NUM2INT(value);
			return;
		case AR_DATA_TYPE_REAL:
	  	//double             realVal;
			arvs->realVal = NUM2DBL(value);
			return;
		case AR_DATA_TYPE_CHAR:
	  	//char              *charVal;
			arvs->charVal = STR2CSTR(value);
			return;
		case AR_DATA_TYPE_DIARY: //might want to subclass String to be able to identify these coming back to us.
	  	//char              *diaryVal;
			arvs->diaryVal = STR2CSTR(value);
			return;
		case AR_DATA_TYPE_ENUM:
			//ARULong32          enumVal;
			arvs->enumVal = NUM2ULONG(value);
			return;
		case AR_DATA_TYPE_TIME: 
	  	//ARTimestamp        timeVal;
			arvs->timeVal = (time_t) rb_func_call(value, rb_ident("to_i"), 0 ); //execute value.to_i to get a time_t
			return;
		case AR_DATA_TYPE_BITMASK:
	  	//ARULong32          maskVal;
			arvs->maskVal = NUM2ULONG(value); 
			return;
		case AR_DATA_TYPE_DECIMA:
			//char              *decimalVal;
			arvs->decimalVal = STR2CSTR(value);
			return;
		case AR_DATA_TYPE_DATE:
			//int                dateVal;
			arvs->dateVal = NUM2INT(value); 
			return;
		case AR_DATA_TYPE_TIME_OF_DAY:
			//ARTime             timeOfDayVal;
			arvs->timeOfDayVal = NUM2INT(value); 
			return;
		case AR_DATA_TYPE_ULONG:
			//ARULong32          ulongVal;
			arvs->ulongVal = NUM2ULONG(value); 
			return;
		case AR_DATA_TYPE_BYTES:
				//ARByteList        *byteListVal;
				//needs its own class to hold the type of byte stream. Right now this is being given as two values in an array.
			 //really should check we have an array of 2 values (i.e. [type, [bytes]]! This could crash badly at the moment.
		{
				arvs->byteListVal = calloc(1, sizeof(ARByteList));
				arvs->byteListVal->type = NUM2INT(rb_ary_entry(value,0));
				arvs->byteListVal->numItems = RARRAY(rb_ary_entry(value,1))->len;
				arvs->byteListVal->bytes = calloc(arvs->byteListVal->numItems, sizeof(char));
				memcpy(arvs->byteListVal->bytes, RARRAY(rb_ary_entry(value,1))->ptr, arvs->byteListVal->numItems); //Better check this actually gets the raw bytes!
				return; 
		}
		case AR_DATA_TYPE_CURRENCY:
				//ARCurrencyStruct  *currencyVal; //need a currency class.
				//Currently return [ valueStr, CurrencyCodeStr, ConvTimeStamp, [ [valueStr, CurrencyCodeStr]...]
			 //really should check we have an array of 4 values! This could crash badly at the moment.
		{	VALUE converted = rb_ary_entry(value,3); //This should be an array of converted values.
			int i;
			
			arvs->currencyVal = calloc(1, sizeof(ARCurrencyStruct));
			arvs->currencyVal->value = STR2CSTR(rb_ary_entry(value,0));
			strncpy(arvs->currencyVal->currencyCode, STR2CSTR(rb_ary_entry(value,1)), AR_MAX_CURRENCY_CODE_SIZE);
			arvs->currencyVal->currencyCode[AR_MAX_CURRENCY_CODE_SIZE] = '\0';
			arvs->currencyVal->conversionDate = (time_t) rb_func_call(rb_ary_entry(value,2), rb_ident("to_i"), 0 );
			arvs->currencyVal->funcList.numItems = RARRAY(converted)->len;
			arvs->currencyVal->funcList.funcCurrencyList = calloc(arvs->currencyVal->funcList.numItems, sizeof(ARFuncCurrencyStruct)); 

			for(i = 0; i < arvs->currencyVal->funcList.numItems; i++)
			{	//converted currency pairs. [value, currencyCode]. Both are stored as strings, though currencyCode is defined as an char array and value as char *
				arvs->currencyVal->funcList.funcCurrencyList[i].value = STR2CSTR(rb_ary_entry(rb_ary_entry(converted,i), 0));
				strncpy(arvs->currencyVal->funcList.funcCurrencyList[i].currencyCode, STR2CSTR(rb_ary_entry(rb_ary_entry(converted,i), 1)), AR_MAX_CURRENCY_CODE_SIZE);
				arvs->currencyVal->funcList.funcCurrencyList[i].currencyCode[AR_MAX_CURRENCY_CODE_SIZE] = '\0';
			}
			return;
		}			
		case AR_DATA_TYPE_COORDS:
			//ARCoordList       *coordListVal; //Needs its own Class. 
			//For now, returns an array of arrays. e.g. [[x1,y1], ... [xn, yn]]
		{	int i;
			
			arvs->coordListVal = calloc(1, sizeof(ARCoordList));
			arvs->coordListVal->numItems = RARRAY(value)->len;
			arvs->coordListVal->coords = calloc(arvs->coordListVal->numItems, sizeof(ARCoordStruct));
			
			for(i = 0; i < arvs->coordListVal->numItems; i++)
			{	
				arvs->arvs->coordListVal->coords->x = rb_ary_entry(rb_ary_entry(value,i), 0)
				arvs->arvs->coordListVal->coords->y = rb_ary_entry(rb_ary_entry(value,i), 1)
			}
			return;
		}
		case AR_DATA_TYPE_ATTACH:
	  	//ARAttachStruct    *attachVal;
			raise(rb_eNotImplementedError, "Haven't implemented attachments yet");
		//I can't see any field in the union, defined in ARValueStruct in ar.h, that holds the following?
		case AR_DATA_TYPE_JOIN:
		case AR_DATA_TYPE_TRIM:    			/* visual trim field */
		case AR_DATA_TYPE_CONTROL: 			/* control (button/menu/icon) field */
		case AR_DATA_TYPE_TABLE:   			/* table field */
		case AR_DATA_TYPE_COLUMN:  			/* column field */
		case AR_DATA_TYPE_PAGE:    			/* page field */
		case AR_DATA_TYPE_PAGE_HOLDER:	/* page holder field */
		case AR_DATA_TYPE_ATTACH_POOL: 	/* attachment pool */
		case AR_DATA_TYPE_VIEW: 			/* view field */
		case AR_DATA_TYPE_DISPLAY: 		/* display field (e.g., flashlets) */
			raise(rb_eNotImplementedError, "Not implemented. Can't yet see where they are stared in the Union.");
		else
			raise(rb_eTypeError, "Unknown type for ARValueStruct");
		}
}

static VALUE ar_to_ruby(ARValueStructClass *arvs)
{
	switch(dataType)
	{
	case AR_DATA_TYPE_NULL:				
		/* code for a NULL value */
		return Qnil;
	case AR_DATA_TYPE_KEYWORD: 		/* code indicating a keyword setting */
		//unsigned int       keyNum;
		return UINT2NUM(arvs->value.keyNum); //Later we should look at returning a symbol with the #define name.
	case AR_DATA_TYPE_INTEGER: 		/* codes for the data type of a value */
  	//ARLong32           intVal;
		return INT2FIX(arvs->value.intVal); 
	case AR_DATA_TYPE_REAL:
  	//double             realVal;
		return rb_float_new(arvs->value.realVal);
	case AR_DATA_TYPE_CHAR:
  	//char              *charVal;
		if(arvs->value.charVal == NULL) return Qnil;
		else	return rb_str_new2(arvs->value.charVal);
	case AR_DATA_TYPE_DIARY: //might want to subclass String to be able to identify these coming back to us.
  	//char              *diaryVal;
		if(arvs->value.diaryVal == NULL) return Qnil;
		else	return rb_str_new2(arvs->value.diaryVal);
	case AR_DATA_TYPE_ENUM:
		//ARULong32          enumVal;
		return ULONG2NUM(arvs->value.enumVal); //Later we should look at returning an enum class, but would need to know what the bits stood for.
	case AR_DATA_TYPE_TIME: 
  	//ARTimestamp        timeVal;
		return rb_time_new((time_t)arvs->value.timeVal, 0);
	case AR_DATA_TYPE_BITMASK:
  	//ARULong32          maskVal;
		return ULONG2NUM(arvs->value.maskVal) //Should create a class so we can spot this coming back to us.
	case AR_DATA_TYPE_BYTES:
		//ARByteList        *byteListVal;
		//needs its own class to hold the type of byte stream. Right now this is being returned as two values in an array.
	{ VALUE b = rb_ary_new2( arvs->value.byteListVal->numItems );
		memcpy(RARRAY(b)->ptr, arvs->value.byteListVal->bytes, arvs->value.byteListVal->numItems);
		return rb_ary_new3(2, INT2FIX(arvs->value.byteListVal.type), b); 
	}
	case AR_DATA_TYPE_DECIMA:
		//char              *decimalVal;
		if(arvs->value.decimalVal == NULL) return Qnil; //needs its own class to hold BCD values.
		else	return rb_str_new2(arvs->value.decimalVal);
	case AR_DATA_TYPE_DATE:
		//int                dateVal;
		return INT2FIX(arvs->value.dateVal); //Not sure of the format, can't convert to Time class.
	case AR_DATA_TYPE_TIME_OF_DAY:
		//ARTime             timeOfDayVal;
		return INT2FIX(arvs->value.timeOfDayVal); //seconds since midnight.
	case AR_DATA_TYPE_ULONG:
		//ARULong32          ulongVal;
		return ULONG2NUM(arvs->value.ulongVal); //no Ruby equivalent.
	case AR_DATA_TYPE_CURRENCY:
			//ARCurrencyStruct  *currencyVal; //need a currency class.
			//Currently return [ valueStr, CurrencyCodeStr, ConvTimeStamp, [ [valueStr, CurrencyCodeStr]...]
	{	VALUE converted = rb_ary_new2( arvs->value.currencyVal->funcList.numItems );
		VALUE currency = rb_ary_new3(4,rb_str_new2(arvs->value.currencyVal->value), rb_str_new2(arvs->value.currencyVal->currencyCode), rb_time_new((time_t)arvs->value.currencyVal->conversionDate, 0), converted);
		int i;
		for(i = 0; i < arvs->value.currencyVal->funcList.numItems; i++)
		{	//converted currency pairs. [value, currencyCode]. Both are stored as strings, though currencyCode is defined as an char array and value as char *
			rb_ary_store( converted, i, rb_ary_new3(2, rb_str_new2(arvs->value.currencyVal->funcList.funcCurrencyList[i].value), rb_str_new2(arvs->value.currencyVal->funcList.funcCurrencyList[i].currencyCode) ) );
		}
		return currency;
	}			
	case AR_DATA_TYPE_COORDS:
		//ARCoordList       *coordListVal; //Needs its own Class. 
		//For now, returns an array of arrays. e.g. [[x1,y1], ... [xn, yn]]
	{	VALUE a = rb_ary_new2( arvs->value.coordListVal->numItems );
		int i;
		for(i = 0; i < arvs->value.coordListVal->numItems; i++)
		{	
			rb_ary_store( a, i, rb_ary_new3(2, INT2FIX(arvs->value.coordListVal->coords->x), INT2FIX(arvs->value.coordListVal->coords->y) ) );
		}
		return a;
	}
	case AR_DATA_TYPE_ATTACH:
  	//ARAttachStruct    *attachVal;
		raise(rb_eNotImplementedError, "Haven't implemented attachments yet");
	//I can't see any field in the union, defined in ARValueStruct in ar.h, that holds the following?
	case AR_DATA_TYPE_JOIN:
	case AR_DATA_TYPE_TRIM:    			/* visual trim field */
	case AR_DATA_TYPE_CONTROL: 			/* control (button/menu/icon) field */
	case AR_DATA_TYPE_TABLE:   			/* table field */
	case AR_DATA_TYPE_COLUMN:  			/* column field */
	case AR_DATA_TYPE_PAGE:    			/* page field */
	case AR_DATA_TYPE_PAGE_HOLDER:	/* page holder field */
	case AR_DATA_TYPE_ATTACH_POOL: 	/* attachment pool */
	case AR_DATA_TYPE_VIEW: 			/* view field */
	case AR_DATA_TYPE_DISPLAY: 		/* display field (e.g., flashlets) */
		raise(rb_eNotImplementedError, "Not implemented. Can't yet see where they are stared in the Union.");
	else
		raise(eTypeError, "Unknown type %d in ARValueStruct", arvs->value.dataType);
	}
}

//This is a read only representation of the C ARStatusList held in other classes
//The header for the status list is copied in at a C level, not in Ruby.
//Hence we have no arguments to .new
static VALUE ar_value_initialize(int argc, VALUE *argv, VALUE obj)
{
	ARValueStructClass *arvs;
  
  Data_Get_Struct(obj, ARValueStructClass, arvs);
	if(argc == 0)
		ar_to_value(arvs, symbol_to_dataType(rb_intern("null")), Qnil)
	else if(argc == 2)
		ar_to_value(arvs, symbol_to_dataType(SYM2ID(argv[0])), argv[1]); //symbol, value
	else
		raise(rb_eArgumentError, "Either 0 arguments, or (Symbol,value)");
		
  return obj;
}

static VALUE ar_get_value(VALUE obj)
{
	ARValueStructClass *arvs;
  
  Data_Get_Struct(obj, ARValueStructClass, arvs);
	return ar_to_ruby(arvs);
}

static VALUE ar_get_type(VALUE obj)
{
	ARValueStructClass *arvs;
  
  Data_Get_Struct(obj, ARValueStructClass, arvs);
	return INT2FIX(arvs->value.dataType);
}

static VALUE ar_set_value(int argc, VALUE *argv, VALUE obj)
{
	ARValueStructClass *arvs;
	VALUE value;
	
	rb_scan_args(argc, argv, "1", &value);
  Data_Get_Struct(obj, ARValueStructClass, arvs);
	ar_to_value(arvs, arvs->value.dataType, &value)	 //save to whatever datatype has already been set.

	return obj;
}

static VALUE ar_set_type(VALUE obj, VALUE symbol)
{
	ARValueStructClass *arvs;
  
  Data_Get_Struct(obj, ARValueStructClass, arvs);
	arvs->value.dataType = symbol_to_dataType(dataType_symbol);
	return obj;
}

static VALUE ar_method_missing(int argc, VALUE *argv, VALUE obj)
{	VALUE method_id, method_arg;
	char *method_name;
	int l;
	ARValueStructClass *arvs;
	int dataType;
  
	
	rb_scan_args(argc, argv, "11", &method_id, &method_arg);
	method_name = rb_id2name(FIX2INT(method_id));

  Data_Get_Struct(obj, ARValueStructClass, arvs);
	
	if((l = strlen(method_name)) > 1 && method_name[l-1] == '=')
	{	//We have an assignment, so should have an argument.
		if(argc != 2) //2, not 1, as the method id is arg 1.
			raise(rb_eArgumentError, "Assignment requires argument");
		//Remove the = and look up the symbol map to dataType.
		method_name[l-1] = '\0';
		if( ( dataType = symbol_to_dataType(rb_intern(method_name)) ) != AR_DATA_TYPE_NULL || (argc == 2 && argv[0] == Qnil))
		{
			ar_to_value(arvs, dataType, argv[1]);
			return obj;
		}
		else //We don't have this dataType.
			rb_raise(NoMethodError, "No %s= method", method_name);
	}
	else if((dataType = symbol_to_dataType(dataType_symbol)) != AR_DATA_TYPE_NULL || (argc == 2 && argv[0] == Qnil))
	{	  //We have a fetch.
			if(argc != 1) //1 Arg, not 0, as the method id is arg 1.
				raise(rb_eArgumentError, "%s has no argument", method_name);
			//Check that the types match.
			if(arvs->value.dataType != dataType)
				raise(rb_eTypeError, "Datatype not %s, but %s", method_name, dataType_to_char(arvs->value.dataType));
			return ar_to_ruby(arvs); 
	}
	else //Unknown Type, so no auto method for it.
		rb_raise(NoMethodError, "No %s method", method_name);
	
	return Qnil; //never get here.
}

//to_s: return "AR_DATA_TYPE_XXX = #{obj.value.to_s}"
static VALUE ar_value_to_s(VALUE obj)
{ VALUE s;
	ARValueStructClass *arvs;
  
  Data_Get_Struct(obj, ARValueStructClass, arvs);
	
	s = rb_str_new2(dataType_to_char(arvs->value.dataType));
	rb_str_cat(s, " = ", 3);
	rb_str_concat(s, rb_func_call( ar_to_ruby(arvs), rb_ident("to_s"), 0 ) );

	return s;
}

VALUE ar_valueStruct_new(ARValueStruct *avrs_new)
{ ARValueStructClass *arvs;
	VALUE arv = ar_value_initialize(0, NULL, myClass);
	
  Data_Get_Struct(obj, ARValueStructClass, arvs);
	memcpy(arvs->value, arvs_new, sizeof(ARValueStruct)); //replace this with copy that copies any subStructs.
	//WARNING    We didn't copy any complex structures pointed to by arvs_new.
	//           The caller must not free these!!!!!!!!!!!!!!!!!!!!!!!!!!
	//We need to fix this by making a copy of data pointed to from ARValueStruct.
	return arv;
}

VALUE ar_valueStructList_new(ARValueList *arvl)
{//ARValueList is just a ruby array equivalent.
	VALUE arvl_value = rb_ary_new2(arvl->numItems);
	int i;
	for(i = 0; i < arvl->numItems; i++)
		rb_ary_store(arvl_value, i, ar_valueStruct_new(arvl->valueList[i]);
	return arvl_value;
}

VALUE ar_valueStructListList_new(ARValueListList *arvll)
{ //ARValueListList is just a ruby array equivalent.
	VALUE arvll_value = rb_ary_new2(arvll->numItems);
	int i;
	for(i = 0; i < arvll->numItems; i++)
		rb_ary_store(arvll_value, i, ar_valueStructList_new(arvll->valueListList[i]);
	return arvll_value;
}

VALUE Init_value(VALUE myModule)
{
  myClass = rb_define_class_under(myModule, "ARValue", rb_cObject);
  rb_define_alloc_func(myClass, ar_value_alloc);
	create_dataType_hash();
	
  rb_define_method(myClass, "initialize", ar_value_initialize, 0);
	//Class methods

  //instance methods.
	rb_define_method(myClass, "value", ar_get_value, 0);
	rb_define_method(myClass, "type", ar_get_type, 0);
  rb_define_method(myClass, "value=", ar_set_value, 1);
	rb_define_method(myClass, "type=", ar_set_type, 1);
	rb_define_method(myClass, "to_s", ar_value_to_s, 0);
	//pick up any attempts to get or set ARValueStruct vars by name.
	rb_define_method(myClass, "method_missing", ar_method_missing, -1); 
  
	return myClass; 
}
