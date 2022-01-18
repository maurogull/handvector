
#ifndef EVENTS_H
#define EVENTS_H

#include "globals.h"
#include "hv1.h"
#include "hv2.h"
#include "hv3.h"
#include <gtk/gtk.h>

gboolean move_start (GtkWidget *, GdkEventButton *, gpointer );

gboolean move_end (GtkWidget *, GdkEventButton *, gpointer );

gboolean mouse_move (GtkWidget *, GdkEventButton *, gpointer );

gboolean expose_result_event (GtkWidget *, GdkEventExpose *, gpointer );

void refresh_drawing();

#endif
