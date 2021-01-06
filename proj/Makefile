appname=labeler

CXXFLAGS=-Wall -std=c++2a

# Set Gurobi flags
CXXFLAGS+=-I${GUROBI_HOME}/include/
LDLIBS+=-L${GUROBI_HOME}/lib/ -lgurobi91 -lgurobi_c++

SRCS=$(shell find . -name "*.cpp")
OBJS=$(subst .cpp,.o,$(SRCS))
DEPS=$(SRCS:.cpp=.d)

.PHONY: all clean

all: $(appname)

$(appname): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname) $(OBJS) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c -MMD -o $@ $<

clean:
	rm -f $(appname) $(OBJS) $(DEPS)

-include $(DEPS)
