#####################################################
#                                                   #
#              mkverobj make recipes                #
#                                                   #
#####################################################

# TODO: until I figure out how to use autoconf/automake,
# and you want to build a debug version:
#
#   1.) you will need to define MKVEROBJ_DEBUG on the
#	    command line, or in the global environment before
#		running make.
#	2.) You may open this file in a text editor and modify
#       the behavior yourself.
#
# at this time, unless that variable is set, an optimized
# release configuration will be used.

# these variables will be stamped into the example applications.
VER_MAJOR   := 1
VER_MINOR   := 2
VER_BUILD   := 3
VER_NOTES   := This is just an example of what mkverobj can do.

# this is the object file that contains the version data and retrieval code.
VER_OBJFILE := VERSION.o

# this is the temporary file used to contain the raw binary data that is
# imported into the object file.
VER_BINFILE := VERSION

# this is an ASM file containing instructions on how to import the raw
# binary data into the object file.
VER_INCFILE := version.S

# compiler/linker commands. you're going to want to make sure $INCLUDE 
# and $LDFLAGS are defined.
CFLAGS          = -Wpedantic -std=c11 -fPIC -I.
CXXFLAGS        = -Wpedantic -v -std=c++20 -fPIC -I.
CFLAGS_NDEBUG   = -O3 -DNDEBUG
CFLAGS_DEBUG    = -g -O0 -DDEBUG
CXXFLAGS_NDEBUG = -O3 -DNDEBUG
CXXFLAGS_DEBUG  = -g -O0 -DDEBUG
LDFLAGS         = 

ifndef (MKVEROBJ_DEBUG)
	CFLAGS   += $(CFLAGS_NDEBUG)
	CXXFLAGS += $(CXXFLAGS_NDEBUG)
else
	CFLAGS   += $(CFLAGS_DEBUG)
	CXXFLAGS += $(CXXFLAGS_DEBUG)
endif

# set up build paths and filenames
BUILDDIR  := build
INTDIR    := $(BUILDDIR)/obj
BINDIR    := $(BUILDDIR)/bin

MKVEROBJ   := mkverobj
CEXAMPLE   := cexample
CXXEXAMPLE := cxxexample

#
# mkverobj
#
TUS_MKVEROBJ := $(MKVEROBJ).cc
OBJ_MKVEROBJ := $(INTDIR)/$(MKVEROBJ).o
BIN_MKVEROBJ := $(BINDIR)/$(MKVEROBJ)

#
# cexample
#
TUS_CEXAMPLE := $(CEXAMPLE).c
OBJ_CEXAMPLE := $(INTDIR)/$(CEXAMPLE).o
BIN_CEXAMPLE := $(BINDIR)/$(CEXAMPLE)

#
# cxxexample
#
TUS_CXXEXAMPLE := $(CXXEXAMPLE).cc
OBJ_CXXEXAMPLE := $(INTDIR)/$(CXXEXAMPLE).o
BIN_CXXEXAMPLE := $(BINDIR)/$(CXXEXAMPLE)

#
# targets
#
all: clean prep compile mkverobj verfile cexample cxxexample

-include $(INTDIR)/*.d

depends:
$(BUILDDIR) : prep
$(INTDIR)   : $(BUILDDIR)
$(BINDIR)   : $(BUILDDIR)

$(OBJ_MKVEROBJ) : $(INTDIR)
$(OBJ_CEXAMPLE) : $(INTDIR)
$(OBJ_CXXEXAMPLE) : $(INTDIR)
$(VER_OBJFILE) : $(INTDIR)

$(BIN_MKVEROBJ) : $(OBJ_MKVEROBJ)
$(VER_INCFILE) : $(BIN_MKVEROBJ)
$(VER_OBJFILE) : $(VER_INCFILE)
$(BIN_CEXAMPLE) : $(VER_OBJFILE)
$(BIN_CXXEXAMPLE) : $(VER_OBJFILE)

compile: depends $(OBJ_MKVEROBJ) $(OBJ_CEXAMPLE) $(OBJ_CXXEXAMPLE) 
$(OBJ_MKVEROBJ) : $(TUS_MKVEROBJ) $(DEPS)
	$(CXX) -MMD -c -o $@ $< $(CXXFLAGS)

$(OBJ_CEXAMPLE) : $(TUS_CEXAMPLE) $(DEPS)
	$(CC) -MMD -c -o $@ $< $(CFLAGS)

$(OBJ_CXXEXAMPLE) : $(TUS_CXXEXAMPLE) $(DEPS)
	$(CXX) -MMD -c -o $@ $< $(CXXFLAGS)

mkverobj: depends $(OBJ_MKVEROBJ)
	$(CXX) -o $(BIN_MKVEROBJ) $(OBJ_MKVEROBJ) $(CXXFLAGS) $(LDFLAGS)

verfile: mkverobj $(VER_INCFILE)
	$(BIN_MKVEROBJ) $(VER_MAJOR) $(VER_MINOR) $(VER_BUILD) "$(VER_NOTES)" "$(VER_OBJFILE)"

cexample: $(OBJ_CEXAMPLE) $(VER_OBJFILE)
	$(CC) -o $(BIN_CEXAMPLE) $(OBJ_CEXAMPLE) $(VER_OBJFILE) $(CFLAGS) $(LDFLAGS)

cxxexample: $(OBJ_CXXEXAMPLE) $(VER_OBJFILE)
	$(CXX) -o $(BIN_CXXEXAMPLE) $(OBJ_CXXEXAMPLE) $(VER_OBJFILE) $(CXXFLAGS) $(LDFLAGS)

prep:
	$(shell mkdir -p $(BINDIR) && mkdir -p $(INTDIR))

clean:
	$(shell if [ -d "$(BUILDDIR)" ]; then rm -rf "$(BUILDDIR)"; fi && \
			if [ -f "$(VER_BINFILE)" ]; then rm -f "$(VER_BINFILE)"; fi && \
			if [ -f "$(VER_INCFILE)" ]; then rm -f "$(VER_INCFILE)"; fi && \
			if [ -f "$(VER_OBJFILE)" ]; then rm -f "$(VER_OBJFILE)"; fi)
	@echo "Cleaned binary and intermediate files."

.PHONY: clean prep
