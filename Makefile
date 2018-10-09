#####################################################
#                                                   #
#              mkverbin make recipes                #
#                                                   #
#####################################################

OBJ=mkverbin.o
SRC=mkverbin.cc
OUT=mkverbin

CXXFLAGS = -Wpedantic -Wno-psabi -std=c++17 -I.

all: $(OUT)

-include *.d

mkverbin.o: $(SRC) $(DEPS)
	$(CXX) -MMD -c -o $@ $< $(CXXFLAGS)

$(OUT): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f *.o *.d $(OUT)

.PHONY: clean
