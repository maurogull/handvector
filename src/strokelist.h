#ifndef HVSTROKELIST_H
#define HVSTROKELIST_H


#include "globals.h"

#define ENDING 1
#define BEGINNING 2


void hv2_clear_strokes();

hvstroke* hv2_new_stroke();

void hv2_add_stroke(hvstroke *);

void hv2_add_point_to_stroke(hvpoint* ,hvstroke*, char order );

int hv2_strokefree_point_count();
hvpoint* hv2_first_strokefree_point() ;

int hv2_stroke_point_count(hvstroke* auxs);

void hv2_delete_stroke(hvstroke* s);

#endif

