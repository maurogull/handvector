

#include "events.h"


gboolean move_start (GtkWidget *event_box, GdkEventButton *event, gpointer data) {
    //g_print ("starting %f,%f\n", event->x, event->y);
    if(event->button==1) drawing=1;
    else drawing=0;

    stroke_start_x=(int)event->x;
    stroke_start_y=(int)event->y;
    stroke_points=0;

    if(drawing==1) gdk_window_set_cursor(mainwin->window, crosscursor);
    if(drawing==0) gdk_window_set_cursor(mainwin->window, dotcursor);

    mouse_move(NULL, event, NULL);

    return TRUE;
}


gboolean move_end (GtkWidget *event_box, GdkEventButton *event, gpointer data){

    gdk_window_set_cursor(mainwin->window, NULL);

    stroke_end_x=(int)event->x;
    stroke_end_y=(int)event->y;

    g_print("stroke started at %d@%d, finished at %d@%d, %d points\n",stroke_start_x,stroke_start_y,stroke_end_x,stroke_end_y,stroke_points);

    hvpoint inicial;
    inicial.x=stroke_start_x;
    inicial.y=stroke_start_y;


    hv1_start_analysis(inicial, drawing);

    hv2_start_analysis();

    hv3_start_analysis();

    refresh_drawing();


    return TRUE;
}



gboolean mouse_move (GtkWidget *event_box, GdkEventButton *event, gpointer data) {

    if(drawing==1) {
        //draw (1 point)
        gdk_draw_point(canvas,context_drawing, (int)event->x, (int)event->y);
        pixels[ (int)event->x ][ (int)event->y ]=1;
    }
    else {
        //erase (bigger)
        int i,j;
        for(i=-3; i<=3; i++) {
            for(j=-3; j<=3; j++) {
                pixels[ (int)event->x + j ][ (int)event->y + i ] = 0;
                gdk_draw_point(canvas,context_erasing, (int)event->x + j, (int)event->y + i);
            }
        }
    }

    stroke_points++;
    gtk_image_set_from_pixmap(mainimage,canvas,NULL);

    return TRUE;
}


gboolean expose_result_event (GtkWidget *canvas, GdkEventExpose *event, gpointer user_data) {
  draw_result();
  return TRUE;
}



void refresh_drawing() {

    gdk_draw_rectangle(canvas, context_erasing, TRUE, 0,0,MAXW,MAXH);

    int i,j;
    for(i=0; i<MAXW; i++) {
        for(j=0; j<MAXH; j++) {
            if(pixels[ i ][ j ]==1)
                gdk_draw_point(canvas,context_drawing, i,j);
        }
    }

    hv1_draw_points();
    hv2_draw_strokes();

    gtk_image_set_from_pixmap(mainimage,canvas,NULL);

    gtk_widget_queue_draw(event_box_result);
}

void draw_result() {

    cairo_t * cr = gdk_cairo_create(event_box_result->window);

    hv3_draw_nodes(cr);


}
