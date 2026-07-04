CFLAGS   = -Wall -Wextra -std=c99 -D_DEFAULT_SOURCE -I. $(shell pkg-config --cflags lua 2>/dev/null)
CXXFLAGS = -Wall -Wextra -std=c++11 -D_DEFAULT_SOURCE -I. $(shell pkg-config --cflags lua 2>/dev/null)
LDLIBS   = $(shell pkg-config --libs lua 2>/dev/null || echo '-llua') -lm

TARGET   = a.out
TARGET_CPP = a_cpp.out

test_all: test test_cpp

test: $(TARGET)
	cd test && ../$(TARGET)

test_cpp: $(TARGET_CPP)
	cd test && ../$(TARGET_CPP)

$(TARGET): test/test.c conf.h
	$(CC) $(CFLAGS) -o $@ test/test.c $(LDLIBS)

$(TARGET_CPP): test/test.cpp conf.h
	$(CXX) $(CXXFLAGS) -o $@ test/test.cpp $(LDLIBS)

clean:
	rm -f $(TARGET) $(TARGET_CPP)
