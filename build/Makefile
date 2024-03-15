CC=gcc
CFLAGS=-c -O2 -g -std=c11 -Wall -Wextra -fPIE -pedantic -Wcast-qual \
	   -Wformat=2 -Wundef  -Wno-error=unused-variable 

PROJECT=router

LIBRARY=nope
LDFLAGS=
INCPATHS=include
LIBPATH=lib
LIBPATHS=.

PATHSRC=../src
PATHUTILS=$(PATHSRC)/utils
PATHRES=$(PATHSRC)/res

SOURCES= $(PATHSRC)/router.c \
		 $(PATHRES)/ipv4/ipv4_table.c $(PATHRES)/arp/arp_table.c \
		 $(PATHRES)/arp/arp.c $(PATHRES)/ipv4/ipv4.c $(PATHRES)/icmp/icmp.c \
		 $(PATHSRC)/utils/queue.c $(PATHSRC)/utils/list.c $(PATHSRC)/utils/lib.c

# Define the bin directory
BINDIR=bin

# Generate a list of object files for the sources excluding router.c
OBJECTS=$(filter-out $(BINDIR)/router.o, $(patsubst $(PATHSRC)/%.c,$(BINDIR)/%.o,$(SOURCES)))

INCFLAGS=$(foreach TMP,$(INCPATHS),-I$(TMP))
LIBFLAGS=$(foreach TMP,$(LIBPATHS),-L$(TMP))

# Set up the output file names for the different output types
BINARY=$(PROJECT)

all: $(BINARY)

$(BINARY): $(BINDIR)/router.o $(OBJECTS)
	$(CC) $(LIBFLAGS) $^ $(LDFLAGS) -o $@

$(BINDIR)/router.o: $(PATHSRC)/router.c
	@mkdir -p $(@D)
	$(CC) $(INCFLAGS) $(CFLAGS) -fPIC $< -o $@

$(BINDIR)/%.o: $(PATHSRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(INCFLAGS) $(CFLAGS) -fPIC $< -o $@

clean:
	sudo rm -rf $(BINARY) $(BINDIR) router *.o hosts_output router_*

run_router0: all
	./$(BINARY) rtable0.txt rr-0-1 r-0 r-1

run_router1: all
	./$(BINARY) rtable1.txt rr-0-1 r-0 r-1
