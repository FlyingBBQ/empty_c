PROG = main

CXX = gcc
CFLAGS = -Wall -pedantic -Werror -g -I/usr/include/
LFLAGS = -lm

SRCDIR   = .
INCDIR   = .
OBJDIR   = .

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(INCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# top-level rule to create the program.
all: $(PROG)

# compiling the source files
$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CXX) $(CFLAGS) -c -s $< -o $@

# linking the program
$(PROG): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LFLAGS) -o $(PROG)

run: $(PROG)
	./$(PROG)

clean:
	rm $(PROG) $(OBJDIR)/*.o
