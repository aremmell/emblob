#####################################################
#                                                   #
#              mkverobj make recipes                #
#                                                   #
#####################################################

#include helper functions for reporting build progress
# to the user via the console output.
include console.mk

# set up build paths and filenames
OUTDIR         = build
INTDIR         = $(OUTDIR)/obj
BINDIR         = $(OUTDIR)/bin
MKVEROBJ       = mkverobj
MKVEROBJ_OBJ   = $(INTDIR)/$(MKVEROBJ).o
MKVEROBJ_OUT   = $(BINDIR)$(MKVEROBJ)
MKVEROBJ_SRC   = $(MKVEROBJ).cc
CEXAMPLE       = c-example
CEXAMPLE_OBJ   = $(INTDIR)/$(CEXAMPLE).o
CEXAMPLE_OUT   = $(BINDIR)/$(CEXAMPLE)
CEXAMPLE_SRC   = $(CEXAMPLE).c
CXXEXAMPLE     = cxx-example
CXXEXAMPLE_OBJ = $(INTDIR)/$(CXXEXAMPLE).o
CXXEXAMPLE_OUT = $(BINDIR)/$(CXXEXAMPLE)
CXXEXAMPLE_SRC = $(CXXEXAMPLE).cc

# some conditionals; if left undefined/invalid,
# we'll make some executive decisions
#ifeq (,$(CFLAGS))
#else
#endif

# $1: The variable in question
# $2: The default value that will be inserted into
#     the variable if it is missing or invalid.
#define set_default_if_required =
#	ifeq(, $(1))
#	else
#	endif
#endef

# compiler/linker commands
CFLAGS   = -I.
CXXFLAGS = -Wpedantic -Wno-psabi -std=c++17 -I.

# depdendency graph
$(OUTDIR)         : prep
$(INTDIR)         : $(OUTDIR)
$(BINDIR)         : $(OUTDIR)
$(CEXAMPLE_OBJ)   : $(CEXAMPLE_SRC)
$(CXXEXAMPLE_OBJ) : $(CXXEXAMPLE_SRC)
$(CEXAMPLE_OUT)   : $(CEXAMPLE_OBJ)
$(CXXEXAMPLE_OUT) : $(CXXEXAMPLE_OBJ)

# recipes
all: prep $(MKVEROBJ_OUT) $(CEXAMPLE_OUT) $(CXXEXAMPLE_OUT)

-include $(INTDIR)/*.d

# create a list of targets
ALL_OUT = $(MKVEROBJ_OUT) $(CEXAMPLE_OUT) $(CXXEXAMPLE_OUT)

$(MKVEROBJ_OBJ): $(MKVEROBJ_SRC) $(DEPS)
	$(CXX) -MMD -c -o $@ $< $(CXXFLAGS)
	$(call echo_info,exported variable is $(CONSOLE_MSG_ERROR))

$(MKVEROBJ_OUT): $(MKVEROBJ_OBJ)
	$(CXX) -o $@ $(MKVEROBJ_OBJ) $(CXXFLAGS) $(LDFLAGS)
	$(call echo_build_success,$(MKVEROBJ_OUT))

prep:
	$(shell mkdir -p $(OUTDIR))
	$(call echo_build_success,$(OUTDIR))

clean:
	$(shell rm -f $(OUTDIR)/*)
	$(call echo_success,cleaned $(OUTDIR) successfully.)


.PHONY: prep clean
