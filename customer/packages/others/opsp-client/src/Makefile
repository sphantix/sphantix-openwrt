#*************************************************
# *
# *  Copyright (c) 2015 Sphantix 
# *  All Rights Reserved
# *
# * ***********************************************
# #
#
#set your own environment option  
MACRO_DEFINES = -DUTL_MEM_LEAK_TRACING -DUTL_MEM_DEBUG
HDR_INC = -I. \
		  -I.. \
		  -I../include \
		  -I../include/os \
		  -I../include/basic \

LIBS = -lsol-util
CFLAGS += -g -fPIC -Wall -Werror -Wfatal-errors $(LDFLAGS) $(LIBS) $(MACRO_DEFINES)
export CFLAGS

#make target lib and relevant obj
PRG = opsp-client 
DIRS = main jsonParser
DIR_LIBS = main/main.a jsonParser/jsonParser.a

#all target  
all: $(PRG)
$(PRG): $(DIR_LIBS)
	$(CC) -o $@ $(CFLAGS) $(DIR_LIBS)

jsonParser/jsonParser.a:
	$(MAKE) -C jsonParser

main/main.a:
	$(MAKE) -C main

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

.PRONY:clean
clean:
	@$(MAKE) clean -C jsonParser
	@$(MAKE) clean -C main
	rm -f $(PRG)

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

-include $(OBJS:.o=.d)
