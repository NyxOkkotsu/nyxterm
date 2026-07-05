#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>
#include "config.h"

GtkWidget* create_window(NyxConfig* config, GtkWidget* terminal);

#endif