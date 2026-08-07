BINDIR = ../../bin
BUILDIR = ../../build

CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic -g -p

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BINDIR)/sudoku-solver

# Create the build and bin directories if they don't exist already
$(BINDIR) $(BUILDIR):
	mkdir -p $(BINDIR) $(BUILDIR)



# Rules to create the object files in $(BUILDIR):

# $@ is an alias for the target
# $^ is an alias for the dependencies
# '|' before BUILDIR requisite means 
# do not update if buildir timestamp changes

# Rules to create the object files for emulator:
$(BUILDIR)/sudoku-solver.o: sudoku-solver.c | $(BUILDIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(BUILDIR)/helpers.o: helpers.c | $(BUILDIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(BUILDIR)/worklist.o: worklist.c | $(BUILDIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(BUILDIR)/cell.o: cell.c | $(BUILDIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(BUILDIR)/grid.o: grid.c | $(BUILDIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(BUILDIR)/backtracking.o: backtracking.c | $(BUILDIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(BUILDIR)/solving.o: solving.c | $(BUILDIR)
	$(CC) $(CFLAGS) -c $^ -o $@


# Rules to create the executable targets:

# Rule to create the emulate executable, requiring emulate's dependent objects: (symbol_table included for helpers module)
$(BINDIR)/sudoku-solver: $(BUILDIR)/sudoku-solver.o $(BUILDIR)/cell.o $(BUILDIR)/helpers.o $(BUILDIR)/grid.o $(BUILDIR)/worklist.o $(BUILDIR)/backtracking.o $(BUILDIR)/solving.o
	$(CC) $(CFLAGS) $^ -o $@


clean:
	$(RM) *.o sudoku-solver
	$(RM) -r $(BINDIR)
	$(RM) -r $(BUILDIR)


