OBJDIR = ./obj/
SRCDIR = ./src/
INCDIR = ./src/
BINDIR = ./

TARGET = $(BINDIR)whiteboard

C_SOURCES  = $(wildcard $(SRCDIR)*.c)
ifneq ($(findstring $(SCANNER_SRC), $(C_SOURCES)), $(SCANNER_SRC))
  C_SOURCES += $(SCANNER_SRC)
endif
ifneq ($(findstring $(PARSER_SRC), $(C_SOURCES)), $(PARSER_SRC))
  C_SOURCES += $(PARSER_SRC)
endif
OBJECTS = $(subst .c,.o,$(subst $(SRCDIR),$(OBJDIR),$(C_SOURCES)))
INTERMEDIATE_FILES = $(OBJDIR)*.o $(OBJDIR)*.d $(TARGET) $(SCANNER_SRC) $(PARSER_SRC) $(PARSER_H)
DEPFILES = $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.d,$(C_SOURCES))
CC = gcc

CFLAGS += -Wall

# LFLAGS += -g
# CFLAGS += -g

LINK = $(CC) $(LFLAGS) -o $@ $^
COMPILE = $(CC) -I$(INCDIR) $(CFLAGS) -o $@ -c $<

vpath %.c $(SRCDIR)
vpath %.o $(OBJDIR)

.PHONY: clean all

all: $(TARGET)

include $(DEPFILES)

$(OBJDIR)%.d: $(SRCDIR)%.c
	set -e; $(CC) -MM -MG $(CFLAGS) -I$(INCDIR) $< \
	| sed -n -e "sW\($*\)\.o[ :]*W$(OBJDIR)\1.o $@ : Wg" -e "p" > $@

$(TARGET): $(OBJECTS)
	$(LINK)

$(OBJDIR)%.o: $(SRCDIR)%.c $(OBJDIR)%.d
	$(COMPILE)

clean:
	-rm -fv $(INTERMEDIATE_FILES)
	-rm -fv *~

