#*************************************************
# *
# *  Copyright (c) 2015 Sphantix 
# *  All Rights Reserved
# *
# * ***********************************************
# #
#
#set your own environment option  
MACRO_DEFINES =
CURR_DIR := $(shell pwd)
HDR_INC = -I$(CURR_DIR) \
		  -I$(CURR_DIR)/libs \
		  -I$(CURR_DIR)/main \

LIBS = -lsol-util -pthread -lcurl -lutil
CFLAGS += -g -std=c++11 -fPIC -Wall -Werror -Wfatal-errors $(HDR_INC) $(LDFLAGS) $(LIBS) $(MACRO_DEFINES)
export CFLAGS

#make target lib and relevant obj
PRG = opsp-client 
DIRS = main libs 
DIR_LIBS = main/main.a libs/libs.a

#all target  
all: $(PRG)
$(PRG): $(DIR_LIBS)
	$(CXX) -o $@ $(CFLAGS) $(DIR_LIBS)

libs/libs.a:
	$(MAKE) -C libs 

main/main.a:
	$(MAKE) -C main

.SUFFIXES: .c .o .cpp
.cpp.o:
	$(CXX) $(CFLAGS) -c $*.cpp -o $*.o

.PRONY:clean
clean:
	@$(MAKE) clean -C libs 
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
