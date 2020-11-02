appname=mergesort

CXXFLAGS=-Wall -std=c++2a

EXECUTABLES = mergesort8 mergesort16 mergesort32 mergesort64
SRCS=$(shell find . -name "*.cpp")
OBJS8=$(subst .cpp,-8.o,$(SRCS))
OBJS16=$(subst .cpp,-16.o,$(SRCS))
OBJS32=$(subst .cpp,-32.o,$(SRCS))
OBJS64=$(subst .cpp,-64.o,$(SRCS))

all: $(EXECUTABLES)

mergesort8: $(OBJS8)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o mergesort8 $(OBJS8) $(LDLIBS)

mergesort16: $(OBJS16)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o mergesort16 $(OBJS16) $(LDLIBS)

mergesort32: $(OBJS32)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o mergesort32 $(OBJS32) $(LDLIBS)

mergesort64: $(OBJS64)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o mergesort64 $(OBJS64) $(LDLIBS)

#mergesort8.o: src/main.cpp

%-8.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -D Q_t=uint8_t -o $@ $<

%-16.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -D Q_t=uint16_t -o $@ $<

%-32.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -D Q_t=uint32_t -o $@ $<

%-64.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -D Q_t=uint64_t -o $@ $<

clean:
	rm -f mergesort* src/*.o

.PHONY: all clean
