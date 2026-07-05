#include "terminal.h"

static void on_child_exited(VteTerminal *terminal, gint status, gpointer user_data) {
    (void)terminal;
    (void)status;
    (void)user_data;
    gtk_main_quit();
}

GtkWidget* create_terminal(NyxConfig* config) {
    GtkWidget *term = vte_terminal_new();
    
    char font_desc_str[256];
    snprintf(font_desc_str, sizeof(font_desc_str), "%s %d", config->font_family, config->font_size);
    PangoFontDescription *font_desc = pango_font_description_from_string(font_desc_str);
    vte_terminal_set_font(VTE_TERMINAL(term), font_desc);
    pango_font_description_free(font_desc);

    vte_terminal_set_scrollback_lines(VTE_TERMINAL(term), config->scrollback_lines);

    GdkRGBA bg, fg;
    gdk_rgba_parse(&bg, config->background);
    gdk_rgba_parse(&fg, config->foreground);
    bg.alpha = config->opacity;

    vte_terminal_set_colors(VTE_TERMINAL(term), &fg, &bg, NULL, 0);

    VteCursorShape shape = VTE_CURSOR_SHAPE_BLOCK;
    if (strcmp(config->cursor_shape, "underline") == 0) shape = VTE_CURSOR_SHAPE_UNDERLINE;
    else if (strcmp(config->cursor_shape, "ibeam") == 0) shape = VTE_CURSOR_SHAPE_IBEAM;
    vte_terminal_set_cursor_shape(VTE_TERMINAL(term), shape);

    VteCursorBlinkMode blink = config->cursor_blink ? VTE_CURSOR_BLINK_ON : VTE_CURSOR_BLINK_OFF;
    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(term), blink);

    gtk_widget_set_margin_start(term, config->padding);
    gtk_widget_set_margin_end(term, config->padding);
    gtk_widget_set_margin_top(term, config->padding);
    gtk_widget_set_margin_bottom(term, config->padding);

    g_signal_connect(term, "child-exited", G_CALLBACK(on_child_exited), NULL);

    char *shell_argv[] = {config->shell, NULL};
    vte_terminal_spawn_async(
        VTE_TERMINAL(term),
        VTE_PTY_DEFAULT,
        NULL,
        shell_argv,
        NULL,
        G_SPAWN_DEFAULT,
        NULL, NULL, NULL,
        -1, NULL, NULL, NULL
    );

    return term;
}