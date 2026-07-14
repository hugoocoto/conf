LUA        ?= lua5.1
LUA_CFLAGS  = $(shell pkg-config --cflags $(LUA) 2>/dev/null)
LUA_LIBS    = $(shell pkg-config --libs $(LUA) 2>/dev/null || echo '-l$(LUA)')

CFLAGS   = -Wall -Wextra -std=c99 -D_DEFAULT_SOURCE -I. $(LUA_CFLAGS)
CXXFLAGS = -Wall -Wextra -std=c++11 -D_DEFAULT_SOURCE -I. $(LUA_CFLAGS)
LDLIBS   = $(LUA_LIBS) -lm

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
