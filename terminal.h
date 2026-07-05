#ifndef TERMINAL_H
#define TERMINAL_H

#include <gtk/gtk.h>
#include <vte/vte.h>
#include "config.h"

GtkWidget* create_terminal(NyxConfig* config);

#endif