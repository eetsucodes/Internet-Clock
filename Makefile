TARGET = main
FNAME = InternetClock
all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(FNAME) $(TARGET).c

clean:
	$(RM) $(TARGET)