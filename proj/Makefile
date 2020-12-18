appname=labeler

CXXFLAGS=-Wall -std=c++2a

# With gurobi:
#Gurobi needs to be installed under /opt as recommended in the gurobi docs
CXXFLAGS+=-I/opt/gurobi910/linux64/include/
LDLIBS+=-L/opt/gurobi910/linux64/lib -lgurobi91 -lgurobi_c++

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
