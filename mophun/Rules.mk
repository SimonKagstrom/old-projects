######################################################################
##
## Copyright (C) 2004,  Blekinge Institute of Technology
##
## Filename:      Rules.mk
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Mophun Makefile rules. Requires GNU make.
##
## $Id: Rules.mk 1104 2005-01-16 05:59:19Z ska $
##
######################################################################

# NOTE: Set PATH to include the mophun/bin path first!

# --- Variables, these can be owerridden. ---
# programs
cc        ?= pip-gcc
CC        ?= pip-g++
ld        ?= pip-gcc
strip     ?= pip-strip
ar        ?= pip-ar
sed       ?= sed
tags	  ?= ctags
make      ?= make
rm        ?= rm
cp        ?= cp
mv        ?= mv
m4        ?= m4
zip       ?= zip
setenv    ?= export

find      ?= find
xargs     ?= xargs
wc        ?= wc
grep      ?= grep

morc      ?= wmorc
mocert    ?= wmocert

# Options
LDLIBS    ?=
INCLUDES  ?=
DEFINES   ?=

PHONE_BT_MAC   ?=00:0A:D9:9A:09:95

MOPHUN_VERSION ?=1.31
MOCERTCATEGORY ?=2
STACK_SIZE ?= 1024
DATA_SIZE  ?= 16192

CFLAGS      ?=-Wall -g -std=c99
CFLAGS_OPT  ?=-Wall -g -std=c99 -O9 -DNDEBUG
CPPFLAGS    ?=-Wall -g -O2
CPPFLAGS_OPT?=-Wall -g
LDFLAGS     ?=-mstack=$(STACK_SIZE) -mdata=$(DATA_SIZE) -L../lib/lib/
LDFLAGS_OPT ?=-mstack=$(STACK_SIZE) -mdata=$(DATA_SIZE) -L../lib/lib/

TAGSOPTS  ?= -e -R


OBJS_OPT=$(patsubst %.o,%_opt.o,$(OBJS))
TARGET_OPT=$(patsubst %.mpn,%,$(TARGET))OPT.mpn
TARGET_RELEASE=$(patsubst %.mpn,%,$(TARGET))C.mpn

# Rules
all: $(SUBDIRS) $(TARGET) $(LIBS)

%.o: %.txt $(RESOURCES)
	$(morc) $<

%.a: $(OBJS_OPT)
	$(ar) crs $@ $<

%_opt.o: %.txt $(RESOURCES)
	$(morc) $<
	$(mv) $(patsubst %.txt, %.o, $<) $@

%.h: %.txt
	$(morc) $<

%.o: %.c $(HEADERS)
	$(cc) $(CFLAGS) $(INCLUDES) -c $<

%_opt.o: %.c $(HEADERS)
	$(cc) $(CFLAGS_OPT) $(INCLUDES) -c $< -o $@

%.o: %.cc $(HEADERS)
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $<

# Targets
$(TARGET): $(OBJS)
	$(ld) $(LDFLAGS) $(LDLIBS) -mversion=$(MOPHUN_VERSION) -o $@ $+ $(LDLIBS)

$(TARGET_OPT): $(OBJS_OPT)
	$(ld) $(LDFLAGS) $(LDLIBS) -mversion=$(MOPHUN_VERSION) -s -o $@ $+ $(LDLIBS)
	$(strip) $@

t610.mpn: $(TARGET_OPT)

t310.mpn: $(OBJS_OPT)
	$(ld) $(LDFLAGS) -mversion=1.1 -s -o $@ $+ $(LDLIBS)
	$(strip) $@



opt: $(TARGET_OPT)

run: $(TARGET)
	wmophun	$<

optrun: $(TARGET_OPT)
	wmophun	$<

# Make a release (Do not list - many secrets here!)
$(TARGET_RELEASE): $(TARGET_OPT)
	$(mocert) -v -category$(MOCERTCATEGORY) -imei $(IMEINUMBER) -pass $(MOCERTPASSWORD) $(TARGET_OPT) $@
	@echo "$@ for IMEI $(IMEINUMBER) produced from $(TARGET)"

release: $(TARGET_RELEASE)

# Download the game to the phone
download: FORCE
	sudo obexftp -b $(PHONE_BT_MAC) -p $(patsubst %.mpn,%,$(TARGET))C.mpn

# Rule for making subdirectories
$(SUBDIRS): FORCE
	$(make) -C $@


tags: FORCE
	$(tags) $(TAGSOPTS) $(SUBDIRS) .

# Count the number of source code lines :-)
lines:
	$(find) . -name "*.c" -or -name "*.S" -or -name "*.h" | $(xargs) $(wc) -l

clean:
	for dir in $(SUBDIRS); do \
		$(make) clean -C $$dir; \
        done
	$(rm) -f *~ *.o *.d
	$(rm) -f $(EXTRA_CLEAN)
	rm -f $(TARGET) $(LIBS) $(TARGET_OPT) t610.mpn t310.mpn

# Dummy rule to force some operations
FORCE:
