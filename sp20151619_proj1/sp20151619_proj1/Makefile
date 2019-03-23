CC = gcc
CFLAGS =
LDFLAGS =
OBJFILES = 20151619.o util.o commands.o
TARGET = 20151619.out

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
clean:
	rm -f $(OBJFILES) $(TARGET) *~
