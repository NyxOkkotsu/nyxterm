#include "window.h"
#include <vte/vte.h>  

static gboolean on_window_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    (void)user_data;
    if (gtk_widget_get_app_paintable(widget)) {
        cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.0);
        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
        cairo_paint(cr);
    }
    return FALSE;
}

static void on_title_changed(GtkWidget *terminal, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(user_data);
    const char *title = vte_terminal_get_window_title(VTE_TERMINAL(terminal));
    if (title) {
        char full_title[512];
        snprintf(full_title, sizeof(full_title), "%s - NyxTerm", title);
        gtk_window_set_title(window, full_title);
    } else {
        gtk_window_set_title(window, "NyxTerm");
    }
}

GtkWidget* create_window(NyxConfig* config, GtkWidget* terminal) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "NyxTerm");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_decorated(GTK_WINDOW(window), config->window_decorations);

    GdkScreen *screen = gtk_widget_get_screen(window);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    if (visual && config->opacity < 1.0) {
        gtk_widget_set_visual(window, visual);
        gtk_widget_set_app_paintable(window, TRUE);
        g_signal_connect(window, "draw", G_CALLBACK(on_window_draw), NULL);
    }

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(terminal, "window-title-changed", G_CALLBACK(on_title_changed), window);

    gtk_container_add(GTK_CONTAINER(window), terminal);
    return window;
}