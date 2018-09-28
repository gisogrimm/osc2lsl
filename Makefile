all: build bin

CXXFLAGS += -std=c++11
LDLIBS += -llo -llsl64

build:
	mkdir -p $@

bin: build/osc2lsl

%: %.o
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

build/%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Local Variables:
# compile-command: "make"
# End:
