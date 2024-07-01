#
# This is a multipurpose makefile
#
######### IMPORTANT DO NOT COMMENT THESE FLAGS

# The compiler to use
CC        = gcc
# The name of the executable to produce
EXEC      = project
# Ansi flags, make the compiler more informative
ANSI      = -ansi -pedantic -Wall -Wextra -std=c99
# Debug flags for gdb/GUD
DEBUG     = -O0 -gdwarf-2 -g3 -ggdb
#DEBUG     = -O0 -gdwarf-2
# Profile flags for gprof
PROFILE   = -pg -fprofile-arcs -ftest-coverage
# Optimization flags for production
OPTIMIZE  = -O9 -D NDEBUG
# Static for static StandAlones
STATIC = -static

#########  COMMENT THESE

# Uncomment the flags that you need in a particular compile
# Always use ANSI
CFLAGS   = $(ANSI)
#CFLAGS  += $(DEBUG)
CFLAGS  += -D_DEFAULT_SOURCE
CFLAGS  += -D NDEBUG
#CFLAGS  += $(PROFILE)
CFLAGS  += $(OPTIMIZE)
#CFLAGS  += $(STATIC)

##  Base Makefile
##  Diferent classes of files
CS    = $(wildcard *.c)
HS    = $(wildcard *.h)
IS    = $(addsuffix .i, $(basename $(CS)))
SS    = $(addsuffix .s, $(basename $(IS)))
OS    = $(addsuffix .o, $(basename $(SS)))
COVS  = $(addsuffix .c.gcov, $(basename $(CS)))
STATS = $(addsuffix .gcda, $(basename $(CS))) $(addsuffix .gcno, $(basename $(CS)))

# Phony targets
.PHONY: all coverage clean dotclean tags depend dox check

# Default Compile
all: $(SS) $(EXEC)

coverage: $(COVS)

## Linking rule
$(EXEC): $(OS)
	@echo Linking: $@
	$(CC) $(CFLAGS) $(OS) -o $@ -lm -lrt -lmd -lbsd

## Run Preprocessor
%.i: %.c
	@echo PreProcess: $<
	$(CC) $(CFLAGS) -E -o $@ $<

## Generate Assembly
%.s: %.i
	@echo Assemble: $<
	$(CC) $(CFLAGS) -S -o $@ $<

## Build Object
%.o: %.s
	@echo Build Object from: $<
	$(CC) $(CFLAGS) -c -o $@ $<

## Make dependencies
depend: depend.mak

## Make dependencies forcing creation of all files
depend.mak: $(CS) $(HS)
	@echo Making dependencies ...
	$(CC) -MM $(CS) | sed 's/\.o/\.i/g' > depend.mak

-include depend.mak

## Clean up
clean:
	@echo Cleaning Up
	rm -f $(EXEC) $(STATS) $(COVS) $(IS) $(SS) $(OS) $(HOLES) gmon.out depend.mak *~ callgrind.out.* cachegrind.out.*

## TAGS
tags: TAGS
TAGS: $(CS) $(HS)
	@echo Making TAGS
	etags $(CS) $(HS)
