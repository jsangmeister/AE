appname=labeler

CXXFLAGS=-Wall -std=c++2a

SRCS=$(shell find . -name "*.cpp")
OBJS=$(subst .cpp,.o,$(SRCS))
DEPS=$(SRCS:.cpp=.d)

.PHONY: all clean

all: $(appname)

$(appname): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname) $(OBJS) $(LDLIBS)

%.o: %.cpp
	$(CXX) -c -MMD -o $@ $<

clean:
	rm -f $(appname) $(OBJS) $(DEPS)

-include $(DEPS)
