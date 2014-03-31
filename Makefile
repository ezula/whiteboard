# Whiteboard
# Authors: Mikael Svens and Henrik Alnestig
# Date: 2014-02-13
#
# Makefile.

OBJDIR = ./obj/
SRCDIR = ./src/
INCDIR = ./src/
BINDIR = ./

TARGET = $(BINDIR)whiteboard

UNAME  = $(shell uname)
ifeq ($(UNAME), Linux)
  SDL = -lSDL2
endif
ifeq ($(UNAME), Darwin)
  SDL = -framework SDL2
endif

C_SOURCES  = $(wildcard $(SRCDIR)*.c)
C_SOURCES += $(wildcard $(SRCDIR)SDL2_gfx/*.c)
OBJECTS = $(subst .c,.o,$(subst $(SRCDIR),$(OBJDIR),$(C_SOURCES)))
INTERMEDIATE_FILES = $(OBJDIR)*.o $(OBJDIR)*.d $(TARGET)
DEPFILES = $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.d,$(C_SOURCES))
CC = gcc
CFLAGS += -Wall -O3


# LFLAGS += -g
# CFLAGS += -g

LINK = $(CC) $(LFLAGS) -o $@ $^ $(SDL) -lm
COMPILE = $(CC) -I$(INCDIR) $(CFLAGS) -o $@ -c $< $(SDL) -lm

vpath %.c $(SRCDIR)
vpath %.o $(OBJDIR)	

.PHONY: clean all

all: $(TARGET)

include $(DEPFILES)

$(OBJDIR)%.d: $(SRCDIR)%.c
	test -d $(OBJDIR) || mkdir $(OBJDIR);
	test -d $(OBJDIR)SDL2_gfx || mkdir $(OBJDIR)SDL2_gfx;
	set -e; $(CC) -MM -MG $(CFLAGS) -I$(INCDIR) $< \
	| sed -n -e "sW\($*\)\.o[ :]*W$(OBJDIR)\1.o $@ : Wg" -e "p" > $@

$(TARGET): $(OBJECTS)
	$(LINK)

$(OBJDIR)%.o: $(SRCDIR)%.c $(OBJDIR)%.d
	$(COMPILE)

clean:
	-rm -fv $(INTERMEDIATE_FILES)
	-rm -fv *~

