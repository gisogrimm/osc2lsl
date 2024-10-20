all: build bin

pack: all
	$(MAKE) -C packaging

CXXFLAGS += -std=c++11
LDLIBS += -llo -llsl

build:
	mkdir -p $@

bin: build/osc2lsl

%: %.o
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

build/%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -Rf build packaging/pack packaging/man


# Local Variables:
# compile-command: "make"
# End:
