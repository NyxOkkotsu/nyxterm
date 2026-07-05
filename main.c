#include <gtk/gtk.h>
#include "config.h"
#include "terminal.h"
#include "window.h"
#include "keybinds.h"

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    NyxConfig *config = load_config();

    GtkWidget *terminal = create_terminal(config);
    GtkWidget *window = create_window(config, terminal);

    g_signal_connect(window, "key-press-event", G_CALLBACK(handle_keypress), terminal);

    gtk_widget_show_all(window);
    gtk_widget_grab_focus(terminal);

    gtk_main();

    free_config(config);
    return 0;
}