CC = gcc
CFLAGS =
LDFLAGS = -lm
OBJFILES = 20151619.o util.o commands.o ds.o
TARGET = a.out

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
clean:
	rm -f $(OBJFILES) $(TARGET) *~
