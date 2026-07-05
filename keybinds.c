#include "keybinds.h"

gboolean handle_keypress(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    (void)widget;
    VteTerminal *term = VTE_TERMINAL(user_data);
    guint modifiers = event->state & gtk_accelerator_get_default_mod_mask();

    if (modifiers == (GDK_CONTROL_MASK | GDK_SHIFT_MASK)) {
        if (event->keyval == GDK_KEY_C || event->keyval == GDK_KEY_c) {
            vte_terminal_copy_clipboard_format(term, VTE_FORMAT_TEXT);
            return TRUE;
        } else if (event->keyval == GDK_KEY_V || event->keyval == GDK_KEY_v) {
            vte_terminal_paste_clipboard(term);
            return TRUE;
        }
    }
    return FALSE;
}