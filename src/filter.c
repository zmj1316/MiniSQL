#include "global.h"
void Filter_add(Filter *filter,Rule * r)
{
    List_append(&(filter->rules_list), r);
}