#####################################################
#                                                   #
#              mkverobj make recipes                #
#                                                   #
#####################################################

# TODO: until I figure out how to use autoconf/automake,
# and you want to build a debug version:
#
#   1.) you will need to define MKVEROBJ_DEBUG=1 on the
#	    command line, or in the global environment before
#		running make.
#	2.) You may open this file in a text editor and modify
#       the behavior yourself.
#
# at this time, unless that variable is set, an optimized
# release configuration will be used.

# libsir
LIBSIRDIR := lib/libsir

# compiler/linker commands. you're going to want to make sure $INCLUDE
# and $LDFLAGS are defined.
CFLAGS          = -Wpedantic -std=c11 -fPIC -I.
CXXFLAGS        = -Wpedantic -std=c++17 -fPIC -I. -I$(LIBSIRDIR) -L$(LIBSIRDIR)/build/lib -lsir_s
CFLAGS_NDEBUG   = -O3 -DNDEBUG
CFLAGS_DEBUG    = -g -O0 -DDEBUG
CXXFLAGS_NDEBUG = -O3 -DNDEBUG
CXXFLAGS_DEBUG  = -g -O0 -DDEBUG
LDFLAGS         =

ifeq ($(MKVEROBJ_DEBUG),1)
	CFLAGS   += $(CFLAGS_DEBUG)
	CXXFLAGS += $(CXXFLAGS_DEBUG)
else
	CFLAGS   += $(CFLAGS_NDEBUG)
	CXXFLAGS += $(CXXFLAGS_NDEBUG)
endif

# set up build paths and filenames
BUILDDIR  := build
INTDIR    := $(BUILDDIR)/obj
BINDIR    := $(BUILDDIR)/bin

MKVEROBJ   := mkverobj
VERFILE    := version
CEXAMPLE   := cexample
CXXEXAMPLE := cxxexample

# these variables will be stamped into the example applications.
VER_MAJOR   := 1
VER_MINOR   := 2
VER_BUILD   := 3
VER_NOTES   := This is just an example of what mkverobj can do.

# this is the object file that contains the version data and retrieval code.
OBJ_VERFILE := $(INTDIR)/$(VERFILE).o

# this is the temporary file containing the raw binary data that is
# imported into OBJ_VERFILE.
BIN_VERFILE := $(INTDIR)/$(VERFILE).bin

# this is an ASM file containing instructions on how to import
# BIN_VERFILE into OBJ_VERFILE.
ASM_VERFILE := $(INTDIR)/$(VERFILE).S

# mkverobj
TUS_MKVEROBJ := $(MKVEROBJ).cc
OBJ_MKVEROBJ := $(INTDIR)/$(MKVEROBJ).o
BIN_MKVEROBJ := $(BINDIR)/$(MKVEROBJ)

# cexample
TUS_CEXAMPLE := $(CEXAMPLE).c
OBJ_CEXAMPLE := $(INTDIR)/$(CEXAMPLE).o
BIN_CEXAMPLE := $(BINDIR)/$(CEXAMPLE)

# cxxexample
TUS_CXXEXAMPLE := $(CXXEXAMPLE).cc
OBJ_CXXEXAMPLE := $(INTDIR)/$(CXXEXAMPLE).o
BIN_CXXEXAMPLE := $(BINDIR)/$(CXXEXAMPLE)

# targets
all: clean prep compile mkverobj verfile cexample cxxexample

-include $(INTDIR)/*.d

depends:
$(BUILDDIR) : prep
$(INTDIR)   : $(BUILDDIR)
$(BINDIR)   : $(BUILDDIR)

$(OBJ_MKVEROBJ) : $(INTDIR) $(LIBSIRDIR)
$(OBJ_CEXAMPLE) : $(INTDIR)
$(OBJ_CXXEXAMPLE) : $(INTDIR)
$(OBJ_VERFILE) : $(INTDIR)
$(BIN_VERFILE) : $(INTDIR)
$(ASM_VERFILE) : $(INTDIR)

$(BIN_MKVEROBJ) : $(OBJ_MKVEROBJ)
$(ASM_VERFILE) : $(BIN_MKVEROBJ)
$(OBJ_VERFILE) : $(ASM_VERFILE)
$(BIN_CEXAMPLE) : $(OBJ_VERFILE)
$(BIN_CXXEXAMPLE) : $(OBJ_VERFILE)

$(LIBSIRDIR): depends
	SIR_NO_SYSTEM_LOGGERS=1 $(MAKE) -C $@ static

compile: depends $(OBJ_MKVEROBJ) $(OBJ_CEXAMPLE) $(OBJ_CXXEXAMPLE)
$(OBJ_MKVEROBJ) : $(TUS_MKVEROBJ) $(DEPS)
	$(CXX) -MMD -c -o $@ $< $(CXXFLAGS)

$(OBJ_CEXAMPLE) : $(TUS_CEXAMPLE) $(DEPS)
	$(CC) -MMD -c -o $@ $< $(CFLAGS)

$(OBJ_CXXEXAMPLE) : $(TUS_CXXEXAMPLE) $(DEPS)
	$(CXX) -MMD -c -o $@ $< $(CXXFLAGS)

mkverobj: depends $(OBJ_MKVEROBJ)
	$(CXX) -o $(BIN_MKVEROBJ) $(OBJ_MKVEROBJ) $(CXXFLAGS) $(LDFLAGS)

verfile: mkverobj $(ASM_VERFILE)
	$(BIN_MKVEROBJ) --major $(VER_MAJOR) --minor $(VER_MINOR) --build $(VER_BUILD) --notes "$(VER_NOTES)" --outfile "$(INTDIR)/$(VERFILE)" --log-level debug

cexample: $(OBJ_CEXAMPLE) $(OBJ_VERFILE)
	$(CC) -o $(BIN_CEXAMPLE) $(OBJ_CEXAMPLE) $(OBJ_VERFILE) $(CFLAGS) $(LDFLAGS)

cxxexample: $(OBJ_CXXEXAMPLE) $(OBJ_VERFILE)
	$(CXX) -o $(BIN_CXXEXAMPLE) $(OBJ_CXXEXAMPLE) $(OBJ_VERFILE) $(CXXFLAGS) $(LDFLAGS)

prep:
	$(shell mkdir -p $(BINDIR) && mkdir -p $(INTDIR))

clean:
	$(shell if [ -d "$(BUILDDIR)" ]; then rm -rf "$(BUILDDIR)"; fi)
	@echo "Cleaned binary and intermediate files."

.PHONY: clean prep
