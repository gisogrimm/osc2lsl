all: build bin

pack: all
	$(MAKE) -C packaging

LO_CFLAGS := $(shell pkg-config --cflags liblo)
LO_LIBS := $(shell pkg-config --libs liblo)

CXXFLAGS += -std=c++11 $(LO_CFLAGS)
LDLIBS += $(LO_LIBS) -llsl

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
