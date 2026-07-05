#ifndef CONFIG_H
#define CONFIG_H

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
} NyxConfig;

NyxConfig* load_config();
void free_config(NyxConfig* config);

#endif