
#ifndef GLOBALS_H
#define GLOBALS_H

#include <gtk/gtk.h>

#define MAXW 500
#define MAXH 500


#define PI 3.14159265

#define DISCOVER_RADIUS 12

#include "fasttrigon.h"


int drawing;

GtkWidget *event_box, *event_box_result;

GtkWidget *mainwin;
GtkWidget *mainimage;
GtkWidget *resultimage;

GdkCursor* dotcursor;
GdkCursor* crosscursor;

GdkPixmap* canvas;
GdkPixmap* resultcanvas;
GdkGC* context_drawing;
GdkGC* context_erasing;
GdkGC* context_points;
GdkGC* context_path[5];

unsigned int stroke_start_x;
unsigned int stroke_start_y;
unsigned int stroke_end_x;
unsigned int stroke_end_y;
unsigned int stroke_points;

unsigned char pixels[MAXW][MAXH];

typedef struct hvpoint {
    int x;
    int y;
    struct hvpoint* next; //next in hv1 point list

    struct hvpoint* next_s; //next in hv2 stroke
    struct hvstroke * stroke;

    int outhanger_x;
    int outhanger_y;
    int inhanger_x;
    int inhanger_y;

    int inangle;
    int outangle;

    char isnode;
    char isasymmetric;
} hvpoint;

hvpoint * pointlist ;
unsigned int pointcount;


typedef struct hvstroke {
    struct hvpoint* points;
    struct hvstroke* next;
    char isclosed;
} hvstroke;

hvstroke * strokelist;
unsigned int strokecount;


#endif
