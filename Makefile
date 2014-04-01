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
  SDL = -lSDL2 -lGL -lSDL2_net
endif
ifeq ($(UNAME), Darwin)
  SDL = -framework SDL2 -framework OpenGL -framework SDL2_net
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

LINK = $(CC) $(LFLAGS) -o $@ $^ $(SDL)
COMPILE = $(CC) -I$(INCDIR) $(CFLAGS) -o $@ -c $< $(SDL)

vpath %.c $(SRCDIR)
vpath %.o $(OBJDIR)	

.PHONY: clean all

all: $(TARGET)

include $(DEPFILES)

$(OBJDIR)%.d: $(SRCDIR)%.c
	test -d $(OBJDIR) || mkdir $(OBJDIR);
	set -e; $(CC) -MM -MG $(CFLAGS) -I$(INCDIR) $< \
	| sed -n -e "sW\($*\)\.o[ :]*W$(OBJDIR)\1.o $@ : Wg" -e "p" > $@

$(TARGET): $(OBJECTS)
	$(LINK)

$(OBJDIR)%.o: $(SRCDIR)%.c $(OBJDIR)%.d
	$(COMPILE)

clean:
	-rm -fv $(INTERMEDIATE_FILES)
	-rm -fv *~

