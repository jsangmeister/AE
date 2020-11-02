appname=mergesort

CXXFLAGS=-Wall -std=c++2a

SRCS=$(shell find . -name "*.cpp")
OBJS=$(subst .cpp,.o,$(SRCS))

all: $(appname)

$(appname): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname) $(OBJS) $(LDLIBS)

$(appname).o: src/main.cpp

clean:
	rm -f $(appname).o $(appname) src/*.o
