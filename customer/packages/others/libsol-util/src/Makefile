#*************************************************
# *
# *  Copyright (c) 2015 sphantix 
# *  All Rights Reserved
# *
# * ***********************************************
# #
#
#set your own environment option  
CC = gcc
AR = ar

CURR_DIR := $(shell pwd)
MACRO_DEFINES =
HDR_INC = -I$(CURR_DIR) \
		  -I$(CURR_DIR)/include \
		  -I$(CURR_DIR)/include/os \
		  -I$(CURR_DIR)/include/basic \

LDFLAGS = -L.
CFLAGS = -g -fPIC -Wall -Werror -Wfatal-errors $(HDR_INC) $(MACRO_DEFINES)
export CC
export AR
export CFLAGS
export LDFLAGS 

LIB_DIR=/usr/lib
OSDIR = $(CURR_DIR)/os
BASICDIR = $(CURR_DIR)/basic

#make target lib and relevant obj, sol is short for sphantix's own lib
PRG = libsol-util.so
OBJS = utl_logging.o utl_prctl.o utl_time.o utl_strconv.o \
	   utl_assert.o utl_memory.o utl_timer.o utl_ini_parser.o
INNER_LIBS = basic/basic.a os/osl.a
OUTTER_LIBS = 

#all target
all: $(PRG)
$(PRG): $(INNER_LIBS) $(OBJS)
	$(CC) $(CFLAGS) -shared -Wl,--whole-archive,-soname,$@ -o $@ \
	      $(OBJS) $(OSDIR)/osl.a $(BASICDIR)/basic.a\
	      -Wl,-lcrypt -Wl,--no-whole-archive

basic/basic.a:
	$(MAKE) -C basic 

os/osl.a:
	$(MAKE) -C os

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

.PRONY:clean install
clean:
	@$(MAKE) clean -C basic 
	@$(MAKE) clean -C os
	rm -f $(OBJS) $(PRG)

install:$(PRG)
	install -m 755 $(PRG) $(LIB_DIR)

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

-include $(OBJS:.o=.d)
