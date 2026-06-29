TARGET  = a.out
CFLAGS  = -Wall -Wextra -std=c99 -D_DEFAULT_SOURCE
LDLIBS  = -llua -lm

$(TARGET): test.c conf.h
	$(CC) $(CFLAGS) -o $@ test.c $(LDLIBS)

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
