#include "config.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    config->shell = strdup("/bin/bash");
    config->window_decorations = 1;
}

static void generate_default_config(const char* filepath) {
    FILE* f = fopen(filepath, "w");
    if (!f) return;
    fprintf(f, "# NyxTerm Configuration\n\n");
    fprintf(f, "font_family Monospace\n");
    fprintf(f, "font_size 12\n");
    fprintf(f, "opacity 0.95\n");
    fprintf(f, "foreground #eceff4\n");
    fprintf(f, "background #2e3440\n");
    fprintf(f, "cursor_shape block\n");
    fprintf(f, "cursor_blink 0\n");
    fprintf(f, "scrollback_lines 10000\n");
    fprintf(f, "padding 8\n");
    fprintf(f, "shell /bin/bash\n");
    fprintf(f, "window_decorations 1\n");
    fclose(f);
}

NyxConfig* load_config() {
    NyxConfig* config = malloc(sizeof(NyxConfig));
    set_defaults(config);

    const char* home = getenv("HOME");
    if (!home) return config;

    char dirpath[512];
    snprintf(dirpath, sizeof(dirpath), "%s/.config/nyxterm", home);
    ensure_dir_exists(dirpath);

    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/nyxterm.conf", dirpath);

    FILE* file = fopen(filepath, "r");
    if (!file) {
        generate_default_config(filepath);
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
    return config;
}

void free_config(NyxConfig* config) {
    if (!config) return;
    free(config->font_family);
    free(config->foreground);
    free(config->background);
    free(config->cursor_shape);
    free(config->shell);
    free(config);
}