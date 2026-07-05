# NyxTerm

A minimalist, lightweight, and fast terminal emulator built using C, GTK3, and VTE. NyxTerm is designed specifically for high performance and low memory consumption, keeping things simple without unnecessary bloatware. 

It features native background transparency, adjustable character spacing, and out-of-the-box Pywal integration to keep your color themes synced.

## Dependencies

Before compiling, make sure your system has a C compiler, Make, and the development headers for GTK3 and VTE.

---

Debian / Ubuntu / Pop!_OS:

```bash
sudo apt install build-essential libgtk-3-dev libvte-2.91-dev
```

Arch Linux / Manjaro:

```bash
sudo pacman -S base-devel gtk3 vte3

```

Fedora:

```bash
sudo dnf install gcc make gtk3-devel vte291-devel

```

## Build and Installation

Use the provided Makefile to compile and install the application.

1. Open your terminal in the project directory.
2. Compile the source code:
```bash
make

```


3. Install the binary to `/usr/local/bin` so it can be launched from anywhere:
```bash
sudo make install

```



To remove the compiled binaries and object files, run:

```bash
make clean

```

## Configuration

On its very first run, NyxTerm will automatically generate a default configuration file at:

```text
~/.config/nyxterm/nyxterm.conf

```

You can modify this file using any text editor to customize your setup.

### Sample `nyxterm.conf`

```text
font_family JetBrainsMono Nerd Font
font_size 11
cell_width 0.95
cell_height 1.0
use_pywal 1
opacity 0.85
padding 12
shell /bin/nyxsh
window_decorations 1
cursor_shape block
cursor_blink 0
scrollback_lines 10000

```

### Key Parameters Explained

* **cell_width / cell_height**: Controls the tightness of the font characters. The default value is `1.0`. If your preferred font looks a bit too spaced out, you can lower this value (e.g., `0.95`).
* **use_pywal**: Set this to `1` to enable Pywal integration. NyxTerm will read your active theme colors directly from `~/.cache/wal/colors.sh` every time it launches.
* **opacity**: Adjusts the background transparency. Accepts values from `0.0` (fully transparent) to `1.0` (fully solid).
* **shell**: The absolute path to your preferred shell. If left blank or invalid, NyxTerm will fall back to your system's `$SHELL` environment variable or `/bin/bash`.

## Default Keyboard Shortcuts

* **Ctrl + Shift + C**: Copy selected text to the clipboard.
* **Ctrl + Shift + V**: Paste text from the clipboard.
