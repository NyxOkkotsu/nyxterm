#ifndef KEYBINDS_H
#define KEYBINDS_H

#include <gtk/gtk.h>
#include <vte/vte.h>

gboolean handle_keypress(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

#endif