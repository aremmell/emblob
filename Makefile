#####################################################
#                                                   #
#              mkverobj make recipes                #
#                                                   #
#####################################################

OUTDIR=build
OBJ=$(OUTDIR)/mkverobj.o
OUT=$(OUTDIR)/mkverobj
SRC=mkverobj.cc

CXXFLAGS = -Wpedantic -Wno-psabi -std=c++17 -I.

all: $(OUT)

-include $(OUTDIR)/*.d

$(OUTDIR)/mkverobj.o: $(SRC) $(DEPS)
	$(CXX) -MMD -c -o $@ $< $(CXXFLAGS)

$(OUT): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(OUTDIR)/*

.PHONY: clean
