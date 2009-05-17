#ifndef AR_BULK_CLASS_H
#define AR_BULK_CLASS_H

//base requirement for all calls is the control struct 
//and the return status structure.

typedef struct ar_bulk_class
{
	ARClass *parent_connection;
	ARStatusList *status;
	int start;
} ARBulkClass;

VALUE ar_bulk_start(int argc, VALUE *argv, VALUE obj);
VALUE Init_bulk_entry(VALUE myModule);

#endif //AR_BULK_CLASS_H