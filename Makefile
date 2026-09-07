# ==============================================================================
# Makefile for Still Yet Another Sokoban (C++20 + SDL3 / Wayland)
# ==============================================================================

CXX        ?= g++
BIN        := syasokoban
PREFIX     ?= /usr/local
BINDIR     ?= $(PREFIX)/bin
DATADIR    ?= $(PREFIX)/share/$(BIN)
DESKTOPDIR ?= $(PREFIX)/share/applications
ICONDIR    ?= $(PREFIX)/share/pixmaps

# ANSI Terminal Color Escapes (octal format for printf %b)
COLOR_RESET := \033[0m
COLOR_BOLD  := \033[1m
COLOR_INFO  := \033[1;36m
COLOR_OK    := \033[1;32m
COLOR_WARN  := \033[1;33m
COLOR_ERR   := \033[1;31m
COLOR_STEP  := \033[1;34m

# ------------------------------------------------------------------------------
# System Dependency Checks (SDL3 and C++20 Compiler)
# ------------------------------------------------------------------------------
PKG_CONFIG := $(shell which pkg-config 2>/dev/null)
ifeq ($(PKG_CONFIG),)
  $(error [ERROR] 'pkg-config' not found! Please install it with 'sudo pacman -S pkgconf')
endif

SDL3_CHECK := $(shell $(PKG_CONFIG) --exists sdl3 && echo ok)
ifneq ($(SDL3_CHECK),ok)
  $(error [ERROR] SDL 3 development libraries not found! Install on Manjaro/Arch with: 'sudo pacman -S sdl3')
endif

SDL_CFLAGS := $(shell $(PKG_CONFIG) --cflags sdl3)
SDL_LIBS   := $(shell $(PKG_CONFIG) --libs sdl3)

CXX_C20_CHECK := $(shell $(CXX) -std=c++20 -dM -E - < /dev/null 2>/dev/null)
ifeq ($(CXX_C20_CHECK),)
  $(error [ERROR] Compiler '$(CXX)' does not support C++20 (-std=c++20)!)
endif

# Compilation & Linker Flags
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -O2 $(SDL_CFLAGS)
LIBS     := $(SDL_LIBS) -lm

SRC := $(shell find src -iname '*.cpp' -type f | sort)
OBJ := $(SRC:.cpp=.o)

# ------------------------------------------------------------------------------
# Targets
# ------------------------------------------------------------------------------
.PHONY: all check-deps clean install uninstall help

all: check-deps $(BIN)
	@printf "%b%b>>> Build completed successfully! Binary: ./%s <<<%b\n" "$(COLOR_OK)" "$(COLOR_BOLD)" "$(BIN)" "$(COLOR_RESET)"

check-deps:
	@printf "%b==> Checking system build dependencies...%b\n" "$(COLOR_INFO)" "$(COLOR_RESET)"
	@printf "    • Compiler:     %s (%s)\n" "$(CXX)" "$$( $(CXX) --version | head -n1 )"
	@printf "    • C++20 standard: %bAvailable%b\n" "$(COLOR_OK)" "$(COLOR_RESET)"
	@printf "    • SDL 3 library:  %bAvailable%b (%s)\n" "$(COLOR_OK)" "$(COLOR_RESET)" "$$( $(PKG_CONFIG) --modversion sdl3 )"

$(BIN): $(OBJ)
	@printf "%b[LINK]%b Creating binary executable: %b%s%b\n" "$(COLOR_STEP)" "$(COLOR_RESET)" "$(COLOR_BOLD)" "$@" "$(COLOR_RESET)"
	@$(CXX) -o $@ $(OBJ) $(LIBS)

%.o: %.cpp
	@printf "%b[CXX]%b Compiling: %s\n" "$(COLOR_STEP)" "$(COLOR_RESET)" "$<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# ------------------------------------------------------------------------------
# Installation & Uninstallation
# ------------------------------------------------------------------------------
install: all
	@printf "%b==> Installing %s to %s...%b\n" "$(COLOR_INFO)" "$(BIN)" "$(PREFIX)" "$(COLOR_RESET)"

	@printf "    • Creating installation directories (chmod 755)...\n"
	@install -d -m 755 "$(DESTDIR)$(DATADIR)"
	@install -d -m 755 "$(DESTDIR)$(BINDIR)"
	@install -d -m 755 "$(DESTDIR)$(DESKTOPDIR)"
	@install -d -m 755 "$(DESTDIR)$(ICONDIR)"

	@printf "    • Installing game assets to %s (chmod 644/755)...\n" "$(DESTDIR)$(DATADIR)/data"
	@cp -r data "$(DESTDIR)$(DATADIR)/"
	@find "$(DESTDIR)$(DATADIR)/data" -type d -exec chmod 755 {} +
	@find "$(DESTDIR)$(DATADIR)/data" -type f -exec chmod 644 {} +

	@printf "    • Installing binary executable to %s (chmod 755)...\n" "$(DESTDIR)$(DATADIR)/$(BIN).bin"
	@install -m 755 $(BIN) "$(DESTDIR)$(DATADIR)/$(BIN).bin"

	@printf "    • Creating launcher wrapper script in %s (chmod 755)...\n" "$(DESTDIR)$(BINDIR)/$(BIN)"
	@printf '#!/bin/sh\ncd "%s" && exec "./%s.bin" "$$@"\n' "$(DATADIR)" "$(BIN)" > "$(DESTDIR)$(BINDIR)/$(BIN)"
	@chmod 755 "$(DESTDIR)$(BINDIR)/$(BIN)"

	@if [ -f "datasrc/sokoban/icon/sokoban.png" ]; then \
		printf "    • Installing desktop icon to %s (chmod 644)...\n" "$(DESTDIR)$(ICONDIR)/$(BIN).png"; \
		install -m 644 datasrc/sokoban/icon/sokoban.png "$(DESTDIR)$(ICONDIR)/$(BIN).png"; \
	fi

	@printf "    • Generating desktop application entry in %s (chmod 644)...\n" "$(DESTDIR)$(DESKTOPDIR)/$(BIN).desktop"
	@printf "[Desktop Entry]\nType=Application\nName=Still Yet Another Sokoban\nComment=Sokoban puzzle game\nExec=%s\nIcon=%s\nTerminal=false\nCategories=Game;LogicGame;\n" \
		"$(BINDIR)/$(BIN)" "$(BIN)" > "$(DESTDIR)$(DESKTOPDIR)/$(BIN).desktop"
	@chmod 644 "$(DESTDIR)$(DESKTOPDIR)/$(BIN).desktop"

	@printf "%b[OK] %s installed successfully! You can run it by typing '%s'.%b\n" "$(COLOR_OK)" "$(BIN)" "$(BIN)" "$(COLOR_RESET)"

uninstall:
	@printf "%b==> Uninstalling %s from %s...%b\n" "$(COLOR_WARN)" "$(BIN)" "$(PREFIX)" "$(COLOR_RESET)"
	@if [ -f "$(DESTDIR)$(BINDIR)/$(BIN)" ]; then \
		rm -f "$(DESTDIR)$(BINDIR)/$(BIN)"; \
		printf "    • Removed launcher:  %s\n" "$(DESTDIR)$(BINDIR)/$(BIN)"; \
	fi
	@if [ -f "$(DESTDIR)$(DESKTOPDIR)/$(BIN).desktop" ]; then \
		rm -f "$(DESTDIR)$(DESKTOPDIR)/$(BIN).desktop"; \
		printf "    • Removed shortcut:  %s\n" "$(DESTDIR)$(DESKTOPDIR)/$(BIN).desktop"; \
	fi
	@if [ -f "$(DESTDIR)$(ICONDIR)/$(BIN).png" ]; then \
		rm -f "$(DESTDIR)$(ICONDIR)/$(BIN).png"; \
		printf "    • Removed icon:      %s\n" "$(DESTDIR)$(ICONDIR)/$(BIN).png"; \
	fi
	@if [ -d "$(DESTDIR)$(DATADIR)" ]; then \
		rm -rf "$(DESTDIR)$(DATADIR)"; \
		printf "    • Removed directory: %s\n" "$(DESTDIR)$(DATADIR)"; \
	fi
	@printf "%b[OK] %s has been completely removed from the system.%b\n" "$(COLOR_OK)" "$(BIN)" "$(COLOR_RESET)"

clean:
	@printf "%b==> Cleaning build artifacts...%b\n" "$(COLOR_WARN)" "$(COLOR_RESET)"
	@rm -f $(BIN) $(OBJ) *~ core
	@printf "%b[OK] Directory clean.%b\n" "$(COLOR_OK)" "$(COLOR_RESET)"

help:
	@printf "%bUsage for %s Makefile:%b\n" "$(COLOR_BOLD)" "$(BIN)" "$(COLOR_RESET)"
	@printf "  %bmake%b            - Compile the game with C++20 and SDL3\n" "$(COLOR_INFO)" "$(COLOR_RESET)"
	@printf "  %bmake check-deps%b - Check build dependencies and compiler support\n" "$(COLOR_INFO)" "$(COLOR_RESET)"
	@printf "  %bmake install%b    - Install the application system-wide (default: %s)\n" "$(COLOR_INFO)" "$(COLOR_RESET)" "$(PREFIX)"
	@printf "  %bmake uninstall%b  - Remove all installed files and directories\n" "$(COLOR_INFO)" "$(COLOR_RESET)"
	@printf "  %bmake clean%b      - Remove object files and compiled binary\n" "$(COLOR_INFO)" "$(COLOR_RESET)"
