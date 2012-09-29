#ifndef AR_STATUS_LIST_H
#define AR_STATUS_LIST_H

#include <ruby.h>

#include "ar.h" 
#include "arextern.h" 
#include "arfree.h" 

typedef struct ar_statuslist
{
	ARStatusList *status_list
} ARStatusListClass;

VALUE ar_status_list_new(ARStatusList *status);
void Init_status_list(VALUE myModule, VALUE parent_class);

#endif //AR_STATUS_LIST_H