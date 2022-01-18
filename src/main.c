#include <stdlib.h>
#include <cairo.h>
#include <gtk/gtk.h>

#include "globals.h"

#include "fasttrigon.h"

#include "events.h"
#include "hv1.h"
#include "hv2.h"
#include "hv3.h"

int main (int argc, char *argv[])
{

    init_fast_trigon();

    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);


    crosscursor = gdk_cursor_new(GDK_CROSSHAIR);
    dotcursor = gdk_cursor_new(GDK_DOT);

    mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (mainwin), 3);
    gtk_window_set_title (GTK_WINDOW (mainwin), "HandVector");
    gtk_window_set_position (GTK_WINDOW (mainwin), GTK_WIN_POS_CENTER);
    gtk_widget_realize (mainwin);
    g_signal_connect (mainwin, "destroy", gtk_main_quit, NULL);

    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);

    GtkWidget *menubar ;
    GtkWidget *filemenu, *viewmenu, *toolmenu;
    GtkWidget *file, *newd, *quit,*sep;
    GtkWidget *tool, *tooldraw, *toolerase;
    GtkWidget *view, *viewnodes, *viewhangers, *viewfilled;

    menubar = gtk_menu_bar_new();

    filemenu = gtk_menu_new();
    file = gtk_menu_item_new_with_label("Archivo");
    newd = gtk_menu_item_new_with_label("Nuevo");
    sep = gtk_separator_menu_item_new();
    quit = gtk_menu_item_new_with_label("Salir");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), newd);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), sep);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);

    toolmenu = gtk_menu_new();
    tool = gtk_menu_item_new_with_label("Herramienta");
    tooldraw = gtk_menu_item_new_with_label("Lapiz");
    toolerase = gtk_menu_item_new_with_label("Borrador");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(tool), toolmenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(toolmenu), tooldraw);
    gtk_menu_shell_append(GTK_MENU_SHELL(toolmenu), toolerase);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), tool);

    viewmenu = gtk_menu_new();
    view = gtk_menu_item_new_with_label("Ver");
    viewnodes = gtk_check_menu_item_new_with_label("Ver nodos");
    viewhangers = gtk_check_menu_item_new_with_label("Ver controladores");
    viewfilled = gtk_check_menu_item_new_with_label("Rellenar");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(view), viewmenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), viewnodes);
    gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), viewhangers);
    gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), viewfilled);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), view);

    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 1);


    viewmenu = gtk_menu_new();
    toolmenu = gtk_menu_new();



    quit = gtk_menu_item_new_with_label("Quit");



    canvas = gdk_pixmap_new(NULL, MAXW,MAXH,24);
    resultcanvas = gdk_pixmap_new(NULL, MAXW,MAXH,24);

    setup_contexts();

    gdk_draw_rectangle(canvas, context_erasing, TRUE, 0,0,MAXW,MAXH);
    gdk_draw_rectangle(resultcanvas, context_erasing, TRUE, 0,0,MAXW,MAXH);


    mainimage = gtk_image_new_from_pixmap (canvas,NULL);
    resultimage = gtk_image_new_from_pixmap (resultcanvas,NULL);

    event_box = gtk_event_box_new ();
    gtk_container_add (GTK_CONTAINER (event_box), mainimage);

    event_box_result = gtk_event_box_new ();
    gtk_container_add (GTK_CONTAINER (event_box_result), resultimage);


    g_signal_connect (G_OBJECT (event_box),"button_press_event",G_CALLBACK (move_start),mainimage);
    g_signal_connect (G_OBJECT (event_box),"button_release_event",G_CALLBACK (move_end),mainimage);
    g_signal_connect (G_OBJECT (event_box),"motion_notify_event",G_CALLBACK (mouse_move),mainimage);

    g_signal_connect (G_OBJECT (resultimage),"expose-event",G_CALLBACK (expose_result_event),mainimage);


    gtk_container_add(GTK_CONTAINER(mainwin), vbox);

    GtkWidget *hbox = gtk_hbox_new(TRUE, 10);
    gtk_box_pack_start(GTK_BOX(hbox), event_box, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), event_box_result, TRUE, TRUE, 0);
    gtk_container_add (GTK_CONTAINER (vbox), hbox);


    gtk_widget_realize(mainimage);


    pointlist = NULL;
    strokelist = NULL;

    gtk_widget_show_all (mainwin);
    gtk_main ();
    return 0;
}


void setup_contexts() {

    context_drawing = gdk_gc_new(canvas);
    context_erasing = gdk_gc_new(canvas);
    context_points = gdk_gc_new(canvas);
    context_path[0] = gdk_gc_new(canvas);
    context_path[1] = gdk_gc_new(canvas);
    context_path[2] = gdk_gc_new(canvas);
    context_path[3] = gdk_gc_new(canvas);
    context_path[4] = gdk_gc_new(canvas);

    GdkColor auxcolorD, auxcolorE, auxcolorP, auxcolorP1, auxcolorP2, auxcolorP3, auxcolorP4, auxcolorP5;
    auxcolorD.pixel = 0x55000000;
    auxcolorE.pixel = 0xffffffff;
    auxcolorP.pixel = 0xff555555;
    auxcolorP1.pixel = 0xffff0000;
    auxcolorP2.pixel = 0xff0066ff;
    auxcolorP3.pixel = 0xffcc00ff;
    auxcolorP4.pixel = 0xff00ff66;
    auxcolorP5.pixel = 0xffccff00;

    gdk_gc_set_foreground(context_drawing, &auxcolorD);
    gdk_gc_set_foreground(context_erasing, &auxcolorE);
    gdk_gc_set_foreground(context_points, &auxcolorP);

    gdk_gc_set_foreground(context_path[0], &auxcolorP1);
    gdk_gc_set_foreground(context_path[1], &auxcolorP2);
    gdk_gc_set_foreground(context_path[2], &auxcolorP3);
    gdk_gc_set_foreground(context_path[3], &auxcolorP4);
    gdk_gc_set_foreground(context_path[4], &auxcolorP5);
}


static void helloWorld (GtkWidget *wid, GtkWidget *win)
{
  GtkWidget *dialog = NULL;

  dialog = gtk_message_dialog_new (GTK_WINDOW (win), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "Hello World!");
  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}
