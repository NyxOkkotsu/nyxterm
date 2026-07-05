CC = gcc
CFLAGS = -O2 -Wall -Wextra $(shell pkg-config --cflags gtk+-3.0 vte-2.91)
LDFLAGS = $(shell pkg-config --libs gtk+-3.0 vte-2.91)

SRC = nyxterm.c
EXEC = nyxterm
ICON = nyxterm.png
DESKTOP_FILE = nyxterm.desktop

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC) $(LDFLAGS)

clean:
	rm -f $(EXEC) *.o $(DESKTOP_FILE)

# Auto-generate file shortcut desktop
$(DESKTOP_FILE):
	@echo "[Desktop Entry]" > $(DESKTOP_FILE)
	@echo "Version=1.0" >> $(DESKTOP_FILE)
	@echo "Name=NyxTerm" >> $(DESKTOP_FILE)
	@echo "Comment=Aesthetic & Fast Linux Terminal" >> $(DESKTOP_FILE)
	@echo "Exec=/usr/local/bin/nyxterm" >> $(DESKTOP_FILE)
	@echo "Icon=/usr/share/pixmaps/nyxterm.png" >> $(DESKTOP_FILE)
	@echo "Terminal=false" >> $(DESKTOP_FILE)
	@echo "Type=Application" >> $(DESKTOP_FILE)
	@echo "Categories=System;TerminalEmulator;Utility;" >> $(DESKTOP_FILE)

install: $(EXEC) $(DESKTOP_FILE)
	install -m 755 $(EXEC) /usr/local/bin/
	@if [ -f $(ICON) ]; then \
		install -m 644 $(ICON) /usr/share/pixmaps/; \
		echo "Icon installed successfully!"; \
	else \
		echo "Warning: nyxterm.png not found in current folder! Icon skipped."; \
	fi
	install -m 644 $(DESKTOP_FILE) /usr/share/applications/
	update-desktop-database /usr/share/applications/ || true

uninstall:
	rm -f /usr/local/bin/$(EXEC)
	rm -f /usr/share/pixmaps/$(ICON)
	rm -f /usr/share/applications/$(DESKTOP_FILE)

.PHONY: all clean install uninstall