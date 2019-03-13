CC = gcc
CFLAGS = -Wall
LDFLAGS =
OBJFILES = main.o util.o commands.o
TARGET = 20151619.out

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
clean:
	rm -f $(OBJFILES) $(TARGET) *~
