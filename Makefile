TOPDIR  := ./
SRCDIR  := $(TOPDIR)src/
OBJDIR  := $(TOPDIR)obj/
BINDIR  := $(TOPDIR)
NAME    := chat
EXE     := $(BINDIR)$(NAME)

SFILES  := cpp
OFILES  := o
CC      := g++
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
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	@rm -f $(OBJECTS) $(EXE)
