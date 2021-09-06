linkTarget = main

CXX=g++
RM=rm -f

CPPFLAGS=-g -std=c++17 -Wall

LDLIBS=

objects=$(patsubst %.cpp,%.o,$(wildcard *.cpp))

rebuildables = $(objects) $(linkTarget)

$(linkTarget): $(objects)
	$(CXX) -o $(linkTarget) $(objects) $(LDLIBS) $(CPPFLAGS)

%.o: %.cpp include/%.h
	$(CXX) -o $@ -c $< $(CPPLFAGS)

clean:
	$(RM) $(rebuildables)
