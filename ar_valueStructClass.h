#ifndef ARVALUESTRUCTCLASS_H
#define ARVALUESTRUCTCLASS_H

typedef struct ar_status_list
{
	ARValueStruct value;
} ARValueStructClass;

VALUE ar_valueStruct_new(ARValueStruct *avrs);
VALUE ar_valueStructList_new(ARValueList *arvl);
VALUE ar_valueStructListList_new(ARValueListList *arvll);

VALUE Init_value(VALUE myModule);

#endif //ARVALUESTRUCTCLASS_H