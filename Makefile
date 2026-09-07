# ==============================================================================
# Makefile para Still Yet Another Sokoban (C++20)
# ==============================================================================

CXX ?= g++
BIN := syasokoban
PREFIX ?= /usr/local
DATADIR ?= $(PREFIX)/share/$(BIN)
BINDIR ?= $(PREFIX)/bin

# Cores e Formatação de Terminal (formato octal para printf %b)
COLOR_RESET  := \033[0m
COLOR_BOLD   := \033[1m
COLOR_INFO   := \033[1;36m
COLOR_OK     := \033[1;32m
COLOR_WARN   := \033[1;33m
COLOR_ERR    := \033[1;31m
COLOR_STEP   := \033[1;34m

# ------------------------------------------------------------------------------
# Verificação de Dependências no Sistema
# ------------------------------------------------------------------------------
SDL_CONFIG := $(shell which sdl-config 2>/dev/null)

ifeq ($(SDL_CONFIG),)
  $(error [ERRO] 'sdl-config' não foi encontrado! Instale a biblioteca SDL 1.2: no Manjaro/Arch: 'sudo pacman -S sdl12-compat')
endif

SDL_CFLAGS := $(shell $(SDL_CONFIG) --cflags)
SDL_LIBS   := $(shell $(SDL_CONFIG) --libs)

CXX_C20_CHECK := $(shell $(CXX) -std=c++20 -dM -E - < /dev/null 2>/dev/null)
ifeq ($(CXX_C20_CHECK),)
  $(error [ERRO] O compilador '$(CXX)' não oferece suporte a C++20 (-std=c++20)!)
endif

# Flags de Compilação
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -O2 -I/usr/local/include $(SDL_CFLAGS)
LIBS     := $(SDL_LIBS) -lm

SRC := $(shell find src -iname '*.cpp' -type f | sort)
OBJ := $(SRC:.cpp=.o)

# ------------------------------------------------------------------------------
# Alvos (Targets)
# ------------------------------------------------------------------------------
.PHONY: all check-deps clean install uninstall help

all: check-deps $(BIN)
	@printf "%b%b>>> Compilação concluída com sucesso! Executável: ./%s <<<%b\n" "$(COLOR_OK)" "$(COLOR_BOLD)" "$(BIN)" "$(COLOR_RESET)"

check-deps:
	@printf "%b==> Verificando dependências do sistema...%b\n" "$(COLOR_INFO)" "$(COLOR_RESET)"
	@printf "    • Compilador: %s (%s)\n" "$(CXX)" "$$( $(CXX) --version | head -n1 )"
	@printf "    • Suporte C++20: %bDisponível%b\n" "$(COLOR_OK)" "$(COLOR_RESET)"
	@printf "    • SDL 1.2: %bDisponível%b (%s)\n" "$(COLOR_OK)" "$(COLOR_RESET)" "$$( $(SDL_CONFIG) --version )"

$(BIN): $(OBJ)
	@printf "%b[LINK]%b Gerando binário final: %b%s%b\n" "$(COLOR_STEP)" "$(COLOR_RESET)" "$(COLOR_BOLD)" "$@" "$(COLOR_RESET)"
	@$(CXX) -o $@ $(OBJ) $(LIBS)

%.o: %.cpp
	@printf "%b[CXX]%b Compilando: %s\n" "$(COLOR_STEP)" "$(COLOR_RESET)" "$<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# ------------------------------------------------------------------------------
# Instalação e Desinstalação
# ------------------------------------------------------------------------------
install: all
	@printf "%b==> Instalando %s em %s...%b\n" "$(COLOR_INFO)" "$(BIN)" "$(PREFIX)" "$(COLOR_RESET)"
	@install -d "$(DESTDIR)$(DATADIR)"
	@install -d "$(DESTDIR)$(BINDIR)"
	@printf "    • Copiando dados para %s\n" "$(DESTDIR)$(DATADIR)"
	@cp -r data "$(DESTDIR)$(DATADIR)/"
	@printf "    • Copiando executável binário para %s\n" "$(DESTDIR)$(DATADIR)/$(BIN).bin"
	@install -m 755 $(BIN) "$(DESTDIR)$(DATADIR)/$(BIN).bin"
	@printf "    • Criando script inicializador em %s\n" "$(DESTDIR)$(BINDIR)/$(BIN)"
	@printf '#!/bin/sh\ncd "%s" && exec "./%s.bin" "$$@"\n' "$(DATADIR)" "$(BIN)" > "$(DESTDIR)$(BINDIR)/$(BIN)"
	@chmod 755 "$(DESTDIR)$(BINDIR)/$(BIN)"
	@printf "%b[OK] %s instalado com sucesso! Execute digitando apenas '%s'.%b\n" "$(COLOR_OK)" "$(BIN)" "$(BIN)" "$(COLOR_RESET)"

uninstall:
	@printf "%b==> Desinstalando %s do sistema...%b\n" "$(COLOR_WARN)" "$(BIN)" "$(COLOR_RESET)"
	@if [ -f "$(DESTDIR)$(BINDIR)/$(BIN)" ]; then \
		rm -f "$(DESTDIR)$(BINDIR)/$(BIN)"; \
		printf "    • Removido: %s\n" "$(DESTDIR)$(BINDIR)/$(BIN)"; \
	fi
	@if [ -d "$(DESTDIR)$(DATADIR)" ]; then \
		rm -rf "$(DESTDIR)$(DATADIR)"; \
		printf "    • Removido diretório: %s\n" "$(DESTDIR)$(DATADIR)"; \
	fi
	@printf "%b[OK] %s desinstalado com sucesso!%b\n" "$(COLOR_OK)" "$(BIN)" "$(COLOR_RESET)"

clean:
	@printf "%b==> Limpando arquivos compilados...%b\n" "$(COLOR_WARN)" "$(COLOR_RESET)"
	@rm -f $(BIN) $(OBJ) *~ core
	@printf "%b[OK] Diretório limpo.%b\n" "$(COLOR_OK)" "$(COLOR_RESET)"

help:
	@printf "%bUso do Makefile para %s:%b\n" "$(COLOR_BOLD)" "$(BIN)" "$(COLOR_RESET)"
	@printf "  %bmake%b            - Compila o jogo com verificações em C++20\n" "$(COLOR_INFO)" "$(COLOR_RESET)"
	@printf "  %bmake check-deps%b - Exibe o status das dependências de compilação\n" "$(COLOR_INFO)" "$(COLOR_RESET)"
	@printf "  %bmake install%b    - Instala o jogo no sistema (padrão: %s)\n" "$(COLOR_INFO)" "$(COLOR_RESET)" "$(PREFIX)"
	@printf "  %bmake uninstall%b  - Remove todos os arquivos instalados\n" "$(COLOR_INFO)" "$(COLOR_RESET)"
	@printf "  %bmake clean%b      - Remove os binários e objetos compilados\n" "$(COLOR_INFO)" "$(COLOR_RESET)"
