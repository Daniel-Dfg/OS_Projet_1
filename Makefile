# Name            Prénom         ULBID           Matricule
# El Hussein      Abdalrahman    aelh0063        000596003
# Ibrahim         Dogan          idog0003        000586544
# Daniel          Defoing        ddef0003        000589910

TOPDIR  := ./
SRCDIR  := $(TOPDIR)src/
OBJDIR  := $(TOPDIR)obj/
BINDIR  := $(TOPDIR)
NAME    := chat
EXE     := $(BINDIR)$(NAME)
#Added
TMP := $(TOPDIR)tmp/

SFILES  := cpp
OFILES  := o
# Added
CC      := g++
#Added
CFLAGS  := -std=gnu++17 -Wall -Wextra -O2 -Wpedantic -pedantic -march=native -Wnull-dereference -Winline -Wconversion -g -fsanitize=address,undefined
LIBS    := -fsanitize=address,undefined

SOURCES := $(shell find $(SRCDIR) -name "*.$(SFILES)")
OBJECTS := $(patsubst $(SRCDIR)%.$(SFILES), $(OBJDIR)%.$(OFILES), $(SOURCES))


ALLFILES := $(SOURCES)

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJECTS)
	$(CC) $^ -o $@ $(LIBS)

$(OBJDIR)%$(OFILES): $(SRCDIR)%$(SFILES)
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	@rm -rf $(OBJECTS) $(EXE) $(TMP)
