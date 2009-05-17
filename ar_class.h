#ifndef AR_CLASS_H
#define AR_CLASS_H

#include <ruby.h>

#include "ar.h" 
#include "arextern.h" 
#include "arfree.h" 

//base requirement for all calls is the control struct 
//and the return status structure.

typedef struct ar_class
{
	ARControlStruct *control;
	ARStatusList *status;
	int connected;
} ARClass;

int ar_type_check(VALUE obj);

#include "ar_status_list.h"
#include "ar_bulk_entry.h"
#include "ar_valueStructClass.h"


#endif //AR_CLASS_H