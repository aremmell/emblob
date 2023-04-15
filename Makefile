#####################################################
#                                                   #
#              mkverobj make recipes                #
#                                                   #
#####################################################

# include helper functions for reporting build progress
# to the user hrough colorized console messages.
include console.mk
# TODO: at this time, there will be no install recipe.
# I have some thinking to do about how to do that reliably.
#$(call echo_warning,no insall recipe is implemented at this time.)
#$(call echo_warning,please utilize your locally compiled components in the interim.)

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

# compiler/linker commands
CFLAGS    = -Wpedantic -std=c11 -I.
CXXFLAGS  = -Wpedantic -std=c++17 -I.
CFLAGS_NDEBUG = -O3 -DNDEBUG
CFLAGS_DEBUG  = -g -O0 -DDEBUG
CXXFLAGS_NDEBUG = -O3 -DNDEBUG
CXXFLAGS_DEBUG  = -g -O0 -DDEBUG

#$(call echo_debug,CFLAGS BEFORE: $(CFLAGS))
#$(call echo_debug,CXXFLAGS BEFORE: $(CXXFLAGS))

ifndef (MKVEROBJ_DEBUG)
#$(call echo_info,MKVEROBJ_DEBUG is undefined; not using debug settings.)
	CFLAGS   += $(CFLAGS_NDEBUG)
	CXXFLAGS += $(CXXFLAGS_NDEBUG)
else
#$(call echo_warning,MKVEROBJ_DEBUG is defined; debug settings will be used.)
	CFLAGS   += $(CFLAGS_DEBUG)
	CXXFLAGS += $(CXXFLAGS_DEBUG)
endif

ifeq ($(OS),Windows_NT)
	CFLAGS   += -D_WIN32
	CXXFLAGS += -D_WIN32
endif

# relevant input direcgtories. if variables like INCLUDE
# are empty, there is no reliable way to guess what they
# might be from within this Makefile, so I'll make guesses.
ifdef (INCLUDE)
	CFLAGS   := $(INCLUDE)
	CXXFLAGS := $(INCLUDE)
endif

#$(call echo_debug, CFLAGS AFTER: '$(CFLAGS)')
#$(call echo_debug, CXXFLAGS AFTER: '$(CXXFLAGS)')

# set up build paths and filenames
BUILDDIR  = build
INTDIR    = $(BUILDDIR)/obj
BINDIR    = $(BUILDDIR)/bin

MKVEROBJ   := mkverobj
CEXAMPLE   := cexample
CXXEXAMPLE := cxxexample

#
# mkverobj
#

# translation units
TUS_MKVEROBJ := $(MKVEROBJ).cc

# intermediate files
OBJ_MKVEROBJ = $(INTDIR)/$(MKVEROBJ).o

# binary target
BIN_MKVEROBJ = $(BINDIR)/$(MKVEROBJ)

#
# cexample
#

# tramslation units
TUS_CEXAMPLE = $(CEXAMPLE).c

# intermediate files
OBJ_CEXAMPLE = $(INTDIR)/$(CEXAMPLE).o

# binary target
BIN_CEXAMPLE = $(BINDIR)/$(CEXAMPLE)

#
# cxxexample
#

# translation units
TUS_CXXEXAMPLE = $(CXXEXAMPLE).cc

# intermediate files
OBJ_CXXEXAMPLE = $(CXXEXAMPLE).o

# binary target
BIN_CXXEXAMPLE = $(BINDIR)/$(CXXEXAMPLE)

#
# targets
#

all: prep $(MKVEROBJ) $(CEXAMPLE) $(CXXEXAMPLE)

-include $(INTDIR)/*.d

$(BUILDDIR) : prep
$(INTDIR)   : $(BUILDDIR)
$(BINDIR)   : $(BUILDDIR)

$(OBJ_MKVEROBJ) : $(INTDIR)
$(OBJ_CEXAMPLE) : $(INTDIR)
$(OBJ_CXXEXAMPLE) : $(INTDIR)

$(OBJ_MKVEROBJ) : $(TUS_MKVEROBJ) $(DEPS)
	$(CXX) -MMD -c -o $@ $< $(CXXFLAGS)

$(OBJ_CEXAMPLE) : $(TUS_CEXAMPLE) $(DEPS)
	$(CC) -MMD -c -o $@ $< $(CFLAGS)

$(OBJ_CXXEXAMPLE) : $(TUS_CXXEXAMPLE) $(DEPS)
	$(CXX) -MMD -c -o $@ $< $(CXXFLAGS)

prep:
ifeq ($(OS),Windows_NT)
	$(shell if not exist "$(BUILDDIR)\NUL" mkdir "$(BUILDDIR)" && \
		if not exist "$(BINDIR)\NUL"   mkdir "$(BINDIR)"   && \
		if not exist "$(INTDIR)\NUL"   mkdir "$(INTDIR)")
else
	$(shell mkdir -p $(BINDIR) && mkdir -p $(INTDIR))
endif
	$(call echo_success,initialized INTDIR and BINDIR successfully.)

mkverobj: $(OBJ_MKVEROBJ)
	$(CXX) -o $(BIN_MKVEROBJ) $(OBJ_MKVEROBJ) $(CXXFLAGS) $(LDFLAGS)
	$(call echo_build_success,$(BIN_MKVEROBJ))

cexample: $(OBJ_CEXAMPLE)
	$(CC) -o $(BIN_CEXAMPLE) $(OBJ_CEXAMPLE) $(CFLAGS) $(LDFLAGS)
	$(call echo_build_success,$(BIN_CEXAMPLE))

cxxexample: $(OBJ_CXXEXAMPLE)
	$(CXX) -o $(BIN_CXXEXAMPLE) $(OBJ_CXXEXAMPLE) $(CXXFLAGS) $(LDFLAGS)
	$(call echo_build_success,$(BIN_CXXEXAMPLE))

# TODO
#install: mkverobj
#	 ifeq ($(OS),Windows_NT)
#	 	$(call echo_error,E_NOTIMPL: no install suppport for Windows yet.)
#	 else
#		@echo copying $(OUT_SHARED) to $(INSTALLDIR) and headers to $(INSTALLINC)...
#		$(shell cp -f $(OUT_SHARED) "$(INSTALLDIR)/" &&
#		        cp -f *.h *.hh "$(INSTALLINC)/")
#		@echo installed libsir successfully.
#	endif

clean:
	$(shell [ -d "$(BUILDDIR)" ] && rm -rf "$(BUILDDIR)" >/dev/null 2>&1; \
		$(call echo_success,INTDIR and BINDIR are cleaned.))

.PHONY: clean
