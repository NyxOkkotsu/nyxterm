#include <gtk/gtk.h>
#include <vte/vte.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

typedef struct {
    char* font_family;
    int font_size;
    double opacity;
    char* foreground;
    char* background;
    char* cursor_shape;
    int cursor_blink;
    int scrollback_lines;
    int padding;
    char* shell;
    int window_decorations;
    double cell_width;
    double cell_height;
    int use_pywal;
    GdkRGBA palette[16];
    int has_palette;
} NyxConfig;

static char* trim(char* str) {
    char* end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

static void ensure_dir_exists(const char* path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }
}

static void set_defaults(NyxConfig* config) {
    config->font_family = strdup("Monospace");
    config->font_size = 12;
    config->opacity = 1.0;
    config->foreground = strdup("#FFFFFF");
    config->background = strdup("#000000");
    config->cursor_shape = strdup("block");
    config->cursor_blink = 0;
    config->scrollback_lines = 10000;
    config->padding = 2;
    config->window_decorations = 1;
    config->cell_width = 1.0;
    config->cell_height = 1.0;
    config->use_pywal = 0;
    config->has_palette = 0;

    const char *env_shell = getenv("SHELL");
    config->shell = strdup(env_shell ? env_shell : "/bin/bash");
}

static void apply_pywal(NyxConfig* config) {
    const char* home = getenv("HOME");
    if (!home) return;

    char path[1024];
    snprintf(path, sizeof(path), "%s/.cache/wal/colors.sh", home);

    FILE* f = fopen(path, "r");
    if (!f) return;

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, f)) != -1) {
        char hex[16];
        int col_idx;

        if (sscanf(line, "background='%15[^']'", hex) == 1) {
            free(config->background);
            config->background = strdup(hex);
        } else if (sscanf(line, "foreground='%15[^']'", hex) == 1) {
            free(config->foreground);
            config->foreground = strdup(hex);
        } else if (sscanf(line, "color%d='%15[^']'", &col_idx, hex) == 2) {
            if (col_idx >= 0 && col_idx < 16) {
                gdk_rgba_parse(&config->palette[col_idx], hex);
                config->has_palette = 1;
            }
        }
    }
    free(line);
    fclose(f);
}

static void generate_default_config(const char* filepath, NyxConfig* config) {
    FILE* f = fopen(filepath, "w");
    if (!f) return;
    fprintf(f, "font_family Monospace\n");
    fprintf(f, "font_size 12\n");
    fprintf(f, "cell_width 1.0\n");
    fprintf(f, "cell_height 1.0\n");
    fprintf(f, "use_pywal 0\n");
    fprintf(f, "opacity 0.95\n");
    fprintf(f, "foreground #eceff4\n");
    fprintf(f, "background #2e3440\n");
    fprintf(f, "cursor_shape block\n");
    fprintf(f, "cursor_blink 0\n");
    fprintf(f, "scrollback_lines 10000\n");
    fprintf(f, "padding 8\n");
    fprintf(f, "shell %s\n", config->shell);
    fprintf(f, "window_decorations 1\n");
    fclose(f);
}

static NyxConfig* load_config() {
    NyxConfig* config = malloc(sizeof(NyxConfig));
    set_defaults(config);

    const char* home = getenv("HOME");
    if (!home) return config;

    char dirpath[512];
    snprintf(dirpath, sizeof(dirpath), "%s/.config/nyxterm", home);
    ensure_dir_exists(dirpath);

    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/nyxterm.conf", dirpath);

    FILE* file = fopen(filepath, "r");
    if (!file) {
        generate_default_config(filepath, config);
        file = fopen(filepath, "r");
        if (!file) return config;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {
        char* trimmed = trim(line);
        if (trimmed[0] == '#' || trimmed[0] == '\0') continue;

        char* space = strchr(trimmed, ' ');
        if (!space) space = strchr(trimmed, '\t');
        if (!space) continue;

        *space = '\0';
        char* key = trim(trimmed);
        char* val = trim(space + 1);

        if (strcmp(key, "font_family") == 0) { free(config->font_family); config->font_family = strdup(val); }
        else if (strcmp(key, "font_size") == 0) config->font_size = atoi(val);
        else if (strcmp(key, "cell_width") == 0) config->cell_width = atof(val);
        else if (strcmp(key, "cell_height") == 0) config->cell_height = atof(val);
        else if (strcmp(key, "use_pywal") == 0) config->use_pywal = atoi(val);
        else if (strcmp(key, "opacity") == 0) config->opacity = atof(val);
        else if (strcmp(key, "foreground") == 0) { free(config->foreground); config->foreground = strdup(val); }
        else if (strcmp(key, "background") == 0) { free(config->background); config->background = strdup(val); }
        else if (strcmp(key, "cursor_shape") == 0) { free(config->cursor_shape); config->cursor_shape = strdup(val); }
        else if (strcmp(key, "cursor_blink") == 0) config->cursor_blink = atoi(val);
        else if (strcmp(key, "scrollback_lines") == 0) config->scrollback_lines = atoi(val);
        else if (strcmp(key, "padding") == 0) config->padding = atoi(val);
        else if (strcmp(key, "shell") == 0) { free(config->shell); config->shell = strdup(val); }
        else if (strcmp(key, "window_decorations") == 0) config->window_decorations = atoi(val);
    }

    free(line);
    fclose(file);

    if (config->use_pywal) {
        apply_pywal(config);
    }

    return config;
}

static void free_config(NyxConfig* config) {
    if (!config) return;
    free(config->font_family);
    free(config->foreground);
    free(config->background);
    free(config->cursor_shape);
    free(config->shell);
    free(config);
}

static gboolean handle_keypress(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
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

static void on_child_exited(VteTerminal *terminal, gint status, gpointer user_data) {
    (void)terminal;
    (void)status;
    (void)user_data;
    gtk_main_quit();
}

static GtkWidget* create_terminal(NyxConfig* config) {
    GtkWidget *term = vte_terminal_new();
    
    char font_desc_str[256];
    snprintf(font_desc_str, sizeof(font_desc_str), "%s %d", config->font_family, config->font_size);
    PangoFontDescription *font_desc = pango_font_description_from_string(font_desc_str);
    vte_terminal_set_font(VTE_TERMINAL(term), font_desc);
    pango_font_description_free(font_desc);

    vte_terminal_set_cell_width_scale(VTE_TERMINAL(term), config->cell_width);
    vte_terminal_set_cell_height_scale(VTE_TERMINAL(term), config->cell_height);
    vte_terminal_set_scrollback_lines(VTE_TERMINAL(term), config->scrollback_lines);

    GdkRGBA bg, fg;
    gdk_rgba_parse(&bg, config->background);
    gdk_rgba_parse(&fg, config->foreground);
    bg.alpha = config->opacity;

    if (config->has_palette) {
        vte_terminal_set_colors(VTE_TERMINAL(term), &fg, &bg, config->palette, 16);
    } else {
        vte_terminal_set_colors(VTE_TERMINAL(term), &fg, &bg, NULL, 0);
    }

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

    char **env = g_get_environ();
    env = g_environ_setenv(env, "TERM", "xterm-256color", TRUE);
    env = g_environ_setenv(env, "TERM_PROGRAM", "NyxTerm", TRUE);
    env = g_environ_setenv(env, "COLORTERM", "truecolor", TRUE);

    char *shell_argv[] = {config->shell, NULL};
    vte_terminal_spawn_async(
        VTE_TERMINAL(term),
        VTE_PTY_DEFAULT,
        NULL,
        shell_argv,
        env,
        G_SPAWN_DEFAULT,
        NULL, NULL, NULL,
        -1, NULL, NULL, NULL
    );
    g_strfreev(env);

    return term;
}

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
    char *title = NULL;
    
    g_object_get(terminal, "window-title", &title, NULL);

    if (title) {
        char full_title[512];
        snprintf(full_title, sizeof(full_title), "%s - NyxTerm", title);
        gtk_window_set_title(window, full_title);
        g_free(title);
    } else {
        gtk_window_set_title(window, "NyxTerm");
    }
}

static GtkWidget* create_window(NyxConfig* config, GtkWidget* terminal) {
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