CXX = g++
SDKDIR	= /opt/yusur/hados
CXXFLAGS = -Wall -std=c++17 -I$(SDKDIR)/include -L$(SDKDIR)/lib
LDLIBS = -lhados_doe

PROGRAM = perf_test func_test confict_test

all: $(PROGRAM)

%: src/%.cpp
	$(CXX) $< -Iinclude $(CXXFLAGS) $(LDLIBS) -o $@

perf_test: src/perf_test.cpp
	$(CXX) $< $(CXXFLAGS) -L/usr/local/lib64 -lhados_doe -lbenchmark -lpthread -o $@

clean:
	rm -f $(PROGRAM)
