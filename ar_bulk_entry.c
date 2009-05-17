/*
descriptions
	Description Marks the beginning of a series of entry API function calls that will be 
grouped together and sent to the ARSystem server as part of one transaction. 
All calls related to create, set, delete, and merge operations made between this 
API call and a trailing AREndBulkEntryTransaction call will not be sent to the 
server until the trailing call is made. 

Privileges 
	All users.
*/
#include "ar_class.h"

static VALUE myClass;


static void ar_bulk_mark(ARBulkClass *ar_bulk)
{
	//don't need to do anything.  
}

static void ar_bulk_free(ARBulkClass *ar_bulk)
{
	if(ar_bulk)
	{
		FreeARStatusList(ar_bulk->status, TRUE); //ensure we free any status records.
		free(ar_bulk);
	}
}

static VALUE ar_bulk_alloc(VALUE klass)
{
	ARBulkClass *ar_bulk = calloc(1, sizeof(ARBulkClass));

	ar_bulk->status = calloc(1, sizeof(ARStatusList));
	
  return Data_Wrap_Struct(klass, ar_bulk_mark, ar_bulk_free, ar_bulk); //wraps a c struct in VALUE
  //recover with Data_Get_Struct()
}

//Class level method, alternate to new.
//Ruby
//	art = ArTransaction.new(connection) do |art|
//    #do stuff
//  art.end
//See also ArTransaction.start
static VALUE ar_bulk_initialize(int argc, VALUE *argv, VALUE obj)
{
	VALUE ar_control;
	ARClass *ar;
	ARBulkClass *ar_bulk;

	//All arguments are optional as we could load them later.
	rb_scan_args(argc, argv, "1", &ar_control);

  Data_Get_Struct(obj, ARBulkClass, ar_bulk);

	//Check that we were passed an ARClass object.
  if(ar_control == Qnil || ar_type_check(ar_control) == 0)
 		rb_raise(rb_eTypeError, "ar_control should be of type ARClass"); 

 	Data_Get_Struct(ar_control, ARClass, ar);
	//Validate that we are connected.
  if(ar->connected == 0)
		rb_raise(rb_eRuntimeError, "Not connected?");
	
	//Start the transaction.
	if (ARBeginBulkEntryTransaction(ar->control, ar_bulk->status) >= AR_RETURN_ERROR)
		rb_raise(ar_status_list_new(ar_bulk->status), "Begin Bulk Entry Transaction Failed");
	
	ar_bulk->parent_connection = ar; //remember our parent connection
	ar_bulk->start = 1;
	
	return obj;
}

static VALUE ar_bulk_end(VALUE obj, VALUE action)
{
	ARBulkClass *ar_bulk;
	unsigned int actionType;
	ARBulkEntryReturnList bulkEntryReturnList;

  Data_Get_Struct(obj, ARBulkClass, ar_bulk);
	if(rb_intern("send") == SYM2ID(action))
		actionType = AR_BULK_ENTRY_ACTION_SEND;
	else
		actionType = AR_BULK_ENTRY_ACTION_CANCEL;
		
  if(ar_bulk->parent_connection->connected == 0)
		rb_raise(rb_eRuntimeError, "Not connected?");
		
	if(ar_bulk->start != 0) //would only be 0 if we called end twice!
	{
		if (AREndBulkEntryTransaction(ar_bulk->parent_connection->control, actionType, &bulkEntryReturnList, ar_bulk->status) >= AR_RETURN_ERROR)
			rb_raise(ar_status_list_new(ar_bulk->status), "End Bulk Entry Transaction Failed"); 
		ar_bulk->start = 0;
	}
	return obj; //!!! will want to return the bulkEntryReturnList here.
}

//Class level method, alternate to new.
//Ruby
//	ArTransaction.start(connection) do |ar|
//    #do stuff
//  end
VALUE ar_bulk_start(int argc, VALUE *argv, VALUE obj)
{
VALUE nobj = rb_class_new_instance( argc, argv, obj); //new instance of this class.
VALUE bulkEntryReturnList

	if(rb_block_given_p()) //if we have a block.
	{	
		rb_yield(nobj);
		//!!!!!!Need to catch errors and cancel the transaction.
		bulkEntryReturnList = ar_bulk_end(nobj,  ID2SYM(rb_intern("send")));
	}
	return bulkEntryReturnList; //if we didn't have a block, then Ar.open === Ar.new
}

////////////////  attr_readers /////////////////////
static VALUE get_status(VALUE obj)
{
	ARBulkClass *ar_bulk;

  Data_Get_Struct(obj, ARBulkClass, ar_bulk);
	return ar_status_list_new(ar_bulk->status);
}

VALUE Init_bulk_entry(VALUE myModule)
{
  myClass = rb_define_class_under(myModule, "ArTransaction", rb_cObject);
  rb_define_alloc_func(myClass, ar_bulk_alloc); 

	//Ar.new
  rb_define_method(myClass, "initialize", ar_bulk_initialize, -1); //returns transaction object.
	//Class methods
  rb_define_module_function(myClass, "start", ar_bulk_start, 0); //yields transaction object

	//instance methods
  rb_define_method(myClass, "end", ar_bulk_end, 1);
  rb_define_method(myClass, "status", ar_get_status, 0);

	return myClass;
}
