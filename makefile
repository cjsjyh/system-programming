CC = gcc
CFLAGS = -g
LDFLAGS = -lm
OBJFILES = 20151619.o commands.o util.o ds.o
TARGET = 20151619.out

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
clean:
	rm -f $(OBJFILES) $(TARGET) *~
