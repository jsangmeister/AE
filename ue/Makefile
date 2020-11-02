appname=labeler

CXXFLAGS=-Wall -std=c++20

SRCS=$(shell find . -name "*.cpp")
OBJS=$(subst .cpp,.o,$(SRCS))

all: $(appname)

$(appname): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname) $(OBJS) $(LDLIBS)

$(appname).o: src/Main.cpp src/Parser.hpp

clean:
	rm -f $(appname).o $(appname) src/*.o
