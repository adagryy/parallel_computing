CC=cc
CFLAGS=-c
LDFLAGS=
SOURCES=child.c err.c 
# cleanup.c ctrl-c.c signals.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=prir

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -f *.o